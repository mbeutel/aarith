#include "int_correctness_check_fun.hpp"

int main()
{
    using namespace std;
    using namespace aarith;

    constexpr size_t Width = 32;
    using UB = uint32_t;
    using B = int32_t;

    using UI = uinteger<Width, uint64_t>;
    using I = integer<Width, uint64_t>;

    check_int_operation<integer, Width, B, false>("addition", &::aarith::add<I>, native_add);

    return 0;
}