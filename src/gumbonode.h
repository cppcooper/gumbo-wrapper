#ifndef QGUMBONODE_H
#define QGUMBONODE_H

#include <vector>
#include <functional>
#include <string>
#include <gumbo.h>
#include "HtmlTag.h"

class GWNode;
class GWAttribute;
class GWDocument;

typedef std::vector<GWNode> 		GWNodes;
typedef std::vector<GWAttribute> 	GWAttributes;

class GWNode
{
public:
    GWNode(const GWNode&) = default;
    GWNode(GWNode&&) noexcept = default;
    GWNode& operator=(const GWNode&) = default;

    HtmlTag tag() const;
    std::string tagName() const;
    std::string nodeName() const;

    std::string id() const;
    std::vector<std::string> classList() const;

    GWNodes getElementById(const std::string&) const;
    GWNodes getElementsByTagName(HtmlTag) const;
    GWNodes getElementsByClassName(const std::string&) const;
    GWNodes childNodes() const;
    GWNodes children() const;

    int childElementCount() const;

    bool isElement() const;
    bool hasAttribute(const std::string&) const;

    std::string innerText() const;
    std::string outerHtml() const;
    std::string getAttribute(const std::string&) const;

    GWAttributes allAttributes() const;

    void forEach(std::function<void(const GWNode&)>) const;

    explicit operator bool() const;

private:
    GWNode();
    GWNode(GumboNode* node);

    friend class GWDocument;
private:
    GumboNode* ptr_;
};

#endif // QGUMBONODE_H
