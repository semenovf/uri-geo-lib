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

#if defined(_WIN32) || defined(_WIN64)
#   include <string.h>
#else
#   include <strings.h>
#endif

int compare_ignore_case (std::string const & a, std::string const & b)
{
#if defined(_WIN32) || defined(_WIN64)
    return _stricmp(a.c_str(), b.c_str());
#else
    return strcasecmp(a.c_str(), b.c_str());
#endif
}

namespace geo = pfs::rfc5870;

template <typename T>
inline constexpr T * nullptr_value ()
{
    return static_cast<T *>(nullptr);
}

struct geo_context
{
    using number_type = double;
    using string_type = std::string;

    number_type latitude {0};
    number_type longitude {0};
    number_type altitude {0};
    string_type crslabel;
    double uval {0};
    std::map<string_type, string_type> parameters;
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
        ctx.parameters.insert(std::make_pair(std::forward<std::string>(key)
            , std::forward<std::string>(value)));
    };

    return ctx;
}

TEST_CASE("advance_pct_encoded") {
    {
        std::string s{R"(%42)"};
        auto pos = std::begin(s);
        int16_t result = 0;
        CHECK(geo::advance_pct_encoded(pos, std::end(s), & result));
        CHECK(result == 0x42);
        CHECK(pos == std::end(s));
    }

    {
        std::string s{R"(%42x)"};
        auto pos = std::begin(s);
        int16_t result = 0;
        CHECK(geo::advance_pct_encoded(pos, std::end(s), & result));
        CHECK(result == 0x42);
        CHECK(pos == (std::begin(s) + 3));
    }

    // Empty
    {
        std::string s{R"()"};
        auto pos = std::begin(s);
        CHECK_FALSE(geo::advance_pct_encoded(pos, std::end(s)));
    }

    // Incomplete
    {
        std::string s{R"(%)"};
        auto pos = std::begin(s);
        CHECK_FALSE(geo::advance_pct_encoded(pos, std::end(s)));
    }

    // Incomplete
    {
        std::string s{R"(%1)"};
        auto pos = std::begin(s);
        CHECK_FALSE(geo::advance_pct_encoded(pos, std::end(s)));
    }

    // Expected hexdigit
    {
        std::string s{R"(%1x)"};
        auto pos = std::begin(s);
        CHECK_FALSE(geo::advance_pct_encoded(pos, std::end(s)));
    }
}

TEST_CASE("advance_sequence_ignorecase") {
    {
        std::string sample{R"(Lorem Ipsum Dolor Sit Amet)"};
        std::string s{R"(lorem ipsum dolor sit amet)"};
        auto pos1 = std::begin(sample);
        auto pos2 = std::begin(s);
        CHECK(geo::advance_sequence_ignorecase(pos1, std::end(sample)
            , pos2, std::end(s)));
        CHECK(pos1 == std::begin(sample) + sample.size());
    }
}

TEST_CASE("advance_geo_scheme") {
    {
        std::string s{R"(geo)"};
        auto pos = std::begin(s);
        CHECK(geo::advance_geo_scheme(pos, std::end(s)));
        CHECK(pos == std::end(s));
    }

    {
        std::string s{R"(GEO)"};
        auto pos = std::begin(s);
        CHECK(geo::advance_geo_scheme(pos, std::end(s)));
        CHECK(pos == std::end(s));
    }
}

TEST_CASE("advance_number") {
    {
        double n;
        std::string num{R"()"};
        auto pos = std::begin(num);
        CHECK_FALSE(geo::advance_number(pos, std::end(num), true, & n));
    }

    {
        double n;
        std::string num{R"(123.)"};
        auto pos = std::begin(num);
        CHECK_FALSE(geo::advance_number(pos, std::end(num), true, & n));
    }

    {
        double n;
        std::string num{R"(.456)"};
        auto pos = std::begin(num);
        CHECK_FALSE(geo::advance_number(pos, std::end(num), true, & n));
    }

    {
        double n;
        std::string num{R"(123)"};
        auto pos = std::begin(num);
        CHECK(geo::advance_number(pos, std::end(num), true, & n));
        CHECK(std::stod("123") == n);
    }

    {
        double n;
        std::string num{R"(123.456)"};
        auto pos = std::begin(num);
        CHECK(geo::advance_number(pos, std::end(num), true, & n));
        CHECK(std::stod("123.456") == n);
    }
}

TEST_CASE("advance_coordinates") {
    {
        std::string sample{R"(48.2010,16.3695,183)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_coordinates(pos, std::end(sample), ctx));
        CHECK(ctx.latitude  == std::stod("48.2010"));
        CHECK(ctx.longitude == std::stod("16.3695"));
        CHECK(ctx.altitude  == std::stod("183"));
    }

    {
        std::string sample{R"(48.2010,16.3695)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_coordinates(pos, std::end(sample), ctx));
        CHECK(ctx.latitude  == std::stod("48.2010"));
        CHECK(ctx.longitude == std::stod("16.3695"));
        CHECK(ctx.altitude  == std::stod("0"));
    }

    {
        std::string sample{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(48.2010)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(48.2010,)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(48.2010,16.3695,)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }
}

TEST_CASE("advance_labeltext") {
    {
        std::string s{R"(label-text)"};
        auto pos = std::begin(s);
        std::string result;

        CHECK(geo::advance_labeltext(pos, std::end(s), false, & result));
        CHECK(result == s);
        CHECK(pos == std::end(s));
    }

    {
        std::string s{R"(label-text?)"};
        auto pos = std::begin(s);
        std::string result;

        CHECK(geo::advance_labeltext(pos, std::end(s), false, & result));
        CHECK(result == "label-text");
        CHECK(pos == std::begin(s) + s.size() - 1);
    }

    // Empty
    {
        std::string s{R"()"};
        auto pos = std::begin(s);
        CHECK_FALSE(geo::advance_labeltext(pos, std::end(s), false, nullptr_value<std::string>()));
    }

    // Invalid character
    {
        std::string s{R"(?)"};
        auto pos = std::begin(s);
        CHECK_FALSE(geo::advance_labeltext(pos, std::end(s), false, nullptr_value<std::string>()));
    }
}

TEST_CASE("advance_crsp") {
    {
        std::string sample{R"(;crs=wgs84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx));
        CHECK(ctx.crslabel == "wgs84");
    }

    {
        std::string sample{R"(;crs=WgS84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx));
        CHECK(ctx.crslabel == "wgs84");
    }

    {
        std::string sample{R"(;crs=LABEL)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx));
        CHECK(ctx.crslabel == "label");
    }

    // Empty
    {
        std::string sample{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx));
    }

    //
    {
        std::string sample{R"(crs=wgs84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx));
    }

    // Bad crslabel
    {
        std::string sample{R"(;crs=+wgs84)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx));
    }
}

TEST_CASE("advance_uncp") {
    {
        std::string sample{R"(;u=123.456)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK(geo::advance_uncp(pos, std::end(sample), ctx));
        CHECK(ctx.uval == std::stod("123.456"));
    }

    {
        std::string sample{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(;u=)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(u=123.456)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(u=123.)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }
}

TEST_CASE("advance_pvalue") {
    {
        std::string s{R"(a)"};
        auto pos = std::begin(s);
        std::string pvalue;

        CHECK(geo::advance_pvalue(pos, std::end(s), & pvalue));
        CHECK(pvalue == s);
    }

    {
        std::string s{R"([]:&+$-_.!~*'()X9)"};
        auto pos = std::begin(s);
        std::string pvalue;

        CHECK(geo::advance_pvalue(pos, std::end(s), & pvalue));
        CHECK(pvalue == s);
    }

    // Empty
    {
        std::string s{R"()"};
        auto pos = std::begin(s);
        std::string pvalue;

        CHECK_FALSE(geo::advance_pvalue(pos, std::end(s), & pvalue));
    }
}

TEST_CASE("advance_parameter") {
    {
        std::string s{R"(;pname)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK(geo::advance_parameter(pos, std::end(s), ctx));
        REQUIRE(ctx.parameters.size() == 1);
        CHECK(ctx.parameters.find("pname") != ctx.parameters.end());
        CHECK(ctx.parameters["pname"] == "");
    }

    {
        std::string s{R"(;pname=pvalue)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK(geo::advance_parameter(pos, std::end(s), ctx));
        REQUIRE(ctx.parameters.size() == 1);
        CHECK(ctx.parameters.find("pname") != ctx.parameters.end());
        CHECK(ctx.parameters["pname"] == "pvalue");
    }

    // Empty
    {
        std::string s{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK_FALSE(geo::advance_parameter(pos, std::end(s), ctx));
    }
}

TEST_CASE("advance_p") {
    {
        std::string s{R"(;pname1=pvalue1;pname2=pvalue2)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK(geo::advance_p(pos, std::end(s), ctx));
        REQUIRE(ctx.parameters.size()  == 2);
        CHECK(ctx.parameters["pname1"] == "pvalue1");
        CHECK(ctx.parameters["pname2"] == "pvalue2");
        CHECK(pos == std::end(s));
    }

    {
        std::string s{R"()"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);
        CHECK(geo::advance_p(pos, std::end(s), ctx));
    }
}

TEST_CASE("advance_geo_uri") {
    {
        std::string s{R"(geo:13.4125,103.8667)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK(geo::advance_geo_uri(pos, std::end(s), ctx));
        CHECK(pos == std::end(s));
        CHECK(ctx.latitude == std::stod("13.4125"));
        CHECK(ctx.longitude == std::stod("103.8667"));
    }

    {
        std::string s{R"(geo:48.2010,-16.3695,183)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK(geo::advance_geo_uri(pos, std::end(s), ctx));
        CHECK(pos == std::end(s));
        CHECK(ctx.latitude  == std::stod("48.2010"));
        CHECK(ctx.longitude == std::stod("-16.3695"));
        CHECK(ctx.altitude  == std::stod("183"));
    }

    {
        std::string s{R"(geo:48.198634,16.371648;crs=wgs84;u=40)"};
        auto ctx = make_geo_context();
        auto pos = std::begin(s);

        CHECK(geo::advance_geo_uri(pos, std::end(s), ctx));
        CHECK(pos == std::end(s));
        CHECK(ctx.latitude  == std::stod("48.198634"));
        CHECK(ctx.longitude == std::stod("16.371648"));
        CHECK(ctx.crslabel  == "wgs84");
        CHECK(ctx.uval      == std::stod("40"));
    }

    {
        std::string s1{R"(geo:66,30;u=6.500;FOo=this%2dthat;Bar)"};
        std::string s2{R"(geo:66.0,30;u=6.5;foo=this-that;bar)"};

        auto ctx1 = make_geo_context();
        auto ctx2 = make_geo_context();
        auto pos1 = std::begin(s1);
        auto pos2 = std::begin(s2);

        CHECK(geo::advance_geo_uri(pos1, std::end(s1), ctx1));
        CHECK(geo::advance_geo_uri(pos2, std::end(s2), ctx2));

        CHECK(pos1 == std::end(s1));
        CHECK(pos2 == std::end(s2));

        CHECK(ctx1.latitude  == ctx2.latitude);
        CHECK(ctx1.longitude == ctx2.longitude);
        CHECK(ctx1.uval == std::stod("6.5"));
        CHECK(ctx1.uval == ctx2.uval);

        REQUIRE(ctx1.parameters.size() == 2);
        REQUIRE(ctx2.parameters.size() == 2);

        CHECK(ctx1.parameters.find("foo") != ctx1.parameters.end());
        CHECK(ctx2.parameters.find("foo") != ctx2.parameters.end());
        CHECK(ctx1.parameters["foo"] == "this-that");
        CHECK(ctx2.parameters["foo"] == "this-that");

        CHECK(ctx1.parameters.find("bar") != ctx1.parameters.end());
        CHECK(ctx2.parameters.find("bar") != ctx2.parameters.end());
        CHECK(ctx1.parameters["bar"].empty());
        CHECK(ctx2.parameters["bar"].empty());
    }
}
