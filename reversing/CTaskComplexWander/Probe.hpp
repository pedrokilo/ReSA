#pragma once
namespace WanderProbe {
inline uint32 calls{};
inline bool recording{};
inline bool light{}, road{};
inline json events = json::array();
}
