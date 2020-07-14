////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.14 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <QString>

namespace pfs {
namespace rfc5870 {

template <>
inline QString wgs84_str<QString> ()
{
    return QString{"wgs84"};
}

inline std::ostream & operator << (std::ostream & out, QString const & s)
{
    out << s.toStdString();
    return out;
}

inline std::wostream & operator << (std::wostream & out, QString const & s)
{
    out << s.toStdWString();
    return out;
}

}} // namespace pfs::rfc5870


