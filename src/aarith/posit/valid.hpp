#pragma once

#include <aarith/posit.hpp>

namespace aarith {

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT> valid<N, ES, WT>::from(const tile<N, ES, WT>& start,
                                                                const tile<N, ES, WT>& end)
{
    valid v;

    v.start = start;
    v.end = end;

    return v;
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT> valid<N, ES, WT>::zero()
{
    return from(tile_type::zero(), tile_type::zero());
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT> valid<N, ES, WT>::one()
{
    return from(tile_type::one(), tile_type::one());
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT> valid<N, ES, WT>::empty()
{
    // To represent the empty set, we can pick any posit p and return the
    // interval (p, p). We pick p = 0 here. This is arbitrary.

    constexpr auto open_interval = tile_type::from(posit_type::zero(), true);
    return from(open_interval, open_interval);
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT> valid<N, ES, WT>::nar()
{
    constexpr auto nar_tile = tile_type::nar();
    return from(nar_tile, nar_tile);
}

template <size_t N, size_t ES, typename WT> constexpr valid<N, ES, WT>::valid()
{
    ensure_canonicalized();
}

template <size_t N, size_t ES, typename WT>
constexpr valid<N, ES, WT>::valid(const valid& other)
    : start(other.start)
    , end(other.end)
{
    ensure_canonicalized();
}

template <size_t N, size_t ES, typename WT>
constexpr valid<N, ES, WT>::valid(const posit<N, ES, WT>& exact_value)
    : start(tile<N, ES, WT>::from(exact_value, true))
    , end(tile<N, ES, WT>::from(exact_value, true))
{
    ensure_canonicalized();
}

template <size_t N, size_t ES, typename WT> valid<N, ES, WT>::~valid()
{
}

template <size_t N, size_t ES, typename WT>
valid<N, ES, WT>& valid<N, ES, WT>::operator=(const valid& other)
{
    start = other.start;
    end = other.end;
    return *this;
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::operator==(const valid& other) const
{
    return start == other.start && end == other.start;
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::operator!=(const valid& other) const
{
    return !(*this == other);
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::operator<(const valid& other) const
{
    const valid& lhs = *this;
    const valid& rhs = other;

    if (lhs == rhs)
    {
        return false;
    }

    if (lhs.is_nar() || rhs.is_nar())
    {
        return false;
    }

    // Based on "The End of Error", Gustafson, 2015, pp. 105. The names might
    // be a bit confusing, but "right" is the right (end) bound of "lhs" and
    // "left" is the left (start) bound of "rhs". We keep names identical to
    // the original formulation here.

    const tile_type& right = lhs.end;
    const tile_type& left = rhs.other.start;

    if (right.is_negative() != left.is_negative())
    {
        return left.is_negative();
    }

    if (lhs == lhs.min() || rhs == rhs.min())
    {
        return lhs.min();
    }

    return false;
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::operator<=(const valid& other) const
{
    return (*this < other) || (*this == other);
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::operator>(const valid& other) const
{
    return other < *this;
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::operator>=(const valid& other) const
{
    return (*this > other) || (this == other);
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT>
valid<N, ES, WT>::operator+(const valid<N, ES, WT>& other) const
{
    throw std::logic_error("valid::operator+ not implemented");
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT>
valid<N, ES, WT>::operator-(const valid<N, ES, WT>& other) const
{
    throw std::logic_error("valid::operator- not implemented");
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT>
valid<N, ES, WT>::operator*(const valid<N, ES, WT>& other) const
{
    throw std::logic_error("valid::operator* not implemented");
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT>
valid<N, ES, WT>::operator/(const valid<N, ES, WT>& other) const
{
    throw std::logic_error("valid::operator/ not implemented");
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::is_zero() const
{
    return *this == zero();
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::is_empty() const
{
    return start.is_uncertain() && end.is_uncertain() && start.value() == end.value();
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr bool valid<N, ES, WT>::is_nar() const
{
    return *this == nar();
}

template <size_t N, size_t ES, typename WT>
[[nodiscard]] constexpr valid<N, ES, WT> valid<N, ES, WT>::canonical_empty()
{
    return empty();
}

template <size_t N, size_t ES, typename WT> void valid<N, ES, WT>::ensure_canonicalized()
{
    if (is_empty())
    {
        *this = canonical_empty();
    }
}

} // namespace aarith
