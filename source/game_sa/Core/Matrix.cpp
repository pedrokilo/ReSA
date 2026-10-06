/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Matrix.h"

uint8* CMatrix::EulerIndices1 = (uint8*)0x866D9C;
uint8* CMatrix::EulerIndices2 = (uint8*)0x866D94;
auto& numMatrices = StaticRef<int32>(0xB74238);
auto& gDummyMatrix = StaticRef<CMatrix>(0xB74240);


void CMatrix::InjectHooks()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Attach, 0x59BD10);
    RH_ScopedInstall(Detach, 0x59ACF0);
    RH_ScopedInstall(CopyOnlyMatrix, 0x59ADD0);
    RH_ScopedInstall(Update, 0x59BB60);
    RH_ScopedInstall(UpdateMatrix, 0x59AD20);
    RH_ScopedInstall(UpdateRW, 0x59BBB0);
    RH_ScopedInstall(UpdateRwMatrix, 0x59AD70);
    RH_ScopedInstall(SetUnity, 0x59AE70);
    RH_ScopedInstall(ResetOrientation, 0x59AEA0);
    RH_ScopedOverloadedInstall(SetScale, "f", 0x59AED0, void(CMatrix::*)(float));
    RH_ScopedOverloadedInstall(SetScale, "fff", 0x59AF00, void(CMatrix::*)(float, float, float));
    RH_ScopedInstall(SetTranslateOnly, 0x59AF80);
    RH_ScopedInstall(SetTranslate, 0x59AF40);
    RH_ScopedInstall(SetRotateXOnly, 0x59AFA0);
    RH_ScopedInstall(SetRotateYOnly, 0x59AFE0);
    RH_ScopedInstall(SetRotateZOnly, 0x59B020);
    RH_ScopedOverloadedInstall(SetRotate, "xyz", 0x59B120, void(CMatrix::*)(float, float, float));
    RH_ScopedInstall(SetRotateX, 0x59B060);
    RH_ScopedInstall(SetRotateY, 0x59B0A0);
    RH_ScopedInstall(SetRotateZ, 0x59B0E0);
    RH_ScopedInstall(RotateX, 0x59B1E0);
    RH_ScopedInstall(RotateY, 0x59B2C0);
    RH_ScopedInstall(RotateZ, 0x59B390);
    RH_ScopedInstall(Rotate, 0x59B460);
    RH_ScopedInstall(Reorthogonalise, 0x59B6A0);
    RH_ScopedInstall(CopyToRwMatrix, 0x59B8B0);
    RH_ScopedOverloadedInstall(SetRotate, "quat", 0x59BBF0, void(CMatrix::*)(const CQuaternion&));
    RH_ScopedInstall(Scale, 0x459350);
    RH_ScopedInstall(ForceUpVector, 0x59B7E0);
    RH_ScopedInstall(ConvertToEulerAngles, 0x59A840);
    RH_ScopedInstall(ConvertFromEulerAngles, 0x59AA40);
    RH_ScopedInstall(operator=, 0x59BBC0);
    RH_ScopedInstall(operator+=, 0x59ADF0);
    RH_ScopedInstall(operator*=, 0x411A80);
    RH_ScopedGlobalOverloadedInstall(operator*, "Mat", 0x59BE30, CMatrix(*)(const CMatrix&, const CMatrix&));
    //RH_ScopedGlobalOverloadedInstall(operator*, "Vec", 0x59C890, CVector(*)(const CMatrix&, const CVector&));
    RH_ScopedGlobalOverloadedInstall(operator+, "", 0x59BFA0, CMatrix(*)(const CMatrix&, const CMatrix&));
    RH_ScopedGlobalOverloadedInstall(Invert, "1", 0x59B920, CMatrix&(*)(CMatrix&, CMatrix&));
    RH_ScopedGlobalOverloadedInstall(Invert, "2", 0x59BDD0, CMatrix(*)(const CMatrix&));
}

CMatrix::CMatrix(const CMatrix& matrix) {
    CMatrix::CopyOnlyMatrix(matrix);
}

// like previous + attach
CMatrix::CMatrix(RwMatrix* matrix, bool temporary) {
    CMatrix::Attach(matrix, temporary);
}

// destructor detaches matrix if attached 
CMatrix::~CMatrix()
{
    CMatrix::Detach();
}

void CMatrix::Attach(RwMatrix* matrix, bool bOwnsMatrix)
{
    CMatrix::Detach();

    m_pAttachMatrix = matrix;
    m_bOwnsAttachedMatrix = bOwnsMatrix;
    CMatrix::Update();
}

void CMatrix::Detach()
{
    if (m_bOwnsAttachedMatrix && m_pAttachMatrix)
        RwMatrixDestroy(m_pAttachMatrix);

    m_pAttachMatrix = nullptr;
}

// copy base RwMatrix to another matrix
void CMatrix::CopyOnlyMatrix(const CMatrix& matrix)
{
    memcpy(this, &matrix, sizeof(RwMatrix));
}

// update RwMatrix with attaching matrix. This doesn't check if attaching matrix is present, so use it only if you know it is present.
// Using UpdateRW() is more safe since it perform this check.
void CMatrix::Update()
{
    CMatrix::UpdateMatrix(m_pAttachMatrix);
}

// update RwMatrix with attaching matrix.
void CMatrix::UpdateRW()
{
    if (!m_pAttachMatrix)
        return;

    CMatrix::UpdateRwMatrix(m_pAttachMatrix);
}

// update RwMatrix with this matrix
void CMatrix::UpdateRwMatrix(RwMatrix* matrix) const
{
    *RwMatrixGetRight(matrix) = m_right;
    *RwMatrixGetUp(matrix) = m_forward;
    *RwMatrixGetAt(matrix) = m_up;
    *RwMatrixGetPos(matrix) = m_pos;

    RwMatrixUpdate(matrix);
}

void CMatrix::UpdateMatrix(RwMatrix* rwMatrix)
{
    m_right = *RwMatrixGetRight(rwMatrix);
    m_forward = *RwMatrixGetUp(rwMatrix);
    m_up = *RwMatrixGetAt(rwMatrix);
    m_pos = *RwMatrixGetPos(rwMatrix);
}

void CMatrix::SetUnity()
{
    CMatrix::ResetOrientation();
    m_pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::ResetOrientation()
{
    m_right.Set  (1.0F, 0.0F, 0.0F);
    m_forward.Set(0.0F, 1.0F, 0.0F);
    m_up.Set     (0.0F, 0.0F, 1.0F);
}

void CMatrix::SetScale(float scale)
{
    m_right.Set  (scale, 0.0F,  0.0F);
    m_forward.Set(0.0F,  scale, 0.0F);
    m_up.Set     (0.0F,  0.0F,  scale);
    m_pos.Set    (0.0F,  0.0F,  0.0F);
}

// scale on three axes
void CMatrix::SetScale(float x, float y, float z)
{
    m_right.Set  (x,     0.0F,  0.0F);
    m_forward.Set(0.0F,  y,     0.0F);
    m_up.Set     (0.0F,  0.0F,  z   );
    m_pos.Set    (0.0F,  0.0F,  0.0F);
}

void CMatrix::SetTranslateOnly(CVector translation)
{
    m_pos = translation;
}

// like previous + reset orientation
void CMatrix::SetTranslate(CVector translation)
{
    CMatrix::ResetOrientation();
    CMatrix::SetTranslateOnly(translation);
}

// 0x59AFA0
// Como el original: fsin/fcos del x87 (precisión interna completa) guardados a float con fstp, sin el
// sin/cos de la CRT, y los mismos campos escritos (sin tocar el relleno ni la posición).
__declspec(naked) void CMatrix::SetRotateXOnly(float angle)
{
    __asm {
        fld dword ptr [esp+4]
        xor eax,eax
        fcos
        mov dword ptr [ecx],3F800000h
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],eax
        mov dword ptr [ecx+10h],eax
        mov dword ptr [ecx+20h],eax
        fld dword ptr [esp+4]
        fsin
        fld st(1)
        fstp dword ptr [ecx+14h]
        fst dword ptr [ecx+18h]
        fchs
        fstp dword ptr [ecx+24h]
        fstp dword ptr [ecx+28h]
        ret 4
    }
}

// 0x59AFE0
// Como el original: igual que SetRotateXOnly, con fsin/fcos del x87.
__declspec(naked) void CMatrix::SetRotateYOnly(float angle)
{
    __asm {
        fld dword ptr [esp+4]
        xor eax,eax
        fcos
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+10h],eax
        mov dword ptr [ecx+14h],3F800000h
        mov dword ptr [ecx+18h],eax
        mov dword ptr [ecx+24h],eax
        fld dword ptr [esp+4]
        fsin
        fld st(1)
        fstp dword ptr [ecx]
        fld st(0)
        fchs
        fstp dword ptr [ecx+8]
        fstp dword ptr [ecx+20h]
        fstp dword ptr [ecx+28h]
        ret 4
    }
}

// 0x59B020
// Como el original: fsin/fcos del x87 (precisión interna completa) guardados a float con fstp,
// y los mismos campos escritos (sin tocar el relleno ni la posición).
__declspec(naked) void CMatrix::SetRotateZOnly(float angle)
{
    __asm {
        fld dword ptr [esp+4]
        xor eax,eax
        fcos
        mov dword ptr [ecx+8],eax
        mov dword ptr [ecx+18h],eax
        mov dword ptr [ecx+20h],eax
        mov dword ptr [ecx+24h],eax
        mov dword ptr [ecx+28h],3F800000h
        fld dword ptr [esp+4]
        fsin
        fld st(1)
        fstp dword ptr [ecx]
        fst dword ptr [ecx+4]
        fchs
        fstp dword ptr [ecx+10h]
        fstp dword ptr [ecx+14h]
        ret 4
    }
}

void CMatrix::SetRotateX(float angle)
{
    CMatrix::SetRotateXOnly(angle);
    m_pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::SetRotateY(float angle)
{
    CMatrix::SetRotateYOnly(angle);
    m_pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::SetRotateZ(float angle)
{
    CMatrix::SetRotateZOnly(angle);
    m_pos.Set(0.0F, 0.0F, 0.0F);
}

// set rotate on 3 axes
// 0x59B120
// Como el original: transcripción exacta del x87 del EXE (fsin/fcos a precisión interna completa, sin
// redondear a float antes de los productos), para obtener los mismos bits.
__declspec(naked) void CMatrix::SetRotate(float x, float y, float z)
{
    __asm {
        fld dword ptr [esp+4]
        xor eax,eax
        fcos
        mov dword ptr [ecx+30h],eax
        mov dword ptr [ecx+34h],eax
        mov dword ptr [ecx+38h],eax
        fld dword ptr [esp+4]
        fsin
        fld dword ptr [esp+8]
        fcos
        fstp dword ptr [esp+4]
        fld dword ptr [esp+8]
        fsin
        fld dword ptr [esp+0Ch]
        fcos
        fld dword ptr [esp+0Ch]
        fsin
        fst dword ptr [esp+8]
        fmul st,st(3)
        fld st(1)
        fmul st,st(4)
        fstp dword ptr [esp+0Ch]
        fld st(1)
        fmul dword ptr [esp+4]
        fld st(1)
        fmul st,st(4)
        fsubp st(1),st
        fstp dword ptr [ecx]
        fld dword ptr [esp+0Ch]
        fmul st,st(3)
        fld dword ptr [esp+8]
        fmul dword ptr [esp+4]
        faddp st(1),st
        fstp dword ptr [ecx+4]
        fld st(2)
        fmul st,st(5)
        fchs
        fstp dword ptr [ecx+8]
        fld dword ptr [esp+8]
        fmul st,st(5)
        fchs
        fstp dword ptr [ecx+10h]
        fld st(1)
        fmul st,st(5)
        fstp dword ptr [ecx+14h]
        fxch st(3)
        fstp dword ptr [ecx+18h]
        fmul st,st(1)
        fxch st(2)
        fmul dword ptr [esp+4]
        faddp st(2),st
        fxch st(1)
        fstp dword ptr [ecx+20h]
        fld dword ptr [esp+8]
        fmul st,st(1)
        fld dword ptr [esp+0Ch]
        fmul dword ptr [esp+4]
        fsubp st(1),st
        fstp dword ptr [ecx+24h]
        fstp st(0)
        fld dword ptr [esp+4]
        fmul st,st(1)
        fstp dword ptr [ecx+28h]
        fstp st(0)
        ret 0Ch
    }
}

// 0x59B1E0
// Como el original: transcripción exacta del x87 del EXE. fcos se queda en la pila sin redondear a float,
// fsin se guarda como float (fstp); los productos se redondean a 24 bits como en el EXE y la posición también se rota.
__declspec(naked) void CMatrix::RotateX(float angle)
{
    __asm {
        sub esp,20h
        fld dword ptr [esp+24h]
        fcos
        fld dword ptr [esp+24h]
        fsin
        fstp dword ptr [esp+24h]
        fld st(0)
        fmul dword ptr [ecx+4]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+8]
        fsubp st(1),st
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+4]
        fld st(2)
        fmul dword ptr [ecx+8]
        faddp st(1),st
        fld st(2)
        fmul dword ptr [ecx+14h]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+18h]
        fsubp st(1),st
        fstp dword ptr [esp+8]
        fld dword ptr [esp+24h]
        mov eax,dword ptr [esp+8]
        fmul dword ptr [ecx+14h]
        mov dword ptr [ecx+14h],eax
        fld st(3)
        fmul dword ptr [ecx+18h]
        faddp st(1),st
        fstp dword ptr [esp+0Ch]
        fld st(2)
        mov edx,dword ptr [esp+0Ch]
        fmul dword ptr [ecx+24h]
        mov dword ptr [ecx+18h],edx
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+28h]
        fsubp st(1),st
        fstp dword ptr [esp+10h]
        fld dword ptr [esp+24h]
        mov eax,dword ptr [esp+10h]
        fmul dword ptr [ecx+24h]
        mov dword ptr [ecx+24h],eax
        fld st(3)
        fmul dword ptr [ecx+28h]
        faddp st(1),st
        fstp dword ptr [esp+14h]
        mov edx,dword ptr [esp+14h]
        fld st(2)
        fmul dword ptr [ecx+34h]
        mov dword ptr [ecx+28h],edx
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+38h]
        fsubp st(1),st
        fstp dword ptr [esp+18h]
        fld dword ptr [esp+24h]
        mov eax,dword ptr [esp+18h]
        fmul dword ptr [ecx+34h]
        mov dword ptr [ecx+34h],eax
        fxch st(3)
        fmul dword ptr [ecx+38h]
        faddp st(3),st
        fxch st(2)
        fstp dword ptr [esp+1Ch]
        mov edx,dword ptr [esp+1Ch]
        fstp dword ptr [ecx+4]
        mov dword ptr [ecx+38h],edx
        fstp dword ptr [ecx+8]
        add esp,20h
        ret 4
    }
}

// 0x59B2C0
// Como el original: transcripción exacta del x87 del EXE. fcos se queda en la pila sin redondear a float,
// fsin se guarda como float con fst pero right.x usa el seno sin redondear; los productos se redondean a 24 bits como en el EXE y la posición también se rota.
__declspec(naked) void CMatrix::RotateY(float angle)
{
    __asm {
        sub esp,20h
        fld dword ptr [esp+24h]
        fcos
        fld dword ptr [esp+24h]
        fsin
        fst dword ptr [esp+24h]
        fmul dword ptr [ecx+8]
        fld st(1)
        fmul dword ptr [ecx]
        faddp st(1),st
        fld st(1)
        fmul dword ptr [ecx+8]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx]
        fsubp st(1),st
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+18h]
        fld st(3)
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fstp dword ptr [esp+8]
        fld st(2)
        mov eax,dword ptr [esp+8]
        fmul dword ptr [ecx+18h]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+10h]
        mov dword ptr [ecx+10h],eax
        fsubp st(1),st
        fstp dword ptr [esp+0Ch]
        fld dword ptr [esp+24h]
        mov edx,dword ptr [esp+0Ch]
        fmul dword ptr [ecx+28h]
        mov dword ptr [ecx+18h],edx
        fld st(3)
        fmul dword ptr [ecx+20h]
        faddp st(1),st
        fstp dword ptr [esp+10h]
        fld st(2)
        mov eax,dword ptr [esp+10h]
        fmul dword ptr [ecx+28h]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+20h]
        mov dword ptr [ecx+20h],eax
        fsubp st(1),st
        fstp dword ptr [esp+14h]
        fld dword ptr [esp+24h]
        mov edx,dword ptr [esp+14h]
        fmul dword ptr [ecx+38h]
        mov dword ptr [ecx+28h],edx
        fld st(3)
        fmul dword ptr [ecx+30h]
        faddp st(1),st
        fstp dword ptr [esp+18h]
        mov eax,dword ptr [esp+18h]
        fxch st(2)
        fmul dword ptr [ecx+38h]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+30h]
        mov dword ptr [ecx+30h],eax
        fsubp st(1),st
        fstp dword ptr [esp+1Ch]
        mov edx,dword ptr [esp+1Ch]
        fstp dword ptr [ecx]
        mov dword ptr [ecx+38h],edx
        fstp dword ptr [ecx+8]
        add esp,20h
        ret 4
    }
}

// 0x59B390
// Como el original: transcripción exacta del x87 del EXE. fcos se queda en la pila sin redondear a float,
// fsin se guarda como float (fstp); los productos se redondean a 24 bits como en el EXE y la posición también se rota.
__declspec(naked) void CMatrix::RotateZ(float angle)
{
    __asm {
        sub esp,20h
        fld dword ptr [esp+24h]
        fcos
        fld dword ptr [esp+24h]
        fsin
        fstp dword ptr [esp+24h]
        fld st(0)
        fmul dword ptr [ecx]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+4]
        fsubp st(1),st
        fld st(1)
        fmul dword ptr [ecx+4]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx]
        faddp st(1),st
        fld st(2)
        fmul dword ptr [ecx+10h]
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+14h]
        fsubp st(1),st
        fstp dword ptr [esp+8]
        fld dword ptr [esp+24h]
        mov eax,dword ptr [esp+8]
        fmul dword ptr [ecx+10h]
        mov dword ptr [ecx+10h],eax
        fld st(3)
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fstp dword ptr [esp+0Ch]
        fld st(2)
        mov edx,dword ptr [esp+0Ch]
        fmul dword ptr [ecx+20h]
        mov dword ptr [ecx+14h],edx
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+24h]
        fsubp st(1),st
        fstp dword ptr [esp+10h]
        fld dword ptr [esp+24h]
        mov eax,dword ptr [esp+10h]
        fmul dword ptr [ecx+20h]
        mov dword ptr [ecx+20h],eax
        fld st(3)
        fmul dword ptr [ecx+24h]
        faddp st(1),st
        fstp dword ptr [esp+14h]
        mov edx,dword ptr [esp+14h]
        fld st(2)
        fmul dword ptr [ecx+30h]
        mov dword ptr [ecx+24h],edx
        fld dword ptr [esp+24h]
        fmul dword ptr [ecx+34h]
        fsubp st(1),st
        fstp dword ptr [esp+18h]
        fld dword ptr [esp+24h]
        mov eax,dword ptr [esp+18h]
        fmul dword ptr [ecx+30h]
        mov dword ptr [ecx+30h],eax
        fxch st(3)
        fmul dword ptr [ecx+34h]
        faddp st(3),st
        fxch st(2)
        fstp dword ptr [esp+1Ch]
        mov edx,dword ptr [esp+1Ch]
        fstp dword ptr [ecx]
        mov dword ptr [ecx+34h],edx
        fstp dword ptr [ecx+4]
        add esp,20h
        ret 4
    }
}

// rotate on 3 axes
void CMatrix::Rotate(CVector rotation)
{
    auto rotMat = CMatrix();
    rotMat.SetRotate(rotation.x, rotation.y, rotation.z);
    m_right =   rotMat.TransformVector(m_right);
    m_forward = rotMat.TransformVector(m_forward);
    m_up =      rotMat.TransformVector(m_up);
    m_pos =     rotMat.TransformVector(m_pos);
}

// 0x59B6A0
// Como el original: transcripción exacta del x87 del EXE (productos vectoriales en línea, normalización
// sin comprobar cero con la suma z² + y² + x² y el recíproco 1/sqrt), para obtener los mismos bits.
static const float s_ReorthogonaliseOne = 1.0f;
__declspec(naked) void CMatrix::Reorthogonalise()
{
    __asm {
        sub esp,28h
        fld dword ptr [ecx+4]
        fmul dword ptr [ecx+18h]
        fld dword ptr [ecx+14h]
        fmul dword ptr [ecx+8]
        fsubp st(1),st
        fld dword ptr [ecx+10h]
        fmul dword ptr [ecx+8]
        fld dword ptr [ecx]
        fmul dword ptr [ecx+18h]
        fsubp st(1),st
        fstp dword ptr [esp+8]
        fld dword ptr [ecx+14h]
        fmul dword ptr [ecx]
        fld dword ptr [ecx+10h]
        fmul dword ptr [ecx+4]
        fsubp st(1),st
        fst dword ptr [esp+0Ch]
        fmul dword ptr [esp+0Ch]
        fld dword ptr [esp+8]
        fmul dword ptr [esp+8]
        faddp st(1),st
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fsqrt
        fdivr s_ReorthogonaliseOne
        fst dword ptr [esp]
        fmulp st(1),st
        fld dword ptr [esp]
        fmul dword ptr [esp+8]
        fld dword ptr [esp]
        fmul dword ptr [esp+0Ch]
        fst dword ptr [esp+0Ch]
        fmul dword ptr [ecx+14h]
        fld st(1)
        fmul dword ptr [ecx+18h]
        fsubp st(1),st
        fld st(2)
        fmul dword ptr [ecx+18h]
        fld dword ptr [esp+0Ch]
        fmul dword ptr [ecx+10h]
        fsubp st(1),st
        fstp dword ptr [esp+14h]
        fld st(1)
        fmul dword ptr [ecx+10h]
        fld st(3)
        fmul dword ptr [ecx+14h]
        fsubp st(1),st
        fst dword ptr [esp+18h]
        fmul dword ptr [esp+18h]
        fld dword ptr [esp+14h]
        fmul dword ptr [esp+14h]
        faddp st(1),st
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fsqrt
        fdivr s_ReorthogonaliseOne
        fstp dword ptr [esp]
        fmul dword ptr [esp]
        fld dword ptr [esp+14h]
        fmul dword ptr [esp]
        fld dword ptr [esp+18h]
        fmul dword ptr [esp]
        fst dword ptr [esp+18h]
        fmul st,st(3)
        fld st(1)
        fmul dword ptr [esp+0Ch]
        fsubp st(1),st
        fstp dword ptr [esp+1Ch]
        fld st(1)
        fmul dword ptr [esp+0Ch]
        fld dword ptr [esp+18h]
        fmul st,st(5)
        fsubp st(1),st
        mov eax,dword ptr [esp+18h]
        fstp dword ptr [esp+20h]
        mov edx,dword ptr [esp+1Ch]
        fld st(0)
        mov dword ptr [ecx+8],eax
        mov eax,dword ptr [esp+20h]
        fmul st,st(4)
        fld st(2)
        mov dword ptr [ecx+10h],edx
        fmul st,st(4)
        mov dword ptr [ecx+14h],eax
        mov eax,dword ptr [esp+0Ch]
        mov dword ptr [ecx+28h],eax
        fsubp st(1),st
        fstp dword ptr [esp+24h]
        mov edx,dword ptr [esp+24h]
        fxch st(1)
        fstp dword ptr [ecx]
        mov dword ptr [ecx+18h],edx
        fstp dword ptr [ecx+4]
        fxch st(1)
        fstp dword ptr [ecx+20h]
        fstp dword ptr [ecx+24h]
        add esp,28h
        ret
    }
}

// similar to UpdateRW(RwMatrixTag *)
void CMatrix::CopyToRwMatrix(RwMatrix* matrix) const
{
    UpdateRwMatrix(matrix);
    RwMatrixUpdate(matrix);
}

void CMatrix::SetRotate(const CQuaternion& quat)
{
    auto vecImag2 = quat.imag + quat.imag;
    auto x2x = vecImag2.x * quat.imag.x;
    auto y2x = vecImag2.y * quat.imag.x;
    auto z2x = vecImag2.z * quat.imag.x;

    auto y2y = vecImag2.y * quat.imag.y;
    auto z2y = vecImag2.z * quat.imag.y;
    auto z2z = vecImag2.z * quat.imag.z;

    auto x2r = vecImag2.x * quat.real;
    auto y2r = vecImag2.y * quat.real;
    auto z2r = vecImag2.z * quat.real;

    m_right.Set  (1.0F-(z2z+y2y),   z2r+y2x,        z2x-y2r);
    m_forward.Set(y2x-z2r,          1.0F-(z2z+x2x), x2r+z2y);
    m_up.Set     (y2r+z2x,          z2y-x2r,        1.0F-(y2y+x2x));
}

void CMatrix::Scale(float scale) {
    ScaleXYZ(scale, scale, scale);
}

void CMatrix::ScaleXYZ(float x, float y, float z) {
    m_right   *= x;
    m_forward *= y;
    m_up      *= z;
}

void CMatrix::ForceUpVector(CVector vecUp) {
    m_right   = CrossProduct(m_forward, vecUp);
    m_forward = CrossProduct(vecUp, m_right);
    m_up      = vecUp;
}

// Como el original: tablas de indices del EXE (0x866D9C y 0x866D94) y epsilon 0x866D90 (1,9073486e-6)
static const uint8 EulerIdx1[4]{0, 1, 2, 0};
static const uint8 EulerIdx2[4]{1, 2, 0, 1};
static const float kEulerEps = 1.9073486328125e-06f;

// 0x59A840: transcripcion x87 del EXE (fsqrt/productos a la precision vigente, fpatan completo; el
// orden de rama y los indices [fila][columna] son los del original, p. ej. Z en Euler usa atan2(m[i2][i1], -m[i3][i1])).
__declspec(naked) void CMatrix::ConvertToEulerAngles(float* pX, float* pY, float* pZ, uint32 uiFlags)
{
    __asm {
        sub   esp, 28h
        mov   eax, dword ptr [ecx]
        mov   edx, dword ptr [ecx+4]
        mov   dword ptr [esp+4], eax
        mov   eax, dword ptr [ecx+8]
        mov   dword ptr [esp+0Ch], eax
        mov   eax, dword ptr [ecx+14h]
        mov   dword ptr [esp+14h], eax
        mov   eax, dword ptr [ecx+20h]
        mov   dword ptr [esp+8], edx
        mov   edx, dword ptr [ecx+10h]
        mov   dword ptr [esp+1Ch], eax
        mov   eax, dword ptr [ecx+28h]
        push  ebx
        mov   dword ptr [esp+14h], edx
        mov   edx, dword ptr [ecx+18h]
        mov   dword ptr [esp+28h], eax
        mov   eax, dword ptr [esp+3Ch]
        push  ebp
        mov   dword ptr [esp+20h], edx
        mov   edx, dword ptr [ecx+24h]
        mov   ecx, eax
        shr   eax, 1
        push  esi
        push  edi
        mov   edi, eax
        shr   eax, 1
        mov   ebx, eax
        shr   eax, 1
        and   ecx, 1
        and   eax, 3
        movzx esi, byte ptr [EulerIdx1+eax]
        mov   dword ptr [esp+10h], ecx
        and   ebx, 1
        mov   ecx, esi
        and   edi, 1
        sub   ecx, ebx
        cmp   edi, 1
        movzx ecx, byte ptr [EulerIdx2+1+ecx]
        mov   dword ptr [esp+30h], edx
        movzx edx, byte ptr [EulerIdx2+esi+ebx]
        jne   L_tait
        lea   eax, [esi+esi*2]
        lea   edi, [eax+ecx]
        fld   dword ptr [esp+edi*4+14h]
        lea   ebp, [eax+edx]
        fld   dword ptr [esp+ebp*4+14h]
        lea   edi, [esp+edi*4+14h]
        fld   st(0)
        lea   ebp, [esp+ebp*4+14h]
        fmul  st, st(1)
        fld   st(2)
        fmul  st, st(3)
        faddp st(1), st
        fsqrt
        fstp  st(2)
        fstp  st(0)
        fcom  dword ptr [kEulerEps]
        fnstsw ax
        test  ah, 41h
        jne   L_eSmall
        fld   dword ptr [ebp]
        mov   eax, dword ptr [esp+3Ch]
        fld   dword ptr [edi]
        mov   edi, esi
        fpatan
        shl   edi, 4
        lea   ebp, [esi+edx*2]
        add   edx, ebp
        fstp  dword ptr [eax]
        fld   dword ptr [esp+edi+14h]
        mov   edi, dword ptr [esp+40h]
        fpatan
        fstp  dword ptr [edi]
        fld   dword ptr [esp+edx*4+14h]
        lea   edx, [esi+ecx*2]
        add   ecx, edx
        fld   dword ptr [esp+ecx*4+14h]
        fchs
        mov   ecx, dword ptr [esp+44h]
        fpatan
        fstp  dword ptr [ecx]
        jmp   L_post
    L_eSmall:
        lea   eax, [ecx+edx*2]
        mov   ecx, edx
        add   ecx, eax
        fld   dword ptr [esp+ecx*4+14h]
        shl   edx, 4
        fchs
        mov   eax, dword ptr [esp+3Ch]
        fld   dword ptr [esp+edx+14h]
        fpatan
        shl   esi, 4
        fstp  dword ptr [eax]
        fld   dword ptr [esp+esi+14h]
        jmp   L_atanY
    L_tait:
        lea   edi, [edx+edx*2]
        mov   eax, esi
        shl   eax, 4
        lea   ebp, [edi+esi]
        fld   dword ptr [esp+ebp*4+14h]
        lea   eax, [esp+eax+14h]
        fld   dword ptr [eax]
        lea   ebp, [esp+ebp*4+14h]
        fld   st(1)
        mov   dword ptr [esp+48h], eax
        fmulp st(2), st
        fld   st(0)
        fmul  st, st(1)
        faddp st(2), st
        fxch  st(1)
        fsqrt
        fstp  st(1)
        fcom  dword ptr [kEulerEps]
        fnstsw ax
        test  ah, 41h
        mov   eax, dword ptr [esp+3Ch]
        jne   L_tSmall
        lea   edi, [ecx+ecx*2]
        add   edx, edi
        fld   dword ptr [esp+edx*4+14h]
        shl   ecx, 4
        fld   dword ptr [esp+ecx+14h]
        fpatan
        add   edi, esi
        mov   ecx, dword ptr [esp+48h]
        fstp  dword ptr [eax]
        fld   dword ptr [esp+edi*4+14h]
        mov   edi, dword ptr [esp+40h]
        fchs
        fxch  st(1)
        fpatan
        fstp  dword ptr [edi]
        fld   dword ptr [ebp]
        fld   dword ptr [ecx]
        mov   ecx, dword ptr [esp+44h]
        fpatan
        fstp  dword ptr [ecx]
        jmp   L_post
    L_tSmall:
        add   edi, ecx
        fld   dword ptr [esp+edi*4+14h]
        shl   edx, 4
        fchs
        fld   dword ptr [esp+edx+14h]
        fpatan
        lea   edx, [esi+ecx*2]
        add   ecx, edx
        fstp  dword ptr [eax]
        fld   dword ptr [esp+ecx*4+14h]
        fchs
        fxch  st(1)
    L_atanY:
        fpatan
        mov   edi, dword ptr [esp+40h]
        mov   ecx, dword ptr [esp+44h]
        fstp  dword ptr [edi]
        mov   dword ptr [ecx], 0
    L_post:
        cmp   ebx, 1
        jne   L_noneg
        fld   dword ptr [eax]
        fchs
        fstp  dword ptr [eax]
        fld   dword ptr [edi]
        fchs
        fstp  dword ptr [edi]
        fld   dword ptr [ecx]
        fchs
        fstp  dword ptr [ecx]
    L_noneg:
        cmp   dword ptr [esp+10h], 1
        pop   edi
        pop   esi
        pop   ebp
        pop   ebx
        jne   L_end
        mov   edx, dword ptr [ecx]
        fld   dword ptr [eax]
        mov   dword ptr [eax], edx
        fstp  dword ptr [ecx]
    L_end:
        add   esp, 28h
        ret   10h
    }
}

// 0x59AA40: transcripcion x87 del EXE (fsin/fcos a precision completa, productos y sumas en el orden del original;
// sinZ queda en la pila x87 sin redondear a float entre sus usos).
__declspec(naked) void CMatrix::ConvertFromEulerAngles(float x, float y, float z, uint32 uiFlags)
{
    __asm {
        mov   eax, dword ptr [esp+10h]
        sub   esp, 38h
        push  ebx
        mov   ebx, eax
        shr   eax, 1
        push  ebp
        mov   ebp, eax
        shr   eax, 1
        push  esi
        push  edi
        mov   edi, eax
        shr   eax, 1
        and   eax, 3
        movzx edx, byte ptr [EulerIdx1+eax]
        and   edi, 1
        movzx esi, byte ptr [EulerIdx2+edx+edi]
        mov   eax, edx
        and   ebx, 1
        sub   eax, edi
        movzx eax, byte ptr [EulerIdx2+1+eax]
        and   ebp, 1
        cmp   ebx, 1
        jne   L_f1
        mov   ebx, dword ptr [esp+4Ch]
        fld   dword ptr [esp+54h]
        mov   dword ptr [esp+4Ch], ebx
        mov   dword ptr [esp+54h], ebx
        jmp   L_f2
    L_f1:
        fld   dword ptr [esp+4Ch]
    L_f2:
        cmp   edi, 1
        jne   L_f3
        fchs
        fld   dword ptr [esp+50h]
        fchs
        fld   dword ptr [esp+54h]
        fchs
        fstp  dword ptr [esp+54h]
        jmp   L_f4
    L_f3:
        fld   dword ptr [esp+50h]
    L_f4:
        fld   st(1)
        mov   edi, edx
        fcos
        shl   edi, 4
        cmp   ebp, 1
        fstp  dword ptr [esp+20h]
        fld   st(0)
        fcos
        fstp  dword ptr [esp+50h]
        fld   dword ptr [esp+54h]
        fcos
        fstp  dword ptr [esp+58h]
        fxch  st(1)
        fsin
        fstp  dword ptr [esp+1Ch]
        fsin
        fstp  dword ptr [esp+4Ch]
        fld   dword ptr [esp+54h]
        fsin
        fld   dword ptr [esp+58h]
        fmul  dword ptr [esp+20h]
        fstp  dword ptr [esp+10h]
        fld   dword ptr [esp+20h]
        fmul  st, st(1)
        fstp  dword ptr [esp+14h]
        fld   dword ptr [esp+1Ch]
        fmul  dword ptr [esp+58h]
        fstp  dword ptr [esp+18h]
        fld   dword ptr [esp+1Ch]
        fmul  st, st(1)
        fstp  dword ptr [esp+54h]
        jne   L_fT
        fld   dword ptr [esp+50h]
        fstp  dword ptr [esp+edi+24h]
        lea   edi, [edx+edx*2]
        fld   dword ptr [esp+4Ch]
        lea   ebx, [edi+esi]
        fmul  dword ptr [esp+1Ch]
        add   edi, eax
        fstp  dword ptr [esp+ebx*4+24h]
        fld   dword ptr [esp+4Ch]
        fmul  dword ptr [esp+20h]
        fstp  dword ptr [esp+edi*4+24h]
        lea   edi, [esi+esi*2]
        lea   ebx, [edi+edx]
        fmul  dword ptr [esp+4Ch]
        add   edi, eax
        fstp  dword ptr [esp+ebx*4+24h]
        mov   ebx, esi
        fld   dword ptr [esp+54h]
        shl   ebx, 4
        fmul  dword ptr [esp+50h]
        fsubr dword ptr [esp+10h]
        fstp  dword ptr [esp+ebx+24h]
        fld   dword ptr [esp+14h]
        fmul  dword ptr [esp+50h]
        fchs
        fsub  dword ptr [esp+18h]
        fstp  dword ptr [esp+edi*4+24h]
        lea   edi, [eax+eax*2]
        fld   dword ptr [esp+4Ch]
        add   edx, edi
        fmul  dword ptr [esp+58h]
        add   edi, esi
        fchs
        fstp  dword ptr [esp+edx*4+24h]
        fld   dword ptr [esp+18h]
        fmul  dword ptr [esp+50h]
        fadd  dword ptr [esp+14h]
        fstp  dword ptr [esp+edi*4+24h]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [esp+50h]
        fsub  dword ptr [esp+54h]
        jmp   L_fEnd
    L_fT:
        fld   dword ptr [esp+58h]
        fmul  dword ptr [esp+50h]
        fstp  dword ptr [esp+edi+24h]
        lea   edi, [edx+edx*2]
        fld   dword ptr [esp+18h]
        lea   ebx, [edi+esi]
        fmul  dword ptr [esp+4Ch]
        add   edi, eax
        fsub  dword ptr [esp+14h]
        fstp  dword ptr [esp+ebx*4+24h]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [esp+4Ch]
        fadd  dword ptr [esp+54h]
        fstp  dword ptr [esp+edi*4+24h]
        lea   edi, [esi+esi*2]
        lea   ebx, [edi+edx]
        fmul  dword ptr [esp+50h]
        add   edi, eax
        fstp  dword ptr [esp+ebx*4+24h]
        mov   ebx, esi
        fld   dword ptr [esp+54h]
        shl   ebx, 4
        fmul  dword ptr [esp+4Ch]
        fadd  dword ptr [esp+10h]
        fstp  dword ptr [esp+ebx+24h]
        fld   dword ptr [esp+14h]
        fmul  dword ptr [esp+4Ch]
        fsub  dword ptr [esp+18h]
        fstp  dword ptr [esp+edi*4+24h]
        lea   edi, [eax+eax*2]
        fld   dword ptr [esp+4Ch]
        add   edx, edi
        fchs
        add   edi, esi
        fstp  dword ptr [esp+edx*4+24h]
        fld   dword ptr [esp+1Ch]
        fmul  dword ptr [esp+50h]
        fstp  dword ptr [esp+edi*4+24h]
        fld   dword ptr [esp+50h]
        fmul  dword ptr [esp+20h]
    L_fEnd:
        shl   eax, 4
        fstp  dword ptr [esp+eax+24h]
        mov   eax, dword ptr [esp+24h]
        mov   edx, dword ptr [esp+28h]
        mov   dword ptr [ecx], eax
        mov   eax, dword ptr [esp+2Ch]
        mov   dword ptr [ecx+4], edx
        mov   edx, dword ptr [esp+30h]
        mov   dword ptr [ecx+8], eax
        mov   eax, dword ptr [esp+34h]
        pop   edi
        mov   dword ptr [ecx+10h], edx
        mov   edx, dword ptr [esp+34h]
        mov   dword ptr [ecx+14h], eax
        mov   eax, dword ptr [esp+38h]
        pop   esi
        mov   dword ptr [ecx+18h], edx
        mov   edx, dword ptr [esp+38h]
        mov   dword ptr [ecx+20h], eax
        mov   eax, dword ptr [esp+3Ch]
        pop   ebp
        mov   dword ptr [ecx+24h], edx
        mov   dword ptr [ecx+28h], eax
        pop   ebx
        add   esp, 38h
        ret   10h
    }
}

void CMatrix::operator=(const CMatrix& other) {
    CopyOnlyMatrix(other);
    UpdateRW();
}

void CMatrix::operator+=(const CMatrix& rvalue)
{
    m_right += rvalue.m_right;
    m_forward += rvalue.m_forward;
    m_up += rvalue.m_up;
    m_pos += rvalue.m_pos;
}

void CMatrix::operator*=(const CMatrix& rvalue)
{
    *this = (*this * rvalue);
}

CMatrix CMatrix::GetIdentity() {
    CMatrix m;
    m.m_right   = CVector{ 1.f, 0.f, 0.f };
    m.m_forward = CVector{ 0.f, 1.f, 0.f };
    m.m_up      = CVector{ 0.f, 0.f, 1.f };
    m.m_pos     = CVector{ 0.f, 0.f, 0.f };
    return m;
}

// 0x59BE30: mismo resultado que el original bit a bit. Cada elemento es tx + ty + tz (tx = a.right·b.x,
// ty = a.forward·b.y, tz = a.up·b.z) pero el EXE agrupa las sumas de forma distinta en cada uno.
CMatrix operator*(const CMatrix& a, const CMatrix& b)
{
    auto result = CMatrix();
    const auto T = [&](int i, const CVector& col, float& tx, float& ty, float& tz) {
        tx = (&a.m_right.x)[i] * col.x;
        ty = (&a.m_forward.x)[i] * col.y;
        tz = (&a.m_up.x)[i] * col.z;
    };
    float tx, ty, tz;
    T(0, b.m_right, tx, ty, tz);   result.m_right.x   = (ty + tx) + tz;
    T(1, b.m_right, tx, ty, tz);   result.m_right.y   = (tz + tx) + ty;
    T(2, b.m_right, tx, ty, tz);   result.m_right.z   = (ty + tz) + tx;
    T(0, b.m_forward, tx, ty, tz); result.m_forward.x = (tz + ty) + tx;
    T(1, b.m_forward, tx, ty, tz); result.m_forward.y = (tz + ty) + tx;
    T(2, b.m_forward, tx, ty, tz); result.m_forward.z = (tx + tz) + ty;
    T(0, b.m_up, tx, ty, tz);      result.m_up.x      = (ty + tz) + tx;
    T(1, b.m_up, tx, ty, tz);      result.m_up.y      = (tz + ty) + tx;
    T(2, b.m_up, tx, ty, tz);      result.m_up.z      = (tx + tz) + ty;
    T(0, b.m_pos, tx, ty, tz);     result.m_pos.x     = ((ty + tz) + tx) + a.m_pos.x;
    T(1, b.m_pos, tx, ty, tz);     result.m_pos.y     = ((tz + ty) + tx) + a.m_pos.y;
    T(2, b.m_pos, tx, ty, tz);     result.m_pos.z     = ((tx + ty) + tz) + a.m_pos.z;
    return result;
}

CVector operator*(const CMatrix& a, const CVector& b) {
    return a.TransformPoint(b);
}

CMatrix operator+(const CMatrix& a, const CMatrix& b)
{
    CMatrix result;
    result.m_right =   a.m_right + b.m_right;
    result.m_forward = a.m_forward + b.m_forward;
    result.m_up =      a.m_up + b.m_up;
    result.m_pos =     a.m_pos + b.m_pos;
    return result;
}

CMatrix& Invert(CMatrix& in, CMatrix& out)
{
    out = in.Inverted();
    return out;
}

CMatrix Invert(const CMatrix& in)
{
    return in.Inverted();
}

CMatrix Lerp(CMatrix from, CMatrix to, float t) {
    from.ScaleAll(1.0f - t);
    to.ScaleAll(t);
    return from + to;
}
