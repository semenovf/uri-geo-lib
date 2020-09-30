////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-uri-geo](https://github.com/semenovf/pfs-uri-geo) library.
//
// Changelog:
//      2020.07.12 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/uri/geo/composer.hpp"
#include <sstream>
#include <iostream>

namespace geo = pfs::uri::geo;

TEST_CASE("composer") {
    {
        geo::uri uri;
        uri.set_latitude(66.0f);
        uri.set_longitude(30.0f);
        uri.set_altitude(100.0f);
        uri.set_crs("ABC");
        uri.set_uncertainty(6.5f);
        uri.insert("foo", "val");
        uri.insert("bar");

        std::string sample{"geo:66,30,100;crs=ABC;u=6.5;bar;foo=val"};
        auto result = geo::compose(uri);

        CHECK(result == sample);
    }
}

TEST_CASE("poles") {
    auto north_pole = geo::north_pole<geo::uri>();
    auto south_pole = geo::south_pole<geo::uri>();

    CHECK(geo::is_north_pole(north_pole));
    CHECK(geo::is_south_pole(south_pole));

    // No matter longitude for North and South poles
    CHECK(geo::is_north_pole(geo::uri(double{90}, double{89})));
    CHECK(geo::is_south_pole(geo::uri(double{-90}, double{89})));
}
