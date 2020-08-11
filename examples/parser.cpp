////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-uri-geo](https://github.com/semenovf/pfs-uri-geo) library.
//
// Changelog:
//      2020.07.14 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/uri/geo/parser.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using std::cout;
namespace geo = pfs::uri::geo;

int main ()
{
    std::string s{R"(geo:66,30;u=6.500;FOo=this%2dthat;Bar)"};

    // Predict if `s` is geo URI representation
    if (!geo::like_geo_uri(s))
        return EXIT_FAILURE;

    geo::uri uri;
    auto ctx = geo::make_context(uri);
    auto pos = std::begin(s);
    auto it = geo::parse(pos, std::end(s), ctx);

    // Bad geo URI string representation
    if (it == std::begin(s))
        return EXIT_FAILURE;

    cout << uri.latitude() << "\n"                          // 66
         << uri.longitude() << "\n"                         // 30
         << std::boolalpha << uri.has_altitude() << "\n"    // false
         << std::boolalpha << uri.is_wgs84() << "\n"        // true
         << uri.crs() << "\n"                               // wgs84
         << std::boolalpha << uri.has_uncertainty() << "\n" // true
         << uri.uncertainty() << "\n"                       // 6.5
         << uri.count() << "\n"                             // 2

         // By default parse policy, parameter names converted to lowercase
         << std::boolalpha << uri.has_parameter("foo") << "\n" // true
         << std::boolalpha << uri.has_parameter("bar") << "\n" // true
         << std::boolalpha << uri.has_parameter("baz") << "\n" // false

         // By default parse policy, parameter value is decoded
         << uri.parameter("foo") << "\n"                          // this-that
         << std::boolalpha << uri.parameter("bar").empty()<< "\n";// true

    return EXIT_SUCCESS;
}
