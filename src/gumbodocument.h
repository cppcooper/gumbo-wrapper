#ifndef QGUMBODOCUMENT_H
#define QGUMBODOCUMENT_H

#include <gumbo.h>

class GWNode;

class GWDocument
{
public:
    ~GWDocument();
    GWDocument(GWDocument&&);

    GWNode rootNode() const;
    static GWDocument parse(const std::string&);

private:
    GWDocument(std::string);

    GWDocument(const GWDocument&) = delete;
    GWDocument& operator=(const GWDocument&) = delete;

    GumboOutput *gumboOutput_ = nullptr;
    const GumboOptions *options_ = nullptr;
    std::string sourceData_;
};

#endif // QGUMBODOCUMENT_H
