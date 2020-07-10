////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.10 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "error.hpp"
// #include <algorithm>
#include <array>
// #include <bitset>
#include <functional>
// #include <iterator>
// #include <memory>
#include <string>
// #include <type_traits>
// #include <utility>
#include <cassert>
#include <locale>
// #include <cstdlib>

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

////////////////////////////////////////////////////////////////////////////////
// Simple API interface
////////////////////////////////////////////////////////////////////////////////
template <typename _UserContext>
class simple_api_interface: public _UserContext
{
public:
    using number_type = typename _UserContext::number_type;
    using string_type = typename _UserContext::string_type;

    std::function<void(_UserContext &, number_type &&)> on_latitude
        = [] (_UserContext &, number_type &&) {};

    std::function<void(_UserContext &, number_type &&)> on_longitude
        = [] (_UserContext &, number_type &&) {};

    std::function<void(_UserContext &, number_type &&)> on_altitude
        = [] (_UserContext &, number_type &&) {};

    std::function<void(_UserContext &, string_type &&)> on_crslabel
        = [] (_UserContext &, string_type &&) {};

    std::function<void(_UserContext &, number_type &&)> on_uval
        = [] (_UserContext &, number_type &&) {};

    std::function<void(_UserContext &, string_type &&, string_type &&)> on_parameter
        = [] (_UserContext &, string_type &&, string_type &&) {};
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
// is_whitespace
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is one of the symbols:
 *      - space (%x20),
 *      - horizontal tab (%x09),
 *      - line feed or new line (%x0A),
 *      - carriage return (%x0D),
 *
 *      otherwise @c false.
 */
// template <typename CharT>
// inline bool is_whitespace (CharT ch)
// {
//     return (ch == CharT('\x20')
//             || ch == CharT('\x09')
//             || ch == CharT('\x0A')
//             || ch == CharT('\x0D'));
// }

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
// is_digit
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is an alpha decimal digit (A-Z / a-z),
 *      otherwise @c false.
 */
template <typename CharT>
inline bool is_alpha (CharT ch)
{
    return ((ch >= CharT('\x41') && ch <= CharT('\x5A'))
            || (ch >= CharT('\x61') && ch <= CharT('\x7A')));
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
// is_unreserved
////////////////////////////////////////////////////////////////////////////////
/**
 * @return @c true if character is one of the characters:
 *      alphanum or mark ("-" / "_" / "." / "!" / "~" / "*" / "'" / "(" / ")")
 */
template <typename _CharT>
inline bool is_unreserved (_CharT ch)
{
    return (is_alpha(ch)
            || is_digit(ch)
            || ch == _CharT('-')
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

    _ForwardIterator p = pos;
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
    _ForwardIterator1 p = pos;

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
        , _NumberType * pnum
        , error_code & ec)
{
    _ForwardIterator p = pos;

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

    if (!strtoreal(n, numstr)) {
        ec = make_error_code(errc::bad_number);
        return false;
    }

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
        , simple_api_interface<_UserContext> & context
        , error_code & ec)
{
    _ForwardIterator p = pos;

    if (p == last)
        return false;

    typename simple_api_interface<_UserContext>::number_type coord;

    if (!advance_number(p, last, true, & coord, ec))
        return false;

    context.on_latitude(context, std::move(coord));

    if (p == last)
        return false;

    if (*p != ',')
        return false;

    ++p;

    if (!advance_number(p, last, true, & coord, ec))
        return false;

    context.on_longitude(context, std::move(coord));

    // Accepted short list
    if (p == last)
        return true;

    // Accepted short list
    if (*p != ',')
        return true;

    ++p;

    if (!advance_number(p, last, true, & coord, ec))
        return false;

    context.on_altitude(context, std::move(coord));

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
bool advance_labeltext (_ForwardIterator & pos, _ForwardIterator last
        , _StringType * labeltext = nullptr)
{
    _ForwardIterator p = pos;

    if (p == last)
        return false;

    if (!(*p == '-' || is_alpha(*p) || is_digit(*p)))
        return false;

    ++p;

    while (p != last && (*p == '-' || is_alpha(*p) || is_digit(*p)))
        ++p;

    if (labeltext)
        *labeltext = _StringType{pos, p};

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
        , simple_api_interface<_UserContext> & context
        , error_code & /*ec*/)
{
    using string_type = typename simple_api_interface<_UserContext>::string_type;
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    _ForwardIterator p = pos;

    if (p == last)
        return false;

    std::array<char_type, 5> prefix {';', 'c', 'r', 's', '='};

    if (!advance_sequence_ignorecase(p, last, std::begin(prefix), std::end(prefix)))
        return false;

    std::array<char_type, 5> wgs84 {'w', 'g', 's', '8', '4'};

    if (advance_sequence_ignorecase(p, last, std::begin(wgs84), std::end(wgs84))) {
        context.on_crslabel(context
            , string_type{std::begin(wgs84), std::end(wgs84)});
    } else {
        string_type crslabel;

        if (!advance_labeltext(p, last, & crslabel))
            return false;

        context.on_crslabel(context, std::move(crslabel));
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
        , simple_api_interface<_UserContext> & context
        , error_code & ec)
{
    using number_type = typename simple_api_interface<_UserContext>::number_type;
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    _ForwardIterator p = pos;

    if (p == last)
        return false;

    std::array<char_type, 3> prefix {';', 'u', '='};

    if (!advance_sequence_ignorecase(p, last, std::begin(prefix), std::end(prefix)))
        return false;

    number_type uval;

    if (!advance_number(p, last, false, & uval, ec))
        return false;

    context.on_uval(context, std::move(uval));

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
    _ForwardIterator p = pos;

    if (p == last)
        return false;

    auto last_pos = p;

    while (p != last) {
        if (is_p_unreserved(*p) || is_unreserved(*p)) {
            ++p;
        } else if (*p == '%') {
            ++p;

            if (!advance_pct_encoded(p, last))
                return false;
        } else {
            break;
        }
    }

    if (pvalue)
        *pvalue = _StringType{last_pos, p};

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
// advance_parameter
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Advance by geo `uncp`.
 *
 * @note Grammar
 * parameter     = ";" pname [ "=" pvalue ]
 * pname         = labeltext
 * pvalue        = 1*paramchar
 * paramchar     = p-unreserved / unreserved / pct-encoded
 * p-unreserved  = "[" / "]" / ":" / "&" / "+" / "$"
 * unreserved    = alphanum / mark
 * pct-encoded   = "%" HEXDIG HEXDIG
 * mark          = "-" / "_" / "." / "!" / "~" / "*" /
 *                 "'" / "(" / ")"
 */
template <typename _ForwardIterator, typename _UserContext>
inline bool advance_parameter (_ForwardIterator & pos, _ForwardIterator last
        , simple_api_interface<_UserContext> & context
        , error_code & ec)
{
    using string_type = typename simple_api_interface<_UserContext>::string_type;
    using char_type = typename std::remove_reference<decltype(*pos)>::type;

    _ForwardIterator p = pos;

    if (p == last)
        return false;

    if (*p != ';')
        return false;

    ++p;

    if (p == last)
        return false;

    string_type pname;

    if (!advance_labeltext(p, last, & pname))
        return false;

    if (p == last)
        return false;

    if (*p != '=')
        return false;

    ++p;

    if (p == last)
        return false;

    auto last_pos = p;

    string_type pvalue;

    while (p != last) {

    }

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
        , simple_api_interface<_UserContext> & context
        , error_code & ec)
{
    // Optional
    advance_crsp(pos, last, context, ec);

    // Optional
    advance_uncp(pos, last, context, ec);

    // Optional
    while (pos != last)
        advance_parameter(pos, last, context, ec);

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
        , simple_api_interface<_UserContext> & context
        , error_code & ec)
{
    _ForwardIterator p = pos;

    if (!advance_coordinates(p, last, context, ec))
        return false;

    if (!advance_p(p, last, context, ec))
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
        , simple_api_interface<_UserContext> & context
        , error_code & ec)
{
    _ForwardIterator p = pos;

    if (!advance_geo_scheme(p, last))
        return false;

    if (p == last)
        return false;

    if (p != ':')
        return false;

    ++p;

    if (!advance_geo_path(p, last, context, ec))
        return false;

    return compare_and_assign(pos, p);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
template <typename _ForwardIterator, typename _SimpleApiContext>
inline _ForwardIterator parse (_ForwardIterator first
        , _ForwardIterator last
        , _SimpleApiContext context)
{
    _ForwardIterator pos = first;

    if (advance_geo_uri(pos, last, context))
        return pos;

    return first;
}

////////////////////////////////////////////////////////////////////////////////
// parse_array
////////////////////////////////////////////////////////////////////////////////
// template <typename ForwardIterator, typename ArrayType>
// typename std::enable_if<std::is_arithmetic<typename ArrayType::value_type>::value, ForwardIterator>::type
// parse_array (ForwardIterator first
//         , ForwardIterator last
//         , parse_policy_set const & parse_policy
//         , ArrayType & arr
//         , error_code & ec)
// {
//     using value_type = typename ArrayType::value_type;
//     //                   v----------No matter the string type here
//     basic_callbacks<std::string, value_type> callbacks;
//     callbacks.on_error  = [& ec] (error_code const & e) { ec = e; };
//     callbacks.on_true   = [& arr] { arr.emplace_back(static_cast<value_type>(true)); };
//     callbacks.on_false  = [& arr] { arr.emplace_back(static_cast<value_type>(false)); };
//     callbacks.on_number = [& arr] (value_type && n) { arr.emplace_back(std::forward<value_type>(n)); };
//     return parse(first, last, parse_policy, callbacks);
// }
//
// template <typename StringType>
// struct is_string;
//
// template <>
// struct is_string<std::string> : std::integral_constant<bool, true> {};
//
// template <typename ForwardIterator, typename ArrayType>
// typename std::enable_if<is_string<typename ArrayType::value_type>::value, ForwardIterator>::type
// parse_array (ForwardIterator first
//         , ForwardIterator last
//         , parse_policy_set const & parse_policy
//         , ArrayType & arr
//         , error_code & ec)
// {
//     using string_type = typename ArrayType::value_type;
//     //                            v------------ No matter the number type here
//     basic_callbacks<string_type, int> callbacks;
//     callbacks.on_error  = [& ec] (error_code const & e) { ec = e; };
//     callbacks.on_string = [& arr] (string_type && s) {
//         arr.emplace_back(std::forward<string_type>(s));
//     };
//     return parse(first, last, parse_policy, callbacks);
// }
//
// template <typename ForwardIterator, typename ArrayType>
// inline ForwardIterator parse_array (ForwardIterator first
//         , ForwardIterator last
//         , ArrayType & arr
//         , error_code & ec)
// {
//     return parse_array(first, last, default_policy(), arr, ec);
// }
//
// template <typename ForwardIterator, typename ArrayType>
// inline ForwardIterator parse_array (ForwardIterator first
//         , ForwardIterator last
//         , ArrayType & arr)
// {
//     error_code ec;
//     auto pos = parse_array(first, last, arr, ec);
//     if (ec)
//         throw std::system_error(ec);
//     return pos;
// }

////////////////////////////////////////////////////////////////////////////////
// parse_object
////////////////////////////////////////////////////////////////////////////////
// template <typename ForwardIterator, typename ObjectType>
// typename std::enable_if<is_string<typename ObjectType::key_type>::value
//         && std::is_arithmetic<typename ObjectType::mapped_type>::value, ForwardIterator>::type
// parse_object (ForwardIterator first
//         , ForwardIterator last
//         , parse_policy_set const & parse_policy
//         , ObjectType & obj
//         , error_code & ec)
// {
//     using value_type = typename ObjectType::mapped_type;
//     using string_type = typename ObjectType::key_type;
//
//     basic_callbacks<string_type, value_type> callbacks;
//     string_type member_name;
//
//     callbacks.on_error  = [& ec] (error_code const & e) { ec = e; };
//     callbacks.on_member_name  = [& member_name] (string_type && name) { member_name = std::move(name); };
//     callbacks.on_true   = [& obj, & member_name] { obj[member_name] = true; };
//     callbacks.on_false  = [& obj, & member_name] { obj[member_name] = false; };
//     callbacks.on_number = [& obj, & member_name] (value_type && n) { obj[member_name] = std::forward<value_type>(n); };
//     return parse(first, last, parse_policy, callbacks);
// }
//
// template <typename ForwardIterator, typename ObjectType>
// typename std::enable_if<is_string<typename ObjectType::key_type>::value
//         && is_string<typename ObjectType::mapped_type>::value, ForwardIterator>::type
// parse_object (ForwardIterator first
//         , ForwardIterator last
//         , parse_policy_set const & parse_policy
//         , ObjectType & obj
//         , error_code & ec)
// {
//     using value_type = typename ObjectType::mapped_type;
//     using string_type = typename ObjectType::key_type;
//
//     basic_callbacks<string_type, value_type> callbacks;
//     string_type member_name;
//
//     callbacks.on_error  = [& ec] (error_code const & e) { ec = e; };
//     callbacks.on_member_name  = [& member_name] (string_type && name) { member_name = std::move(name); };
//     callbacks.on_string = [& obj, & member_name] (string_type && s) {
//             obj[member_name] = std::forward<string_type>(s);
//     };
//     return parse(first, last, parse_policy, callbacks);
// }
//
// template <typename ForwardIterator, typename ObjectType>
// inline ForwardIterator parse_object (ForwardIterator first
//         , ForwardIterator last
//         , ObjectType & obj
//         , error_code & ec)
// {
//     return parse_object(first, last, default_policy(), obj, ec);
// }
//
// template <typename ForwardIterator, typename ObjectType>
// inline ForwardIterator parse_object (ForwardIterator first
//         , ForwardIterator last
//         , ObjectType & obj)
// {
//     error_code ec;
//     auto pos = parse_object(first, last, obj, ec);
//     if (ec)
//         throw std::system_error(ec);
//     return pos;
// }

}} // // namespace pfs::rfc5870
