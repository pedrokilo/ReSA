/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMatrix
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

#include "Matrix.h"
#include "X87Math.h"

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
    //RH_ScopedGlobalOverloadedInstall(operator*, "Vec", 0x59C890, CVector(*)(const CMatrix&, const CVector&)); // ver InjectHooksMultiplyVector
    RH_ScopedGlobalOverloadedInstall(operator+, "", 0x59BFA0, CMatrix(*)(const CMatrix&, const CMatrix&));
    RH_ScopedGlobalOverloadedInstall(Invert, "1", 0x59B920, CMatrix&(*)(CMatrix&, CMatrix&));
    RH_ScopedGlobalOverloadedInstall(Invert, "2", 0x59BDD0, CMatrix(*)(const CMatrix&));
}

// CMatrix::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); esta es la unica entrada de CMatrix que se instala (0x59C890 ya esta reconstruida con la x87 del EXE).
void CMatrix::InjectHooksMultiplyVector()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

#pragma warning(push)
#pragma warning(disable: 4996) // operator*(CMatrix, CVector) esta marcado [[deprecated]] para el codigo nuevo, pero es el destino del hook
    RH_ScopedGlobalOverloadedInstall(operator*, "Vec", 0x59C890, CVector(*)(const CMatrix&, const CVector&));
#pragma warning(pop)
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
// Como el original: fcos/fsin del x87 (X87Math, MatrixSetRotateXOnlyCpp) guardados a float, sin el sin/cos de la CRT,
// y los mismos campos escritos (sin tocar el relleno ni la posición).
void CMatrix::SetRotateXOnly(float angle)
{
    MatrixSetRotateXOnlyCpp(reinterpret_cast<float*>(this), angle);
}

// 0x59AFE0
// Como el original: igual que SetRotateXOnly, con fsin/fcos del x87 (MatrixSetRotateYOnlyCpp).
void CMatrix::SetRotateYOnly(float angle)
{
    MatrixSetRotateYOnlyCpp(reinterpret_cast<float*>(this), angle);
}

// 0x59B020
// Como el original: fsin/fcos del x87 guardados a float (MatrixSetRotateZOnlyCpp),
// y los mismos campos escritos (sin tocar el relleno ni la posición).
void CMatrix::SetRotateZOnly(float angle)
{
    MatrixSetRotateZOnlyCpp(reinterpret_cast<float*>(this), angle);
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
// Como el original: MatrixSetRotateCpp (X87Math) reproduce el x87 del EXE (fsin/fcos a precisión interna completa, sin
// redondear a float antes de los productos salvo cos(y), sin(z) y cos(z)·sin(x), que el EXE guarda en la pila), para obtener los mismos bits.
void CMatrix::SetRotate(float x, float y, float z)
{
    MatrixSetRotateCpp(reinterpret_cast<float*>(this), x, y, z);
}

// 0x59B1E0
// Como el original: MatrixRotateXCpp (X87Math). fcos se queda sin redondear a float, fsin se guarda como float (fstp);
// los productos se redondean a 24 bits como en el EXE y la posición también se rota.
void CMatrix::RotateX(float angle)
{
    MatrixRotateXCpp(reinterpret_cast<float*>(this), angle);
}

// 0x59B2C0
// Como el original: MatrixRotateYCpp (X87Math). fcos se queda sin redondear a float, fsin se guarda como float (fst)
// pero right.x usa el seno sin redondear; los productos se redondean a 24 bits como en el EXE y la posición también se rota.
void CMatrix::RotateY(float angle)
{
    MatrixRotateYCpp(reinterpret_cast<float*>(this), angle);
}

// 0x59B390
// Como el original: HoldEntityRotateZCpp (X87Math) es la misma rotación en Z del EXE (fcos sin redondear a float,
// fsin guardado como float; los productos se redondean a 24 bits y la posición también se rota).
void CMatrix::RotateZ(float angle)
{
    HoldEntityRotateZCpp(reinterpret_cast<float*>(this), angle);
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
// Como el original: MatrixReorthogonaliseCpp (X87Math) reproduce el x87 del EXE (productos vectoriales en línea, normalización
// sin comprobar cero con la suma z² + y² + x² y el recíproco 1/sqrt), para obtener los mismos bits.
void CMatrix::Reorthogonalise()
{
    MatrixReorthogonaliseCpp(reinterpret_cast<float*>(this));
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

// Ejes i, j, k que el EXE obtiene de los bits 2..4 de las banderas (bit 2 = paridad, bits 3..4 = primer eje):
// i = Idx1[(f >> 3) & 3], k = Idx2[i + paridad], j = Idx2[1 + i - paridad].
struct EulerAxes { int i, j, k; };
static EulerAxes EulerAxesFromFlags(uint32 flags)
{
    const int parity = int((flags >> 2) & 1);
    const int i = EulerIdx1[(flags >> 3) & 3];
    return { i, EulerIdx2[1 + i - parity], EulerIdx2[i + parity] };
}

// 0x59A840: MatrixConvertToEulerCpp (X87Math) reproduce el x87 del EXE (fsqrt/productos a la precision vigente, fpatan completo; el
// orden de rama y los indices [fila][columna] son los del original, p. ej. Z en Euler usa atan2(m[i2][i1], -m[i3][i1])).
void CMatrix::ConvertToEulerAngles(float* pX, float* pY, float* pZ, uint32 uiFlags)
{
    const auto axes = EulerAxesFromFlags(uiFlags);
    MatrixConvertToEulerCpp(reinterpret_cast<const float*>(this), pX, pY, pZ, uiFlags, axes.i, axes.j, axes.k);
}

// 0x59AA40: MatrixConvertFromEulerCpp (X87Math) reproduce el x87 del EXE (fsin/fcos a precision completa, productos y sumas en el orden del original;
// sinZ queda en la pila x87 sin redondear a float entre sus usos).
void CMatrix::ConvertFromEulerAngles(float x, float y, float z, uint32 uiFlags)
{
    const auto axes = EulerAxesFromFlags(uiFlags);
    MatrixConvertFromEulerCpp(reinterpret_cast<float*>(this), x, y, z, uiFlags, axes.i, axes.j, axes.k);
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

// 0x59C890 (cdecl; puntero de retorno oculto como primer argumento, EAX = destino): matriz * punto con las sumas, los redondeos a 24 bits y la seleccion de NaN de la x87 del EXE
// (x = (up.x*z + fwd.x*y) + right.x*x + pos.x, y = (up.y*z + right.y*x) + fwd.y*y + pos.y, z = (up.z*z + right.z*x) + fwd.z*y + pos.z). El EXE calcula las tres
// componentes antes de escribir ninguna: la salida puede solaparse con la matriz o con el punto (Hp65MatrixTimesPointCpp hace lo mismo con tres
// variables locales). Los llamadores del EXE (225 leen EAX) no leen ECX ni EDX (ver reversing/Helpers65D/README.md).
CVector operator*(const CMatrix& a, const CVector& b) {
    CVector result;
    Hp65MatrixTimesPointCpp(reinterpret_cast<const float*>(&a), &b.x, &result.x);
    return result;
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
