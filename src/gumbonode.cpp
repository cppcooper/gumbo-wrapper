#include <cstring>
#include <sstream>
#include <string>
#include <algorithm>
//#include <stringList>
#include <cassert>
#include "gumbonode.h"
#include "gumboattribute.h"

#include <regex>
std::vector<std::string> split(const std::string &input, const std::string &regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
            last;
    return {first, last};
}

namespace {

const char* ID_ATTRIBUTE 		= "id";
const char* CLASS_ATTRIBUTE 	= "class";

template<typename TFunctor>
bool iterateTree(GumboNode* node, TFunctor& functor)
{
    if (!node || node->type != GUMBO_NODE_ELEMENT)
        return false;

    if (functor(node))
        return true;

    for (uint i = 0; i < node->v.element.children.length; ++i) {
        if (iterateTree(static_cast<GumboNode*>(node->v.element.children.data[i]), functor))
            return true;
    }

    return false;
}

template<typename TFunctor>
bool iterateChildren(GumboNode* node, TFunctor& functor)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return false;

    GumboVector& vec = node->v.element.children;

    for (uint i = 0, e = vec.length; i < e; ++i) {
        GumboNode* node = static_cast<GumboNode*>(vec.data[i]);
        if (functor(node))
            return true;
    }

    return false;
}

} /* namespace */


GWNode::GWNode()
{
}

GWNode::GWNode(GumboNode* node) :
    ptr_(node)
{
    if (!ptr_)
        throw std::runtime_error("can't create Node from nullptr");
}

GWNodes GWNode::getElementById(const std::string& nodeId) const
{
    assert(ptr_);
    if (nodeId.empty())
        throw std::invalid_argument("id can't be empty string");

    GWNodes nodes;

    auto functor = [&nodes, &nodeId] (GumboNode* node) {
        GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, ID_ATTRIBUTE);
        if (attr) {
            const std::string value(attr->value);
            if (value == nodeId) {
                nodes.emplace_back(GWNode(node));
                return true;
            }
        }
        return false;
    };
    iterateTree(ptr_, functor);

    return nodes;
}

GWNodes GWNode::getElementsByTagName(HtmlTag tag) const
{
    assert(ptr_);

    GumboTag tag_ = static_cast<GumboTag>(tag);
    GWNodes nodes;

    auto functor = [&nodes, tag_](GumboNode* node) {
        if (node->v.element.tag == tag_) {
            nodes.emplace_back(GWNode(node));
        }
        return false;
    };

    iterateTree(ptr_, functor);

    return nodes;
}

GWNodes GWNode::getElementsByClassName(const std::string& name) const
{
    assert(ptr_);

    if (name.empty())
        throw std::invalid_argument("class name can't be empty string");

    GWNodes nodes;

    auto functor = [&nodes, &name] (GumboNode* node) {
        GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, CLASS_ATTRIBUTE);
        if (attr) {
            const std::string value(attr->value);
            auto parts = split(value, " ");
            for (const std::string &part : parts) {
                if (part == name) {
                    nodes.emplace_back(GWNode(node));
                    break;
                }
            }
        }
        return false;
    };

    iterateTree(ptr_, functor);

    return nodes;
}

GWNodes GWNode::childNodes() const
{
    assert(ptr_);

    GWNodes nodes;

    auto functor = [&nodes] (GumboNode* node) {
        nodes.emplace_back(GWNode(node));
        return false;
    };

    iterateChildren(ptr_, functor);

    return nodes;
}

GWNodes GWNode::children() const
{
    assert(ptr_);

    GWNodes nodes;

    auto functor = [&nodes] (GumboNode* node) {
        if (node->type == GUMBO_NODE_ELEMENT) {
            nodes.emplace_back(GWNode(node));
        }
        return false;
    };

    iterateChildren(ptr_, functor);

    return nodes;
}

int GWNode::childElementCount() const
{
    assert(ptr_);

    int count = 0;

    auto functor = [&count] (GumboNode* node) {
        if (node->type == GUMBO_NODE_ELEMENT)
            ++count;
        return false;
    };

    iterateChildren(ptr_, functor);

    return count;
}

std::string GWNode::innerText() const
{
    assert(ptr_);

    std::string text;

    auto functor = [&text] (GumboNode* node) {
        if (node->type == GUMBO_NODE_TEXT) {
            text += std::string(node->v.text.text);
        }
        return false;
    };

    iterateChildren(ptr_, functor);

    return text;
}

std::string GWNode::outerHtml() const
{
    assert(ptr_);

    std::string text;
    switch (ptr_->type) {
    case GUMBO_NODE_DOCUMENT: {
        throw std::runtime_error("invalid node type");
    }
    case GUMBO_NODE_ELEMENT: {
        const auto& elem = ptr_->v.element;
        const auto& tag = elem.original_tag;
        if (tag.data && tag.length) {
            int lenght = elem.end_pos.offset - elem.start_pos.offset + elem.original_end_tag.length;
            assert(lenght > 0);
            text = tag.data;
        }
        break;
    }
    default: {
        const auto& str = ptr_->v.text.original_text;
        text = str.data;
    }}
    return text;
}

HtmlTag GWNode::tag() const
{
    if (isElement())
        return HtmlTag(ptr_->v.element.tag);

    return HtmlTag::UNKNOWN;
}

std::string GWNode::tagName() const
{
    assert(ptr_);
    GumboTag tag = ptr_->v.element.tag;
    return gumbo_normalized_tagname(tag);
}

std::string GWNode::nodeName() const
{
    return tagName();
}

std::string GWNode::id() const
{
    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes, ID_ATTRIBUTE);
    if (attr)
        return attr->value;

    return std::string();
}

std::vector<std::string> GWNode::classList() const
{
    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes, CLASS_ATTRIBUTE);
    if (attr) {
        std::string values(attr->value);
        return split(values, " ");
    }

    return std::vector<std::string>();
}

bool GWNode::isElement() const
{
    return ptr_->type == GUMBO_NODE_ELEMENT;
}

bool GWNode::hasAttribute(const std::string& name) const
{
    if (name.empty())
        throw std::invalid_argument("attribute can't be empty string");

    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes,
                                               name.c_str());
    return attr != nullptr;
}

std::string GWNode::getAttribute(const std::string& attrName) const
{
    if (attrName.empty())
        throw std::invalid_argument("attribute name can't be empty string");

    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes,
                                               attrName.c_str());
    if (attr)
        return (attr->value);

    return std::string();
}

GWAttributes GWNode::allAttributes() const
{
    assert(ptr_);

    GWAttributes attrs;

    for (uint i = 0, len = ptr_->v.element.attributes.length; i < len; ++i) {
        GumboAttribute* attr =
                static_cast<GumboAttribute*>(ptr_->v.element.attributes.data[i]);
        attrs.emplace_back(GWAttribute(attr->name, attr->value));
    }
    return attrs;
}


void GWNode::forEach(std::function<void(const GWNode&)> func) const
{
    assert(ptr_);

    auto functor = [&func](GumboNode* node) {
        func(GWNode(node));
        return false;
    };

    iterateTree(ptr_, functor);
}

GWNode::operator bool() const
{
    return ptr_;
}
