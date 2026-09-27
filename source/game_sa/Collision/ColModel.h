/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "BoundingBox.h"
#include "ColSphere.h"
#include "CollisionData.h"

/**
 * Modelo de colisión completo asociado normalmente a una entidad o model info.
 *
 * Combina volúmenes delimitadores rápidos con CCollisionData opcional. Los datos
 * detallados pueden vivir en una única asignación contigua o en varias asignaciones;
 * m_bIsSingleColDataAlloc determina el procedimiento correcto de liberación.
 */
class CColModel {
public:
    CBoundingBox m_boundBox;    //!< Caja delimitadora usada para rechazo rápido.
    CSphere      m_boundSphere; //!< Esfera delimitadora del modelo completo.
    uint8        m_nColSlot;    //!< Slot/origen del recurso de colisión.
    union {
        struct {
            uint8 m_bHasCollisionVolumes : 1; // Indica que el modelo dispone de volúmenes utilizables; AKA m_bNotEmpty. // AKA `m_bNotEmpty`
            uint8 m_bIsSingleColDataAlloc : 1; // Cabecera y arrays comparten una única asignación.
            uint8 m_IsActive : 1; // Controla si el destructor debe liberar la información asociada.
        };
        uint8 m_nFlags;
    };
    CCollisionData* m_pColData; //!< Geometría detallada; su ownership depende del modo de asignación anterior.

public:
    static void InjectHooks();

    CColModel();
    ~CColModel();

    static void* operator new(unsigned size);
    static void operator delete(void* data);
    CColModel& operator=(const CColModel& colModel);

public:
    // Crea CCollisionData independiente sin reservar todavía arrays geométricos.
    void AllocateData();
    // Construye cabecera y arrays en un único bloque contiguo.
    void AllocateData(int32 numSpheres, int32 numBoxes, int32 numLines, int32 numVertices, int32 numTriangles, bool bUsesDisks);
    // Convierte un bloque único en CCollisionData y arrays con asignaciones independientes.
    void MakeMultipleAlloc();
    // Libera los datos usando la estrategia correspondiente al modo de asignación.
    void RemoveCollisionVolumes();
    void CalculateTrianglePlanes();
    void RemoveTrianglePlanes();

private:
    void AllocateData(int32 size);

public:
    // HELPERS
    [[nodiscard]] auto GetTriCount() const noexcept     { return m_pColData ? m_pColData->m_nNumTriangles : 0u; }
    [[nodiscard]] float GetBoundRadius() const noexcept { return m_boundSphere.m_fRadius; }
    auto& GetBoundCenter() { return m_boundSphere.m_vecCenter; }
    auto& GetBoundingBox() { return m_boundBox; }
    auto& GetBoundingSphere() { return m_boundSphere; }
    CCollisionData* GetData() const { return m_pColData; }
};

VALIDATE_SIZE(CColModel, 0x30);
