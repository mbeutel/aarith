#pragma once

#include <aarith/core/traits.hpp>
#include <aarith/core/word_array.hpp>
#include <aarith/core/word_array_cast_operations.hpp>
#include <optional>

namespace aarith {

/**
 * @brief Counts the number of bits set to zero before the first one appears
 * (from MSB to LSB).
 *
 * Optional parameter offset makes it possible to skip the first offset-many
 * MSB. For example, calling count_leading_zeroes(0b100111, offset=1) would
 * return 2 as the first one is skipped.
 *
 * @tparam Width Width of the word_array
 * @param value The word to count the leading zeroes in
 * @param offset The number of MSB to skip before conuting.
 * @return The number of zeroes
 */
template <size_t Width, typename WordType>
constexpr size_t count_leading_zeroes(const word_array<Width, WordType>& value, size_t offset = 0)
{
    // if offset is chosen too big, skip all bits, counting none

    if (offset >= Width)
    {
        return 0;
    }

    // if offset is set to a reasonable value, count bit by bit

    const size_t start = Width - offset;

    for (auto i = start; i > 0; --i)
    {
        if (value.bit(i - 1))
        {
            return (Width - i - offset);
        }
    }

    return Width - offset;
}

/**
 * @brief Counts the number of bits set to ones before the first zero appears
 * (from MSB to LSB).
 *
 * Optional parameter offset makes it possible to skip the first offset-many
 * MSB. For example, calling count_leading_ones(0b011000, offset=1) would
 * return 2 as the first zero is skipped.
 *
 * @tparam Width Width of the word_array
 * @param value The word to count the leading ones in
 * @param offset The number of MSB to skip before conuting.
 * @return The number of ones
 */
template <size_t Width, typename WordType>
constexpr size_t count_leading_ones(const word_array<Width, WordType>& value, size_t offset = 0)
{
    return count_leading_zeroes(~value, offset);
}

/**
 * @brief Return a mask that has the n least significant bits set to one
 * and all other bits set to zero.
 *
 * If n is greater than Width, this function returns a word array with
 * all Width-many bits set to one.
 *
 * @tparam Width Width of the returned word_array
 * @param n The number of ones in the returned array
 * @return The mask with n-many bits set to one.
 */
template <size_t Width, typename WordType>
[[nodiscard]] constexpr word_array<Width, WordType> get_low_mask(size_t n)
{
    word_array<Width, WordType> w(0);

    for (size_t idx = 0; idx < n && idx < Width; ++idx)
    {
        w.set_bit(idx, true);
    }

    return w;
}

/**
 * @brief Computes the position of the first set bit (i.e. a bit set to one) in the word_array from
 * MSB to LSB and returns an empty optional if the word_array contains zeroes only.
 *
 * @tparam Width Width of the word_array
 * @param value The word_array whose first set bit should be found
 * @return The index of the first set bit in value
 */
template <size_t Width, typename WordType>
std::optional<size_t> first_set_bit(const word_array<Width, WordType>& value)
{
    const size_t leading_zeroes = count_leading_zeroes(value);

    if (leading_zeroes == Width)
    {
        return std::nullopt;
    }
    else
    {
        return Width - (leading_zeroes + 1);
    }
}

/**
 * @brief Computes the index of the first unset bit (i.e. a bit set to zero) in the word_array from
 * MSB to LSB and returns an empty optional if the word_array contains ones only.
 *
 * @tparam Width Width of the word_array
 * @param value The word_array whose first set bit should be found
 * @return The index of the first set bit in value
 */
template <size_t Width, typename WordType>
std::optional<size_t> first_unset_bit(const word_array<Width, WordType>& value)
{
    const size_t leading_ones = count_leading_ones(value);

    if (leading_ones == Width)
    {
        return std::nullopt;
    }
    else
    {
        return Width - (leading_ones + 1);
    }
}

/**
 * @brief Sets the most significant bit to one
 * @tparam W Type that behaves like a word array (can, e.g., also be an uinteger)
 * @tparam V The bit width of the type W
 * @tparam WordType The word type the data is stored in
 * @param w The word array like parameter
 * @return w with the most significant bit set to one
 */
template <template <size_t, typename> class W, size_t V, typename WordType = uint64_t>
[[nodiscard]] constexpr W<V, WordType> msb_one(const W<V, WordType>& w)
{
    constexpr W<V, WordType> msbone{word_array<V, WordType>::msb_one()};
    return (w | msbone);
}

/**
 * @brief Extracts a range from the word array
 *
 * Note that the indexing is done
 *  - zero based starting from the LSB
 *  - is inclusive (i.e. the start and end point are part of the range)
 *
 * @tparam S Starting index (inclusive, from left to right)
 * @tparam E  Ending index (inclusive, from left to right)
 * @tparam W Width of the word container that the range is taken from
 * @param w  Word container from which the range is taken from
 * @return Range word[E,S] inclusive
 */
template <size_t S, size_t E, size_t W, typename WordType>
[[nodiscard]] constexpr word_array<(S - E) + 1, WordType>
bit_range(const word_array<W, WordType>& w)
{
    static_assert(S < W, "Range must start within the word");
    static_assert(E <= S, "Range must be positive (i.e. this method will not reverse the word");

    return width_cast<(S - E) + 1>(w >> E);
}

template <size_t W, typename WordType>
[[nodiscard]] constexpr word_array<W, WordType>
dynamic_bit_range(const word_array<W, WordType>& w, size_t start_idx, size_t end_idx_exclusive)
{
    // check arguments

    if (end_idx_exclusive < start_idx)
    {
        throw std::invalid_argument("end_idx must be greater or equal to start_idx");
    }

    if (start_idx >= W)
    {
        throw std::invalid_argument("start_idx must be a valid index of a W-bit word array");
    }

    if (end_idx_exclusive > W)
    {
        throw std::invalid_argument("end_idx must be a valid index of a W-bit word array");
    }

    // arguments are okay; do copying

    word_array<W, WordType> result;

    for (size_t i = start_idx; i < end_idx_exclusive; ++i)
    {
        result.set_bit(i, w.bit(i));
    }

    return result;
}

template <size_t W, typename WordType>
[[nodiscard]] constexpr word_array<W, WordType> flip(const word_array<W, WordType>& w)
{
    word_array<W, WordType> copy;

    for (size_t widx = 0; widx < W; ++widx)
    {
        const size_t copyidx = W - 1 - widx;
        copy.set_bit(copyidx, w.bit(widx));
    }

    return copy;
}

/**
 * @brief Concatenates two word_arrays
 *
 * @tparam W Size of the first word_array
 * @tparam V Size of the second word_array
 * @tparam WordType The type to store the word_array's data in
 * @param w First word_array
 * @param v Second word_array
 * @return word_array of size W+V containing the bits of the inputs
 */
template <size_t W, size_t V, typename WordType>
word_array<W + V, WordType> concat(const word_array<W, WordType>& w,
                                   const word_array<V, WordType>& v)
{
    word_array<W + V, WordType> result{w};
    result = result << V;
    result = result | word_array<W + V, WordType>{v};
    return result;
}

/**
 * @brief Splits the word container at the given splitting point
 * @tparam S Splitting point
 * @tparam W Width of the word container to be split
 * @param w Word container that is split
 * @return Pair of <word[W-1,S+1], word[S,0]>
 */
template <size_t S, size_t W, typename WordType>
[[nodiscard]] constexpr std::pair<word_array<W - (S + 1), WordType>, word_array<S + 1, WordType>>
split(const word_array<W, WordType>& w)
{
    static_assert(S < W - 1 && S >= 0);

    const word_array<W - (S + 1), WordType> lhs{width_cast<W - (S + 1)>(w >> (S + 1))};

    const word_array<S + 1, WordType> rhs = width_cast<S + 1>(w);

    return std::make_pair(lhs, rhs);
}

} // namespace aarith
