#include <aarith/integer.hpp>
#include <catch.hpp>

using namespace aarith;

SCENARIO("Casting unsigned integers to uint8_t", "[integer][unsigned][conversion_operation]")
{
    GIVEN("An unsigned integer width bit-width <= 8")
    {

        constexpr uint8_t val = 13;

        constexpr uinteger<8> a{val};
        constexpr uinteger<7> b{val};
        constexpr uinteger<6> c{val};
        constexpr uinteger<5> d{val};

        THEN("The cast function should succeed and yield the stored value")
        {
            constexpr uint8_t a8 = to_uint8_t(a);
            constexpr uint8_t b8 = to_uint8_t(b);
            constexpr uint8_t c8 = to_uint8_t(c);
            constexpr uint8_t d8 = to_uint8_t(d);

            REQUIRE(a8 == val);
            REQUIRE(b8 == val);
            REQUIRE(c8 == val);
            REQUIRE(d8 == val);
        }

        THEN("The conversion operator should succeed and yield the stored value")
        {
            constexpr uint8_t a8 = static_cast<uint8_t>(a);
            constexpr uint8_t b8 = static_cast<uint8_t>(b);
            constexpr uint8_t c8 = static_cast<uint8_t>(c);
            constexpr uint8_t d8 = static_cast<uint8_t>(d);

            REQUIRE(a8 == val);
            REQUIRE(b8 == val);
            REQUIRE(c8 == val);
            REQUIRE(d8 == val);
        }
    }

    GIVEN("An unsigned integer with bit-width > 8")
    {

        constexpr uint16_t min = std::numeric_limits<uint8_t>::min();
        constexpr uint16_t max = std::numeric_limits<uint8_t>::max();

        WHEN("The uinteger value fits a uint8_t")
        {

            const uint16_t val = GENERATE(take(5, random(min, max)));

            uinteger<9> a{val};
            uinteger<16> b{val};
            uinteger<32> c{val};
            uinteger<64> d{val};
            uinteger<150> e{val};

            THEN("The cast function should succeed and yield the stored value")
            {
                uint8_t a8 = to_uint8_t(a);
                uint8_t b8 = to_uint8_t(b);
                uint8_t c8 = to_uint8_t(c);
                uint8_t d8 = to_uint8_t(d);
                uint8_t e8 = to_uint8_t(e);

                REQUIRE(a8 == val);
                REQUIRE(b8 == val);
                REQUIRE(c8 == val);
                REQUIRE(d8 == val);
                REQUIRE(e8 == val);
            }

            THEN("The conversion operator should succeed and yield the stored value")
            {
                uint8_t a8 = static_cast<uint8_t>(a);
                uint8_t b8 = static_cast<uint8_t>(b);
                uint8_t c8 = static_cast<uint8_t>(c);
                uint8_t d8 = static_cast<uint8_t>(d);
                uint8_t e8 = static_cast<uint8_t>(e);

                REQUIRE(a8 == val);
                REQUIRE(b8 == val);
                REQUIRE(c8 == val);
                REQUIRE(d8 == val);
                REQUIRE(e8 == val);
            }
        }

        WHEN("The uinteger values does not fit into a uint8_t")
        {

            THEN("The conversion operator truncates the result")
            {
                const uint16_t val = GENERATE(
                    take(5, random(uint16_t{max + 1}, std::numeric_limits<uint16_t>::max())));

                uinteger<9> a{val};
                uinteger<16> b{val};
                uinteger<32> c{val};
                uinteger<64> d{val};
                uinteger<150> e{val};

                uint8_t a8 = static_cast<uint8_t>(a);
                uint8_t b8 = static_cast<uint8_t>(b);
                uint8_t c8 = static_cast<uint8_t>(c);
                uint8_t d8 = static_cast<uint8_t>(d);
                uint8_t e8 = static_cast<uint8_t>(e);

                //                uinteger<16,uint8_t> foo{val};
                //                [[maybe_unused]] uint8_t f8 = static_cast<uint16_t>(foo);

                uint8_t expected = static_cast<uint8_t>(val);

                REQUIRE(a8 == expected);
                REQUIRE(b8 == expected);
                REQUIRE(c8 == expected);
                REQUIRE(d8 == expected);
                REQUIRE(e8 == expected);
            }

            THEN("The cast function throws an exception")
            {
                const uint16_t val = GENERATE(
                    take(5, random(uint16_t{max + 1}, std::numeric_limits<uint16_t>::max())));

                uinteger<9> a{val};
                uinteger<16> b{val};
                uinteger<32> c{val};
                uinteger<64> d{val};
                uinteger<150> e{val};

                //                to_uint8_t(a);

                REQUIRE_THROWS(to_uint8_t(a));
                //                CHECK_THROWS_AS(to_uint8_t(b), std::domain_error);
                //                CHECK_THROWS_AS(to_uint8_t(c), std::domain_error);
                //                CHECK_THROWS_AS(to_uint8_t(d), std::domain_error);
                //                CHECK_THROWS_AS(to_uint8_t(e), std::domain_error);f
            }
        }
    }
}