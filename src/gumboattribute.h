#ifndef QGUMBOATTRIBUTE_H
#define QGUMBOATTRIBUTE_H

#include <string>

class GWAttribute {
public:
    GWAttribute(const char* utf8name, const char* utf8value);

    const std::string& name() const { return name_; }
    const std::string& value() const { return value_; }

private:
    std::string name_;
    std::string value_;
};

#endif // QGUMBOATTRIBUTE_H
