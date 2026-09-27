/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "RGBA.h"

/**
 * Caja tridimensional alineada con los ejes utilizada como volumen delimitador.
 *
 * m_vecMin y m_vecMax representan los extremos mínimo y máximo. Recalc() puede
 * corregir cajas creadas con esos extremos intercambiados.
 */
class CBox {
public:
    CVector m_vecMin{}, m_vecMax{}; //!< Límites mínimo y máximo en X/Y/Z.

    constexpr CBox() = default;
    constexpr CBox(CVector min, CVector max) : m_vecMin(min), m_vecMax(max) {}

    void Set(const CVector& vecMin, const CVector& vecMax);

    //! Normaliza cada eje intercambiando sus extremos cuando min > max.
    void Recalc();

    CVector GetSize()   const { return m_vecMax - m_vecMin; }
    float   GetWidth()  const { return m_vecMax.x - m_vecMin.x; }
    float   GetLength() const { return m_vecMax.y - m_vecMin.y; }
    float   GetHeight() const { return m_vecMax.z - m_vecMin.z; }
    CVector GetCenter() const { return (m_vecMax + m_vecMin) / 2.f; }

    //! Comprueba si un punto queda dentro o exactamente sobre la caja.
    bool IsPointInside(const CVector& point) const;

    //! Expande los límites lo mínimo necesario para incluir el punto pt.
    void StretchToPoint(const CVector& pt);

    /*!
    * @addr notsa
    * @brief Dibuja las doce aristas de la caja en el mundo 3D como ayuda visual.
    *
    * @param transform Transformación aplicada a las ocho esquinas antes de dibujarlas.
    * @param color     Color utilizado para las líneas.
    */
    void DrawWireFrame(CRGBA color, const CMatrix& transform = CMatrix::Unity()) const;

    /*!
     * @notsa
     * @brief Obtiene la separación mínima por eje entre la caja y el punto.
     * El vector resultante es cero cuando pt se encuentra dentro de la caja.
     */
    CVector GetShortestVectorDistToPt(const CVector& pt) const;

public:
    static void InjectHooks();
};

VALIDATE_SIZE(CBox, 0x18);
