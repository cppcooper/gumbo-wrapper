#include "gumboattribute.h"
#include <stdexcept>


GWAttribute::GWAttribute(const char* n, const char* value) {
    if (!n)
        throw std::invalid_argument("attribute name can't be empty");

    name_ = n;

    if (value)
        value_ = value;
}
