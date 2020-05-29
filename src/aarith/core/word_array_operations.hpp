#pragma once

#include <aarith/core/word_array.hpp>

namespace aarith {

template <typename W>[[nodiscard]] constexpr auto operator&(const W& lhs, const W& rhs) -> W
{
    static_assert(::aarith::is_word_array_v<W>);
    W bitwise_and;
    for (auto counter = 0U; counter < lhs.word_count(); ++counter)
    {
        bitwise_and.set_word(counter, lhs.word(counter) & rhs.word(counter));
    }
    return bitwise_and;
}

template <typename W> auto constexpr operator|(const W& lhs, const W& rhs) -> W
{
    static_assert(::aarith::is_word_array_v<W>);
    W bitwise_or;
    for (auto counter = 0U; counter < lhs.word_count(); ++counter)
    {
        bitwise_or.set_word(counter, lhs.word(counter) | rhs.word(counter));
    }
    return bitwise_or;
}

template <typename W> constexpr auto operator^(const W& lhs, const W& rhs) -> W
{
    static_assert(::aarith::is_word_array_v<W>);
    W bitwise_xor;
    for (auto counter = 0U; counter < lhs.word_count(); ++counter)
    {
        bitwise_xor.set_word(counter, lhs.word(counter) ^ rhs.word(counter));
    }
    return bitwise_xor;
}

template <typename W>[[nodiscard]] constexpr auto operator~(const W& rhs) -> W
{
    static_assert(::aarith::is_word_array_v<W>);
    W bitwise_not;
    for (auto counter = 0U; counter < rhs.word_count(); ++counter)
    {
        bitwise_not.set_word(counter, ~rhs.word(counter));
    }
    return bitwise_not;
}

/**
 * @brief  Counts the number of bits set to zero before the first one appears (from MSB to LSB)
 * @tparam Width Width of the word_array
 * @param value The word to count the leading zeroes in
 * @return
 */
template <size_t Width, typename WordType>
constexpr auto count_leading_zeroes(const word_array<Width, WordType>& value) -> size_t
{
    for (auto i = Width; i > 0; --i)
    {
        if (value.bit(i - 1))
        {
            return (Width - i);
        }
    }
    return Width;
}

/**
 * @brief Computes the position of the first set bit (i.e. a bit set to one) in the word_array from
 * MSB to LSB
 *
 * @tparam Width Width of the word_array
 * @param value The word_array whose first set bit should be found
 * @return The index of the first set bit in value
 */
template <size_t Width, typename WordType>
size_t first_set_bit(const word_array<Width, WordType>& value)
{
    size_t first_bit = 0UL;

    // we have the operator bool checking whether (value >> first_bit) != 0
    while (value >> first_bit)
    {
        ++first_bit;
    }
    return first_bit;
}

/**
 * @brief Left-shift operator
 * @tparam W The word_container type to work on
 * @param lhs The word_container to be shifted
 * @param rhs The number of bits to shift
 * @return The shifted word_container
 */
template <typename W>[[nodiscard]] constexpr auto operator<<(const W& lhs, const size_t rhs) -> W
{
    static_assert(::aarith::is_word_array_v<W>);

    constexpr size_t width = W::width();

    if (rhs >= width)
    {
        return W{0U};
    }
    if (rhs == 0)
    {
        return lhs;
    }
    W shifted;
    const auto skip_words = rhs / lhs.word_width();
    const auto shift_word_left = rhs - skip_words * lhs.word_width();
    const auto shift_word_right = lhs.word_width() - shift_word_left;

    using word_type = typename W::word_type;

    for (auto counter = lhs.word_count(); counter > 0; --counter)
    {
        if (counter + skip_words < lhs.word_count())
        {
            word_type new_word = lhs.word(counter) << shift_word_left;
            if (shift_word_right < lhs.word_width())
            {
                new_word = new_word | (lhs.word(counter - 1) >> shift_word_right);
            }
            shifted.set_word(counter + skip_words, new_word);
        }
    }
    word_type new_word = lhs.word(0) << shift_word_left;
    shifted.set_word(skip_words, new_word);

    return shifted;
}

/**
 * @brief Right-shift operator
 * @tparam Width The width of the word_array
 * @param lhs The word_array that is to be shifted
 * @param rhs The number of bits to shift
 * @return The shifted word_array
 */
template <typename W> auto constexpr operator>>(const W& lhs, const size_t rhs) -> W
{

    static_assert(::aarith::is_word_array_v<W>);

    /*
     * This prevents this shift operator to be chosen by the compiler when using signed integers.
     * For signed integers, the correct arithmetic right-shift will be used.
     */
    if constexpr (::aarith::is_integral_v<W>)
    {
        static_assert(::aarith::is_unsigned_v<W>);
    }

    constexpr size_t width = W::width();

    if (rhs >= width)
    {
        return W{0U};
    }
    if (rhs == 0)
    {
        return lhs;
    }

    W shifted;
    const auto skip_words = rhs / lhs.word_width();
    const auto shift_word_right = rhs - skip_words * lhs.word_width();
    const auto shift_word_left = lhs.word_width() - shift_word_right;

    using word_type = typename W::word_type;

    for (auto counter = skip_words; counter < lhs.word_count(); ++counter)
    {
        word_type new_word = lhs.word(counter) >> shift_word_right;
        if (shift_word_left < lhs.word_width() && counter + 1 < lhs.word_count())
        {
            new_word = new_word | (lhs.word(counter + 1) << shift_word_left);
        }
        shifted.set_word(counter - skip_words, new_word);
    }
    word_type new_word = lhs.word(lhs.word_count() - 1) >> shift_word_right;
    shifted.set_word(lhs.word_count() - skip_words - 1, new_word);

    return shifted;
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

    return width_cast<(S - E) + 1, W, WordType>(w >> E);
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

/**
 * @brief Applies a given function to all words within the word_array
 * @tparam F "Catch-all" parameter for functions operating on the words of the word_array
 * @tparam W Bit width of the word_array to operate on
 * @param w The word_array to operate on
 * @param f Function of type word_array<W>::word_type -> word_array<W>::word_type
 * @return A new word_array with the transformed words
 */
template <class F, size_t W, typename WordType>
[[nodiscard]] constexpr word_array<W, WordType> map(const word_array<W, WordType>& w, const F f)
{

    word_array<W, WordType> result;
    for (size_t i = 0; i < w.word_count(); ++i)
    {
        result.set_word(i, f(w.word(i)));
    }
    return result;
}

/**
 * @brief Applies a function to all words of two word_arrays and zips them to a new
 * word_array
 *
 * If the word_arrays have a different number of words, the result will be of size min(W,V);
 *
 * @tparam F "Catch-all" parameter for functions operating on the words of the word_array
 * @tparam W Bit width of the first word_array to operate on
 * @tparam V Bit width of the secnd word_array to operate on
 * @param w The first word_array to operate on
 * @param v The second word_array to operate on
 * @param f Function of type (word_array<W>::word_type, word_array<W>::word_type) ->
 * word_array<W>
 * @return The newly created, zipped word_array
 */
template <class F, size_t W, size_t V, typename WordType>
[[nodiscard]] constexpr word_array<std::min(W, V), WordType>
zip_with(const word_array<W, WordType>& w, const word_array<V, WordType>& v, F f)
{
    constexpr size_t L = std::min(W, V);
    word_array<L, WordType> result;

    for (size_t i = 0; i < result.word_count(); ++i)
    {
        result.set_word(i, f(w.word(i), v.word(i)));
    }
    return result;
}

/**
 * @brief Applies a function to all words of two word_arrays and zips them to a new
 * word_array carrying along a state
 *
 * The state that is carry will be parsed to the next function call on the two words of the
 * container and may be updated in the process (e.g. generating a carry that propagates through the
 * container).
 *
 * If the word_arrays have a different number of words, the result will be of size min(W,V);
 *
 * @tparam F "Catch-all" parameter for functions operating on the words of the word_array
 * @tparam W Bit width of the first word_array to operate on
 * @tparam V Bit width of the secnd word_array to operate on
 * @param w The first word_array to operate on
 * @param v The second word_array to operate on
 * @param f Function of type (word_array<W>::word_type, word_array<W>::word_type,
 * word_array<W>::word_type) -> std::pari<word_array<W>,word_array<W>::word_type>
 * @param initial_state The initial state to be passed into the operation
 * @return The newly created, zipped word_array
 */
template <class F, size_t W, size_t V, typename WordType>
[[nodiscard]] constexpr word_array<std::min(W, V), WordType>
zip_with_state(const word_array<W, WordType>& w, const word_array<V, WordType>& v, const F f,
               const typename word_array<std::min(W, V), WordType>::word_type initial_state =
                   typename word_array<std::min(W, V), WordType>::word_type())
{
    constexpr size_t L = std::min(W, V);
    word_array<L, WordType> result;

    using wt = typename word_array<L, WordType>::word_type;

    wt state = initial_state;

    for (size_t i = 0; i < result.word_count(); ++i)
    {
        const auto fun_res = f(w.word(i), v.word(i), state);
        state = fun_res.second;
        wt new_word = fun_res.first;
        result.set_word(i, new_word);
    }
    return result;
}

/**
 * @brief Zips two word containers that might have a different number of words, expanding missing
 * words with zeroes.
 *
 * If the word_arrays have a different number of words, the "missing" words are filled up with
 * zeroes such that this method will return a word_array that has the max width of both input
 * word_arrays.
 *
 * @tparam F "Catch all" type for the zip with function
 * @tparam W Width of the first word_array
 * @tparam V Width of the second word_array
 * @param w First word_array
 * @param v Second word_array
 * @param f The zip_with function of type (word_array<W>::word_type,
 * word_array<W>::word_type) -> word_array<W>
 * @return
 */
template <class F, size_t W, size_t V, typename WordType>
[[nodiscard]] constexpr word_array<std::max(W, V), WordType>
zip_with_expand(const word_array<W, WordType>& w, const word_array<V, WordType>& v, const F f)
{
    constexpr size_t wc = word_array<W, WordType>::word_count();
    constexpr size_t vc = word_array<V, WordType>::word_count();

    // simply return the "standard" zip_with when the word widths are identical
    if constexpr (wc == vc)
    {
        return zip_with(w, v, f);
    }
    else
    {

        constexpr size_t max = std::max(W, V);

        word_array<max, WordType> w_{w};
        word_array<max, WordType> v_{v};

        return zip_with(w_, v_, f);
    }
}

/**
 * @brief Zips two word containers that might have a different number of words, expanding missing
 * words with zeroes carrying along a state.
 *
 * The state that is carry will be parsed to the next function call on the two words of the
 * container and may be updated in the process (e.g. generating a carry that propagates through the
 * container).
 *
 *
 * If the word_arrays have a different number of words, the "missing" words are filled up with
 * zeroes such that this method will return a word_array that has the max width of both input
 * word_arrays.
 *
 * @tparam F "Catch all" type for the zip with function
 * @tparam W Width of the first word_array
 * @tparam V Width of the second word_array
 * @param w First word_array
 * @param v Second word_array
 * @param f Function of type (word_array<W>::word_type, word_array<W>::word_type,
 * word_array<W>::word_type) -> std::pari<word_array<W>,word_array<W>::word_type>
 * @param initial_state The initial state to be passed into the operation
 * @return
 */
template <class F, size_t W, size_t V, typename WordType>
[[nodiscard]] constexpr word_array<std::max(W, V), WordType>
zip_with_state_expand(const word_array<W, WordType>& w, const word_array<V, WordType>& v, const F f,
                      const typename word_array<std::max(W, V), WordType>::word_type initial_state =
                          typename word_array<std::max(W, V), WordType>::word_type())
{
    constexpr size_t wc = word_array<W, WordType>::word_count();
    constexpr size_t vc = word_array<V, WordType>::word_count();

    // simply return the "standard" zip_with when the word widths are identical
    if constexpr (wc == vc)
    {
        return zip_with_state(w, v, f);
    }
    else
    {

        constexpr size_t max = std::max(W, V);

        word_array<max, WordType> w_{w};
        word_array<max, WordType> v_{v};

        return zip_with_state(w_, v_, f, initial_state);
    }
}

/**
 *
 * @brief Reduces the vector using a given two-ary function.
 *
 * This is also known as a fold.
 *
 * @tparam R Return type of the reducing function
 * @tparam F "Catch all" type for the reducing function
 * @tparam W The width of the word_array
 * @param w The word_array
 * @param f The reducing function of type (word_array<W>::word_type, R) -> R
 * @param initial_value The initial value that is fed into f
 * @return The value from reducing the word_array using f and the given initial value
 */
template <class R, class F, size_t W, typename WordType>
[[nodiscard]] R constexpr reduce(const word_array<W, WordType>& w, const F f, const R initial_value)
{
    R result = initial_value;

    for (size_t i = 0; i < w.word_count(); ++i)
    {
        result = static_cast<R>(f(w.word(i), result));
    }

    return result;
}

/**
 * @brief zips and reduces two word_arrays
 *
 * @tparam R The return type of the reducing function
 * @tparam F "Catch all" type for the reducing function
 * @tparam W The width of the first word_array
 * @tparam V The width of the second word_array
 * @param w The first word_array
 * @param v The second word_array
 * @param f The reducing function of type (word_array<W>::word_type,
 * word_array<V>__word_type, R) -> R
 * @param initial_value The initial value that is fed into f
 * @return The value from zipping and reducing the two word_arrays
 */
template <class R, class F, size_t W, size_t V, typename WordType>
[[nodiscard]] R constexpr zip_reduce(const word_array<W, WordType>& w,
                                     const word_array<V, WordType>& v, const F f,
                                     const R initial_value)
{
    R result = initial_value;

    constexpr size_t L = std::min(W, V);

    for (size_t i = 0; i < word_array<L, WordType>::word_count(); ++i)
    {
        result = f(w.word(i), v.word(i), result);
    }

    return result;
}

/**
 * @brief zips and reduces two word_arrays that might have a different word count
 *
 * This method expands missing words by using zeros instead. This means that no values are skipped.
 *
 * @tparam R The return type of the reducing function
 * @tparam F "Catch all" type for the reducing function
 * @tparam W The width of the first word_array
 * @tparam V The width of the second word_array
 * @param w The first word_array
 * @param v The second word_array
 * @param f The reducing function of type (word_array<W>::word_type,
 * word_array<V>__word_type, R) -> R
 * @param initial_value The initial value that is fed into f
 * @return The value from zipping and reducing the two word_arrays
 */
template <class R, class F, size_t W, size_t V, typename WordType>
[[nodiscard]] R zip_reduce_expand(const word_array<W, WordType>& w,
                                  const word_array<V, WordType>& v, const F f,
                                  const R initial_value)
{

    R result = initial_value;

    constexpr size_t wc = w.word_count();
    constexpr size_t vc = v.word_count();

    if constexpr (wc == vc)
    {
        return zip_reduce(w, v, f, initial_value);
    }
    else
    {

        constexpr size_t max = std::max(W, V);

        word_array<max, WordType> w_{w};
        word_array<max, WordType> v_{v};

        return zip_reduce(w_, v_, f, initial_value);
    }
}

} // namespace aarith