////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.14 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <QLocale>
#include <QString>
#include <QTextStream>

namespace pfs {
namespace rfc5870 {

template <>
class imbue_C_guard<QTextStream>
{
    QLocale _saved_loc;
    QTextStream & _out;

public:
    imbue_C_guard (QTextStream & out) : _out(out)
    {
        _saved_loc = _out.locale();
        _out.setLocale(QLocale{"C"});
    }

    ~imbue_C_guard ()
    {
        _out.setLocale(_saved_loc);
    }
};

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


