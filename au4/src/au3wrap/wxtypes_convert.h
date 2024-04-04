#ifndef AU_AU3WRAP_WXTYPES_CONVERT_H
#define AU_AU3WRAP_WXTYPES_CONVERT_H

#include <string>
#include "global/types/string.h"
#include "wx/string.h"

namespace au::au3 {
inline std::string wxToStdSting(const wxString& s)
{
    return s.ToStdString(wxConvUTF8);
}

inline mu::String wxToSting(const wxString& s)
{
    return mu::String::fromStdString(wxToStdSting(s));
}
}

#endif // AU_AU3WRAP_WXTYPES_CONVERT_H
