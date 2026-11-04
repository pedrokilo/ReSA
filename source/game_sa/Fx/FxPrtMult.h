#pragma once

#include "RenderWare.h"

class FxPrtMult_c {
public:
    RwRGBAReal m_Color;
    float      m_fSize;
    float      m_Rot;
    float      m_fLife;

public:
    static void InjectHooks();

public:
    FxPrtMult_c();
    FxPrtMult_c(float red, float green, float blue, float alpha, float size, float rot, float life);

    void SetUp(float red, float green, float blue, float alpha, float size, float rot, float life);

    FxPrtMult_c* Constructor();   // 0x4AB270 (hook): construye en el sitio

    inline void SetColor(float red, float green, float blue, float alpha = 1.0f) {
        m_Color.red   = red;
        m_Color.green = green;
        m_Color.blue  = blue;
        m_Color.alpha = alpha;
    }
};

VALIDATE_SIZE(FxPrtMult_c, 0x1C);
