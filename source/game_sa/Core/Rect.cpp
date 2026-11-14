/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRect
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
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

#include "Rect.h"
#include "General.h"
#include "X87Math.h"

namespace {
// 0x552EE0 (lote 192A, sin validar; el listado decia CQuadTreeNode<T>: lee cuatro floats (+0x0 izquierda, +0x4, +0x8 derecha, +0xC) como un CRect; sin llamadores ni referencias en el EXE, nombre inferido): punto aleatorio dentro del
// rectangulo. thiscall, un argumento de pila (CVector2D* de salida), `ret 4`, 144 bytes. Pasos del EXE: ancho = _ftol2(|(+0x8) - (+0x0)|) (el valor absoluto es `fcom 0.0f; test ah,5; jp; fchs`: solo se niega si es
// estrictamente menor que cero, un NaN no se toca; _ftol2 trunca a 64 bits y se usan los 32 bajos); alto = |(+0xC) - (+0x4)| guardado como float; x = float(rand() % ancho + (+0x0)) con `fild` del resto y `fadd` (un solo redondeo a
// float, idiv con signo: con ancho 0 el EXE y esta funcion fallan igual con la excepcion de division); segunda rand() para y = float(rand() % _ftol2(alto) + (+0x4)). Orden de colaboradores: _ftol2(ancho), alto, rand() (x),
// rand() (y), _ftol2(alto). rand() es CGeneral::GetRandomNumber. Los resultados se escriben como floats en out->x y out->y. Se engancha como S.
__declspec(noinline) void __fastcall RectGetRandomPoint(CRect* self, void* /*edx*/, CVector2D* out) {
    const auto absIfNegative = [](float v) { return v < 0.0f ? -v : v; };
    const auto ftol          = [](float v) { return static_cast<int32>(static_cast<int64>(v)); };

    const int32 width  = ftol(absIfNegative(self->right - self->left));
    const float height = absIfNegative(self->bottom - self->top);
    const int32 xRem   = static_cast<int32>(CGeneral::GetRandomNumber()) % width;
    const float x      = FloatPlusIntCpp(self->left, xRem);
    const int32 yRem   = static_cast<int32>(CGeneral::GetRandomNumber()) % ftol(height);
    const float y      = FloatPlusIntCpp(self->top, yRem);
    out->x = x;
    out->y = y;
}
} // namespace

/**
 * Registra las operaciones geométricas de CRect en sus direcciones originales.
 */
void CRect::InjectHooks()
{
    RH_ScopedClass(CRect);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(IsFlipped, 0x404190);
    RH_ScopedInstall(Restrict, 0x404200);
    RH_ScopedInstall(Resize, 0x404260);
    RH_ScopedOverloadedInstall(IsPointInside, "", 0x404290, bool(CRect::*)(const CVector2D&) const);
    RH_ScopedOverloadedInstall(IsPointInside, "Tolerance", 0x4042D0, bool(CRect::*)(const CVector2D&, float) const);
    RH_ScopedInstall(SetFromCenter, 0x43E020);
    RH_ScopedOverloadedInstall(GetCenter, "", 0x43E050, void(CRect::*)(float*, float*) const);
    RH_ScopedInstall(StretchToPoint, 0x5327F0);
    RH_ScopedGlobalInstall(RectMin, 0x404330);
    RH_ScopedNamedGlobalInstall(RectGetRandomPoint, "GetRandomPoint", 0x552EE0); // lote 192A (sin validar)
}

// 0x404330
// Devuelve el primer valor solo si es estrictamente menor que el segundo; en cualquier otro caso (mayor, igual o comparacion no ordenada con algun NaN) devuelve el segundo, cargado
// con fld en ST0 (un NaN senalizante sale silenciado). FCOM deja C0, C2 y C3 a 1 con NaN: `test ah,5` da paridad par y el EXE salta a devolver el segundo.
float RectMin(float first, float second)
{
    return first < second ? first : second;
}

// 0x404200
// Expande los límites actuales cuando restriction sobresale de ellos; pese al
// nombre histórico, el resultado abarca también el rectángulo recibido.
inline void CRect::Restrict(const CRect& restriction)
{
    if (restriction.left < left)
        left = restriction.left;

    if (restriction.right > right)
        right = restriction.right;

    if (restriction.bottom < bottom)
        bottom = restriction.bottom;

    if (restriction.top > top)
        top = restriction.top;
}

// 0x404260
// Amplía o contrae simétricamente el rectángulo en ambos ejes.
inline void CRect::Resize(float resizeX, float resizeY)
{
    left   -= resizeX;
    right  += resizeX;
    bottom    -= resizeY;
    top += resizeY;
}

// 0x404290
// Comprueba inclusión usando límites cerrados: los puntos sobre el borde cuentan.
inline bool CRect::IsPointInside(const CVector2D& point) const
{
    // Como el original: comparaciones negadas, un NaN cuenta como dentro.
    return !(point.x < left)
        && !(point.x > right)
        && !(point.y < bottom)
        && !(point.y > top);
}

// 0x4042D0
// Igual que IsPointInside(), pero extiende virtualmente cada borde por tolerance.
inline bool CRect::IsPointInside(const CVector2D& point, float tolerance) const
{
    // Como el original: comparaciones negadas, un NaN cuenta como dentro.
    return !(left   - tolerance > point.x)
        && !(tolerance + right  < point.x)
        && !(bottom - tolerance > point.y)
        && !(tolerance + top    < point.y);
}

// Comprueba si existe intersección entre ambos rangos según la convención de ejes
// utilizada por CRect. Se conserva el nombre histórico de la función.
bool CRect::IsRectInside(const CRect& rect) const {
    return rect.left <= right && rect.right >= left
        && rect.top <= bottom && rect.bottom >= top;
}

// 0x43E020
// Construye un cuadrado a partir de un centro y una semiextensión común.
inline void CRect::SetFromCenter(float x, float y, float size)
{
    left   = x - size;
    bottom    = y - size;
    right  = x + size;
    top = y + size;
}

// 0x43E050
// Devuelve el punto medio aritmético de los límites horizontal y vertical.
inline void CRect::GetCenter(float* x, float* y) const
{
    *x = (right + left) / 2.0f;
    *y = (bottom + top) / 2.0f;
}

// 0x5327F0
// Ajusta únicamente los bordes necesarios para que el punto pase a quedar incluido.
inline void CRect::StretchToPoint(float x, float y)
{
    if (x < left)
        left = x;

    if (x > right)
        right = x;

    if (y < bottom)
        bottom = y;

    if (y > top)
        top = y;
}

/**
 * Satura cada coordenada de @p pt al intervalo permitido por el rectángulo.
 * @return true si al menos una coordenada tuvo que modificarse.
 */
bool CRect::DoConstrainPoint(CVector2D& pt) const {
    auto hasConstrained = false;

    const auto DoConstrain = [&](float& outPos, float constraint) {
        outPos         = constraint;
        hasConstrained = true;
    };

    if (right < pt.x) {
        DoConstrain(pt.x, right);
    } else if (pt.x < left) {
        DoConstrain(pt.x, left);
    }

    if (pt.y > top) {
        DoConstrain(pt.y, top);
    } else if (bottom > pt.y) {
        DoConstrain(pt.y, bottom);
    }

    return hasConstrained;
}

// NOTSA
// Comprueba solapamiento incluyendo el contacto exacto sobre los bordes.
bool CRect::OverlapsWith(const CRect& o) const {
    return (right >= o.left && left <= o.right)
        && (bottom <= o.top && top >= o.bottom);
}

// NOTSA
// Comprueba que todos los límites de o queden dentro o sobre este rectángulo.
bool CRect::Contains(const CRect& o) const {
    return (o.left >= left && o.right <= right)
        && (o.top <= top && o.bottom >= bottom);
}
