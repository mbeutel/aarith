#pragma once

namespace aarith {

template <class Type> class is_integral
{
public:
    static constexpr bool value = false;
};

template <class Type> class is_unsigned
{
public:
    static constexpr bool value = false;
};

} // namespace aarith