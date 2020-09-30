////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-uri-geo](https://github.com/semenovf/pfs-uri-geo) library.
//
// Changelog:
//      2020.07.12 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <map>

namespace pfs {
namespace uri {
namespace geo {

template <typename _Key, typename _Value>
using map = std::map<_Key, _Value>;

// Map inserting trait for std::map
template <typename _Key, typename _Value>
inline void insert_map (map<_Key, _Value> & m, _Key && key, _Value && value)
{
    m.insert(std::make_pair<_Key, _Value>(std::forward<_Key>(key)
        , std::forward<_Value>(value)));
}

template <typename _StringType>
inline _StringType construct_string (std::initializer_list<char> const & il)
{
    _StringType result;
    result.reserve(il.size());

    for (auto const & ch: il)
        result.push_back(ch);

    return result;
}

struct wgs84_crs {};

/**
 * Geo URI representation
 */
template <typename _NumberType = double
    , typename _StringType = std::string
    , template <typename _Key, typename _Value> class _MapType = map>
class basic_uri
{
public:
    using number_type = _NumberType;
    using string_type = _StringType;
    using parameters_type = _MapType<string_type, string_type>;

    static constexpr number_type min_wgs84_latitude = number_type{-90};
    static constexpr number_type max_wgs84_latitude = number_type{90};
    static constexpr number_type min_wgs84_longitude = number_type{-180};
    static constexpr number_type max_wgs84_longitude = number_type{180};

private:
    number_type _latitude  {0};
    number_type _longitude {0};

    /*
     * 3.4.2.Component Description for WGS-84
     * ...
     * If the altitude of the location is unknown, <altitude> (and the comma
     * before) MUST NOT be present in the URI.  Specifically, unknown
     * altitude MUST NOT be represented by setting <altitude> to "0" (or any
     * other arbitrary value).
     */
    std::pair<number_type, bool> _altitude  {0, false};

    string_type _crslabel  {wgs84_string()};
    std::pair<number_type, bool> _uval {0, false}; // Location Uncertainty
    parameters_type _parameters;

private:
    static string_type wgs84_string ()
    {
        return construct_string<string_type>({'w', 'g', 's', '8', '4'});
    }

public:
    basic_uri () = default;
    basic_uri (basic_uri const &) = default;
    basic_uri (basic_uri &&) = default;
    basic_uri & operator = (basic_uri const &) = default;
    basic_uri & operator = (basic_uri &&) = default;

    basic_uri (number_type latitude, number_type longitude)
    {
        set_latitude(latitude);
        set_longitude(longitude);
    }

    basic_uri (number_type latitude, number_type longitude, number_type altitude)
    {
        set_latitude(latitude);
        set_longitude(longitude);
        set_altitude(altitude);
    }

    /**
     * Latitude value according to CRS (in range from -90 to 90 decimal degrees
     * in the reference system WGS-84).
     */
    number_type latitude () const noexcept
    {
        return _latitude;
    }

    /**
     * Set latitude value to @a n (must be in range from -90 to 90 decimal
     * degrees in the reference system WGS-84).
     */
    void set_latitude (number_type const & n) noexcept
    {
        _latitude = n;
    }

    /**
     * Longitude value according to CRS (in range from -180 to 180 decimal degrees
     * in the reference system WGS-84).
     */
    number_type longitude () const noexcept
    {
        return _longitude;
    }

    /**
     * Set longitude value to @a n (must be in range from -180 to 180 decimal
     * degrees in the reference system WGS-84).
     */
    void set_longitude (number_type const & n) noexcept
    {
        _longitude = n;
    }

    /**
     * Altitude value (in meters)
     */
    number_type altitude () const noexcept
    {
        return _altitude.first;
    }

    /**
     * Set altitude value to @a n (in meters).
     */
    void set_altitude (number_type const & n) noexcept
    {
        _altitude.first = n;
        _altitude.second = true;
    }

    /**
     * Unset altitude value.
     */
    void clear_altitude ()
    {
        _altitude.first = 0;
        _altitude.second = false;
    }

    /**
     * Checks if altitude is present in geo URI
     */
    bool has_altitude () const noexcept
    {
        return _altitude.second;
    }

    /**
     * Coordinate reference system (CRS) value
     */
    string_type crs () const noexcept
    {
        return _crslabel;
    }

    /**
     * Set Coordinate reference system (CRS) value to @a s.
     */
    void set_crs (string_type const & s)
    {
        _crslabel = s;
    }

    /**
     * Checks if CRS is WGS-84.
     */
    bool is_wgs84 () const noexcept
    {
        return _crslabel == wgs84_string();
    }

    /**
     * The amount of uncertainty in the location as a value in meters.
     */
    number_type uncertainty () const noexcept
    {
        return _uval.first;
    }

    /**
     * Set uncertainty value to @a n.
     */
    void set_uncertainty (number_type const & n) noexcept
    {
        _uval.first = n;
        _uval.second = true;
    }

    /**
     * Unset uncertainty value.
     */
    void clear_uncertainty ()
    {
        _uval.first = 0;
        _uval.second = false;
    }

    /**
     * Checks if uncertainty is present in geo URI
     */
    bool has_uncertainty () const noexcept
    {
        return _uval.second;
    }

    /**
     * Inserts new parameter (the behaviour is identical as inserting to _MapType)
     */
    void insert (string_type && pname, string_type && pvalue = string_type{})
    {
        insert_map(_parameters
            , std::forward<string_type>(pname)
            , std::forward<string_type>(pvalue));
    }

    bool has_parameter (string_type const & pname) const
    {
        return _parameters.find(pname) != _parameters.end();
    }

    auto count () const -> decltype(_parameters.size())
    {
        return _parameters.size();
    }

    string_type parameter (string_type const & pname) const
    {
        auto it = _parameters.find(pname);

        return it != _parameters.end()
            ? it->second
            : string_type{};
    }

    template <typename _BinaryOp>
    void foreach_parameter (_BinaryOp && f) const
    {
        for (auto it = _parameters.begin(), last = _parameters.end()
                ; it != last; ++it)
            f(it->first, it->second);
    }

    template <typename _BinaryOp>
    void foreach_mutable_parameter (_BinaryOp && f)
    {
        for (auto it = _parameters.begin(), last = _parameters.end()
                ; it != last; ++it)
            f(it->first, it->second);
    }
};

using uri = basic_uri<double, std::string>;
using wuri = basic_uri<double, std::wstring>;

/*
 * 3.4.2. Component Description for WGS-84
 * ...
 * The <longitude> of coordinate values reflecting the poles (<latitude>
 * set to -90 or 90 degrees) SHOULD be set to "0", although consumers of
 * 'geo' URIs MUST accept such URIs with any longitude value from -180
 * to 180.
 */
/**
 * Constructs North Pole URI in the reference system WGS-84.
 */
template <typename _UriType>
inline _UriType north_pole ()
{
    using number_type = typename _UriType::number_type;

    // By default CRS is WGS-84
    return _UriType{_UriType::max_wgs84_latitude, number_type{0}};
}

/**
 * Constructs South Pole URI in the reference system WGS-84.
 */
template <typename _UriType>
inline _UriType south_pole ()
{
    using number_type = typename _UriType::number_type;

    // By default CRS is WGS-84
    return _UriType{_UriType::min_wgs84_latitude, number_type{0}};
}

/**
 * Checks if URI @a uri represents North Pole URI in the reference system WGS-84.
 */
template <typename _UriType>
inline bool is_north_pole (_UriType const & uri)
{
    return uri.is_wgs84() && uri.latitude() == _UriType::max_wgs84_latitude;
}

/**
 * Checks if URI @a uri represents North Pole URI in the reference system WGS-84.
 */
template <typename _UriType>
inline bool is_south_pole (_UriType const & uri)
{
    return uri.is_wgs84() && uri.latitude() == _UriType::min_wgs84_latitude;
}

}}} // namespace pfs::uri::geo
