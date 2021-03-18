#include "int_correctness_check_fun.hpp"

int main()
{
    using namespace std;
    using namespace aarith;

    constexpr size_t Width = 32;
    using UB = uint32_t;
    using WordType = uint64_t;
    using UI = uinteger<Width, WordType>;

    check_int_operation<uinteger, Width, UB, false, WordType>(
        "mul_schoolbook", &::aarith::schoolbook_mul<UI>, native_mul);

    return 0;
}