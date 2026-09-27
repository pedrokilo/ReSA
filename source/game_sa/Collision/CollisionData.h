/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ColSphere.h"
#include "ColBox.h"
#include "ColLine.h"
#include "ColTriangle.h"
#include "ColTrianglePlane.h"
#include "ColDisk.h"
#include "Link.h"
#include "CompressedVector.h"

namespace ColHelpers {
struct TFaceGroup;
};

//
// https://gtamods.com/wiki/Collision_File
//

/**
 * Contenedor de los volúmenes y mallas que componen un modelo de colisión.
 *
 * Puede apuntar a arrays asignados por separado o a regiones de una asignación
 * contigua administrada por CColModel. Además mantiene datos opcionales de sombras,
 * face groups y una caché derivada de planos de triángulo.
 *
 * La estructura reproduce el layout binario original; los punteros no implican por
 * sí solos el tipo de asignación. CColModel::m_bIsSingleColDataAlloc determina cómo
 * debe liberarse el conjunto.
 */
class CCollisionData {
public:
    CCollisionData();

public:
    uint16 m_nNumSpheres;
    uint16 m_nNumBoxes;
    uint16 m_nNumTriangles;
    uint8  m_nNumLines;

    struct {
        uint8 bUsesDisks : 1;     // 0x1 - Always set to false
        uint8 bHasFaceGroups : 1; // 0x2 - See the huge comment below
        uint8 bHasShadowInfo : 1; // 0x4 - See wiki.
    };

    CColSphere* m_pSpheres;
    CColBox* m_pBoxes;

    union {
        CColLine* m_pLines;
        CColDisk* m_pDisks;
    };

    CompressedVector* m_pVertices;

    // If you take a look here: https://gtamods.com/wiki/Collision_File#Body
    // You may notice there's an extra section called `TFaceGroups` before `TFace` (triangles)
    // And it is used in `CCollision::ProcessColModels`.
    //
    // The following is only true if `bHasFaceGroups` flag is set (Which is the case only when loaded by `CFileLoader::LoadCollisionModelVer2/3/4`). **
    // All the data is basically stored in a big buffer, and all the data pointers - except `m_pTrianglePlanes` - point into it.
    // In case the flag `bHasFaceGroups` is set there's an array of `TFaceGroup` before the triangles, but there's no pointer to it.
    // In order to access it you have to do some black magic with `pTriangles`. Here's the memory layout of the `TFaceGroups` data:
    //
    // FaceGroup[]          - -0x8 - And growing downwards by `sizeof(FaceGroup)` (Which is 28)
    // uint32 nFaceGroups;  - -0x4
    // <Triangles>          - FaceGroup data is before the triangles!
    //
    // Whenever accessing this section make sure both `CColModel::bSingleAlloc` and `bHasFaceGroups` is set
    // (also, please, assert if `bHasFaceGroups` is set but `bSingleAlloc` isnt)
    //
    // NOTEs:
    // ** Col models may also be loaded by the Collision plugin from a clump file - In this case `CFileLoader::LoadCollisionModelVer2/3` is called, but then
    //    the col data is reallocated using `MakeMultipleAlloc` which uses `Copy` to copy the data, in this case the face groups aren't copied. (And the flag is set to false in the ctor)
    CColTriangle*      m_pTriangles;          // 0x18
    CColTrianglePlane* m_pTrianglePlanes;     // 0x1C
    uint32             m_nNumShadowTriangles; // 0x20
    uint32             m_nNumShadowVertices;  // 0x24
    CompressedVector*  m_pShadowVertices;     // 0x28
    CColTriangle*      m_pShadowTriangles;    // 0x2C

    // <size 0x30>

public:
    static void InjectHooks();

    // Libera la geometría asignada por separado y deja esta instancia vacía.
    void RemoveCollisionVolumes();
    // Copia profundamente geometría principal y de sombras; no conserva face groups.
    void Copy(const CCollisionData& src);
    // Genera la caché de planos y reserva al final espacio para el enlace de caché.
    void CalculateTrianglePlanes();
    // Elimina únicamente la caché derivada de planos.
    void RemoveTrianglePlanes();
    void GetTrianglePoint(CVector& outVec, int32 vertId);
    void GetShadTrianglePoint(CVector& outVec, int32 vertId);
    // Guarda el enlace en el espacio alineado posterior a m_pTrianglePlanes.
    void SetLinkPtr(CLink<CCollisionData*>* link);
    // Recupera el enlace almacenado junto a la caché de planos.
    CLink<CCollisionData*>* GetLinkPtr();

    // NOTSA section
    auto GetNumFaceGroups() const -> uint32;

    auto GetSpheres()        const { return std::span{ m_pSpheres, m_nNumSpheres }; }
    auto GetBoxes()          const { return std::span{ m_pBoxes, m_nNumBoxes }; }
                             
    auto GetNumTris()        const { return m_nNumTriangles; }
    auto GetTris()           const { return std::span{ m_pTriangles, m_nNumTriangles }; }
    auto GetTriVerts()       const { return m_pVertices; } // Sadly there's no easy way to provide a span here - we don't know the number of vertices, and finding it is expensive
    auto GetTriPlanes()      const { return std::span{ m_pTrianglePlanes, m_nNumTriangles }; }
                             
    auto GetShdwTris()       const { return std::span{ m_pShadowTriangles, m_nNumShadowTriangles }; }
    auto GetShdwTriVerts()   const { return std::span{ m_pShadowVertices, m_nNumShadowVertices }; }
                             
    auto GetLines()          const { return std::span{ m_pLines, m_nNumLines }; }
                             
    auto GetFaceGroups()     const -> std::span<ColHelpers::TFaceGroup>;

    [[nodiscard]] auto GetTriVertices(const CColTriangle& tri) const->std::array<CVector, 3>;

    void AllocateLines(uint32 num);

    void SetSpheres(const CColSphere* spheres);

private:
    // HELPERS
    template <typename T> T* GetPointerToColArray(size_t byteOffset) {
        return reinterpret_cast<T*>(&reinterpret_cast<uint8*>(this)[byteOffset]);
    }

    friend class CColModel;
};
VALIDATE_SIZE(CCollisionData, 0x30);
