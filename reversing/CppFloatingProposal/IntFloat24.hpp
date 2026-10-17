#pragma once

#include <array>
#include <cstdint>

// Núcleo experimental: FILD entero exacto, FLD binary32, FMUL PC=24,
// FSTP binary32. No usa aritmética float, asm, intrinsics ni cuerpos del EXE.
// Las excepciones se devuelven como datos; no modifica el estado físico x87.
namespace ReSA::CppFloatingProposal::IntFloat24 {

enum class Rounding : std::uint8_t { NearestEven, Down, Up, TowardZero };
enum class Class : std::uint8_t { Zero, Finite, Infinity, NaN };
enum Exception : std::uint8_t {
    None = 0,
    Invalid = 1,
    DenormalOperand = 2,
    Overflow = 8,
    Underflow = 16,
    Inexact = 32,
};

struct Extended24 {
    Class kind{Class::Zero};
    bool negative{};
    // Finite: significand * 2^scale, significand has <=24 significant bits.
    // NaN: full binary80 significand, with integer and quiet bits set.
    std::uint64_t significand{};
    int scale{};
};

struct Result {
    Extended24 intermediate{};
    std::uint32_t bits{};
    std::uint8_t exceptions{};
    bool multiplicationInexact{};
    bool multiplicationRoundedMagnitudeUp{};
    bool storeInexact{};
};

namespace Detail {
constexpr unsigned Width(std::uint64_t value) noexcept {
    unsigned width{};
    while (value) {
        ++width;
        value >>= 1;
    }
    return width;
}

struct Rounded {
    std::uint64_t value{};
    bool inexact{};
    bool magnitudeUp{};
};

constexpr Rounded RoundRight(std::uint64_t value, unsigned shift,
                             bool negative, Rounding rounding) noexcept {
    if (!shift)
        return {value, false, false};
    std::uint64_t quotient{};
    bool discarded{}, aboveHalf{}, exactlyHalf{};
    if (shift < 64) {
        quotient = value >> shift;
        const auto remainder = value & ((std::uint64_t{1} << shift) - 1);
        const auto half = std::uint64_t{1} << (shift - 1);
        discarded = remainder != 0;
        aboveHalf = remainder > half;
        exactlyHalf = remainder == half;
    } else if (shift == 64) {
        discarded = value != 0;
        aboveHalf = value > (std::uint64_t{1} << 63);
        exactlyHalf = value == (std::uint64_t{1} << 63);
    } else {
        discarded = value != 0;
        // uint64 magnitude is strictly below half a unit for shift >64.
    }
    bool increment{};
    if (discarded) {
        switch (rounding) {
        case Rounding::NearestEven:
            increment = aboveHalf || (exactlyHalf && (quotient & 1));
            break;
        case Rounding::Down: increment = negative; break;
        case Rounding::Up: increment = !negative; break;
        case Rounding::TowardZero: break;
        }
    }
    return {quotient + static_cast<unsigned>(increment), discarded, increment};
}

constexpr std::uint32_t OverflowBits(bool negative, Rounding rounding) noexcept {
    const bool infinite = rounding == Rounding::NearestEven ||
        (rounding == Rounding::Up && !negative) ||
        (rounding == Rounding::Down && negative);
    return (negative ? 0x80000000u : 0u) | (infinite ? 0x7F800000u : 0x7F7FFFFFu);
}

// Only called with intermediate values produced by Multiply below. That domain
// never has an inexact subnormal store: integer*binary32 is on the 2^-149 grid,
// and PC24 rounding can only coarsen that grid. No binary80 underflow/overflow.
constexpr void Store(Result& result, Rounding rounding) noexcept {
    const auto& value = result.intermediate;
    const auto sign = value.negative ? 0x80000000u : 0u;
    switch (value.kind) {
    case Class::Zero: result.bits = sign; return;
    case Class::Infinity: result.bits = sign | 0x7F800000u; return;
    case Class::NaN:
        result.bits = sign | 0x7F800000u |
            static_cast<std::uint32_t>((value.significand >> 40) & 0x7FFFFFu);
        return;
    case Class::Finite: break;
    }
    const auto width = Width(value.significand);
    const int exponent = value.scale + static_cast<int>(width) - 1;
    if (exponent > 127) {
        result.bits = OverflowBits(value.negative, rounding);
        result.exceptions |= Overflow | Inexact;
        result.storeInexact = true;
        return;
    }
    if (exponent >= -126) {
        const auto significand = value.significand << (24 - width);
        result.bits = sign | (static_cast<std::uint32_t>(exponent + 127) << 23) |
            (static_cast<std::uint32_t>(significand) & 0x7FFFFFu);
        return;
    }
    // All finite products in this API have scale >= -149. The shift is in
    // [0,22] here and the resulting significand occupies at most 23 bits.
    result.bits = sign |
        static_cast<std::uint32_t>(value.significand << (value.scale + 149));
}

constexpr Result Multiply(std::uint32_t magnitude, bool integerNegative,
                          std::uint32_t floatBits, Rounding rounding) noexcept {
    Result result{};
    const auto fraction = floatBits & 0x7FFFFFu;
    const auto exponentBits = (floatBits >> 23) & 0xFFu;
    const bool floatNegative = (floatBits & 0x80000000u) != 0;
    auto& value = result.intermediate;
    value.negative = integerNegative != floatNegative;
    if (exponentBits == 0xFFu) {
        if (fraction) {
            value.kind = Class::NaN;
            // FLD quiets a signaling NaN before the multiply. NaN propagation
            // keeps the source NaN sign/payload, regardless of integer sign.
            value.negative = floatNegative;
            value.significand = 0x8000000000000000ull |
                (static_cast<std::uint64_t>(fraction | 0x400000u) << 40);
            if (!(fraction & 0x400000u))
                result.exceptions |= Invalid;
        } else if (!magnitude) {
            value.kind = Class::NaN;
            value.negative = true;
            value.significand = 0xC000000000000000ull; // x87 indefinite.
            result.exceptions |= Invalid;
        } else {
            value.kind = Class::Infinity;
        }
        Store(result, rounding);
        return result;
    }
    if (!exponentBits && fraction)
        result.exceptions |= DenormalOperand; // FLD m32real input.
    if (!magnitude || (!exponentBits && !fraction)) {
        Store(result, rounding);
        return result;
    }
    value.kind = Class::Finite;
    const std::uint32_t floatSignificand = exponentBits ? fraction | 0x800000u : fraction;
    value.scale = exponentBits ? static_cast<int>(exponentBits) - 150 : -149;
    // 32-bit magnitude times a 24-bit significand fits exactly in uint64.
    value.significand = static_cast<std::uint64_t>(magnitude) * floatSignificand;
    const auto width = Width(value.significand);
    if (width > 24) {
        const auto shift = width - 24;
        const auto rounded = RoundRight(value.significand, shift, value.negative, rounding);
        value.significand = rounded.value;
        value.scale += static_cast<int>(shift);
        result.multiplicationInexact = rounded.inexact;
        result.multiplicationRoundedMagnitudeUp = rounded.magnitudeUp;
        if (rounded.inexact)
            result.exceptions |= Inexact;
        if (value.significand == (std::uint64_t{1} << 24)) {
            value.significand >>= 1;
            ++value.scale;
        }
    }
    Store(result, rounding);
    return result;
}
} // namespace Detail

constexpr Result MulInt32(std::int32_t integer, std::uint32_t floatBits,
                         Rounding rounding = Rounding::NearestEven) noexcept {
    const bool negative = integer < 0;
    const auto unsignedValue = static_cast<std::uint32_t>(integer);
    const auto magnitude = negative ? std::uint32_t{0} - unsignedValue : unsignedValue;
    return Detail::Multiply(magnitude, negative, floatBits, rounding);
}

constexpr Result MulUInt32(std::uint32_t integer, std::uint32_t floatBits,
                          Rounding rounding = Rounding::NearestEven) noexcept {
    // uint32 values >=2^31 require exact nonnegative FILD qword or equivalent
    // original sequence. A literal FILD dword treats those bits as int32.
    return Detail::Multiply(integer, false, floatBits, rounding);
}

constexpr std::array<std::uint8_t, 10> IntermediateBinary80(const Result& result) noexcept {
    const auto& value = result.intermediate;
    std::uint64_t significand{};
    std::uint16_t exponent{};
    switch (value.kind) {
    case Class::Zero: break;
    case Class::Finite: {
        const auto width = Detail::Width(value.significand);
        significand = value.significand << (64 - width);
        exponent = static_cast<std::uint16_t>(value.scale + static_cast<int>(width) - 1 + 16383);
        break;
    }
    case Class::Infinity:
        significand = 0x8000000000000000ull;
        exponent = 0x7FFF;
        break;
    case Class::NaN:
        significand = value.significand;
        exponent = 0x7FFF;
        break;
    }
    if (value.negative)
        exponent |= 0x8000;
    std::array<std::uint8_t, 10> bytes{};
    for (unsigned i = 0; i < 8; ++i)
        bytes[i] = static_cast<std::uint8_t>(significand >> (8 * i));
    bytes[8] = static_cast<std::uint8_t>(exponent);
    bytes[9] = static_cast<std::uint8_t>(exponent >> 8);
    return bytes;
}

static_assert(MulInt32(-1, 0x00000000u).bits == 0x80000000u);
static_assert(MulInt32(-2147483647 - 1, 0x3F800000u).bits == 0xCF000000u);
static_assert(MulUInt32(0xFFFFFFFFu, 0x3F800000u).bits == 0x4F800000u);
static_assert(MulUInt32(0xFFFFFFFFu, 0x3F800000u, Rounding::Down).bits == 0x4F7FFFFFu);
static_assert(MulInt32(0, 0x7F800000u).bits == 0xFFC00000u);
static_assert(MulInt32(1, 0x00000001u).bits == 0x00000001u);
static_assert(MulInt32(1, 0x7F800001u).bits == 0x7FC00001u);
} // namespace ReSA::CppFloatingProposal::IntFloat24
