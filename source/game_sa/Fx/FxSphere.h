#pragma once

#include "FxPlane.h"
#include "Sphere.h"

/**
 * Esfera auxiliar usada para bounds/culling del sistema FX.
 *
 * Puede vivir en el heap global o en la arena FX; m_nNumPlanesPassed conserva estado
 * incremental de pruebas contra planos.
 */
class FxSphere_c : public CSphere {
public:
    uint32 m_nNumPlanesPassed; // m_LastPlaneRejected

public:
    static void InjectHooks();

    static void* operator new(uint32 size, bool32 bUseGlobalHeep);
    static void  operator delete(void* data);
    static void  operator delete(void* data, bool32 bUseGlobalHeep);

    FxSphere_c();
    // 0x4A9F70 (lote 148A, banco Fn148A): constructor del EXE (solo anula m_nNumPlanesPassed; centro, radio y planos quedan sin escribir); devuelve this
    FxSphere_c* Constructor() { m_nNumPlanesPassed = 0; return this; }

    // NOTSA
    FxSphere_c(CVector center, float radius) : CSphere(center, radius), m_nNumPlanesPassed(0) {}

    inline bool  IsCollision(const FxSphere_c& sphere) const;
    inline float GetDistToPlane(const FxPlane_c& plane) const;
};

VALIDATE_SIZE(FxSphere_c, 0x14);
