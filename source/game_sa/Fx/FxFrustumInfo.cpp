#include "StdInc.h"
#include "FxFrustumInfo.h"
#include "X87Math.h"

void FxFrustumInfo_c::InjectHooks() {
    RH_ScopedClass(FxFrustumInfo_c);
    RH_ScopedCategory("Fx");
    RH_ScopedNamedInstall(IsCollisionRegs, "IsCollision", 0x4AA030);
}

// 0x4AA030
// El recorrido circular empieza en el ultimo plano que rechazo la esfera; si pasa todos no cambia ese indice.
// Como el original: el cuadrado de la distancia se suma como (z² + y²) + x² y las comparaciones son estrictas (NaN pasa).
// Sin comprobaciones de ejecucion: RTC llamaria a __RTC_CheckEsp y cambiaria ECX/EDX tras la ultima llamada.
#pragma runtime_checks("", off)
uint32 FxFrustumInfo_c::IsCollisionRegs(FxSphere_c& sphere) {
    bool collides = true;
    const float dx = sphere.m_vecCenter.x - m_Sphere.m_vecCenter.x;
    const float dy = sphere.m_vecCenter.y - m_Sphere.m_vecCenter.y;
    const float dz = sphere.m_vecCenter.z - m_Sphere.m_vecCenter.z;
    const float radius = m_Sphere.m_fRadius + sphere.m_fRadius;
    if (!(radius * radius > dz * dz + dy * dy + dx * dx)) {
        collides = false;
    } else {
        uint32 plane = sphere.m_nNumPlanesPassed;
        for (int32 i = 0; i < 4; i++, plane++) {
            const uint32 index = plane & 3;
            const auto&  p     = m_Planes[index];
            const float  distance = p.normal.y * sphere.m_vecCenter.y + p.normal.x * sphere.m_vecCenter.x + p.normal.z * sphere.m_vecCenter.z - p.distance;
            if (distance > sphere.m_fRadius) {
                sphere.m_nNumPlanesPassed = index;
                collides = false;
                break;
            }
        }
    }
    return ResidueRegsCpp(reinterpret_cast<uintptr_t>(this), reinterpret_cast<uintptr_t>(&sphere), collides ? 1u : 0u);
}
#pragma runtime_checks("", restore)
