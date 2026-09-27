/*
        Plugin-SDK file
        Authors: GTA Community. See more here
        https://github.com/DK22Pac/plugin-sdk
        Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

/**
 * Segmento de colisión definido por dos puntos 3D.
 *
 * Además de los extremos conserva dos campos de tamaño heredados del formato de
 * colisión original. Las utilidades de esta clase sólo operan sobre la geometría
 * del segmento salvo que se indique lo contrario.
 */
class CColLine {
public:
    static void InjectHooks();

    CColLine() = default;
    CColLine(const CVector& start, const CVector& end);

    void Set(const CVector& start, const CVector& end);

    /*!
    * @notsa
    * @brief Calcula la distancia desde un punto al segmento usando CCollision::DistToLine().
    */
    float DistTo(CVector pos) const;

    /*!
    * @addr notsa
    * @brief Dibuja el segmento en el mundo 3D como ayuda visual.
    *
    * @param transform Transformación aplicada a ambos extremos.
    * @param color     Color utilizado para el segmento.
    */
    void DrawWireFrame(CRGBA color, const CMatrix& transform = CMatrix::Unity()) const;

    //! Indica si ambos extremos comparten exactamente la misma posición XY.
    bool IsVertical() const { return CVector2D{ m_vecStart } == CVector2D{ m_vecEnd }; }

    friend auto TransformObject(const CColLine& ln, const CMatrix& transform) -> CColLine {
        return { transform.TransformPoint(ln.m_vecStart), transform.TransformPoint(ln.m_vecEnd) };
    }

public:
    CVector m_vecStart;   //!< Extremo inicial.
    float   m_fStartSize; //!< Campo de tamaño conservado por compatibilidad con el formato original.
    CVector m_vecEnd;     //!< Extremo final.
    float   m_fEndSize;   //!< Campo de tamaño conservado por compatibilidad con el formato original.

};

VALIDATE_SIZE(CColLine, 0x20);
