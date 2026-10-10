#pragma once

#include "Vector.h"

class CVehicle;
struct RwFrame;

class  CBouncingPanel {
    static inline auto& BOUNCE_SPRING_DAMP_MULT = StaticRef<float>(0x8D3954); // 0.95
    static inline auto& BOUNCE_SPRING_RETURN_MULT = StaticRef<float>(0x8D3958); // 0.1
    static inline auto& BOUNCE_VEL_CHANGE_LIMIT = StaticRef<float>(0x8D395C); // 0.1
    static inline auto& BOUNCE_HANGING_DAMP_MULT = StaticRef<float>(0x8D3960); // 0.98
    static inline auto& BOUNCE_HANGING_RETURN_MULT = StaticRef<float>(0x8D3964); // 0.02

public:
    // Como el original (CAutomobile::CAutomobile 0x6B0AF2): el constructor solo pone m_nFrameId a -1 (mov word ptr [eax],0FFFFh);
    // el resto no se inicializa (SetPanel lo rellena), asi que no se usa inicializacion por valor.
    uint16 m_nFrameId{(uint16)-1};
    uint16 m_nAxis;
    float    m_fAngleLimit;
    CVector  m_vecRotation;
    CVector  m_vecPos;

public:
    CBouncingPanel() = default;

    static void InjectHooks();

    void ResetPanel();
    void SetPanel(int16 frameId, int16 axis, float angleLimit);
    float GetAngleChange(float velocity) const;
    void ProcessPanel(CVehicle* vehicle, RwFrame* frame, CVector arg2, CVector arg3, float arg4, float arg5);
};
VALIDATE_SIZE(CBouncingPanel, 0x20);
