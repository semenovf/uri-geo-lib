////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.14 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/rfc5870/composer.hpp"
#include <iostream>
#include <cstdlib>

#if defined(HAVE_QT5_CORE)
#   include "pfs/rfc5870/qt_suppl.hpp"
#endif

using std::cout;
namespace geo = pfs::rfc5870;

template <typename _GeoUriType, typename _OstreamType>
void composer_example (_OstreamType & out)
{
    using string_type = typename _GeoUriType::string_type;

    _GeoUriType uri;
    uri.set_latitude(66.0f);
    uri.set_longitude(30.0f);
    uri.set_altitude(100.0f);
    uri.set_crs(geo::construct_string<string_type>({'A', 'B', 'C'}));
    uri.set_uncertainty(6.5f);
    uri.insert(geo::construct_string<string_type>({'f', 'o', 'o'})
        , geo::construct_string<string_type>({'v', 'a', 'l'}));
    uri.insert(geo::construct_string<string_type>({'b', 'a', 'r'}));

    out << uri << "\n";
}


int main ()
{
    composer_example<geo::uri>(std::cout);
    composer_example<geo::wuri>(std::wcout);

#if defined(HAVE_QT5_CORE)
    composer_example<geo::basic_uri<double, QString>>(std::cout);
    composer_example<geo::basic_uri<double, QString>>(std::wcout);

    QTextStream stream(stdout);
    composer_example<geo::basic_uri<double, QString>>(stream);
#endif

    return EXIT_SUCCESS;
}
