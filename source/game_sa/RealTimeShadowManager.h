#pragma once

#include "ShadowCamera.h"

class CRealTimeShadow;

class CRealTimeShadowManager {
public:
    static constexpr int32 NUM_REALTIME_SHADOWS = 16;

public:
    char             m_bInitialised{};
    char             m_bNeedsReinit{}; // AKA `m_bNotReInitialized`
    CRealTimeShadow* m_apShadows[NUM_REALTIME_SHADOWS]{};
    CShadowCamera    m_BlurCamera{};
    CShadowCamera    m_GradientCamera{};

public:
    static void InjectHooks();

    // 0x706770 (lote 146A, banco Fn146A): constructor del EXE (thiscall, `ret`). Como el original: pone a cero los dos bytes de estado, las 16 ranuras de sombras y los cuatro punteros de las dos camaras
    // (todo el objeto menos los dos bytes de relleno +2 y +3) y devuelve `this`. Sin llamadores directos de la DLL (el objeto global vive en el EXE).
    CRealTimeShadowManager* Constructor();
    // 0x7067A0 (lote 146A, banco Fn146A): destructor del EXE (thiscall, `ret`): destruye las dos camaras (la de degradado, +0x4C, y luego la de desenfoque, +0x44) con CShadowCamera::Destroy (0x705400).
    void Destructor();

    void Init();
    void Exit();
    void ReInit();
    void Update();

    CRealTimeShadow* GetRealTimeShadow(CPhysical* physical);
    void             DoShadowThisFrame(CPhysical* physical);
    void             ReturnRealTimeShadow(CRealTimeShadow* pShadow);
};
VALIDATE_SIZE(CRealTimeShadowManager, 0x54);

extern CRealTimeShadowManager& g_realTimeShadowMan;
