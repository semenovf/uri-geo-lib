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
#include <sstream>

namespace pfs {
namespace rfc5870 {

template <typename _Ostream
    , typename _NumberType
    , typename _StringType
    , template <typename _Key, typename _Value> class _MapType>
_Ostream & operator << (_Ostream & out, basic_uri<_NumberType, _StringType, _MapType> const & u)
{
    using string_type = _StringType;

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

            if (!pvalue.empty()) {
                out << "=" << pvalue;
            }
        });
    }

    return out;
}

std::string compose (uri const & u)
{
    std::ostringstream out;
    out << u;
    return out.str();
}

}} // namespace pfs::rfc5870
