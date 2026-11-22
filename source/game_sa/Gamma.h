#pragma once
#include "DatosExe/DataBssE1.h"

class CGamma {
public:
    inline static auto& ms_SavedGamma = StaticRef<D3DGAMMARAMP>(0xC8D4C8);
    inline static auto& ms_GammaTable = StaticRef<D3DGAMMARAMP>(0xC8DAC8);

public:
    bool  m_IsEnabled;
    float m_CurrentLevel;
    float m_TargetLevel;
    float m_PrevLevel;

public:
    static void InjectHooks();

    void Init();
    void SetGamma(float gammaLevel, bool fade);
    static void SetGammaNoFade(float gammaLevel);
};

VALIDATE_SIZE(CGamma, 0x10);

inline static auto& gbGammaChanged = DatosExe::Bss::bC920C8;
inline static auto& gamma          = StaticRef<CGamma>(0xC92134);
