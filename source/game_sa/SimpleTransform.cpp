/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSimpleTransform
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
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
#include "X87Math.h"

#include "SimpleTransform.h"

// UpdateRwMatrix y UpdateMatrix (lote 102A, banco Fn102A) e Invert (lote 116A, banco Fn116A).
void CSimpleTransform::InjectHooks()
{
    RH_ScopedClass(CSimpleTransform);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(UpdateRwMatrix, 0x54EF40);
    RH_ScopedInstall(UpdateMatrix, 0x54F1B0);
    RH_ScopedInstall(Invert, 0x54EF90);
}

// 0x54EF40
void CSimpleTransform::UpdateRwMatrix(RwMatrix* out)
{
    // Como el original (0x54EF40): fcos/fsin a precision completa y luego se guardan como float
    const float heading = m_fHeading;
    const float sinHeading = TrigSimpleSinCpp(heading), cosHeading = TrigSimpleCosCpp(heading);

    out->right = { cosHeading, sinHeading, 0.0f };
    out->up = { -sinHeading, cosHeading, 0.0f };
    out->at = { 0.0f, 0.0f, 1.0f };
    out->pos = m_vPosn;

    RwMatrixUpdate(out);

}

// Como el original (fld + fchs + fstp de un float): invierte el signo y silencia un NaN senalizante; los desnormales se conservan (con SSE y DAZ se vaciarian).
static float NegateLikeX87(float value)
{
    uint32 bits = std::bit_cast<uint32>(value);
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0) {
        bits |= 0x00400000u;
    }
    return std::bit_cast<float>(bits ^ 0x80000000u);
}

// 0x54EF90
void CSimpleTransform::Invert(const CSimpleTransform& base)
{
    // Como el original (0x54EF90): fcos/fsin a precision completa y productos redondeados a 24 bits por el x87
    // x e y se calculan en una funcion x87 aparte; base se relee tras escribir x, igual que el EXE (por si base == this)
    SimpleTransformInvertCpp(reinterpret_cast<const float*>(&base), reinterpret_cast<float*>(this));
    m_vPosn.z = NegateLikeX87(base.m_vPosn.z);
    m_fHeading = NegateLikeX87(base.m_fHeading);
}

// 0x54F1B0
void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
    out->SetTranslate(m_vPosn);
    out->SetRotateZOnly(m_fHeading);
}
