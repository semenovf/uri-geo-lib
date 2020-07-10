////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.06.20 Initial version
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/rfc5870/parser.hpp"
#include <string>
#include <vector>
#include <utility>

static std::string geo_uri{R"(geo:48.2010,16.3695,183)"};

namespace geo = pfs::rfc5870;

struct geo_context
{
    using number_type = double;
    using string_type = std::string;

    number_type latitude {0};
    number_type longitude {0};
    number_type altitude {0};
    string_type crslabel;
    double uval {0};
    std::vector<std::pair<string_type, string_type>> parameters;
};

geo::simple_api_interface<geo_context> make_geo_context ()
{
    geo::simple_api_interface<geo_context> ctx;
    ctx.on_latitude  = [] (geo_context & ctx, double && n) { ctx.latitude  = n; };
    ctx.on_longitude = [] (geo_context & ctx, double && n) { ctx.longitude = n; };
    ctx.on_altitude  = [] (geo_context & ctx, double && n) { ctx.altitude  = n; };
    ctx.on_crslabel  = [] (geo_context & ctx, std::string && s) {
        ctx.crslabel = std::forward<std::string>(s);
    };
    ctx.on_uval= [] (geo_context & ctx, double && n) {
        ctx.uval = n;
    };
    ctx.on_parameter = [] (geo_context & ctx, std::string && key, std::string && value) {
        ctx.parameters.push_back(std::make_pair(std::forward<std::string>(key)
            , std::forward<std::string>(value)));
    };

    return ctx;
}

TEST_CASE("advance_geo_scheme") {
    auto pos = std::begin(geo_uri);
    CHECK(geo::advance_geo_scheme(pos, std::end(geo_uri)));
}

TEST_CASE("advance_number") {
    std::error_code ec;

    {
        double n;
        std::string num{R"()"};
        auto pos = std::begin(num);
        CHECK_FALSE(geo::advance_number(pos, std::end(num), true, & n, ec));
    }

    {
        double n;
        std::string num{R"(123.)"};
        auto pos = std::begin(num);
        CHECK_FALSE(geo::advance_number(pos, std::end(num), true, & n, ec));
    }

    {
        double n;
        std::string num{R"(.456)"};
        auto pos = std::begin(num);
        CHECK_FALSE(geo::advance_number(pos, std::end(num), true, & n, ec));
    }

    {
        double n;
        std::string num{R"(123)"};
        auto pos = std::begin(num);
        CHECK(geo::advance_number(pos, std::end(num), true, & n, ec));
        CHECK(std::stod("123") == n);
    }

    {
        double n;
        std::string num{R"(123.456)"};
        auto pos = std::begin(num);
        CHECK(geo::advance_number(pos, std::end(num), true, & n, ec));
        CHECK(std::stod("123.456") == n);
    }
}

TEST_CASE("advance_coordinates") {
    std::error_code ec;

    {
        std::string sample{R"(48.2010,16.3695,183)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_coordinates(pos, std::end(sample), ctx, ec));
        CHECK(ctx.latitude  == std::stod("48.2010"));
        CHECK(ctx.longitude == std::stod("16.3695"));
        CHECK(ctx.altitude  == std::stod("183"));
    }

    {
        std::string sample{R"(48.2010,16.3695)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_coordinates(pos, std::end(sample), ctx, ec));
        CHECK(ctx.latitude  == std::stod("48.2010"));
        CHECK(ctx.longitude == std::stod("16.3695"));
        CHECK(ctx.altitude  == std::stod("0"));
    }

    {
        std::string sample{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx, ec));
    }

    {
        std::string sample{R"(48.2010)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx, ec));
    }

    {
        std::string sample{R"(48.2010,)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx, ec));
    }

    {
        std::string sample{R"(48.2010,16.3695,)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx, ec));
    }
}

TEST_CASE("advance_crsp") {
    std::error_code ec;

    {
        std::string sample{R"(;crs=wgs84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx, ec));
        CHECK(ctx.crslabel == "wgs84");
    }

    {
        std::string sample{R"(;crs=WgS84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx, ec));
        CHECK(ctx.crslabel == "wgs84");
    }

    {
        std::string sample{R"(;crs=LABEL)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx, ec));
        CHECK(ctx.crslabel == "LABEL");
    }

    // Empty
    {
        std::string sample{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx, ec));
    }

    //
    {
        std::string sample{R"(crs=wgs84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx, ec));
    }

    // Bad crslabel
    {
        std::string sample{R"(;crs=+wgs84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx, ec));
    }
}

TEST_CASE("advance_uncp") {
    std::error_code ec;

    {
        std::string sample{R"(;u=123.456)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_uncp(pos, std::end(sample), ctx, ec));
        CHECK(ctx.uval == std::stod("123.456"));
    }

    {
        std::string sample{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx, ec));
    }

    {
        std::string sample{R"(;u=)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx, ec));
    }

    {
        std::string sample{R"(u=123.456)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx, ec));
    }

    {
        std::string sample{R"(u=123.)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx, ec));
    }
}

TEST_CASE("advance_pvalue") {
    std::error_code ec;

    {
        std::string sample{R"(a)"};
        auto pos = std::begin(sample);
        std::string pvalue;

        CHECK(geo::advance_pvalue(pos, std::end(sample), & pvalue));
        CHECK(pvalue == "a");
    }

    {
        std::string sample{R"(a)"};
        auto pos = std::begin(sample);
        std::string pvalue;

        CHECK(geo::advance_pvalue(pos, std::end(sample), & pvalue));
        CHECK(pvalue == "a");
    }
}

// TEST_CASE("advance_parameter") {
//     std::error_code ec;
//
//     {
//         std::string geo_uri{R"(;parm1=value1)"};
//         auto ctx = make_geo_context();
//         auto pos = std::begin(geo_uri);
//
//         CHECK(geo::advance_parameter(pos, std::end(geo_uri), ctx, ec));
//         REQUIRE(ctx.parameters.size() == 1);
//         CHECK(ctx.parameters[0].first == "param1");
//         CHECK(ctx.parameters[0].second == "value1");
//     }
// }
