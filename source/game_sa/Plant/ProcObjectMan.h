#pragma once

#include "List_c.h"
#include "ProcSurfaceInfo.h"

class ProcSurfaceInfo_c;
class CObject;
class CEntity;
class CPlantLocTri;

// Registro del pool que vincula un objeto vegetal generado con el triángulo que
// lo produjo. El indicador señala si se asignó una matriz para ese elemento.
class ProcObjectListItem : public ListItem_c<ProcObjectListItem> {
public:
    CEntity*      m_Obj;
    CPlantLocTri* m_LocTri;
    bool          m_bAllocatedMatrix;

public:
    static void InjectHooks();

    // 0x5A3070 (lote 186A, sin validar): constructor del EXE (thiscall, `ret`): solo pone a cero el enlace anterior y siguiente (ListItem_c, 0x4A8DB0) y devuelve `this`.
    ProcObjectListItem* Constructor();
};
VALIDATE_SIZE(ProcObjectListItem, 0x14);

// Gestor global de objetos procedurales y parámetros de generación por
// superficie. Los arreglos fijos respaldan los pools y la lista reúne los activos.
class ProcObjectMan_c {
public:
    int32 m_numAllocatedMatrices;
    int32 m_numProcSurfaceInfos;
    ProcSurfaceInfo_c           m_ProcObjSurfaceInfos[128];
    ProcObjectListItem          m_Objects[512];
    TList_c<ProcObjectListItem> m_ObjectsList;

public:
    static void InjectHooks();

    ProcObjectMan_c() = default;  // 0x5A3E20
    ~ProcObjectMan_c() = default; // 0x5A3090

    // 0x5A3E20 (lote 186A, sin validar): constructor del EXE (thiscall, `ret`, devuelve `this`).
    ProcObjectMan_c* Constructor();
    // 0x5A3090 (lote 186A, sin validar): destructor del EXE (thiscall, `ret`; no devuelve nada).
    void Destructor();

    bool Init();
    void Update();
    void Exit();
    void LoadDataFile();

    ProcObjectListItem* GetEntityFromPool();
    void ReturnEntityToPool(ProcObjectListItem* item);

    uint8 ProcessTriangleAdded(CPlantLocTri* plant);
    void ProcessTriangleRemoved(CPlantLocTri* plant);
};
VALIDATE_SIZE(ProcObjectMan_c, 0x4C14);

static inline auto& g_procObjMan = StaticRef<ProcObjectMan_c>(0xBB7CB0);
