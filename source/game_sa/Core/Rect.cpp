/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRect
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/8 entradas.
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
    return point.x >= left
        && point.x <= right
        && point.y >= bottom
        && point.y <= top;
}

// 0x4042D0
// Igual que IsPointInside(), pero extiende virtualmente cada borde por tolerance.
inline bool CRect::IsPointInside(const CVector2D& point, float tolerance) const
{
    return left   - tolerance <= point.x
        && right  + tolerance >= point.x
        && bottom - tolerance <= point.y
        && top    + tolerance >= point.y;
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
