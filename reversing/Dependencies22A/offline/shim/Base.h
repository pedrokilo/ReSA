#pragma once
#include <cstdint>
#include <cstring>
#include <ranges>
#include <bit>
#include <array>
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using uint64 = std::uint64_t;
using int8 = std::int8_t; using int16 = std::int16_t; using int32 = std::int32_t; using int64 = std::int64_t;
namespace rng = std::ranges;
inline constexpr std::uintptr_t kShift = 0x2FC00000;
template<typename T> inline T& StaticRef(std::uintptr_t a) { return *reinterpret_cast<T*>(a + kShift); }
#define VALIDATE_SIZE(s, n) static_assert(sizeof(s) == n)
#define ZoneScoped
