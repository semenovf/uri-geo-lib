////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.12 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "geo.hpp"
#include <locale>
#include <sstream>

namespace pfs {
namespace rfc5870 {

template <typename _OstreamType
    , typename _NumberType
    , typename _StringType
    , template <typename _Key, typename _Value> class _MapType>
_OstreamType & operator << (_OstreamType & out
    , basic_uri<_NumberType, _StringType, _MapType> const & u)
{
    using string_type = _StringType;

    auto saved_loc = out.imbue(std::locale("C"));

    out << "geo:" << u.latitude() << "," << u.longitude();

    if (u.has_altitude())
        out << "," << u.altitude();

    if (!u.is_wgs84())
        out << ";crs=" << u.crs();

    if (u.has_uncertainty())
        out << ";u=" << u.uncertainty();

    if (u.count() > 0) {
        u.foreach_parameter([& out] (string_type const & pname
                , string_type const & pvalue) {
            out << ";" << pname;

            if (pvalue == string_type{}) {
                out << "=" << pvalue;
            }
        });
    }

    // Restore locale
    out.imbue(saved_loc);
    return out;
}

std::string compose (uri const & u)
{
    std::ostringstream out;
    out << u;
    return out.str();
}

std::wstring compose (wuri const & u)
{
    std::wostringstream out;
    out << u;
    return out.str();
}

}} // namespace pfs::rfc5870
