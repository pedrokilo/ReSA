/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "SimpleTransform.h"

// 0x54EF40
void CSimpleTransform::UpdateRwMatrix(RwMatrix* out)
{
    // Como el original (0x54EF40): fcos/fsin a precision completa y luego se guardan como float
    const float heading = m_fHeading;
    float sinHeading, cosHeading;
    __asm {
        fld dword ptr [heading]
        fcos
        fstp dword ptr [cosHeading]
        fld dword ptr [heading]
        fsin
        fstp dword ptr [sinHeading]
    }

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
    const float heading = base.m_fHeading, baseX = base.m_vPosn.x, baseY = base.m_vPosn.y;
    float outX, outY;
    __asm {
        fld dword ptr [heading]
        fcos
        fld dword ptr [heading]
        fsin
        fld st(1)
        fmul dword ptr [baseX]
        fchs
        fld st(1)
        fmul dword ptr [baseY]
        fsubp st(1), st(0)
        fstp dword ptr [outX]
        fmul dword ptr [baseX]
        fxch st(1)
        fmul dword ptr [baseY]
        fsubp st(1), st(0)
        fstp dword ptr [outY]
    }
    m_vPosn.x = outX;
    m_vPosn.y = outY;
    m_vPosn.z = -base.m_vPosn.z;
    m_fHeading = -base.m_fHeading;
}

// 0x54F1B0
void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
    out->SetTranslate(m_vPosn);
    out->SetRotateZOnly(m_fHeading);
}
