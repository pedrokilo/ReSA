/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CompressedVector.h"
#include <extensions/FixedFloat.hpp>
#include "Vector.h"

class CStoredCollPoly;
class CColTriangle;
    
/**
 * Representación compacta del plano definido por un triángulo de colisión.
 *
 * Almacena la normal comprimida, el término constante y una orientación discreta
 * basada en el eje dominante de la normal. No es propietario de los vértices de
 * los que se construye.
 */
class CColTrianglePlane {
public:
    // Eje dominante de la normal y signo correspondiente.
    enum class Orientation : uint8 {
        POS_X,
        NEG_X,
        POS_Y,
        NEG_Y,
        POS_Z,
        NEG_Z,
    };
public:
    CompressedUnitVector     m_normal{};       //< Normal unitaria comprimida de la superficie.
    FixedFloat<int16, 128.f> m_normalOffset{}; //< Término vA.Dot(m_normal) almacenado en punto fijo.
    Orientation              m_orientation{};  //< Eje dominante y signo calculados desde la normal.

public:
    static void InjectHooks();

    CColTrianglePlane(const CStoredCollPoly& poly);
    CColTrianglePlane(const CColTriangle& tri, const CompressedVector* vertices);
    CColTrianglePlane(const CVector& a, const CVector& b, const CVector& c);

    void GetNormal(CVector& out);
    CVector GetNormal() const noexcept { return m_normal; }
    void Set(const CompressedVector* vertices, const CColTriangle& triangle);

    // Distancia firmada escalada por la normal unitaria respecto al plano.
    float GetPtDotNormal(const CVector& pt) const { return pt.Dot(m_normal) - m_normalOffset; }
};
VALIDATE_SIZE(CColTrianglePlane, 0xA);
