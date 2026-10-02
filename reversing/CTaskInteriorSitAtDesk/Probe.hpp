#pragma once
namespace SitAtDeskProbe {
inline unsigned calls{};
inline bool recording{};
inline CAnimBlendAssociation* nextAnim{};
inline json events = json::array();
inline void Record(const char* name, float blendDelta, int animId) {
    if (recording) events.push_back({name, blendDelta, animId});
}
}
