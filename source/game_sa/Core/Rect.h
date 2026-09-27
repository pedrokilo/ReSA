/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector2D.h"

/* CRect describe un rectángulo alineado con los ejes.

    La nomenclatura left/top/right/bottom procede de la estructura original y
    convive con una orientación de Y que puede resultar poco intuitiva. Por ello
    conviene usar las operaciones de la propia clase en lugar de reinterpretar
    manualmente sus límites.

    A(left;top)_____________________→
    |                               |
    |                               |
    |                               |
    |_________________B(right;bottom)
    ↓
*/

class CRect {
public:
    // El estado por defecto queda deliberadamente "flipped". Esto permite usar
    // StretchToPoint() para construir progresivamente un rectángulo desde puntos.
    //union {
    //    struct {
    //        CVector2D tl{1000000.0F, -1000000.0F};
    //    }
    //    struct {
    //        float left, top;
    //    };
    //    struct {
    //        float x1, y1;
    //    };
    //};
    //union {
    //    struct {
    //        CVector2D br{-1000000.0F, 1000000.0F};
    //    };
    //    struct {
    //        float right, bottom;
    //    };
    //    struct {
    //        float x2, y2;
    //    };
    //};
    float left      =  1000000.0F; // x1
    float top       = -1000000.0F; // y1
    float right     = -1000000.0F; // x2
    float bottom    =  1000000.0F; // y2

public:
    static void InjectHooks();

    CRect() = default; // 0x4041C0 - TODO: Fix retarded argument order to be: (left, top), (right, bottom) - This shit comes from the fact that the original struct had incorrect naming

    constexpr CRect(float left, float bottom, float right, float top) : // minX, minY, maxX, maxY
        left{ left },
        bottom{ bottom },
        right{ right },
        top{ top }
    {
        // If you're here because one of the scripted video games...
        // Just uncomment this check and they'll work
        // Explanation:
        // The positions in tScriptRectangle are supposed to be min x, y max x, y
        // Well, guess what... they aren't
        // So shit works as expected (cause the GPU doesn't really care, it's gonna draw it),
        // but the rectangles are flipped, hence the assert is hit
        assert(!IsFlipped());
    }

    constexpr CRect(const CVector2D& min, const CVector2D& max) : // top-left (min x, y), bottom-right (max x, y)
        CRect{min.x, min.y, max.x, max.y}
    {
    }

    /// Construye el rectángulo delimitador de un círculo con centro y radio dados.
    constexpr CRect(const CVector2D& center, float radius) :
        CRect{ center.x - radius, center.y - radius, center.x + radius, center.y + radius }
    {
    }

    // Detecta límites todavía invertidos o inválidos según la convención de CRect.
    [[nodiscard]] constexpr inline bool IsFlipped() const { // 0x404190
        return left > right || bottom > top;
    }

    // Ajusta los límites para abarcar también restriction.
    void Restrict(const CRect& restriction);
    // Desplaza simétricamente los cuatro bordes por las cantidades indicadas.
    void Resize(float resizeX, float resizeY);
    // Comprueba inclusión con bordes incluidos.
    [[nodiscard]] bool IsPointInside(CVector2D const& point) const;
    // Permite una tolerancia adicional alrededor de los cuatro bordes.
    [[nodiscard]] bool IsPointInside(CVector2D const& point, float tolerance) const;
    [[nodiscard]] bool IsRectInside(const CRect& rect) const; // NOTSA (or maybe it is?)
    void SetFromCenter(float x, float y, float size);
    void GetCenter(float* x, float* y) const;
    [[nodiscard]] inline CVector2D GetCenter() const { return { (right + left) * 0.5F, (bottom + top) * 0.5F }; }
    // Expande sólo los límites que sean necesarios para incluir el punto.
    void StretchToPoint(float x, float y);

    CVector2D GetTopLeft() const { return { left, top }; }
    CVector2D GetBottomRight() const { return { right, bottom }; }

    /*!
    * @addr notsa
    * @brief Restringe un punto para que quede dentro del rectángulo.
    *
    * @param pt Punto que se ajustará in-place.
    *
    * @return true si alguna coordenada fue modificada.
    */
    bool DoConstrainPoint(CVector2D& pt) const;

    /*!
    * @addr notsa
    * @brief Obtiene las esquinas en orden: superior izquierda, superior derecha, inferior derecha e inferior izquierda.
    * 
    * @param z Coordenada Z común para las cuatro esquinas.
    */
    auto GetCorners3D(float z) const -> std::array<CVector, 4> {
        return {
            CVector{ left,  top, z},
            CVector{ right, top, z},
            CVector{ right, bottom, z},
            CVector{ left,  bottom, z}
        };
    }
    
    /*!
    * @notsa
    * @brief Comprueba si este rectángulo se solapa con otro, incluyendo sus bordes.
    */
    bool OverlapsWith(const CRect& o) const;

    /*!
    * @notsa
    * @brief Comprueba si el rectángulo recibido está contenido por completo en éste.
    */
    bool Contains(const CRect& o) const;

    bool operator==(const CRect&) const = default;
    bool operator!=(const CRect&) const = default;
};

VALIDATE_SIZE(CRect, 0x10);
