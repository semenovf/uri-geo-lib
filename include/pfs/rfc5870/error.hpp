////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.10 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <system_error>

namespace pfs {
namespace rfc5870 {

////////////////////////////////////////////////////////////////////////////////
// Error codes, category, exception class
////////////////////////////////////////////////////////////////////////////////
using error_code = std::error_code;

enum class errc
{
      success = 0

// Parser errors
    , bad_number

//
//     , type_error
//     , null_pointer
};

class error_category : public std::error_category
{
public:
    virtual char const * name () const noexcept override
    {
        return "rfc5870_category";
    }

    virtual std::string message (int ev) const override
    {
        switch (ev) {
//             case static_cast<int>(errc::success):
//                 return std::string{"no error"};
//             case static_cast<int>(errc::forbidden_root_element):
//                 return std::string{"root element is forbidden"};
            case static_cast<int>(errc::bad_number):
                return std::string{"bad number"};
//             case static_cast<int>(errc::bad_escaped_char):
//                 return std::string{"bad escaped char"};
//             case static_cast<int>(errc::bad_encoded_char):
//                 return std::string{"bad encoded char"};
//             case static_cast<int>(errc::unbalanced_array_bracket):
//                 return std::string{"unbalanced array bracket"};
//             case static_cast<int>(errc::unbalanced_object_bracket):
//                 return std::string{"unbalanced object bracket"};
//             case static_cast<int>(errc::bad_member_name):
//                 return std::string{"bad member name"};
//             case static_cast<int>(errc::bad_json_sequence):
//                 return std::string{"bad json sequence"};
//
//             case static_cast<int>(errc::type_error):
//                 return std::string{"type error"};
//
//             case static_cast<int>(errc::null_pointer):
//                 return std::string{"null pointer"};

            default: return std::string{"unknown RFC 5870"};
        }
    }
};

inline std::error_category const & get_error_category ()
{
    static error_category instance;
    return instance;
}

inline std::error_code make_error_code (errc e)
{
    return std::error_code(static_cast<int>(e), get_error_category());
}

inline std::system_error make_exception (errc e)
{
    return std::system_error(make_error_code(e));
}

// inline std::system_error make_cast_exception (std::string const & from
//         , std::string const & to)
// {
//     return std::system_error(make_error_code(errc::type_error)
//             , std::string("can not cast from ") + from + " to " + to);
// }

}} // // namespace pfs::rfc5870
