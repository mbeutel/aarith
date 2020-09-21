#include <aarith/integer_no_operators.hpp>
#include <catch.hpp>

#include "../test-signature-ranges.hpp"

using namespace aarith;

TEMPLATE_TEST_CASE_SIG("Left/right shifting signed integers",
                       "[integer][signed][operation][utility][foo]", AARITH_TEST_SIGNATURE,
                       AARITH_INT_TEST_TEMPLATE_PARAM_RANGE)
{
    using I = integer<W, WordType>;

    GIVEN("A positive integer")
    {

        I b{I::zero()};
        I bs1{I::zero()};
        I bs2{I::zero()};
        I bs3{I::zero()};

        for (size_t i = 0; i < I::word_count(); ++i)
        {
            b.set_word(i, 8U);
            bs1.set_word(i, 4U);
            bs2.set_word(i, 2U);
            bs3.set_word(i, 1U);
        }

        I d{b};
        I e{b};

        WHEN("Right Shifting")
        {
            THEN("It should behave like division by a power of two")
            {
                d >>= 1;
                REQUIRE((b >> 1) == bs1);
                REQUIRE(d == bs1);

                d >>= 1;
                e >>= 2;
                REQUIRE((b >> 2) == bs2);
                REQUIRE(d == bs2);
                REQUIRE(e == bs2);
                REQUIRE((bs1 >> 1) == bs2);

                d >>= 1;
                REQUIRE((b >> 3) == bs3);
                REQUIRE(d == bs3);
                REQUIRE((bs2 >> 1) == bs3);
            }
            THEN("It should move correctly over word boundaries")
            {
                if constexpr (I::word_count() > 1)
                {
                    I a{I::zero()};
                    a.set_word(1, 1U);
                    constexpr auto expected =
                        I{static_cast<WordType>(1U) << (I::word_width() - 1U)};

                    I k{a};
                    k >>= 1;
                    REQUIRE((a >> 1) == expected);
                    REQUIRE(k == expected);
                }
            }

            THEN("The it should also work when moving farther than the word width")
            {
                if constexpr (I::word_count() > 2)
                {
                    I c{I::zero()};

                    c.set_word(2, 23U);
                    I expected{23U};
                    REQUIRE((c >> 2 * I::word_width()) == expected);
                }
            }
        }
        WHEN("Left Shifting")
        {
            THEN("It should behave like multiplication by a power of two")
            {

                const I l1s3 = bs3 << 1;
                const I l2s3 = bs3 << 2;
                const I l3s3 = bs3 << 3;

                I l1s3a{bs3};
                l1s3a <<= 1;

                I l2s3a{bs3};
                l2s3a <<= 2;

                I l3s3a{bs3};
                l3s3a <<= 3;

                REQUIRE(l1s3 == bs2);
                REQUIRE(l2s3 == bs1);
                REQUIRE(l3s3 == b);

                REQUIRE(l1s3a == bs2);
                REQUIRE(l2s3a == bs1);
                REQUIRE(l3s3a == b);
            }
        }
    }

    GIVEN("The integer -1")
    {
        WHEN("Right shifting")
        {
            THEN("-1 should not be affected")
            {
                constexpr I minus_one{I::minus_one()};
                constexpr I shifted1 = minus_one >> 1;
                constexpr I shifted2 = minus_one >> 22;
                constexpr I shifted3 = minus_one >> 23;
                constexpr I shifted4 = minus_one >> 149;
                constexpr I shifted5 = minus_one >> 150;
                constexpr I shifted6 = minus_one >> 1151;

                I min_one{I::minus_one()};

                for (size_t s : {1, 22, 23, 149, 150, 1151})
                {
                    min_one >>= s;
                    std::cout << min_one << "\t" << to_binary(min_one) << "\n";
                    REQUIRE(min_one == minus_one);
                }
                CHECK(shifted1 == minus_one);
                CHECK(shifted2 == minus_one);
                REQUIRE(shifted3 == minus_one);
                REQUIRE(shifted4 == minus_one);
                REQUIRE(shifted5 == minus_one);
                REQUIRE(shifted6 == minus_one);
            }
        }
    }
}
