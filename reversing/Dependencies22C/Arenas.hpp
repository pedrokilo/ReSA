#pragma once
// Construccion de los arenas falsos del banco Dependencies22C (sin dependencias del juego; fixture_oracle.py repite el diseno byte a byte).
// Un arena es un bloque propio del banco: ningun objeto del motor FX ni del juego se toca (los colaboradores de CreateParticles estan sustituidos por stubs trazados).
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Fixtures.hpp"
namespace Dependencies22CFix {
inline void Put32(std::uint8_t* base, std::size_t at, std::uint32_t v) { std::memcpy(base + at, &v, 4); }
inline std::uint32_t Get32(const std::uint8_t* base, std::size_t at) { std::uint32_t v; std::memcpy(&v, base + at, 4); return v; }
inline void Put16(std::uint8_t* base, std::size_t at, std::uint16_t v) { std::memcpy(base + at, &v, 2); }
inline std::uint32_t Ptr(const std::uint8_t* base, std::size_t at) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base + at)); }

// ---- CCam::WrapAngle ----
// 48 bytes: el float vive en 8 + 4 * slot; el resto es relleno (0x00, 0xFF, 0xA5 o 0x5A).
constexpr std::size_t kWrapArena = 48, kWrapAngleAt = 8;
constexpr std::size_t WrapAngleOffset(const WrapFx& f) { return kWrapAngleAt + 4 * f.slot; }
inline void BuildWrap(std::uint8_t* base, const WrapFx& f) {
    constexpr std::uint8_t fills[4] = {0x00, 0xFF, 0xA5, 0x5A};
    std::memset(base, fills[f.fill], kWrapArena);
    Put32(base, WrapAngleOffset(f), f.angle);
}

// ---- FxEmitter_c::CreateParticles ----
// Offsets reales de las clases: FxEmitter_c (0x14: vptr, m_PrimBP +4, m_System +8, m_bEnabled +0xC, m_fEmissionIntensity +0x10), FxPrimBP_c (m_FxInfoManager +0x2C: m_nLodStart +0x38,
// m_nLodEnd +0x3A), FxSystem_c (m_SystemBP +8, m_ParentMatrix +0xC, m_LocalMatrix +0x10, m_UseConstTime +0x52, m_fCameraDistance +0x58, m_nRateMult +0x5E, banderas +0x62: bit 1 = m_createLocal),
// FxSystemBP_c (m_fLength +0xC).
constexpr std::size_t kCpEmitter = 0x000, kCpPrim = 0x040, kCpSystem = 0x0C0, kCpSystemBp = 0x140, kCpParent = 0x160, kCpMats = 0x1A0, kCpArena = 0x280;
constexpr std::size_t CpMatrix(unsigned i) { return kCpMats + 0x40 * i; }
inline void BuildCreate(std::uint8_t* base, const CreateFx& f) {
    std::memset(base, 0x5C, kCpArena);
    Put32(base, kCpEmitter + 0, 0x0085A7A4); Put32(base, kCpEmitter + 4, Ptr(base, kCpPrim)); Put32(base, kCpEmitter + 8, Ptr(base, kCpSystem));
    base[kCpEmitter + 0xC] = 1; Put32(base, kCpEmitter + 0x10, f.init);
    Put16(base, kCpPrim + 0x38, static_cast<std::uint16_t>(f.lodStart)); Put16(base, kCpPrim + 0x3A, static_cast<std::uint16_t>(f.lodEnd));
    Put32(base, kCpSystem + 0, 0); Put32(base, kCpSystem + 4, 0);
    Put32(base, kCpSystem + 8, Ptr(base, kCpSystemBp)); Put32(base, kCpSystem + 0xC, f.parent ? Ptr(base, kCpParent) : 0);
    for (unsigned i = 0; i < 16; ++i) Put32(base, kCpSystem + 0x10 + 4 * i, 0x3F000000u + f.emissionFill + i);
    base[kCpSystem + 0x52] = static_cast<std::uint8_t>(f.emissionFill & 1);
    Put32(base, kCpSystem + 0x58, f.dist); Put16(base, kCpSystem + 0x5E, static_cast<std::uint16_t>(f.rate));
    base[kCpSystem + 0x62] = static_cast<std::uint8_t>((f.flagsOther & 0xFD) | (f.createLocal ? 2 : 0));
    Put32(base, kCpSystemBp + 0xC, 0x41200000);
    for (unsigned i = 0; i < 16; ++i) Put32(base, kCpParent + 4 * i, 0x3E000000u + f.time + 3 * i);
    for (unsigned m = 0; m < 3; ++m) std::memset(base + CpMatrix(m), 0x77, 0x40);
}
// EmissionInfo_t que escribe el stub de ProcessEmissionInfo (26 palabras): relleno por caso salvo count (+0) y los limites de viento/lluvia (+0x58..+0x64).
constexpr unsigned kEmissionWords = 26;
inline void BuildEmission(std::uint32_t* out, const CreateFx& f) {
    for (unsigned i = 0; i < kEmissionWords; ++i) out[i] = Mix(f.emissionFill + i * 77u);
    out[0] = f.count; out[22] = f.minWind; out[23] = f.maxWind; out[24] = f.minRain; out[25] = f.maxRain;
}
}  // namespace Dependencies22CFix
