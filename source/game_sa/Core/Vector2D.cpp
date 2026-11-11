/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVector2D
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"
#include "Vector2D.h"
#include "Vector.h"
#include "General.h"

void CVector2D::InjectHooks() {
    RH_ScopedClass(CVector2D);
    RH_ScopedCategory("Core");

    //RH_ScopedInstall(Magnitude, 0x420860); - se instala (como copia fuera de linea con la x87 exacta) en CVector::InjectHooksExactMath, lote 124A
    //RH_ScopedInstall(Normalise, 0x44E480); - Added extra parameter, must unhook
    // operator= (0x43E110) se instala por InjectHooksLote162A (banco Fn162A).
}

// CVector2D::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); esta es la unica entrada de CVector2D que se instala (lote 162A, banco Fn162A): operator= (0x43E110; thiscall, un
// puntero, `ret 4`; copia x e y como dwords y devuelve this).
void CVector2D::InjectHooksLote162A() {
    RH_ScopedClass(CVector2D);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator=, 0x43E110);
}

CVector2D::CVector2D(const CVector& v3) :
    CVector2D{v3.x, v3.y}
{
}

void CVector2D::Normalise(float* outMag) {
    auto mag = Magnitude();
    if (mag > 0.0f) {
        auto recip = 1.0F / mag;
        x *= recip;
        y *= recip;
    } else {
        x = 1.0f;
    }
    if (outMag) {
        *outMag = mag;
    }
}

uint32 CVector2D::NodeHeading() const {
    return CGeneral::GetNodeHeadingFromVector(x, y);
}

CVector2D CVector2D::RotatedBy(float rad) const {
    const auto s = std::sin(rad), c = std::cos(rad);
    return {
        x * c - y * s,
        x * s - y * c,
    };
}

bool CVector2D::ApproxEqualTo(CVector2D o, float epsilon) {
    return approxEqual(x, o.x, epsilon) && approxEqual(y, o.y, epsilon);
}
