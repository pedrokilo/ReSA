#pragma once
namespace GunProbe {
inline unsigned calls{};
inline bool recording{}, controlled{}, startCreates{};
inline CAnimBlendAssociation* nextAnim{};
inline json events = json::array();
inline void Record(const char* name, float value = 0.f) {
    if (recording) events.push_back({name, value});
}
}
