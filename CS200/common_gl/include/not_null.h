/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */
//  Reference : https://github.com/microsoft/GSL

#pragma once

#include <cassert>
#include <stdexcept>

namespace util
{
    //
    // not_null
    //
    // Restricts a pointer to only hold non-null values.
    //
    template <class T> class not_null
    {
    public:
        constexpr not_null(T u) : ptr_(u)
        {
            assert(ptr_ != nullptr);
            if (ptr_ == nullptr)
            {
                throw std::runtime_error("not_null pointer was created with a nullptr!");
            }
        }

        [[nodiscard]] constexpr operator T() const noexcept
        {
            return ptr_;
        }

        // prevents compilation when someone attempts to assign a null pointer constant
        not_null(std::nullptr_t)            = delete;
        not_null& operator=(std::nullptr_t) = delete;

    private:
        T ptr_;
    };
}
