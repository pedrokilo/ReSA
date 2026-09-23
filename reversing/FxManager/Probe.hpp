#pragma once
namespace FxManagerProbe {
inline unsigned calls{}, updateCalls{};
inline bool active{}, invalidateLink{};
inline std::vector<FxSystem_c*> entered, destroyed;
}
