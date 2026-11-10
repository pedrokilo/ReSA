/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVector
 * Reconstrucción: COMPLETA — 22/22 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 22/22 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
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

// 0x420860 (lote 124A, banco Fn124A): copia fuera de linea de CVector2D::Magnitude del EXE (thiscall, sin argumentos de pila, `ret`, 22 bytes, sin llamadores ni vtable visibles): raiz de
// x*x + y*y en la x87 (24 bits de mantisa por la palabra de control del juego, exponente ampliado). Mismo cuerpo que CVector::Magnitude2D (0x406D50). Los valores intermedios quedan en
// registros, asi que son `double`; el resultado sale en ST0 sin redondear a float (devuelve `double`). ECX y EDX no se tocan.
static double __fastcall Vector2DMagnitudeExe(const CVector2D* self, void* /*edx*/)
{
    return sqrt((double)self->x * self->x + (double)self->y * self->y);
}

void CVector::InjectHooks()
{
    RH_ScopedClass(CVector);
    RH_ScopedCategory("Core");

    // Magnitude, Magnitude2D, Normalise y NormaliseAndMag se instalan por separado en InjectHooksExactMath
    RH_ScopedInstall(Cross_OG, 0x70F890);
    // Sum (0x40FDD0) se instala por separado en InjectHooksExactOperators (lote 153A)
    // Difference (0x40FE00) se instala por separado en InjectHooksExactOperators (lote 99A)
    // FromMultiply (0x59C670) se instala por separado en InjectHooksExactOperators (lote 101A)
    // FromMultiply3x3 (0x59C6D0) se instala por separado en InjectHooksExactOperators (lote 103A)
    // CrossProduct (0x59C730), DotProduct (0x40FDB0), operator+= (0x411A00) y operator*=(float) (0x40FEF0) se instalan por separado en InjectHooksExactOperators
}

// CVector::InjectHooks completo esta desactivado en InjectHooksMain (rendimiento); estas son las unicas entradas de CVector que se instalan:
// Magnitude (0x4082C0), Magnitude2D (0x406D50), SquaredMagnitudeExe (0x406DA0, lote 102A), Normalise (0x59C910) y NormaliseAndMag (0x59C970), reconstruidas con la x87 del EXE (ver abajo) y validadas por el banco Fn96A.
void CVector::InjectHooksExactMath()
{
    RH_ScopedClass(CVector);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Magnitude, 0x4082C0);
    RH_ScopedInstall(Magnitude2D, 0x406D50);
    RH_ScopedInstall(SquaredMagnitudeExe, 0x406DA0);
    RH_ScopedInstall(Normalise, 0x59C910);
    RH_ScopedInstall(NormaliseAndMag, 0x59C970);

    // Lote 124A (banco Fn124A): CVector2D::Magnitude fuera de linea del EXE (0x420860); el listado de pendientes lo atribuia a CCarAI.
    {
        RH_ScopedClass(CVector2D);
        RH_ScopedCategory("Core");

        RH_ScopedNamedGlobalInstall(Vector2DMagnitudeExe, "Magnitude", 0x420860);
    }
}

CVector* VectorDividedByFloat(CVector* out, const CVector* vector, float divisor); // 0x4119D0 (lote 105A), definida mas abajo

// 0x70F890 (lote 129A, banco Fn129A): producto vectorial con resultado en `this` (el listado de pendientes decia CSprite; es CVector::Cross_OG), thiscall, dos punteros en la pila, `ret 8`, 57
// bytes, sin llamadores en el EXE. A diferencia de CrossProduct (0x59C730, que lee las seis componentes antes de escribir), guarda cada componente antes de calcular la siguiente, asi que
// si `out` se solapa con a o b las siguientes lecturas ven la escritura. x = b.z*a.y - a.z*b.y, y = a.z*b.x - a.x*b.z, z = a.x*b.y - b.x*a.y; cada par de productos queda en la x87 (24
// bits de mantisa, exponente ampliado) y solo el resultado se guarda como float.
static void __fastcall CrossProductInPlaceExe(CVector* out, void* /*edx*/, const CVector* a, const CVector* b)
{
    volatile CVector* const o = out;
    const volatile CVector* const va = a;
    const volatile CVector* const vb = b;
    {
        const double p = vb->z; const double t = p * va->y;
        const double q = va->z; const double u = q * vb->y;
        o->x = float(t - u);
    }
    {
        const double p = va->z; const double t = p * vb->x;
        const double q = va->x; const double u = q * vb->z;
        o->y = float(t - u);
    }
    {
        const double p = va->x; const double t = p * vb->y;
        const double q = vb->x; const double u = q * va->y;
        o->z = float(t - u);
    }
}

// 0x404350 (lote 150A, banco Fn150A; el listado decia CRect): constructor por defecto de CVector del EXE (thiscall, sin argumentos de pila, `ret`, 3 bytes: `mov eax,ecx; ret`): no escribe nada y devuelve `this`.
// Sus llamadores (CAutomobile::ProcessCarWheelPair y el iterador de constructores de vectores `__vec_ctor(ptr, 12, 4, 0x404350)`) lo usan con ECX = el vector. EDX no se toca.
static CVector* __fastcall VectorDefaultConstructorExe(CVector* self, void* /*edx*/)
{
    return self;
}

// Operadores y productos del EXE con la x87 exacta (lote 97A), validados por el banco Fn97A: DotProduct (0x40FDB0), CrossProduct (0x59C730), Multiply3x3 matriz * vector (0x59C790),
// la suma (0x40FE30) y la resta (0x40FE60) de vectores, los productos escalar * vector (0x40FE90) y vector * escalar (0x40FEC0), operator+= (0x411A00) y operator*=(float) (0x40FEF0).
void CVector::InjectHooksExactOperators()
{
    RH_ScopedClass(CVector);
    RH_ScopedCategory("Core");

    RH_ScopedGlobalOverloadedInstall(DotProduct, "v3d*v3d*", 0x40FDB0, float(*)(CVector*, CVector*));
    RH_ScopedGlobalOverloadedInstall(CrossProduct, "out", 0x59C730, CVector*(*)(CVector*, CVector*, CVector*));
    RH_ScopedNamedGlobalInstall(CrossProductInPlaceExe, "Cross_OG", 0x70F890);
    RH_ScopedNamedGlobalInstall(VectorDefaultConstructorExe, "DefaultConstructor", 0x404350); // lote 150A (banco Fn150A)
    RH_ScopedGlobalInstall(Multiply3x3Exe, 0x59C790);
    RH_ScopedGlobalInstall(Multiply3x3VMExe, 0x59C810);
    RH_ScopedInstall(Sum, 0x40FDD0); // lote 153A (banco Fn153A)
    RH_ScopedInstall(Difference, 0x40FE00);
    RH_ScopedInstall(FromMultiply, 0x59C670);
    RH_ScopedInstall(FromMultiply3x3, 0x59C6D0);
    RH_ScopedGlobalInstall(VectorAdd, 0x40FE30);
    RH_ScopedGlobalInstall(VectorSubtract, 0x40FE60);
    RH_ScopedGlobalInstall(FloatTimesVector, 0x40FE90);
    RH_ScopedGlobalInstall(VectorTimesFloat, 0x40FEC0);
    RH_ScopedGlobalInstall(VectorDividedByFloat, 0x4119D0);
    RH_ScopedOverloadedInstall(operator-=, "v3d", 0x406D70, void(CVector::*)(const CVector&));
    RH_ScopedOverloadedInstall(operator+=, "v3d", 0x411A00, void(CVector::*)(const CVector&));
    RH_ScopedOverloadedInstall(operator*=, "float", 0x40FEF0, void(CVector::*)(float));
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

// 0x406DA0: (x*x + y*y) + z*z en la x87 sin guardar como float (ECX y EDX no se tocan; el valor sale en ST0)
float CVector::SquaredMagnitudeExe() const
{
    return float(((double)x * x + (double)y * y) + (double)z * z);
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
// 0x40FDD0 (lote 153A, banco Fn153A; thiscall, `ret 8`): como el original, cada componente se carga (fld left.k, en el registro), se suma (fadd right.k, en memoria) y se guarda antes de pasar a la siguiente: con
// `this` solapado con `left` o `right` se ven las escrituras anteriores. Las lecturas volatiles fijan que operando va en el registro de la x87 (cuenta para los NaN senalizantes).
void CVector::Sum(const CVector& left, const CVector &right)
{
    const volatile float* l = &left.x;
    const double lx = l[0]; x = float(lx + right.x);
    const double ly = l[1]; y = float(ly + right.y);
    const double lz = l[2]; z = float(lz + right.z);
}

// Subtracts left - right and stores result
// 0x40FE00 (thiscall, `ret 8`): como el original, cada componente se carga (fld left.k, en el registro), se resta (fsub right.k, en memoria) y se guarda antes de pasar a la siguiente: con
// `this` solapado con `left` o `right` se ven las escrituras anteriores. Las lecturas volatiles fijan que operando va en el registro de la x87 (cuenta para los NaN senalizantes).
void CVector::Difference(const CVector& left, const CVector &right)
{
    const volatile float* l = &left.x;
    const double lx = l[0]; x = float(lx - right.x);
    const double ly = l[1]; y = float(ly - right.y);
    const double lz = l[2]; z = float(lz - right.z);
}

// Adds value from the second vector.
// 0x411A00: como el original, el operando `right` va en el registro (fld) y el componente propio en memoria (fadd): x = right.x + x, luego y y z.
void CVector::operator+=(const CVector& right)
{
    x = right.x + x;
    y = right.y + y;
    z = right.z + z;
}

// 0x406D70 (lote 105A): como el original, cada componente es un fld de la propia (registro) + fsub de la del otro (memoria) + fstp, de x a z, leyendo la memoria viva (si `right` solapa
// con este vector, las escrituras anteriores se ven). El EXE devuelve EAX = this, que ningun llamador lee.
void CVector::operator-=(const CVector& right)
{
    volatile float* me = &x;
    const volatile float* other = &right.x;
    const double mx = me[0]; me[0] = float(mx - other[0]);
    const double my = me[1]; me[1] = float(my - other[1]);
    const double mz = me[2]; me[2] = float(mz - other[2]);
}

void CVector::operator*=(const CVector& right)
{
    x *= right.x;
    y *= right.y;
    z *= right.z;
}

// Multiplies vector by a floating point value
// 0x40FEF0: como el original, el factor va en el registro (fld) y el componente en memoria (fmul), componente a componente. Las lecturas volatiles de las funciones de este archivo
// fijan que operando va en el registro y cual en memoria: la x87 elige entre dos NaN segun eso (un NaN senalizante de memoria pierde frente a un NaN del registro).
void CVector::operator *= (float multiplier)
{
    const volatile float* factor = &multiplier;
    const double fx = *factor; x = float(fx * x);
    const double fy = *factor; y = float(fy * y);
    const double fz = *factor; z = float(fz * z);
}

// Divides vector by a floating point value
void CVector::operator /= (float divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
}

// 0x59C670 (thiscall, ECX = this = destino, `ret 8`): this = matriz * punto con la x87 del EXE (lote 101A). x = ((up.x*z + fwd.x*y) + right.x*x) + pos.x, y = ((up.y*z + right.y*x) + fwd.y*y) + pos.y,
// z = ((up.z*z + right.z*x) + fwd.z*y) + pos.z; el primer factor de cada producto va en el registro (fld) y el segundo en memoria (fmul) y la posicion se suma desde memoria (fadd). Cada componente
// se escribe antes de calcular la siguiente, leyendo la memoria viva: con `this` solapado con el vector o con la matriz (v.FromMultiply(m, v)) las lecturas siguientes ven la escritura anterior.
#pragma float_control(precise, on, push)
void CVector::FromMultiply(const CMatrix& matrix, const CVector& vector) {
    const volatile float* const mr = reinterpret_cast<const float*>(&matrix);   // operandos de registro (fld); la matriz son 16 floats: right 0..2, forward 4..6, up 8..10, pos 12..14
    const float* const m = reinterpret_cast<const float*>(&matrix);
    const float* const v = &vector.x;
    volatile float* const out = &x;
    {
        const double a1 = mr[8]; const double p1 = a1 * v[2];
        const double a2 = mr[4]; const double p2 = a2 * v[1];
        const double a3 = mr[0]; const double p3 = a3 * v[0];
        out[0] = float(((p1 + p2) + p3) + m[12]);
    }
    {
        const double b1 = mr[9]; const double q1 = b1 * v[2];
        const double b2 = mr[1]; const double q2 = b2 * v[0];
        const double b3 = mr[5]; const double q3 = b3 * v[1];
        out[1] = float(((q1 + q2) + q3) + m[13]);
    }
    {
        const double c1 = mr[10]; const double r1 = c1 * v[2];
        const double c2 = mr[2]; const double r2 = c2 * v[0];
        const double c3 = mr[6]; const double r3 = c3 * v[1];
        out[2] = float(((r1 + r2) + r3) + m[14]);
    }
}
#pragma float_control(pop)

// 0x59C6D0 (thiscall, `ret 8`; ECX = destino, matriz, vector): como FromMultiply (0x59C670) sin la posicion. x = (m8*v.z + m4*v.y) + m0*v.x, y = (m9*v.z + m1*v.x) + m5*v.y, z = (m10*v.z + m2*v.x) + m6*v.y;
// primer factor de cada producto en el registro (fld) y el vector como operando de memoria (fmul), y cada componente se escribe antes de calcular la siguiente leyendo la memoria viva: con `this` solapado
// con el vector o con la matriz las lecturas siguientes ven la escritura anterior (lote 103A, banco Fn103A).
#pragma float_control(precise, on, push)
void CVector::FromMultiply3x3(const CMatrix& matrix, const CVector& vector) {
    const volatile float* const mr = reinterpret_cast<const float*>(&matrix);   // operandos de registro (fld)
    const float* const v = &vector.x;
    volatile float* const out = &x;
    {
        const double a1 = mr[8]; const double p1 = a1 * v[2];
        const double a2 = mr[4]; const double p2 = a2 * v[1];
        const double a3 = mr[0]; const double p3 = a3 * v[0];
        out[0] = float((p1 + p2) + p3);
    }
    {
        const double b1 = mr[9]; const double q1 = b1 * v[2];
        const double b2 = mr[1]; const double q2 = b2 * v[0];
        const double b3 = mr[5]; const double q3 = b3 * v[1];
        out[1] = float((q1 + q2) + q3);
    }
    {
        const double c1 = mr[10]; const double r1 = c1 * v[2];
        const double c2 = mr[2]; const double r2 = c2 * v[0];
        const double c3 = mr[6]; const double r3 = c3 * v[1];
        out[2] = float((r1 + r2) + r3);
    }
}
#pragma float_control(pop)

float CVector::Heading(bool limitAngle) const {
    const auto radians = std::atan2(-x, y);
    if (limitAngle) {
        return CGeneral::LimitRadianAngle(radians);
    }
    return radians;
}

// 0x59C730: cada componente es la diferencia de dos productos que la x87 deja en registros (24 bits de mantisa, exponente ampliado); solo se guarda como float el resultado.
// El EXE lee las seis componentes antes de escribir ninguna (`out` puede solaparse con `a` o `b`). Orden de los operandos como el EXE (primer factor en el registro, segundo en memoria):
// x = b.z*a.y - a.z*b.y, y = a.z*b.x - b.z*a.x, z = a.x*b.y - b.x*a.y.
CVector* CrossProduct(CVector* out, CVector* a, CVector* b)
{
    const volatile CVector* va = a;
    const volatile CVector* vb = b;
    const double p1 = vb->z; const double t1 = p1 * a->y;
    const double q1 = va->z; const double u1 = q1 * b->y;
    const float x = float(t1 - u1);
    const double p2 = va->z; const double t2 = p2 * b->x;
    const double q2 = vb->z; const double u2 = q2 * a->x;
    const float y = float(t2 - u2);
    const double p3 = va->x; const double t3 = p3 * b->y;
    const double q3 = vb->x; const double u3 = q3 * a->y;
    const float z = float(t3 - u3);
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

// 0x59C790: matriz (3x3) por vector, con las sumas en el orden del EXE ((z + y) + x para x; (z + x) + y para y y z) y el elemento de la matriz en el registro; lee todo antes de escribir
// (`out` puede solaparse con `vector`).
CVector* Multiply3x3Exe(CVector* out, const CMatrix* matrix, const CVector* vector)
{
    const volatile float* m = reinterpret_cast<const float*>(matrix);   // right = m[0..2], forward = m[4..6], up = m[8..10]
    const double a1 = m[8]; const double xz = a1 * vector->z;
    const double a2 = m[4]; const double xy = a2 * vector->y;
    const double a3 = m[0]; const double xx = a3 * vector->x;
    const float x = float((xz + xy) + xx);
    const double b1 = m[9]; const double yz = b1 * vector->z;
    const double b2 = m[1]; const double yx = b2 * vector->x;
    const double b3 = m[5]; const double yy = b3 * vector->y;
    const float y = float((yz + yx) + yy);
    const double c1 = m[10]; const double zz = c1 * vector->z;
    const double c2 = m[2]; const double zx = c2 * vector->x;
    const double c3 = m[6]; const double zy = c3 * vector->y;
    const float z = float((zz + zx) + zy);
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

// 0x59C810: vector * matriz (3x3, columnas por producto punto), cdecl con resultado por puntero oculto devuelto en EAX. x = (right.y*v.y + right.z*v.z) + v.x*right.x,
// y = (fwd.y*v.y + fwd.x*v.x) + fwd.z*v.z, z = (up.y*v.y + up.x*v.x) + up.z*v.z; en los dos primeros productos de cada suma el elemento de la matriz va en el registro y el vector en
// memoria, en el tercer producto de x el vector va en el registro (fld v.x; fmul right.x). Lee todo antes de escribir (`out` puede solaparse con `vector` o con la matriz).
CVector* Multiply3x3VMExe(CVector* out, const CVector* vector, const CMatrix* matrix)
{
    const volatile float* m = reinterpret_cast<const float*>(matrix);   // right = m[0..2], forward = m[4..6], up = m[8..10]
    const volatile float* v = &vector->x;
    const double a1 = m[1]; const double xy = a1 * vector->y;
    const double a2 = m[2]; const double xz = a2 * vector->z;
    const double a3 = v[0]; const double xx = a3 * m[0];
    const float x = float((xy + xz) + xx);
    const double b1 = m[5]; const double yy = b1 * vector->y;
    const double b2 = m[4]; const double yx = b2 * vector->x;
    const double b3 = m[6]; const double yz = b3 * vector->z;
    const float y = float((yy + yx) + yz);
    const double c1 = m[9]; const double zy = c1 * vector->y;
    const double c2 = m[8]; const double zx = c2 * vector->x;
    const double c3 = m[10]; const double zz = c3 * vector->z;
    const float z = float((zy + zx) + zz);
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

// 0x40FE30 / 0x40FE60: suma y resta de vectores (cdecl, resultado por puntero oculto que se devuelve en EAX); las seis lecturas van antes de las escrituras.
CVector* VectorAdd(CVector* out, const CVector* a, const CVector* b)
{
    const float z = a->z + b->z;
    const float y = a->y + b->y;
    const float x = a->x + b->x;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

CVector* VectorSubtract(CVector* out, const CVector* a, const CVector* b)
{
    const float z = a->z - b->z;
    const float y = a->y - b->y;
    const float x = a->x - b->x;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

// 0x4119D0 (lote 105A; cdecl, puntero de retorno oculto como primer argumento, EAX = destino): vector / escalar como el EXE, que multiplica por el inverso: r = 1.0 / divisor (fdiv de la x87, 24 bits de
// mantisa y exponente ampliado), `fst` de r a la pila (float redondeado) y z = r * v.z con r SIN redondear a float (el registro), y = float(r) * v.y y x = float(r) * v.x con el reciproco guardado;
// se lee todo antes de escribir (la salida puede solapar el vector). Un divisor diminuto o enorme da un reciproco fuera del rango de float que solo usa la z.
CVector* VectorDividedByFloat(CVector* out, const CVector* vector, float divisor)
{
    const volatile float* d = &divisor;
    const double r = 1.0 / double(*d);
    volatile float stored = float(r);
    const double z = r * vector->z;
    const double sy = stored; const double y = sy * vector->y;
    const double sx = stored; const double x = sx * vector->x;
    out->x = float(x);
    out->y = float(y);
    out->z = float(z);
    return out;
}

// 0x40FE90 (escalar * vector) y 0x40FEC0 (vector * escalar): el escalar va en el registro (fld) y cada componente del vector en memoria (fmul), de z a x; se escribe despues de leer todo.
CVector* FloatTimesVector(CVector* out, float factor, const CVector* vector)
{
    const volatile float* f = &factor;
    const double fz = *f; const double z = fz * vector->z;
    const double fy = *f; const double y = fy * vector->y;
    const double fx = *f; const double x = fx * vector->x;
    out->x = float(x);
    out->y = float(y);
    out->z = float(z);
    return out;
}

CVector* VectorTimesFloat(CVector* out, const CVector* vector, float factor)
{
    const volatile float* f = &factor;
    const double fz = *f; const double z = fz * vector->z;
    const double fy = *f; const double y = fy * vector->y;
    const double fx = *f; const double x = fx * vector->x;
    out->x = float(x);
    out->y = float(y);
    out->z = float(z);
    return out;
}

CVector CrossProduct(const CVector& a, const CVector& b)
{
    return a.Cross(b);
}

// 0x40FDB0: ((z*z + y*y) + x*x) en la x87 sin guardar como float (24 bits de mantisa, exponente ampliado); el valor devuelto sale en ST0 sin redondear a float.
float DotProduct(CVector* v1, CVector* v2)
{
    return float(((double)v1->z * v2->z + (double)v1->y * v2->y) + (double)v1->x * v2->x);
}

float DotProduct(const CVector& v1, const CVector& v2)
{
    return v1.z * v2.z + v1.y * v2.y + v1.x * v2.x;
}

float DotProduct2D(const CVector& v1, const CVector& v2)
{
    return v1.y * v2.y + v1.x * v2.x;
}
