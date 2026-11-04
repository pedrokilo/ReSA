/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVector
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
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

// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// Magnitude, Magnitude2D, Normalise y NormaliseAndMag necesitan la x87 del EXE (palabra de control de 24 bits, exponente ampliado en los registros).
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "Vector.h"
#include "Vector2D.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)

void CVector::InjectHooks()
{
    RH_ScopedClass(CVector);
    RH_ScopedCategory("Core");

    // Magnitude, Magnitude2D, Normalise y NormaliseAndMag se instalan por separado en InjectHooksExactMath
    RH_ScopedInstall(Cross_OG, 0x70F890);
    RH_ScopedInstall(Sum, 0x40FDD0);
    RH_ScopedInstall(Difference, 0x40FE00);
    RH_ScopedInstall(FromMultiply, 0x59C670);
    RH_ScopedInstall(FromMultiply3x3, 0x59C6D0);
    RH_ScopedGlobalOverloadedInstall(CrossProduct, "out", 0x59C730, CVector*(*)(CVector*, CVector*, CVector*));
    RH_ScopedGlobalOverloadedInstall(DotProduct, "v3d*v3d*", 0x40FDB0, float(*)(CVector*, CVector*));
}

// CVector::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); estas son las unicas entradas de CVector que se instalan:
// Magnitude (0x4082C0), Magnitude2D (0x406D50), Normalise (0x59C910) y NormaliseAndMag (0x59C970), reconstruidas con la x87 del EXE (ver abajo) y validadas por el banco Fn96A.
void CVector::InjectHooksExactMath()
{
    RH_ScopedClass(CVector);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Magnitude, 0x4082C0);
    RH_ScopedInstall(Magnitude2D, 0x406D50);
    RH_ScopedInstall(Normalise, 0x59C910);
    RH_ScopedInstall(NormaliseAndMag, 0x59C970);
}

/*!
* @brief From a 2D vector and Z position
*/
CVector::CVector(const CVector2D& v2d, float Z) :
    RwV3d{ v2d.x, v2d.y, Z }
{
}

/*!
* @returns A vector with each of its components set to a number in the given range [min, max)
*/
CVector CVector::Random(CVector min, CVector max) {
    const auto Get = [=](float fmin, float fmax) { return CGeneral::GetRandomNumberInRange(fmin, fmax); };
    return { Get(min.x, max.x), Get(min.y, max.y), Get(min.z, max.z) };
}

CVector CVector::Random(float min, float max) {
    const auto Get = [=] { return CGeneral::GetRandomNumberInRange(min, max); };
    return { Get(), Get(), Get() };
}

// Las cuatro funciones siguientes (0x4082C0, 0x406D50, 0x59C910, 0x59C970) reproducen la x87 del EXE: este archivo se compila con /O2 /Oi /arch:IA32 /fp:fast
// (source/CMakeLists.txt) y las sumas llevan el mismo orden que el EXE, ((x*x + y*y) + z*z); los valores intermedios (suma, raiz, inverso) quedan en los registros de la x87
// (24 bits de mantisa por la palabra de control del juego y exponente ampliado): son `double` y no se guardan como float. Solo se guardan como float los componentes del vector
// (y el valor devuelto sale en ST0 sin redondear a float). Como en el EXE, ECX y EDX no se tocan.

// 0x4082C0: raiz de la suma de cuadrados, en ST0
float CVector::Magnitude() const
{
    return float(sqrt(((double)x * x + (double)y * y) + (double)z * z));
}

// 0x406D50
float CVector::Magnitude2D() const
{
    return float(sqrt((double)x * x + (double)y * y));
}

// 0x59C910: normaliza in situ; con la suma de cuadrados igual a 0 solo pone x = 1 (y y z intactos). Con NaN sigue el camino normal (la comparacion con 0 es "no mayor" solo si no es NaN)
void CVector::Normalise()
{
    const double fDot = ((double)x * x + (double)y * y) + (double)z * z;
    if (fDot <= 0.0)
    {
        x = 1.0F;
        return;
    }

    const double fRecip = 1.0 / sqrt(fDot);
    x = float(fRecip * x);
    y = float(fRecip * y);
    z = float(fRecip * z);
}

// 0x59C970: como Normalise y devuelve 1 / (1 / raiz) sin redondear a float (1.0 si la suma es 0)
float CVector::NormaliseAndMag()
{
    const double fDot = ((double)x * x + (double)y * y) + (double)z * z;
    if (fDot <= 0.0)
    {
        x = 1.0F;
        return 1.0F;
    }

    const double fRecip = 1.0 / sqrt(fDot);
    x = float(fRecip * x);
    y = float(fRecip * y);
    z = float(fRecip * z);

    return float(1.0 / fRecip);
}

auto CVector::Dot(const CVector& o) const -> float{
    return DotProduct(*this, o);
}

auto CVector::Dot2D(const CVector& o) const -> float {
    return y * o.y
         + x * o.x;
}

// notsa
CVector CVector::Cross(const CVector& o) const {
    return {
        y * o.z - z * o.y,
        z * o.x - x * o.z,
        x * o.y - y * o.x
    };
}

// 0x70F890
void CVector::Cross_OG(const CVector& a, const CVector& b) {
    *this = a.Cross(b);
}

// Adds left + right and stores result
void CVector::Sum(const CVector& left, const CVector &right)
{
    x = left.x + right.x;
    y = left.y + right.y;
    z = left.z + right.z;
}

// Subtracts left - right and stores result
void CVector::Difference(const CVector& left, const CVector &right)
{
    x = left.x - right.x;
    y = left.y - right.y;
    z = left.z - right.z;
}

// Adds value from the second vector.
void CVector::operator+=(const CVector& right)
{
    x += right.x;
    y += right.y;
    z += right.z;
}

void CVector::operator-=(const CVector& right)
{
    x -= right.x;
    y -= right.y;
    z -= right.z;
}

void CVector::operator*=(const CVector& right)
{
    x *= right.x;
    y *= right.y;
    z *= right.z;
}

// Multiplies vector by a floating point value
void CVector::operator *= (float multiplier)
{
    x *= multiplier;
    y *= multiplier;
    z *= multiplier;
}

// Divides vector by a floating point value
void CVector::operator /= (float divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
}

void CVector::FromMultiply(const CMatrix& matrix, const CVector& vector) {
    *this = matrix.TransformPoint(vector);
}

void CVector::FromMultiply3x3(const CMatrix& matrix, const CVector& vector) {
    // Como el original (0x59C6D0): orden de sumas del EXE
    x = (matrix.GetUp().x * vector.z + matrix.GetForward().x * vector.y) + matrix.GetRight().x * vector.x;
    y = (matrix.GetUp().y * vector.z + matrix.GetRight().y * vector.x) + matrix.GetForward().y * vector.y;
    z = (matrix.GetUp().z * vector.z + matrix.GetRight().z * vector.x) + matrix.GetForward().z * vector.y;
}

float CVector::Heading(bool limitAngle) const {
    const auto radians = std::atan2(-x, y);
    if (limitAngle) {
        return CGeneral::LimitRadianAngle(radians);
    }
    return radians;
}

CVector* CrossProduct(CVector* out, CVector* a, CVector* b)
{
    *out = a->Cross(*b);
    return out;
}

CVector CrossProduct(const CVector& a, const CVector& b)
{
    return a.Cross(b);
}

float DotProduct(CVector* v1, CVector* v2)
{
    return v1->z * v2->z + v1->y * v2->y + v1->x * v2->x;
}

float DotProduct(const CVector& v1, const CVector& v2)
{
    return v1.z * v2.z + v1.y * v2.y + v1.x * v2.x;
}

float DotProduct2D(const CVector& v1, const CVector& v2)
{
    return v1.y * v2.y + v1.x * v2.x;
}
