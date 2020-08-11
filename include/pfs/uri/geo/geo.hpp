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

private:
    number_type _latitude  {0};
    number_type _longitude {0};
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
    /**
     * Latitude value
     */
    number_type latitude () const noexcept
    {
        return _latitude;
    }

    /**
     * Set latitude value to @a n.
     */
    void set_latitude (number_type const & n) noexcept
    {
        _latitude = n;
    }

    /**
     * Longitude value
     */
    number_type longitude () const noexcept
    {
        return _longitude;
    }

    /**
     * Set longitude value to @a n.
     */
    void set_longitude (number_type const & n) noexcept
    {
        _longitude = n;
    }

    /**
     * Altitude value
     */
    number_type altitude () const noexcept
    {
        return _altitude.first;
    }

    /**
     * Set altitude value to @a n.
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
    void foreach_parameter (_BinaryOp f) const
    {
        for (auto it = _parameters.begin(), last = _parameters.end()
                ; it != last; ++it)
            f(it->first, it->second);
    }

    template <typename _BinaryOp>
    void foreach_mutable_parameter (_BinaryOp f)
    {
        for (auto it = _parameters.begin(), last = _parameters.end()
                ; it != last; ++it)
            f(it->first, it->second);
    }
};

using uri = basic_uri<double, std::string>;
using wuri = basic_uri<double, std::wstring>;

}}} // namespace pfs::uri::geo
