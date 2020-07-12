////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.10 Initial version
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
    string_type crslabel {"wgs84"}; // is default CRS
    double uval {0};
    std::map<string_type, string_type> parameters;
};

geo::simple_api_interface<geo_context> make_geo_context (geo_context & c)
{
    geo::simple_api_interface<geo_context> ctx(c);
    ctx.on_latitude  = [& c] (double && n) { c.latitude  = n; };
    ctx.on_longitude = [& c] (double && n) { c.longitude = n; };
    ctx.on_altitude  = [& c] (double && n) { c.altitude  = n; };
    ctx.on_crslabel  = [& c] (std::string && s) {
        c.crslabel = std::forward<std::string>(s);
    };
    ctx.on_uval = [& c] (double && n) {
        c.uval = n;
    };
    ctx.on_parameter = [& c] (std::string && key, std::string && value) {
        c.parameters.insert(std::make_pair(std::forward<std::string>(key)
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
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK(geo::advance_coordinates(pos, std::end(sample), ctx));
        CHECK(c.latitude  == std::stod("48.2010"));
        CHECK(c.longitude == std::stod("16.3695"));
        CHECK(c.altitude  == std::stod("183"));
    }

    {
        std::string sample{R"(48.2010,16.3695)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK(geo::advance_coordinates(pos, std::end(sample), ctx));
        CHECK(c.latitude  == std::stod("48.2010"));
        CHECK(c.longitude == std::stod("16.3695"));
        CHECK(c.altitude  == std::stod("0"));
    }

    {
        std::string sample{R"()"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(48.2010)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(48.2010,)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_coordinates(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(48.2010,16.3695,)"};
        geo_context c;
        auto ctx = make_geo_context(c);
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
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx));
        CHECK(c.crslabel == "wgs84");
    }

    {
        std::string sample{R"(;crs=WgS84)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx));
        CHECK(c.crslabel == "wgs84");
    }

    {
        std::string sample{R"(;crs=LABEL)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK(geo::advance_crsp(pos, std::end(sample), ctx));
        CHECK(c.crslabel == "label");
    }

    // Empty
    {
        std::string sample{R"()"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx));
    }

    //
    {
        std::string sample{R"(crs=wgs84)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx));
    }

    // Bad crslabel
    {
        std::string sample{R"(;crs=+wgs84)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_crsp(pos, std::end(sample), ctx));
    }
}

TEST_CASE("advance_uncp") {
    {
        std::string sample{R"(;u=123.456)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK(geo::advance_uncp(pos, std::end(sample), ctx));
        CHECK(c.uval == std::stod("123.456"));
    }

    {
        std::string sample{R"()"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(;u=)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(u=123.456)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(sample);

        CHECK_FALSE(geo::advance_uncp(pos, std::end(sample), ctx));
    }

    {
        std::string sample{R"(u=123.)"};
        geo_context c;
        auto ctx = make_geo_context(c);
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
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK(geo::advance_parameter(pos, std::end(s), ctx));
        REQUIRE(c.parameters.size() == 1);
        CHECK(c.parameters.find("pname") != c.parameters.end());
        CHECK(c.parameters["pname"] == "");
    }

    {
        std::string s{R"(;pname=pvalue)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK(geo::advance_parameter(pos, std::end(s), ctx));
        REQUIRE(c.parameters.size() == 1);
        CHECK(c.parameters.find("pname") != c.parameters.end());
        CHECK(c.parameters["pname"] == "pvalue");
    }

    // Empty
    {
        std::string s{R"()"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK_FALSE(geo::advance_parameter(pos, std::end(s), ctx));
    }
}

TEST_CASE("advance_p") {
    {
        std::string s{R"(;pname1=pvalue1;pname2=pvalue2)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK(geo::advance_p(pos, std::end(s), ctx));
        REQUIRE(c.parameters.size()  == 2);
        CHECK(c.parameters["pname1"] == "pvalue1");
        CHECK(c.parameters["pname2"] == "pvalue2");
        CHECK(pos == std::end(s));
    }

    {
        std::string s{R"()"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);
        CHECK(geo::advance_p(pos, std::end(s), ctx));
    }
}

TEST_CASE("advance_geo_uri") {
    {
        std::string s{R"(geo:13.4125,103.8667)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK(geo::advance_geo_uri(pos, std::end(s), ctx));
        CHECK(pos == std::end(s));
        CHECK(c.latitude == std::stod("13.4125"));
        CHECK(c.longitude == std::stod("103.8667"));
    }

    {
        std::string s{R"(geo:48.2010,-16.3695,183)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK(geo::advance_geo_uri(pos, std::end(s), ctx));
        CHECK(pos == std::end(s));
        CHECK(c.latitude  == std::stod("48.2010"));
        CHECK(c.longitude == std::stod("-16.3695"));
        CHECK(c.altitude  == std::stod("183"));
    }

    {
        std::string s{R"(geo:48.198634,16.371648;crs=wgs84;u=40)"};
        geo_context c;
        auto ctx = make_geo_context(c);
        auto pos = std::begin(s);

        CHECK(geo::advance_geo_uri(pos, std::end(s), ctx));
        CHECK(pos == std::end(s));
        CHECK(c.latitude  == std::stod("48.198634"));
        CHECK(c.longitude == std::stod("16.371648"));
        CHECK(c.crslabel  == "wgs84");
        CHECK(c.uval      == std::stod("40"));
    }

    {
        std::string s1{R"(geo:66,30;u=6.500;FOo=this%2dthat;Bar)"};
        std::string s2{R"(geo:66.0,30;u=6.5;foo=this-that;bar)"};

        geo_context c1;
        geo_context c2;
        auto ctx1 = make_geo_context(c1);
        auto ctx2 = make_geo_context(c2);
        auto pos1 = std::begin(s1);
        auto pos2 = std::begin(s2);

        CHECK(geo::advance_geo_uri(pos1, std::end(s1), ctx1));
        CHECK(geo::advance_geo_uri(pos2, std::end(s2), ctx2));

        CHECK(pos1 == std::end(s1));
        CHECK(pos2 == std::end(s2));

        CHECK(c1.latitude  == c2.latitude);
        CHECK(c1.longitude == c2.longitude);
        CHECK(c1.uval == std::stod("6.5"));
        CHECK(c1.uval == c2.uval);

        REQUIRE(c1.parameters.size() == 2);
        REQUIRE(c2.parameters.size() == 2);

        CHECK(c1.parameters.find("foo") != c1.parameters.end());
        CHECK(c2.parameters.find("foo") != c2.parameters.end());
        CHECK(c1.parameters["foo"] == "this-that");
        CHECK(c2.parameters["foo"] == "this-that");

        CHECK(c1.parameters.find("bar") != c1.parameters.end());
        CHECK(c2.parameters.find("bar") != c2.parameters.end());
        CHECK(c1.parameters["bar"].empty());
        CHECK(c2.parameters["bar"].empty());
    }

    {
        using geo_uri = geo::uri;

        std::string s{R"(geo:66,30;u=6.500;FOo=this%2dthat;Bar)"};

        geo_uri uri;
        auto ctx = geo::make_context(uri);
        auto pos = std::begin(s);
        auto it = geo::parse(pos, std::end(s), ctx);
        REQUIRE(it == std::end(s));

        CHECK(uri.latitude()  == std::stod("66"));
        CHECK(uri.longitude() == std::stod("30"));
        CHECK_FALSE(uri.has_altitude());
        CHECK(uri.uncertainty() == std::stod("6.5"));

        REQUIRE(uri.count() == 2);
        CHECK(uri.has_parameter("foo"));
        CHECK(uri.parameter("foo") == "this-that");
        CHECK(uri.has_parameter("bar"));
        CHECK(uri.parameter("bar").empty());
    }
}
