////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2020 Vladislav Trifochkin
//
// This file is part of [pfs-rfc5870](https://github.com/semenovf/pfs-rfc5870) library.
//
// Changelog:
//      2020.07.14 Initial version
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
    , unique_crs_requirement_broken
    , unique_u_requirement_broken
    , unique_uncertainty_requirement_broken = unique_u_requirement_broken
    , u_out_of_order
    , uncertainty_out_of_order = u_out_of_order
};

class error_category : public std::error_category
{
public:
    virtual char const * name () const noexcept override
    {
        return "pfs::rfc5870::error_category";
    }

    virtual std::string message (int ev) const override
    {
        switch (ev) {
            case static_cast<int>(errc::success):
                return std::string{"no error"};
            case static_cast<int>(errc::unique_crs_requirement_broken):
                return std::string{"unique CRS requirement broken"};
            case static_cast<int>(errc::unique_u_requirement_broken):
                return std::string{"unique uncertainty requirement broken"};
            case static_cast<int>(errc::u_out_of_order):
                return std::string{"uncertainty is out of order"};

            default: return std::string{"unknown JSON error"};
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

}} // // namespace pfs::rfc5870

