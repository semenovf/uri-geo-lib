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
#include <ostream>
#include <sstream>

namespace pfs {
namespace rfc5870 {

template <typename _OstreamType>
class imbue_C_guard
{
    std::locale _saved_loc;
    _OstreamType & _out;

public:
    imbue_C_guard (_OstreamType & out) : _out(out)
    {
        _saved_loc = _out.imbue(std::locale("C"));
    }

    ~imbue_C_guard ()
    {
        _out.imbue(_saved_loc);
    }
};

template <typename _OstreamType
    , typename _NumberType
    , typename _StringType
    , template <typename _Key, typename _Value> class _MapType>
_OstreamType & operator << (_OstreamType & out
    , basic_uri<_NumberType, _StringType, _MapType> const & u)
{
    using string_type = _StringType;

    // Need "C" locale for numbers output
    imbue_C_guard<_OstreamType>{out};

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

            if (pvalue != string_type{}) {
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

std::wstring compose (wuri const & u)
{
    std::wostringstream out;
    out << u;
    return out.str();
}

}} // namespace pfs::rfc5870
