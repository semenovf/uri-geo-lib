![logo](resources/pfs-rfc5870-black_64x64.png)

[![Standard](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17-blue)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://travis-ci.org/semenovf/pfs-rfc5870.svg?branch=master)](https://travis-ci.org/semenovf/pfs-rfc5870)

# pfs-rfc5870
A Uniform Resource Identifier for Geographic Locations (RFC5870) parser implementation

## Parsing

### Parsing using predefined structures

```cpp
#include "pfs/rfc5870/parser.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using std::cout;
namespace geo = pfs::rfc5870;

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

### Parsing using custom callbacks

```cpp

```

## Composing

**Note**
Logo made by [Vectors Market](https://www.flaticon.com/authors/vectors-market) from [www.flaticon.com](https://www.flaticon.com/)
