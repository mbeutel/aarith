#include <catch.hpp>

#include <aarith/types/word_container.hpp>
#include <aarith/utilities/bit_operations.hpp>
using namespace aarith;

SCENARIO("Splitting uint64_ts", "[util]")
{
    WHEN("Considering an uint64_t n")
    {
        THEN("Splitting and unsplitting should yield the original number")
        {
            uint64_t n = 0u;

            for (uint i = 0; i < 10000000; i++)
            {
                const auto [u, l] = split(n);
                const auto result = unsplit(u, l);
                REQUIRE(result == n);

                n += 15381;
            }
        }
    }
}

SCENARIO("Counting bits in word_container", "[util]")
{
    WHEN("Multiple word_containers are given")
    {
        THEN("The leading zeroe index should be computed correctly")
        {
            word_container<64> a_zero{0U};
            word_container<64> a_one{1U};
            word_container<32> b_zero{0U};
            word_container<32> b_one{1U};
            word_container<15> c_zero{0U};
            word_container<15> c_one{1U};
            word_container<150> d_zero{0U};
            word_container<150> d_one{1U};
            
            CHECK(count_leading_zeroes(a_zero) == 64);
            CHECK(count_leading_zeroes(b_zero) == 32);
            CHECK(count_leading_zeroes(c_zero) == 15);
            CHECK(count_leading_zeroes(d_zero) == 150);
            CHECK(count_leading_zeroes(a_one) == 64-1);
            CHECK(count_leading_zeroes(b_one) == 32-1);
            CHECK(count_leading_zeroes(c_one) == 15-1);
            CHECK(count_leading_zeroes(d_one) == 150-1);
        }
    }
}


SCENARIO("Casting uintegers into different width", "[uinteger]")
{
    GIVEN("width_cast is called")
    {
        static constexpr uint16_t test_value = 123;
        static constexpr size_t SourceWidth = 16;
        const word_container<SourceWidth> uint{test_value};

        WHEN("The source width <= destination width")
        {
            static constexpr size_t DestinationWidth = 32;
            auto const result = width_cast<DestinationWidth>(uint);

            THEN("The result has the destination width")
            {
                REQUIRE(result.width() == DestinationWidth);
            }
            AND_THEN("The result is not cut off")
            {
                REQUIRE(result.word(0) == test_value);
            }
        }
        WHEN("The source width > destination width")
        {
            static constexpr size_t DestinationWidth = 8;
            auto const result = width_cast<DestinationWidth>(uint);

            THEN("The result has the destination width")
            {
                REQUIRE(result.width() == DestinationWidth);
            }
            AND_THEN("The result is cut off")
            {
                REQUIRE(result.word(0) == (test_value & 0xff));
            }
        }
    }
}