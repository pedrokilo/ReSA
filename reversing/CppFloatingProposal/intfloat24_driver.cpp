#include "IntFloat24.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>

int main() {
    using namespace ReSA::CppFloatingProposal::IntFloat24;
    unsigned unsignedInput{}, mode{};
    std::uint32_t integerBits{}, floatBits{};
    while (std::cin >> unsignedInput >> integerBits >> floatBits >> mode) {
        if (unsignedInput > 1 || mode > 3)
            return 2;
        // Conversion from the bits is explicit and defined, including INT_MIN.
        const auto signedInput = integerBits <= 0x7FFFFFFFu
            ? static_cast<std::int32_t>(integerBits)
            : static_cast<std::int32_t>(static_cast<std::int64_t>(integerBits) - 0x100000000ll);
        const auto result = unsignedInput
            ? MulUInt32(integerBits, floatBits, static_cast<Rounding>(mode))
            : MulInt32(signedInput, floatBits, static_cast<Rounding>(mode));
        std::cout << result.bits << ' ' << static_cast<unsigned>(result.exceptions) << ' '
                  << result.multiplicationInexact << ' '
                  << result.multiplicationRoundedMagnitudeUp << ' ' << result.storeInexact << ' ';
        for (const auto byte : IntermediateBinary80(result))
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(byte);
        std::cout << std::dec << '\n';
    }
    return std::cin.eof() ? 0 : 3;
}
