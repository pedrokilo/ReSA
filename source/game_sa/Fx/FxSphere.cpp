/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxSphere_c
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxSphere.h"
#include "FxPlane.h"

/**
 * Registra asignación y pruebas geométricas de las esferas FX.
 */
void FxSphere_c::InjectHooks() {
    RH_ScopedClass(FxSphere_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(operator new, 0x4A9F80);
    RH_ScopedOverloadedInstall(operator delete, "", 0x4A9FB0, void(*)(void*));
    RH_ScopedInstall(IsCollision, 0x4A9FC0);
    RH_ScopedInstall(GetDistToPlane, 0x4AA010);
}

// 0x4A9F70
FxSphere_c::FxSphere_c() {
    m_nNumPlanesPassed = 0;
}

// 0x4A9F80
// Selecciona entre heap global y arena FX según el ciclo de vida solicitado.
void* FxSphere_c::operator new(uint32 size, bool32 bUseGlobalHeep) {
    if (bUseGlobalHeep)
        return CMemoryMgr::Malloc(size);
    else
        return g_fxMan.GetMemPool().GetMem(size);
}

// 0x4A9FB0
void FxSphere_c::operator delete(void* data) {
    CMemoryMgr::Free(data);
}

void FxSphere_c::operator delete(void* data, bool32 bUseGlobalHeep) {
    if (bUseGlobalHeep) {
        CMemoryMgr::Free(data);
    }
}

// 0x4A9FC0
// Comprueba solapamiento estricto comparando distancia al cuadrado y suma de radios.
bool FxSphere_c::IsCollision(const FxSphere_c& sphere) const {
    // Como el original: la distancia al cuadrado se suma en el orden z, y, x.
    const float dx = sphere.m_vecCenter.x - m_vecCenter.x;
    const float dy = sphere.m_vecCenter.y - m_vecCenter.y;
    const float dz = sphere.m_vecCenter.z - m_vecCenter.z;
    const float radii = sphere.m_fRadius + m_fRadius;
    return radii * radii > dz * dz + dy * dy + dx * dx;
}

// 0x4AA010
// Devuelve la distancia firmada del centro al plano; el radio se evalúa externamente.
float FxSphere_c::GetDistToPlane(const FxPlane_c& plane) const {
    // Como el original: (y + x) + z, luego resta la distancia
    return (plane.normal.y * m_vecCenter.y + plane.normal.x * m_vecCenter.x) + plane.normal.z * m_vecCenter.z - plane.distance;
}
