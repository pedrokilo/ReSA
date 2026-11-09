/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMatrix
 * Reconstrucción: COMPLETA — 34/34 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 34/34 entradas.
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

    // Attach (0x59BD10) y operator= (0x59BBC0) se instalan por InjectHooksAttachAssign.
    RH_ScopedInstall(Detach, 0x59ACF0);
    RH_ScopedInstall(CopyOnlyMatrix, 0x59ADD0);
    RH_ScopedInstall(Update, 0x59BB60);
    RH_ScopedInstall(UpdateMatrix, 0x59AD20);
    // UpdateRW (0x59BBB0), UpdateRwMatrix (0x59AD70), SetTranslate (0x59AF40) y SetRotateZOnly (0x59B020) se instalan por InjectHooksExactOps.
    // SetUnity (0x59AE70), ResetOrientation (0x59AEA0), SetScale (0x59AED0, 0x59AF00), SetTranslateOnly (0x59AF80), SetRotate xyz (0x59B120), Invert (0x59B920) y la multiplicacion
    // de matrices (0x59BE30) se instalan por InjectHooksLote99A.
    // SetRotateXOnly (0x59AFA0), SetRotateYOnly (0x59AFE0), Rotate (0x59B460), SetRotate(quat) (0x59BBF0), ForceUpVector (0x59B7E0) y operator+ (0x59BFA0)
    // se instalan por CMatrixInjectHooksLote103A (lote 103A, banco Fn103A).
    // SetRotateX/Y/Z (0x59B060, 0x59B0A0, 0x59B0E0), RotateX/Y/Z (0x59B1E0, 0x59B2C0, 0x59B390), Reorthogonalise (0x59B6A0), CopyToRwMatrix (0x59B8B0) e Invert(in) (0x59BDD0)
    // se instalan por CMatrixInjectHooksLote101A (lote 101A, banco Fn101A).
    RH_ScopedInstall(Scale, 0x459350);
    RH_ScopedInstall(ConvertToEulerAngles, 0x59A840);
    RH_ScopedInstall(ConvertFromEulerAngles, 0x59AA40);
    RH_ScopedInstall(operator+=, 0x59ADF0);
    // operator*= (0x411A80) se instala por CMatrixInjectHooksLote105A (lote 105A, banco Fn105A).
    //RH_ScopedGlobalOverloadedInstall(operator*, "Vec", 0x59C890, CVector(*)(const CMatrix&, const CVector&)); // ver InjectHooksMultiplyVector
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

// CMatrix::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); estas son, junto con InjectHooksMultiplyVector, las unicas entradas de CMatrix que se instalan:
// Attach (0x59BD10) y operator= (0x59BBC0), ambas reconstruidas en C++ puro y validadas por el banco MatrixOps76.
void CMatrix::InjectHooksAttachAssign()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Attach, 0x59BD10);
    RH_ScopedInstall(operator=, 0x59BBC0);
}

namespace {
// 0x41BD00 (lote 140A, banco Fn140A): copia fuera de linea de CMatrix::GetRight del EXE (thiscall, sin argumentos, `ret`, 3 bytes: `mov eax,ecx`): devuelve la direccion del vector derecho (+0x0, el propio this).
// Lo llaman los vehiculos (CAutomobile) sobre la matriz de CPlaceable::GetMatrix.
CVector* __fastcall MatrixGetRightCopy(CMatrix* self, void* /*edx*/) { return &self->GetRight(); }

// 0x41BD10 (lote 140A, banco Fn140A): copia fuera de linea de CMatrix::GetForward del EXE (thiscall, sin argumentos, `ret`, 4 bytes: `lea eax,[ecx+10h]`): direccion del vector de avance (+0x10).
CVector* __fastcall MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) { return &self->GetForward(); }

// 0x54E8E0 (lote 140A, banco Fn140A): trampolin del destructor de CMatrix del EXE (thiscall, sin argumentos, 5 bytes: `jmp 0x59ACD0`); es el que pasan a los iteradores de vectores de matrices. Salta
// al cuerpo del destructor del EXE (0x59ACD0, con hook: CMatrix::Destructor). Sin expansion en linea (llamada a la funcion del hook).
__declspec(noinline) void __fastcall MatrixDestructorThunk(CMatrix* self, void* /*edx*/) { self->Destructor(); }
} // namespace

// CMatrix::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); estas son, junto con InjectHooksMultiplyVector y InjectHooksAttachAssign, las unicas entradas de CMatrix que se instalan
// (lote 97A, bancos Fn97A y Fn97B): UpdateRW (0x59BBB0), UpdateRwMatrix (0x59AD70), SetTranslate (0x59AF40), SetRotateZOnly (0x59B020), el destructor (0x59ACD0) y los constructores
// de copia (0x59BCF0) y con RwMatrix (0x59C050).
void CMatrix::InjectHooksExactOps()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(MatrixGetRightCopy, "GetRightCopy", 0x41BD00);       // lote 140A (banco Fn140A)
    RH_ScopedNamedGlobalInstall(MatrixGetForwardCopy, "GetForwardCopy", 0x41BD10);   // lote 140A (banco Fn140A)
    RH_ScopedNamedGlobalInstall(MatrixDestructorThunk, "DestructorThunk", 0x54E8E0); // lote 140A (banco Fn140A)

    RH_ScopedInstall(UpdateRW, 0x59BBB0);
    RH_ScopedInstall(UpdateRwMatrix, 0x59AD70);
    RH_ScopedInstall(SetTranslate, 0x59AF40);
    RH_ScopedInstall(SetRotateZOnly, 0x59B020);
    RH_ScopedInstall(Destructor, 0x59ACD0);
    RH_ScopedInstall(ConstructCopy, 0x59BCF0);
    RH_ScopedInstall(ConstructAttached, 0x59C050);
}

// CMatrix::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); estas son, junto con las demas listas activas, las unicas entradas de CMatrix que se instalan
// (lote 99A, banco Fn99A): SetUnity (0x59AE70), ResetOrientation (0x59AEA0), SetScale (0x59AED0 y 0x59AF00), SetTranslateOnly (0x59AF80), SetRotate xyz (0x59B120), Invert(in, out) (0x59B920)
// y la multiplicacion de matrices (0x59BE30; se engancha MatrixMultiplyExe, con el puntero de salida explicito: operator* la llama con una matriz local).
void CMatrix::InjectHooksLote99A()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(SetUnity, 0x59AE70);
    RH_ScopedInstall(ResetOrientation, 0x59AEA0);
    RH_ScopedOverloadedInstall(SetScale, "f", 0x59AED0, void(CMatrix::*)(float));
    RH_ScopedOverloadedInstall(SetScale, "fff", 0x59AF00, void(CMatrix::*)(float, float, float));
    RH_ScopedInstall(SetTranslateOnly, 0x59AF80);
    RH_ScopedOverloadedInstall(SetRotate, "xyz", 0x59B120, void(CMatrix::*)(float, float, float));
    RH_ScopedGlobalOverloadedInstall(Invert, "1", 0x59B920, CMatrix&(*)(CMatrix&, CMatrix&));
    RH_ScopedGlobalInstall(MatrixMultiplyExe, 0x59BE30);
}

// Lote 101A (banco Fn101A): SetRotateX/Y/Z (0x59B060, 0x59B0A0, 0x59B0E0), RotateX/Y/Z (0x59B1E0, 0x59B2C0, 0x59B390), Reorthogonalise (0x59B6A0), CopyToRwMatrix (0x59B8B0)
// e Invert(in) (0x59BDD0). Funcion libre (no miembro de CMatrix) para no tocar Matrix.h, que incluyen cientos de archivos; se llama desde InjectHooksMain.cpp junto a las demas listas activas.
void CMatrixInjectHooksLote101A()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(SetRotateX, 0x59B060);
    RH_ScopedInstall(SetRotateY, 0x59B0A0);
    RH_ScopedInstall(SetRotateZ, 0x59B0E0);
    RH_ScopedInstall(RotateX, 0x59B1E0);
    RH_ScopedInstall(RotateY, 0x59B2C0);
    RH_ScopedInstall(RotateZ, 0x59B390);
    RH_ScopedInstall(Reorthogonalise, 0x59B6A0);
    RH_ScopedInstall(CopyToRwMatrix, 0x59B8B0);
    RH_ScopedGlobalOverloadedInstall(Invert, "2", 0x59BDD0, CMatrix(*)(const CMatrix&));
}

// Lote 103A (banco Fn103A): SetRotateXOnly (0x59AFA0), SetRotateYOnly (0x59AFE0), Rotate (0x59B460), ForceUpVector (0x59B7E0), SetRotate(CQuaternion) (0x59BBF0) y operator+ (0x59BFA0).
// Funcion libre (no miembro de CMatrix) para no tocar Matrix.h; se llama desde InjectHooksMain.cpp junto a las demas listas activas.
void CMatrixInjectHooksLote103A()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(SetRotateXOnly, 0x59AFA0);
    RH_ScopedInstall(SetRotateYOnly, 0x59AFE0);
    RH_ScopedInstall(Rotate, 0x59B460);
    RH_ScopedInstall(ForceUpVector, 0x59B7E0);
    RH_ScopedOverloadedInstall(SetRotate, "quat", 0x59BBF0, void(CMatrix::*)(const CQuaternion&));
    RH_ScopedGlobalOverloadedInstall(operator+, "", 0x59BFA0, CMatrix(*)(const CMatrix&, const CMatrix&));
}

// Lote 105A (banco Fn105A): operator*= (0x411A80). Funcion libre para no tocar Matrix.h; se llama desde InjectHooksMain.cpp junto a las demas listas activas.
void CMatrixInjectHooksLote105A()
{
    RH_ScopedClass(CMatrix);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator*=, 0x411A80);
}

CMatrix::CMatrix(const CMatrix& matrix) {
    CMatrix::ConstructCopy(matrix);
}

// like previous + attach
CMatrix::CMatrix(RwMatrix* matrix, bool temporary) {
    CMatrix::ConstructAttached(matrix, temporary);
}

// 0x59BCF0 - Como el original: la matriz enlazada (+0x40) y la bandera de propiedad (dword +0x44, con su relleno) se ponen a cero ANTES de copiar los 16 dwords de la RwMatrix.
CMatrix* CMatrix::ConstructCopy(const CMatrix& matrix)
{
    m_pAttachMatrix = nullptr;
    *reinterpret_cast<uint32*>(&m_bOwnsAttachedMatrix) = 0;
    CMatrix::CopyOnlyMatrix(matrix);
    return this;
}

// 0x59C050 - Como el original: solo se pone a cero la matriz enlazada (+0x40) antes de Attach; la bandera de propiedad (+0x44) la escribe Attach.
CMatrix* CMatrix::ConstructAttached(RwMatrix* matrix, bool temporary)
{
    m_pAttachMatrix = nullptr;
    CMatrix::Attach(matrix, temporary);
    return this;
}

// 0x59ACD0 - Cuerpo del destructor del EXE: la bandera de propiedad se LEE como dword (+0x44; el relleno cuenta) y se comprueba antes que la matriz; no se pone a cero la matriz enlazada.
// El destructor de la DLL (~CMatrix) sigue usando Detach (bandera de un byte): el relleno de una CMatrix construida por la DLL no esta inicializado y no puede leerse como bandera.
CMatrix* CMatrix::Destructor()
{
    if (*reinterpret_cast<const uint32*>(&m_bOwnsAttachedMatrix) && m_pAttachMatrix)
        RwMatrixDestroy(m_pAttachMatrix);
    return this;
}

// destructor detaches matrix if attached
CMatrix::~CMatrix()
{
    CMatrix::Detach();
}

// 0x59BD10 - Como el original: la bandera de propiedad se LEE como dword (+0x44; los tres bytes de relleno cuentan) y se ESCRIBE como dword con el byte del argumento sin normalizar
// (movzx); el destructor de RenderWare (0x7F2A20) solo se llama si hay matriz y la bandera es distinta de cero. Despues copia los 12 dwords de la RwMatrix (sin las banderas ni el relleno).
void CMatrix::Attach(RwMatrix* matrix, bool bOwnsMatrix)
{
    uint32& ownsWord = *reinterpret_cast<uint32*>(&m_bOwnsAttachedMatrix);
    if (m_pAttachMatrix && ownsWord)
        RwMatrixDestroy(m_pAttachMatrix);

    m_pAttachMatrix = matrix;
    ownsWord = static_cast<uint32>(std::bit_cast<uint8>(bOwnsMatrix));
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
// Como el original: MatrixRotateX101Cpp (X87Math, lote 101A). fcos se queda sin redondear a float, fsin se guarda como float (fstp); los productos se redondean a 24 bits
// con el primer factor en el registro y el segundo como operando de memoria (selección de NaN como el EXE) y la posición también se rota.
void CMatrix::RotateX(float angle)
{
    MatrixRotateX101Cpp(reinterpret_cast<float*>(this), angle);
}

// 0x59B2C0
// Como el original: MatrixRotateY101Cpp (X87Math, lote 101A). fcos se queda sin redondear a float, fsin se guarda como float (fst)
// pero right.x usa el seno sin redondear; los productos se redondean a 24 bits como en el EXE y la posición también se rota.
void CMatrix::RotateY(float angle)
{
    MatrixRotateY101Cpp(reinterpret_cast<float*>(this), angle);
}

// 0x59B390
// Como el original: MatrixRotateZ101Cpp (X87Math, lote 101A): fcos sin redondear a float, fsin guardado como float; los productos se redondean a 24 bits
// (primer factor en el registro, segundo como operando de memoria) y la posición también se rota.
void CMatrix::RotateZ(float angle)
{
    MatrixRotateZ101Cpp(reinterpret_cast<float*>(this), angle);
}

// 0x59B460 (thiscall, `ret 0Ch`; el vector por valor en la pila): rotate on 3 axes.
// Como el original: SetRotate(x, y, z) y las cuatro transformaciones van en linea con la x87 del EXE (MatrixRotate103Cpp, X87Math, lote 103A): fsin/fcos sin redondear, las diez magnitudes
// guardadas como float, una fila por (f9*m2 + f7*m1) + f4*m0 con el primer factor en el registro y la matriz como operando de memoria; las doce lecturas van antes de las escrituras.
void CMatrix::Rotate(CVector rotation)
{
    MatrixRotate103Cpp(reinterpret_cast<float*>(this), rotation.x, rotation.y, rotation.z);
}

// 0x59B6A0
// Como el original: MatrixReorthogonalise101Cpp (X87Math, lote 101A) reproduce el x87 del EXE (productos vectoriales en línea, normalización
// sin comprobar cero con la suma z² + y² + x² y el recíproco 1/sqrt, operandos de registro y de memoria como el EXE), para obtener los mismos bits.
void CMatrix::Reorthogonalise()
{
    MatrixReorthogonalise101Cpp(reinterpret_cast<float*>(this));
}

// 0x59B8B0 (thiscall, `ret 4`): similar to UpdateRW(RwMatrixTag *).
// Como el original: copia los doce dwords de right, forward, up y pos (+0x00..+0x08, +0x10..+0x18, +0x20..+0x28, +0x30..+0x38) como enteros, leyendo y escribiendo de dword en dword en ese
// orden (la RwMatrix puede solaparse con la matriz) y llama UNA sola vez a RwMatrixUpdate (0x7F18A0, cdecl). No pasa por UpdateRwMatrix (que ya llama a RwMatrixUpdate: se llamaria dos veces).
// El EXE devuelve la RwMatrix en EAX, pero ninguno de sus nueve llamadores lo lee.
void CMatrix::CopyToRwMatrix(RwMatrix* matrix) const
{
    const volatile uint32* const src = reinterpret_cast<const volatile uint32*>(this);
    volatile uint32* const dst = reinterpret_cast<volatile uint32*>(matrix);
    for (unsigned row = 0; row < 16; row += 4) {
        dst[row] = src[row];
        dst[row + 1] = src[row + 1];
        dst[row + 2] = src[row + 2];
    }
    RwMatrixUpdate(matrix);
}

// 0x59BBF0 (thiscall, `ret 4`): matriz de rotacion de un cuaternion. Como el original: MatrixSetRotateQuat103Cpp (X87Math, lote 103A) reproduce la x87 del EXE (productos de 2q por q y por la parte real,
// guardados como float o dejados en el registro, 1 - (a + b) con la constante 0x858624); escribe solo m0, m1, m2, m4, m5, m6, m8, m9 y m10.
void CMatrix::SetRotate(const CQuaternion& quat)
{
    MatrixSetRotateQuat103Cpp(reinterpret_cast<float*>(this), reinterpret_cast<const float*>(&quat));
}

void CMatrix::Scale(float scale) {
    ScaleXYZ(scale, scale, scale);
}

void CMatrix::ScaleXYZ(float x, float y, float z) {
    m_right   *= x;
    m_forward *= y;
    m_up      *= z;
}

// 0x59B7E0 (thiscall, `ret 0Ch`; el vector por valor en la pila): right = forward x up, forward = up x right, up = vecUp, con la x87 del EXE (MatrixForceUpVector103Cpp, X87Math, lote 103A):
// los tres dwords de vecUp pasan como enteros (un NaN senalizante se copia tal cual a la fila up).
void CMatrix::ForceUpVector(CVector vecUp) {
    const uint32* const u = reinterpret_cast<const uint32*>(&vecUp);
    MatrixForceUpVector103Cpp(reinterpret_cast<float*>(this), u[0], u[1], u[2]);
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

// 0x411A80 (thiscall, `ret 4`): como el EXE, la matriz temporal vive en la pila SIN constructor: 0x59BE30 (producto, con la salida explicita) la rellena con this * rvalue, 0x59BBC0 (operator=) la copia
// en this y 0x59ACD0 (cuerpo del destructor del EXE) la destruye. El EXE devuelve EAX = this, que ningun llamador lee.
void CMatrix::operator*=(const CMatrix& rvalue)
{
    alignas(CMatrix) unsigned char raw[sizeof(CMatrix)];
    CMatrix* const tmp = reinterpret_cast<CMatrix*>(raw);
    MatrixMultiplyExe(tmp, this, &rvalue);
    *this = *tmp;
    tmp->Destructor();
}

CMatrix CMatrix::GetIdentity() {
    CMatrix m;
    m.m_right   = CVector{ 1.f, 0.f, 0.f };
    m.m_forward = CVector{ 0.f, 1.f, 0.f };
    m.m_up      = CVector{ 0.f, 0.f, 1.f };
    m.m_pos     = CVector{ 0.f, 0.f, 0.f };
    return m;
}

// 0x59BE30 (cdecl; puntero de retorno oculto como primer argumento, EAX = destino): out = a * b con las sumas y los redondeos a 24 bits de la x87 del EXE (X87Math, MatrixMultiplyExeCpp: cada
// elemento es (r1*m1 + r2*m2) + r3*m3 [+ posicion] con el orden de operandos del EXE). Las tres matrices pueden ser la misma: el EXE lee todo antes de escribir. Pone a cero +0x40 (matriz enlazada)
// y +0x44 (bandera de propiedad, dword) de la salida; las palabras de relleno no se escriben (el EXE copia ahi basura de la pila, nadie la lee).
CMatrix* MatrixMultiplyExe(CMatrix* out, const CMatrix* a, const CMatrix* b)
{
    MatrixMultiplyExeCpp(reinterpret_cast<float*>(out), reinterpret_cast<const float*>(a), reinterpret_cast<const float*>(b));
    return out;
}

CMatrix operator*(const CMatrix& a, const CMatrix& b)
{
    CMatrix result;
    MatrixMultiplyExe(&result, &a, &b);
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

// 0x59BFA0 (cdecl, puntero de retorno oculto como primer argumento, EAX = destino): suma componente a componente con la x87 del EXE (MatrixAdd103Cpp, X87Math, lote 103A). El EXE suma las doce
// componentes en un temporal de pila antes de escribir (por eso se leen aqui a un temporal local antes de construir el resultado: la salida oculta puede solaparse con a o b), copia los 16 dwords
// (las palabras de relleno +0x0C, +0x1C, +0x2C y +0x3C son basura de la pila: aqui no se escriben) y pone a cero +0x40 y +0x44.
CMatrix operator+(const CMatrix& a, const CMatrix& b)
{
    uint32 tmp[18]{};
    MatrixAdd103Cpp(reinterpret_cast<float*>(tmp), reinterpret_cast<const float*>(&a), reinterpret_cast<const float*>(&b));
    CMatrix result;
    uint32* const out = reinterpret_cast<uint32*>(&result);
    out[16] = 0;
    for (unsigned i = 0; i < 16; ++i) {
        if ((i & 3) != 3)
            out[i] = tmp[i];
    }
    out[17] = 0;
    return result;
}

// 0x59B920 (cdecl, devuelve out en EAX): transposicion de la rotacion y posicion = -(pos * rotacion) con la x87 del EXE (X87Math, MatrixInvertExeCpp); no copia la matriz enlazada ni la bandera
// de propiedad de `out` (no llama a operator=) y no actualiza ninguna RwMatrix, como el original.
CMatrix& Invert(CMatrix& in, CMatrix& out)
{
    MatrixInvertExeCpp(reinterpret_cast<const float*>(&in), reinterpret_cast<float*>(&out));
    return out;
}

// 0x59BDD0 (cdecl, puntero de retorno oculto como primer argumento, EAX = destino): invierte `in` con Invert(in, tmp) (0x59B920) sobre un temporal de pila cuya matriz enlazada (+0x40) y bandera
// de propiedad (dword +0x44) se ponen a cero antes, copia los 16 dwords del temporal al resultado (con el relleno +0x0C, +0x1C, +0x2C y +0x3C que deje Invert), pone a cero +0x40 y +0x44 del
// resultado y, como el destructor del temporal, llama a RwMatrixDestroy (0x7F2A20) solo si la bandera y la matriz enlazada del temporal son distintas de cero (nunca: Invert no las escribe).
CMatrix Invert(const CMatrix& in)
{
    uint32 tmp[18]{};
    Invert(const_cast<CMatrix&>(in), *reinterpret_cast<CMatrix*>(tmp));
    CMatrix result;
    uint32* const out = reinterpret_cast<uint32*>(&result);
    out[16] = 0;
    out[17] = 0;
    for (unsigned i = 0; i < 16; ++i)
        out[i] = tmp[i];
    if (tmp[17] != 0 && tmp[16] != 0)
        RwMatrixDestroy(reinterpret_cast<RwMatrix*>(tmp[16]));
    return result;
}

CMatrix Lerp(CMatrix from, CMatrix to, float t) {
    from.ScaleAll(1.0f - t);
    to.ScaleAll(t);
    return from + to;
}
