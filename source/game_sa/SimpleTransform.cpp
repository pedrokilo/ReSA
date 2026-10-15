/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "X87Math.h"

#include "SimpleTransform.h"

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

// 0x54EF90
void CSimpleTransform::Invert(const CSimpleTransform& base)
{
    // Como el original (0x54EF90): fcos/fsin a precision completa y productos redondeados a 24 bits por el x87
    // x e y se calculan en una funcion x87 aparte; base se relee tras escribir x, igual que el EXE (por si base == this)
    SimpleTransformInvertCpp(reinterpret_cast<const float*>(&base), reinterpret_cast<float*>(this));
    m_vPosn.z = -base.m_vPosn.z;
    m_fHeading = -base.m_fHeading;
}

// 0x54F1B0
void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
    out->SetTranslate(m_vPosn);
    out->SetRotateZOnly(m_fHeading);
}
