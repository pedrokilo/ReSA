#pragma once
// Cabecera sustituta SOLO para host_check/fixture_dump (sin el juego): lo minimo que necesita FxInterpInfo.h.
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using FILESTREAM = std::FILE*;
#define NOTSA_EXPORT_VTABLE
