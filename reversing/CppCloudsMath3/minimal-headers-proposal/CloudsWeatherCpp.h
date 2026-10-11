#pragma once
#include "Vector.h"
#include "eWeatherType.h"
// Same types and addresses as Weather.h; avoid its audio/entity include graph.
namespace CloudsWeatherCpp {
    inline auto& CloudCoverage = StaticRef<float>(0xC81304);
    inline auto& ExtraSunnyness = StaticRef<float>(0xC812F8);
    inline auto& Foggyness = StaticRef<float>(0xC81300);
    inline auto& Foggyness_SF = StaticRef<float>(0xC812FC);
    inline auto& NewWeatherType = StaticRef<eWeatherType>(0xC8131C);
    inline auto& OldWeatherType = StaticRef<eWeatherType>(0xC81320);
    inline auto& Rainbow = StaticRef<float>(0xC812E4);
    inline auto& UnderWaterness = StaticRef<float>(0xC8132C);
    inline auto& Wind = StaticRef<float>(0xC812F0);
    inline auto& WindDir = StaticRef<CVector>(0xC813E0);
}
