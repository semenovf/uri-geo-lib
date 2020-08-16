![logo](resources/pfs-uri-geo-black_64x64.png)

[![Standard](resources/badge/cxx-11-14-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](resources/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Header Only](resources/badge/header-only.svg)](https://en.wikipedia.org/wiki/Header-only)
[![Build Status](https://travis-ci.org/semenovf/pfs-uri-geo.svg?branch=master)](https://travis-ci.org/semenovf/pfs-uri-geo)
[![Build status](https://ci.appveyor.com/api/projects/status/owogk328rraglcbp/branch/master?svg=true)](https://ci.appveyor.com/project/semenovf/pfs-uri-geo/branch/master)
[![codecov](https://codecov.io/gh/semenovf/pfs-uri-geo/branch/master/graph/badge.svg)](https://codecov.io/gh/semenovf/pfs-uri-geo)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fsemenovf%2Fpfs-uri-geo.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fsemenovf%2Fpfs-uri-geo?ref=badge_shield)

# pfs-uri-geo
A Uniform Resource Identifier for Geographic Locations (RFC5870) parser implementation

## Parsing

### Parsing using predefined structures

```cpp
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
```

This code outputs:

```sh
66
30
false
true
wgs84
true
6.5
2
true
true
false
this-that
true
```

### Parsing using custom callbacks

```cpp
#include "pfs/uri/geo/parser.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using std::cout;
namespace geo = pfs::uri::geo;

struct custom_context
{
    using number_type = double;
    using string_type = std::string;
};

int main ()
{
    std::string s{R"(geo:66,30;u=6.500;FOo=this%2dthat;Bar)"};

    // Predict if `s` is geo URI representation
    if (!geo::like_geo_uri(s))
        return EXIT_FAILURE;

    geo::parser_interface<custom_context> ctx;
    ctx.on_latitude  = [] (double && n) { cout << "Latitude: " << n << "\n"; };
    ctx.on_longitude = [] (double && n) { cout << "Longitude: " << n << "\n"; };
    ctx.on_altitude  = [] (double && n) { cout << "Altitude: " << n << "\n"; };
    ctx.on_crslabel  = [] (std::string && s) { cout << "CRS: " << s << "\n"; };
    ctx.on_uval      = [] (double && n) { cout << "Uncertainty: " << n << "\n"; };
    ctx.on_parameter = [] (std::string && key, std::string && value) {
        cout << key << ": [" << value << "]\n";
    };

    auto pos = std::begin(s);
    auto it = geo::parse(pos, std::end(s), ctx);

    // Bad geo URI string representation
    if (it == std::begin(s))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
```

This code outputs:

```sh
Latitude: 66
Longitude: 30
Uncertainty: 6.5
foo: [this-that]
bar: []
```

## Composing

```cpp
#include "pfs/uri/geo/composer.hpp"
#include <iostream>
#include <cstdlib>

using std::cout;
namespace geo = pfs::uri::geo;

int main ()
{
    geo::uri uri;
    uri.set_latitude(66.0f);
    uri.set_longitude(30.0f);
    uri.set_altitude(100.0f);
    uri.set_crs("ABC");
    uri.set_uncertainty(6.5f);
    uri.insert("foo", "val");
    uri.insert("bar");

    cout << uri << "\n";

    return EXIT_SUCCESS;
}
```

This code outputs:

```sh
geo:66,30,100;crs=ABC;u=6.5;bar;foo=val
```

### For Qt users

Qt supplementary header `pfs/uri/geo/qt_suppl.hpp` allows using `QString` as
string type for `pfs::uri::geo::basic_uri`.

```cpp
#include "pfs/uri/geo/composer.hpp"
#include "pfs/uri/geo/qt_suppl.hpp"

...

int main ()
{
    geo::basic_uri<double, QString> uri;
    uri.set_latitude(66.0f);
    uri.set_longitude(30.0f);

    ...

    cout << uri << "\n";
    // or
    wcout << uri << "\n";
    // or
    QTextStream stream(stdout);
    stream << uri << "\n";

    return EXIT_SUCCESS;
}
```

## Licences

Logo made by [Vectors Market](https://www.flaticon.com/authors/vectors-market) from [www.flaticon.com](https://www.flaticon.com/)<br/>
Badges generated by [shields.io](https://shields.io)<br/>
