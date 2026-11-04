/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSphere
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Sin PCH (CMakeLists.txt): StdInc.h redefine NOMINMAX y el aviso C4005 es error; mismo recurso que Task.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "Sphere.h"
#include "Lines.h"

void CSphere::InjectHooks() {
    RH_ScopedClass(CSphere);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40FCF0);
}

// 0x40FCF0 (thiscall, `ret 8`): radio primero y luego el centro, dword a dword. Los llamadores del EXE conservan ECX (this) tras la llamada (el marco /Od de depuracion lo recarga con
// otro valor): se optimiza por pragma (CMakeLists.txt: /Zi y sin comprobaciones de ejecucion), que deja el cuerpo en EAX/EDX.
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void CSphere::Set(float radius, const CVector& center) {
    m_fRadius = radius;
    // Componente a componente y en este orden, como el EXE (0x40FCF8..0x40FD08): `CVector::operator=` sin expandir en linea es una llamada que reescribe ECX.
    m_vecCenter.x = center.x;
    m_vecCenter.y = center.y;
    m_vecCenter.z = center.z;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)

// NOTSA
bool CSphere::IsPointWithin(const CVector& p) const {
    return (p - m_vecCenter).SquaredMagnitude() <= sq(m_fRadius);
}

// NOTSA
void CSphere::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    const CVector center = transform.TransformPoint(m_vecCenter);
 
    CVector v13 = center;
    v13.z += m_fRadius;

    CVector v21 = center;
    v21.z -= m_fRadius;

    CVector v32 = center;
    v32.x += m_fRadius;

    CVector v41 = center;
    v41.x -= m_fRadius;

    CVector v52 = center;
    v52.y += m_fRadius;

    CVector v61 = center;
    v61.y -= m_fRadius;

    const auto colorARGB = color.ToIntARGB();
    CLines::RenderLineNoClipping(v13, v32, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v13, v41, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v32, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v41, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v13, v52, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v13, v61, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v52, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v61, colorARGB, colorARGB);
}

auto CSphere::GetTransformed(const CMatrix& transform) const -> CSphere {
    return { transform.TransformPoint(m_vecCenter), m_fRadius };
}

auto TransformObject(const CSphere& sp, const CMatrix& transform) -> CSphere {
    return { transform.TransformPoint(sp.m_vecCenter), sp.m_fRadius };
}
