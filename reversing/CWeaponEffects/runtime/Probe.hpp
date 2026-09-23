#pragma once
namespace WeaponEffectsProbe {
inline bool recording{};
inline unsigned calls{};
inline json commands = json::array();
inline void Record(const char* kind, std::initializer_list<double> numbers, std::initializer_list<int64_t> exact) {
    if(recording)commands.push_back({{"kind",kind},{"numbers",numbers},{"exact",exact}});
}
}
