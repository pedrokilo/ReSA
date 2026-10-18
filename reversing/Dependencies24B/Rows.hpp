#pragma once
// Filas JSON comunes del banco Dependencies24B (las usan LiveComparison.hpp y offline/main.cpp). Requiere `json` (nlohmann) declarado antes.
#include "Fixtures.hpp"
namespace Dependencies24BFix {
inline std::string HexBytes(const void* p, size_t n) {
    constexpr char digits[] = "0123456789abcdef";
    const auto* b = static_cast<const u8*>(p);
    std::string out;
    out.reserve(n * 2);
    for (size_t i = 0; i < n; ++i) { out += digits[b[i] >> 4]; out += digits[b[i] & 15]; }
    return out;
}
inline json InputRow(unsigned c, const Fix& f) {
    const auto table = BuildTable(f);
    const auto res = ExpectedResidue(f.stack_seed);
    return {{"case", c}, {"dev_mode", f.dev_mode}, {"mutator", f.mutator}, {"table_kind", f.table_kind}, {"table_in", HexBytes(table.data(), table.size())},
            {"prop_mode", {f.prop_mode[0], f.prop_mode[1]}}, {"enum_ret", f.enum_ret}, {"caps_hr", {f.caps_hr[0], f.caps_hr[1]}}, {"prop_hr", {f.prop_hr[0], f.prop_hr[1]}},
            {"vid", {f.vid[0], f.vid[1]}}, {"caps_fill", f.caps_fill}, {"stack_seed", f.stack_seed}, {"table_seed", f.table_seed},
            {"residue", std::vector<u32>(res.begin(), res.end())}};
}
inline json ObservedRow(const Observed& o) {
    return {{"table", HexBytes(o.table.data(), o.table.size())}, {"dev1", o.dev1}, {"dev2", o.dev2}, {"traps", o.traps}, {"trace", o.trace}};
}
}
