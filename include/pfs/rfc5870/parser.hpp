////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.10 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "geo.hpp"
#include <array>
#include <bitset>
#include <functional>
#include <string>
#include <cassert>
#include <locale>

namespace pfs {
namespace rfc5870 {

/**
 * [RFC 5870](https://tools.ietf.org/html/rfc5870)
 */

/*
 *  RFC 5870:   A Uniform Resource Identifier for Geographic Locations ('geo' URI)
 *  URL: https://tools.ietf.org/html/rfc5870
 *  ----------------------------------------------------------------------------
 *
 *  geo-URI       = geo-scheme ":" geo-path
 *  geo-scheme    = "geo"
 *  geo-path      = coordinates p
 *  coordinates   = coord-a "," coord-b [ "," coord-c ]
 *
 *  coord-a       = num
 *  coord-b       = num
 *  coord-c       = num
 *
 *  p             = [ crsp ] [ uncp ] *parameter
 *  crsp          = ";crs=" crslabel
 *  crslabel      = "wgs84" / labeltext
 *  uncp          = ";u=" uval
 *  uval          = pnum
 *  parameter     = ";" pname [ "=" pvalue ]
 *  pname         = labeltext
 *  pvalue        = 1*paramchar
 *  paramchar     = p-unreserved / unreserved / pct-encoded
 *
 *  labeltext     = 1*( alphanum / "-" )
 *  pnum          = 1*DIGIT [ "." 1*DIGIT ]
 *  num           = [ "-" ] pnum
 *  unreserved    = alphanum / mark
 *  mark          = "-" / "_" / "." / "!" / "~" / "*" /
 *                  "'" / "(" / ")"
 *  pct-encoded   = "%" HEXDIG HEXDIG
 *  p-unreserved  = "[" / "]" / ":" / "&" / "+" / "$"
 *  alphanum      = ALPHA / DIGIT
 *
 * Parameter names are case insensitive, but use of the lowercase
 * representation is preferred.  Case sensitivity of non-numeric
 * parameter values MUST be described in the specification of the
 * respective parameter.  For the 'crs' parameter, values are case
 * insensitive, and lowercase is preferred.
 */

enum parse_policy_flag {
      lowercase_labeltext  // convert label text to lowercase
    , parse_policy_count
};

using parse_policy_set = std::bitset<parse_policy_count>;

inline parse_policy_set relaxed_policy ()
{
    parse_policy_set result;
    return result;
}

inline parse_policy_set strict_policy ()
{
    parse_policy_set result;
    result.set(lowercase_labeltext, true);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Simple API interface
////////////////////////////////////////////////////////////////////////////////
template <typename _UserContext>
class simple_api_interface
{
    _UserContext & _context;

public:
    parse_policy_set policy = strict_policy();

    using number_type = typename _UserContext::number_type;
    using string_type = typename _UserContext::string_type;

public:
    simple_api_interface (_UserContext & ctx) : _context(ctx)
    {}

    std::function<void(number_type &&)> on_latitude
        = [] (number_type &&) {};

    std::function<void(number_type &&)> on_longitude
        = [] (number_type &&) {};

    std::function<void(number_type &&)> on_altitude
        = [] (number_type &&) {};

    // Coordinate reference system (CRS)
    std::function<void(string_type &&)> on_crslabel
        = [] (string_type &&) {};

    std::function<void(number_type &&)> on_uval
        = [] (number_type &&) {};

    std::function<void(string_type &&, string_type &&)> on_parameter
        = [] (string_type &&, string_type &&) {};
};

////////////////////////////////////////////////////////////////////////////////
// is_equals_ignorecase
////////////////////////////////////////////////////////////////////////////////
template <typename _CharT>
inline bool is_equals_ignorecase (_CharT a, _CharT b)
{
    auto loc = std::locale();
    return std::toupper(a, loc) == std::toupper(b, loc);
}

////////////////////////////////////////////////////////////////////////////////
// to_lower
////////////////////////////////////////////////////////////////////////////////
template <typename _CharT>
inline _CharT to_lower (_CharT ch)
{
    auto loc = std::locale();
    return std::tolower(ch, loc);
}

////////////////////////////////////////////////////////////////////////////////
// locale_decimal_point
////////////////////////////////////////////////////////////////////////////////
/*
 * Get locale-specific decimal point character.
 */
inline char locale_decimal_point ()
{
    std::lconv const * loc = std::localeconv();
    assert(loc);
    return (loc->decimal_point && *loc->decimal_point != '\x0')
            ? *loc->decimal_point : '.';
}

////////////////////////////////////////////////////////////////////////////////
// Convert string to number functions
////////////////////////////////////////////////////////////////////////////////
inline bool strtoreal (double & n, std::string const & numstr)
{
    char * endptr = nullptr;
    n = std::strtod(numstr.c_str(), & endptr);
    return (errno != ERANGE && endptr == & *numstr.end());
}

////////////////////////////////////////////////////////////////////////////////
// compare_and_assign
////////////////////////////////////////////////////////////////////////////////
/**
 * Helper function assigns @a b to @a a if @a a != @a b.
 */
template <typename _ForwardIterator>
inline bool compare_and_assign (_ForwardIterator & a, _ForwardIterator b)
{
    if (a != b) {
        a = b;
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// is_digit
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is a decimal digit (0..9), otherwise @c false.
 */
template <typename CharT>
inline bool is_digit (CharT ch)
{
    return (ch == CharT('0')
            || ch == CharT('1')
            || ch == CharT('2')
            || ch == CharT('3')
            || ch == CharT('4')
            || ch == CharT('5')
            || ch == CharT('6')
            || ch == CharT('7')
            || ch == CharT('8')
            || ch == CharT('9'));
}

////////////////////////////////////////////////////////////////////////////////
// is_alpha
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is an alpha (A-Z / a-z), otherwise @c false.
 */
template <typename CharT>
inline bool is_alpha (CharT ch)
{
    return ((ch >= CharT('\x41') && ch <= CharT('\x5A'))
            || (ch >= CharT('\x61') && ch <= CharT('\x7A')));
}

////////////////////////////////////////////////////////////////////////////////
// is_alphanum
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is an alpha or digit, otherwise @c false.
 */
template <typename CharT>
inline bool is_alphanum (CharT ch)
{
    return is_alpha(ch) || is_digit(ch);
}

////////////////////////////////////////////////////////////////////////////////
// is_hexdigit
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is a hexadecimal digit (0..9A..Fa..f).
 */
template <typename _CharT>
inline bool is_hexdigit (_CharT ch)
{
    return (is_digit(ch)
            || ch == _CharT('a')
            || ch == _CharT('b')
            || ch == _CharT('c')
            || ch == _CharT('d')
            || ch == _CharT('e')
            || ch == _CharT('f')
            || ch == _CharT('A')
            || ch == _CharT('B')
            || ch == _CharT('C')
            || ch == _CharT('D')
            || ch == _CharT('E')
            || ch == _CharT('F'));
}

////////////////////////////////////////////////////////////////////////////////
// is_p_unreserved
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is one of the characters:
 *      "[" / "]" / ":" / "&" / "+" / "$"
 */
template <typename _CharT>
inline bool is_p_unreserved (_CharT ch)
{
    return (ch == _CharT('[')
            || ch == _CharT(']')
            || ch == _CharT(':')
            || ch == _CharT('&')
            || ch == _CharT('+')
            || ch == _CharT('$'));
}

////////////////////////////////////////////////////////////////////////////////
// is_mark
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is one of the characters:
 *      "-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")"
 */
template <typename _CharT>
inline bool is_mark (_CharT ch)
{
    return (ch == _CharT('-')
        || ch == _CharT('_')
        || ch == _CharT('.')
        || ch == _CharT('!')
        || ch == _CharT('~')
        || ch == _CharT('*')
        || ch == _CharT('\'')
        || ch == _CharT('(')
        || ch == _CharT(')'));
}

////////////////////////////////////////////////////////////////////////////////
// is_unreserved
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is alphanum (@see is_alphanum) or
 *      mark (@see is_mark).
 */
template <typename _CharT>
inline bool is_unreserved (_CharT ch)
{
    return (is_alphanum(ch) || is_mark(ch));
}

////////////////////////////////////////////////////////////////////////////////
// to_digit
////////////////////////////////////////////////////////////////////////////////
/**
 * @return Base-@a radix digit converted from character @a ch,
 *      or -1 if conversion is impossible. @a radix must be between 2 and 36
 *      inclusive.
 */
template <typename CharT>
int to_digit (CharT ch, int radix = 10)
{
    int digit = 0;

    // Bad radix
    if (radix < 2 || radix > 36)
        return -1;

    if (int(ch) >= int('0') && int(ch) <= int('9'))
        digit = int(ch) - int('0');
    else if (int(ch) >= int('a') && int(ch) <= int('z'))
        digit = int(ch) - int('a') + 10;
    else if (int(ch) >= int('A') && int(ch) <= int('Z'))
        digit = int(ch) - int('A') + 10;
    else
        return -1;

    if (digit >= radix)
        return -1;

    return digit;
}

////////////////////////////////////////////////////////////////////////////////
// advance_pct_encoded
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance pct-encoded sequence.
 *
 * @note Grammar
 * pct-encoded = "%" HEXDIG HEXDIG
 */
template <typename _ForwardIterator>
bool advance_pct_encoded (_ForwardIterator & pos, _ForwardIterator last
    , int16_t * result = nullptr)
{
    static constexpr int16_t multipliers[] = { 16, 1 };
    static constexpr int count = sizeof(multipliers) / sizeof(multipliers[0]);

    if (pos == last)
        return false;

    auto p = pos;
    int index = 0;
    int16_t encoded_char = 0;

    if (*p != '%')
        return false;

    ++p;

    if (p == last)
        return false;

    for (; p != last && is_hexdigit(*p) && index < count; ++p, ++index) {
        int16_t n = to_digit(*p, 16);
        encoded_char += n * multipliers[index];
    }

    if (index != count)
        return false;

    if (result)
        *result = encoded_char;

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_sequence
// Based on pfs/algo/advance.hpp:advance_sequence
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by sequence of charcters.
 * @param pos On input - first position, on output - last good position.
 * @param last End of sequence position.
 * @return @c true if advanced by all character sequence [first2, last2),
 *      otherwise returns @c false.
 */
template <typename _ForwardIterator1, typename _ForwardIterator2>
inline bool advance_sequence_ignorecase (_ForwardIterator1 & pos, _ForwardIterator1 last
        , _ForwardIterator2 first2, _ForwardIterator2 last2)
{
    auto p = pos;

    while (p != last && first2 != last2 && is_equals_ignorecase(*p++, *first2++))
        ;

    if (first2 == last2) {
        pos = p;
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// advance_geo_scheme
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance geo scheme.
 *
 * @note Grammar
 * geo-scheme    = "geo"
 */
template <typename _ForwardIterator>
bool advance_geo_scheme (_ForwardIterator & pos
        , _ForwardIterator last)
{
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    if (pos == last)
        return false;

    std::array<char_type, 3> sample {'g', 'e', 'o'};

    return advance_sequence_ignorecase(pos, last, std::begin(sample), std::end(sample));
}

////////////////////////////////////////////////////////////////////////////////
// advance_number
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by number.
 *
 * @note Grammar
 * num  = [ "-" ] pnum
 * pnum = 1*DIGIT [ "." 1*DIGIT ]
 */
template <typename _ForwardIterator, typename _NumberType>
bool advance_number (_ForwardIterator & pos, _ForwardIterator last
        , bool allow_negative_sign
        , _NumberType * pnum)
{
    auto p = pos;

    if (p == last)
        return false;

    std::string numstr;
    int sign = 1;

    ////////////////////////////////////////////////////////////////////////////
    // Advance sign
    ////////////////////////////////////////////////////////////////////////////
    if (allow_negative_sign) {
        if (p != last) {
            if (*p == '-') {
                sign = -1;
                numstr.push_back('-');
                ++p;
            }
        }
    }

    auto last_pos = p;

    ////////////////////////////////////////////////////////////////////////////
    // Advance integral part (mandatory)
    // int = 1*DIGIT
    ////////////////////////////////////////////////////////////////////////////
    while (p != last && is_digit(*p)) {
        numstr.push_back(*p);
        ++p;
    }

    // No digit found
    if (p == last_pos)
        return false;

    last_pos = p;

    ////////////////////////////////////////////////////////////////////////////
    // Advance fractional part (optional)
    // frac = "." 1*DIGIT
    ////////////////////////////////////////////////////////////////////////////
    if (p != last) {
        if (*p == '.') {
            numstr.push_back(locale_decimal_point());

            ++p;

            if (p == last)
                return false;

            if (!is_digit(*p))
                return false;

            numstr.push_back(*p);
            ++p;

            while (p != last && is_digit(*p)) {
                numstr.push_back(*p);
                ++p;
            }
        }
    }

    last_pos = p;

    double n = 0;

    if (!strtoreal(n, numstr))
        return false;

    if (pnum)
        *pnum = n;

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_coordinates
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by coordinates.
 *
 * @note Grammar
 * coordinates = coord-a "," coord-b [ "," coord-c ]
 */
template <typename _ForwardIterator, typename _UserContext>
bool advance_coordinates (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    auto p = pos;

    if (p == last)
        return false;

    typename simple_api_interface<_UserContext>::number_type coord;

    if (!advance_number(p, last, true, & coord))
        return false;

    context.on_latitude(std::move(coord));

    if (p == last)
        return false;

    if (*p != ',')
        return false;

    ++p;

    if (!advance_number(p, last, true, & coord))
        return false;

    context.on_longitude(std::move(coord));

    // Accepted short list
    if (p == last || *p != ',') {
        return compare_and_assign(pos, p);
    }

    ++p;

    if (!advance_number(p, last, true, & coord))
        return false;

    context.on_altitude(std::move(coord));

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_labeltext
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by `labeltext`.
 *
 * @note Grammar
 * labeltext = 1*( alphanum / "-" )
 * alphanum  = ALPHA / DIGIT
 */
template <typename _ForwardIterator, typename _StringType>
inline bool advance_labeltext (_ForwardIterator & pos, _ForwardIterator last
    , bool lowercase
    , _StringType * labeltext)
{
    auto p = pos;

    if (p == last)
        return false;

    if (!(*p == '-' || is_alpha(*p) || is_digit(*p)))
        return false;

    if (labeltext) {
        if (lowercase)
            labeltext->push_back(to_lower(*p));
        else
            labeltext->push_back(*p);
    }

    ++p;

    while (p != last && (*p == '-' || is_alpha(*p) || is_digit(*p))) {
        if (labeltext) {
            if (lowercase)
                labeltext->push_back(to_lower(*p));
            else
                labeltext->push_back(*p);
        }
        ++p;
    }

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_crsp
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by geo crsp.
 *
 * @note Grammar
 * crsp     = ";crs=" crslabel
 * crslabel = "wgs84" / labeltext
 */
template <typename _ForwardIterator, typename _UserContext>
inline bool advance_crsp (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    using string_type = typename simple_api_interface<_UserContext>::string_type;
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    auto p = pos;

    if (p == last)
        return false;

    std::array<char_type, 5> prefix {';', 'c', 'r', 's', '='};

    if (!advance_sequence_ignorecase(p, last, std::begin(prefix), std::end(prefix)))
        return false;

    std::array<char_type, 5> wgs84 {'w', 'g', 's', '8', '4'};

    if (advance_sequence_ignorecase(p, last, std::begin(wgs84), std::end(wgs84))) {
        context.on_crslabel(string_type{std::begin(wgs84), std::end(wgs84)});
    } else {
        string_type crslabel;

        auto lowercase = context.policy.test(lowercase_labeltext);

        if (!advance_labeltext(p, last, lowercase, & crslabel))
            return false;

        context.on_crslabel(std::move(crslabel));
    }

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_uncp
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by geo `uncp`.
 *
 * @note Grammar
 * uncp = ";u=" uval
 * uval = pnum ; positive number
 */
template <typename _ForwardIterator, typename _UserContext>
inline bool advance_uncp (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    using number_type = typename simple_api_interface<_UserContext>::number_type;
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    auto p = pos;

    if (p == last)
        return false;

    std::array<char_type, 3> prefix {';', 'u', '='};

    if (!advance_sequence_ignorecase(p, last, std::begin(prefix), std::end(prefix)))
        return false;

    number_type uval;

    if (!advance_number(p, last, false, & uval))
        return false;

    context.on_uval(std::move(uval));

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_pvalue
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by parameter's value.
 *
 * @note Grammar
 * pvalue        = 1*paramchar
 * paramchar     = p-unreserved / unreserved / pct-encoded
 * p-unreserved  = "[" / "]" / ":" / "&" / "+" / "$"
 * unreserved    = alphanum / mark
 * pct-encoded   = "%" HEXDIG HEXDIG
 * mark          = "-" / "_" / "." / "!" / "~" / "*" /
 *                 "'" / "(" / ")"
 */
template <typename _ForwardIterator, typename _StringType>
inline bool advance_pvalue (_ForwardIterator & pos, _ForwardIterator last
        , _StringType * pvalue = nullptr)
{
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    auto p = pos;

    if (p == last)
        return false;

    int16_t encoded_char;

    while (p != last) {
        if (is_p_unreserved(*p) || is_unreserved(*p)) {
            if (pvalue)
                pvalue->push_back(*p);
            ++p;
        } else if (advance_pct_encoded(p, last, & encoded_char)) {
            if (pvalue)
                pvalue->push_back(static_cast<char_type>(encoded_char));
        } else {
            break;
        }
    }

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_parameter
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by parameter.
 *
 * @note Grammar
 * parameter = ";" pname [ "=" pvalue ]
 */
template <typename _ForwardIterator, typename _UserContext>
inline bool advance_parameter (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    using string_type = typename simple_api_interface<_UserContext>::string_type;

    auto p = pos;

    if (p == last)
        return false;

    if (*p != ';')
        return false;

    ++p;

    if (p == last)
        return false;

    string_type pname;

    auto lowercase = context.policy.test(lowercase_labeltext);

    if (!advance_labeltext(p, last, lowercase, & pname))
        return false;

    // pvalue is optional
    if (p == last || *p != '=') {
        context.on_parameter(std::move(pname), string_type{});
        return compare_and_assign(pos, p);
    }

    ++p;

    string_type pvalue;

    if (!advance_pvalue(p, last, & pvalue))
        return false;

    context.on_parameter(std::move(pname), std::move(pvalue));

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_p
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by geo extra properties.
 *
 * @note Grammar
 * p  = [ crsp ] [ uncp ] *parameter
 */
template <typename _ForwardIterator, typename _UserContext>
inline bool advance_p (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    if (pos != last) {
        // Optional
        advance_crsp(pos, last, context);

        // Optional
        advance_uncp(pos, last, context);

        // Optional
        while (pos != last)
            advance_parameter(pos, last, context);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// advance_geo_path
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by geo path.
 *
 * @note Grammar
 * geo-path = coordinates p
 */
template <typename _ForwardIterator, typename _UserContext>
bool advance_geo_path (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    auto p = pos;

    if (!advance_coordinates(p, last, context))
        return false;

    if (!advance_p(p, last, context))
        return false;

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_geo_uri
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by geo URI.
 *
 * @note Grammar
 * geo-URI = geo-scheme ":" geo-path
 */
template <typename _ForwardIterator, typename _UserContext>
bool advance_geo_uri (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context)
{
    auto p = pos;

    if (!advance_geo_scheme(p, last))
        return false;

    if (p == last)
        return false;

    if (*p != ':')
        return false;

    ++p;

    if (!advance_geo_path(p, last, context))
        return false;

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// parse
////////////////////////////////////////////////////////////////////////////////
template <typename _ForwardIterator, typename _UserContext>
inline auto parse (_ForwardIterator first
        , _ForwardIterator last
        , simple_api_interface<_UserContext> & context) -> _ForwardIterator
{
    auto pos = first;

    if (advance_geo_uri(pos, last, context))
        return pos;

    return first;
}

////////////////////////////////////////////////////////////////////////////////
// make_context
////////////////////////////////////////////////////////////////////////////////
/**
 * Makes a context for parsing.
 *
 * @param uri Geo URI to store parsed data.
 * @param policy Parsing policy.
 */
template <typename _GeoUri>
simple_api_interface<_GeoUri> make_context (_GeoUri & uri
    , parse_policy_set const & policy = strict_policy())
{
    using number_type = typename _GeoUri::number_type;
    using string_type = typename _GeoUri::string_type;

    simple_api_interface<_GeoUri> ctx(uri);
    ctx.policy = policy;

    ctx.on_latitude = [& uri] (number_type && n) {
        uri.set_latitude(n);
    };

    ctx.on_longitude = [& uri] (number_type && n) {
        uri.set_longitude(n);
    };

    ctx.on_altitude = [& uri] (number_type && n) {
        uri.set_altitude(n);
    };

    ctx.on_crslabel = [& uri] (string_type && s) {
        uri.set_crs(std::forward<string_type>(s));
    };

    ctx.on_uval = [& uri] (number_type && n) {
        uri.set_uncertainty(n);
    };

    ctx.on_parameter = [& uri] (string_type && key, string_type && value) {
        uri.insert(std::forward<string_type>(key), std::forward<string_type>(value));
    };

    return ctx;
}

////////////////////////////////////////////////////////////////////////////////
// like_geo_uri
////////////////////////////////////////////////////////////////////////////////
/**
 * Attempt to predict if string may be a geo URI representation.
 *
 * @param first Start position of sequence for upcoming parsing of geo URI.
 * @param last  End position of sequence for upcoming parsing of geo URI.
 */
template <typename _ForwardIterator>
inline bool like_geo_uri (_ForwardIterator first, _ForwardIterator last)
{
    return advance_geo_scheme(first, last);
}

/**
 * Attempt to predict if string may be a geo URI representation.
 *
 * @param s String for upcoming parsing of geo URI.
 */
template <typename _StringType>
inline bool like_geo_uri (_StringType const & s)
{
    return like_geo_uri(std::begin(s), std::end(s));
}

}} // // namespace pfs::rfc5870
