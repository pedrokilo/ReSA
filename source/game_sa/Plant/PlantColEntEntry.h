#pragma once


class CEntity;
class CPlantLocTri;

/**
 * Entrada de seguimiento de una CEntity cuya malla puede generar vegetación.
 *
 * Mantiene un array paralelo de CPlantLocTri* indexado por triángulo de colisión y
 * forma parte de listas doblemente enlazadas controladas por CPlantMgr.
 */
class CPlantColEntEntry {
public:
    CEntity*           m_Entity;       //!< Referencia segura a la entidad; no owned.
    CPlantLocTri**     m_Objects;      //!< Array propiedad de la entrada; los triángulos se liberan antes.
    uint16             m_numTriangles; //!< Longitud de m_Objects y número de triángulos del ColData.
    CPlantColEntEntry* m_NextEntry;
    CPlantColEntEntry* m_PrevEntry;

public:
    static void InjectHooks();

    CPlantColEntEntry* AddEntry(CEntity* entity);
    void ReleaseEntry();
};
VALIDATE_SIZE(CPlantColEntEntry, 0x14);
