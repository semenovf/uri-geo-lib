////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-uri-geo](https://github.com/semenovf/pfs-uri-geo) library.
//
// Changelog:
//      2020.07.12 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "geo.hpp"
#include <bitset>
#include <functional>
#include <locale>
#include <ostream>
#include <sstream>

namespace pfs {
namespace uri {
namespace geo {

enum class composer_policy_flag {
      ignore_wgs84_crs    //<! do not output CRS if it equals to WGS84
    , composer_policy_count
};

using composer_policy_set = std::bitset<static_cast<int>(composer_policy_flag::composer_policy_count)>;

inline composer_policy_set relaxed_composer_policy ()
{
    composer_policy_set result;
    result.set(static_cast<int>(composer_policy_flag::ignore_wgs84_crs), true);
    return result;
}

inline composer_policy_set strict_composer_policy ()
{
    composer_policy_set result;
    return result;
}

template <typename _OstreamType, typename _NumberType, typename _StringType>
struct composer_interface
{
    using ostream_type = _OstreamType;
    using number_type = _NumberType;
    using string_type = _StringType;

    composer_policy_set policy = relaxed_composer_policy();

    std::function<void(ostream_type &, number_type const &)> compose_coordinate
        = [] (ostream_type & out, number_type const & n) {
            out << n;
        };

    std::function<void(ostream_type &, string_type const &)> compose_crs
        = [] (ostream_type & out, string_type const & s) {
            out << s;
        };

    std::function<void(ostream_type &, number_type const &)> compose_uncertainty
        = [] (ostream_type & out, number_type const & n) {
            out << n;
        };

    std::function<void(ostream_type &, string_type const &)> compose_property_name
        = [] (ostream_type & out, string_type const & s) {
            out << s;
        };

    std::function<void(ostream_type &, string_type const &)> compose_property_value
        = [] (ostream_type & out, string_type const & s) {
            out << s;
        };
};

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
_OstreamType & compose (_OstreamType & out
    , basic_uri<_NumberType, _StringType, _MapType> const & u
    , composer_interface<_OstreamType, _NumberType, _StringType> const & ctx)
{
    using string_type = _StringType;

    // Need "C" locale for numbers output
    imbue_C_guard<_OstreamType>{out};

    out << "geo:";// << ctx.compose_coordinate(out, u.latitude())
    ctx.compose_coordinate(out, u.latitude());
    out << ",";
    ctx.compose_coordinate(out, u.longitude());

    if (u.has_altitude()) {
        out << ",";
        ctx.compose_coordinate(out, u.altitude());
    }

    if (!(u.is_wgs84() && !ctx.policy.test(static_cast<int>(composer_policy_flag::ignore_wgs84_crs)))) {
        out << ";crs=";
        ctx.compose_crs(out, u.crs());
    }

    if (u.has_uncertainty()) {
        out << ";u=";
        ctx.compose_uncertainty(out, u.uncertainty());
    }

    if (u.count() > 0) {
        auto pctx = & ctx;

        u.foreach_parameter([& out, pctx] (string_type const & pname
                , string_type const & pvalue) {
            out << ";";
            pctx->compose_property_name(out, pname);

            if (pvalue != string_type{}) {
                out << "=";
                pctx->compose_property_value(out, pvalue);
            }
        });
    }

    return out;
}

template <typename _OstreamType
    , typename _NumberType
    , typename _StringType
    , template <typename _Key, typename _Value> class _MapType>
inline _OstreamType & operator << (_OstreamType & out
    , basic_uri<_NumberType, _StringType, _MapType> const & u)
{
    return compose<_OstreamType, _NumberType, _StringType, _MapType>(out, u
        , composer_interface<_OstreamType, _NumberType, _StringType>());
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

}}} // namespace pfs::uri::geo
