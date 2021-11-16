#include <string>
#include <stdexcept>
#include "gumbodocument.h"
#include "gumbonode.h"

GWDocument GWDocument::parse(const std::string &input) {
    return GWDocument(GWDocument(input));
}

GWDocument::GWDocument(std::string arr) :
    options_(&kGumboDefaultOptions),
    sourceData_(arr)
{
    gumboOutput_ = gumbo_parse_with_options(options_,
                                            sourceData_.c_str(),
                                            sourceData_.length());
    if (!gumboOutput_)
        throw std::runtime_error("the data can't be parsed");
}

GWDocument::~GWDocument()
{
    if (gumboOutput_)
        gumbo_destroy_output(options_, gumboOutput_);
    if (options_ != &kGumboDefaultOptions)
        delete options_;
}

GWDocument::GWDocument(GWDocument &&source) :
    gumboOutput_(source.gumboOutput_),
    options_(source.options_),
    sourceData_(source.sourceData_)
{
    source.gumboOutput_ = nullptr;
    source.options_ = nullptr;
}

GWNode GWDocument::rootNode() const
{
    return GWNode(gumboOutput_->root);
}
