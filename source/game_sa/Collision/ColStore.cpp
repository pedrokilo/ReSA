/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColStore
 * Reconstrucción: COMPLETA — 23/23 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 23/23 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ColStore.h"
#include "TheScripts.h"

auto& ms_pColPool = StaticRef<CColPool*>(0x965560);

using ColTreeNode  = CQuadTreeNode<ColDef*>;
auto& ms_pQuadTree = StaticRef<ColTreeNode*>(0x96555C);

namespace {
// Como el original: los bucles del EXE recorren siempre 255 slots fijos (0x2BD4 / 0x2C), no la capacidad del pool.
constexpr int32 COL_SLOTS = 255;

// Como el original: el código de área de la entidad se lee con movzx (sin signo), no con signo como m_AreaCode.
eAreaCodes EntityAreaCode(const CEntity* entity) {
    return static_cast<eAreaCodes>(static_cast<uint8>(entity->GetAreaCode()));
}
} // namespace

void* ColDef::operator new(unsigned size) {
    return ms_pColPool->New();
}

void ColDef::operator delete(void* data) {
    ms_pColPool->Delete(static_cast<ColDef*>(data));
}

// 0x4113F0
// Initialise
void CColStore::Initialise()
{
    const auto rect = CRect(-3000.0F, -3000.0F, 3000.0F, 3000.0F);
    ms_bCollisionNeeded = false;
    if (!ms_pColPool) {
        // Como el original: operator new de 0x14 bytes y el constructor del pool (0x410F80): objetos y mapa con
        // new[] sin relleno del almacenamiento, cada byte del mapa en 0x80; el byte +0x11 no se escribe.
        auto* const pool = static_cast<uint8*>(::operator new(sizeof(CColPool)));
        if (pool) {
            *reinterpret_cast<void**>(pool + 0x0) = ::operator new[](TOTAL_COL_MODEL_IDS * sizeof(ColDef));
            auto* const flags                     = static_cast<uint8*>(::operator new[](TOTAL_COL_MODEL_IDS));
            *reinterpret_cast<uint8**>(pool + 0x4) = flags;
            pool[0x10]                             = 1;
            *reinterpret_cast<int32*>(pool + 0x8)  = TOTAL_COL_MODEL_IDS;
            *reinterpret_cast<int32*>(pool + 0xC)  = -1;
            for (auto i = 0u; i < TOTAL_COL_MODEL_IDS; i++) {
                flags[i] = 0x80;
            }
        }
        ms_pColPool = reinterpret_cast<CColPool*>(pool);
    }

    AddColSlot("generic");
    ms_pQuadTree = new ColTreeNode(rect, 3);
}

// 0x4114D0
void CColStore::Shutdown()
{
    for (auto i = 0; i < COL_SLOTS; i++) {
        if (ms_pColPool->GetAt(i)) {
            RemoveColSlot(i);
        }
    }

    // Como el original: sin el destructor de CPool (que rellena y deja otros valores); se ponen a cero
    // almacenamiento, mapa, capacidad y primer libre, y solo se libera la memoria propia del pool.
    if (auto* const pool = reinterpret_cast<uint8*>(ms_pColPool)) {
        if (*reinterpret_cast<int32*>(pool + 0x8) > 0) {
            if (pool[0x10]) {
                delete[] *reinterpret_cast<uint8**>(pool + 0x0);
                delete[] *reinterpret_cast<uint8**>(pool + 0x4);
            }
            *reinterpret_cast<uint8**>(pool + 0x0) = nullptr;
            *reinterpret_cast<uint8**>(pool + 0x4) = nullptr;
            *reinterpret_cast<int32*>(pool + 0x8)  = 0;
            *reinterpret_cast<int32*>(pool + 0xC)  = 0;
        }
        operator delete(pool);
    }
    ms_pColPool = nullptr;

    delete ms_pQuadTree;
    ms_pQuadTree = nullptr;
}

// 0x4103D0
void SetIfCollisionIsRequired(const CVector2D& vecPos, ColDef* def) {
    if (CColStore::ms_EntityAreaCode == AREA_CODE_NORMAL_WORLD && def->m_bInterior) {
        return;
    }
    if (!def->m_Area.IsPointInside(vecPos)) {
        return;
    }
    def->m_bCollisionIsRequired = true;
}

// 0x410470
void SetIfCollisionIsRequiredReducedBB(const CVector2D& vecPos, ColDef* def) {
    if ((CColStore::ms_EntityAreaCode != AREA_CODE_NORMAL_WORLD) != def->m_bInterior) {
        return;
    }
    if (!def->m_Area.IsPointInside(vecPos, -80.0F)) {
        return;
    }
    def->m_bCollisionIsRequired = true;
}

// 0x411140
int32 CColStore::AddColSlot(const char* name) {
    // Como el original: el slot sale de CPool::New (0x411030) sin construir el ColDef; el nombre no se guarda.
    auto* const def = ms_pColPool->New();
    def->m_bActive              = false;
    def->m_bCollisionIsRequired = false;
    def->m_bProcedural          = false;
    def->m_bInterior            = false;
    def->m_Area                 = CRect();
    def->m_nModelIdStart        = SHRT_MAX;
    def->m_nModelIdEnd          = SHRT_MIN;
    def->m_nRefCount            = 0;
    if (!_stricmp(name, "procobj") || !_stricmp(name, "proc_int") || !_stricmp(name, "proc_int2")) {
        def->m_bProcedural = true;
    }
    if (!_strnicmp(name, "int_la", 6)
        || !_strnicmp(name, "int_sf", 6)
        || !_strnicmp(name, "int_veg", 7)
        || !_strnicmp(name, "int_cont", 8)
        || !_strnicmp(name, "gen_int1", 8)
        || !_strnicmp(name, "gen_int2", 8)
        || !_strnicmp(name, "gen_int3", 8)
        || !_strnicmp(name, "gen_int4", 8)
        || !_strnicmp(name, "gen_int5", 8)
        || !_strnicmp(name, "gen_intb", 8)
        || !_strnicmp(name, "savehous", 8)
        || !_stricmp(name, "props")
        || !_stricmp(name, "props2")
        || !_strnicmp(name, "levelmap", 8)
        || !_strnicmp(name, "stadint", 7)) {
        def->m_bInterior = true;
    }
    return ms_pColPool->GetIndex(def);
}

// 0x4103A0
void CColStore::AddCollisionNeededAtPosn(const CVector& pos)
{
    ms_vecCollisionNeeded = pos;
    ms_bCollisionNeeded = true;
}

// 0x4107A0
void CColStore::AddRef(int32 colNum)
{
    auto *colDef = ms_pColPool->GetAt(colNum);
    ++colDef->m_nRefCount; //BUG: We don't check whether the GetAt returned nullptr, which it can
}

// Como el original no hay búsqueda por nombre (`FindColSlot()` devuelve -1) y AddColSlot no guarda el nombre,
// así que esta variante tampoco encuentra nada: cada archivo COL recibe un slot nuevo, como en el EXE.
int32 CColStore::FindColSlot(const char*)
{
    return -1;
}

// 0x410EC0
void CColStore::BoundingBoxesPostProcess() {
    ZoneScoped;

    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        if (CColAccel::isCacheLoading())
        {
            CColAccel::getColDef(*def);
        }
        else
        {
            def->m_Area.Resize(120.0F, 120.0F);
            CColAccel::addColDef(*def);
        }

        ms_pQuadTree->AddItem(def, def->m_Area);
    }
}

// 0x410AD0
void CColStore::EnsureCollisionIsInMemory(const CVector& pos)
{
    if (CStreaming::ms_disableStreaming)
        return;
    // Como el original: FindPlayerPed se llama dos veces (comprobación y lectura del área).
    const auto area = FindPlayerPed() ? EntityAreaCode(FindPlayerPed()) : CGame::GetCurrentAreaCode();
    if (area != CGame::GetCurrentAreaCode()) {
        return;
    }
    // Como el original: se pasa AREA_CODE_NONE, así que SetCollisionRequired vuelve a buscar al jugador.
    SetCollisionRequired(pos, AREA_CODE_NONE);
    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def || !def->m_bCollisionIsRequired)
            continue;

        if (def->m_Area.IsPointInside(pos, -110.0F) && !CStreaming::GetInfo(COLToModelId(i)).IsLoaded()) {
            CStreaming::RequestModel(COLToModelId(i), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);
            if (!TheCamera.GetScreenFadeStatus())
                FrontEndMenuManager.MessageScreen("LOADCOL", false, false);

            CTimer::Suspend();
            CStreaming::LoadAllRequestedModels(true);
            CTimer::Resume();
        }

        def->m_bCollisionIsRequired = false;
    }
}

// 0x410800
CRect* CColStore::GetBoundingBox(int32 colSlot)
{
    return &ms_pColPool->GetAt(colSlot)->m_Area;
}

// 0x410820
void CColStore::IncludeModelIndex(int32 colSlot, int32 modelId)
{
    auto* def = ms_pColPool->GetAt(colSlot);

    if (modelId < def->m_nModelIdStart)
        def->m_nModelIdStart = modelId;

    if (modelId > def->m_nModelIdEnd)
        def->m_nModelIdEnd = modelId;
}

// 0x410CE0
bool CColStore::HasCollisionLoaded(const CVector& pos, eAreaCodes areaCode)
{
    SetCollisionRequired(pos, areaCode);
    auto foundInd = -1;
    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def || !def->m_bCollisionIsRequired)
            continue;

        def->m_bCollisionIsRequired = false;
        if (def->m_Area.IsPointInside(pos, -110.0F) && !def->m_bActive)
        {
            foundInd = i; // Slightly modified the method logic to be more readable, result is the same
            break;
        }
    }

    if (foundInd == -1)
        return true;

    for (auto i = foundInd; i < COL_SLOTS; i++) {
        auto* innerDef = ms_pColPool->GetAt(i);
        if (!innerDef)
            continue;

        innerDef->m_bCollisionIsRequired = false;
    }

    return false;
}

// 0x4113D0
void CColStore::LoadAllBoundingBoxes() {
    ZoneScoped;

    if (CColAccel::isCacheLoading())
        CColAccel::cacheLoadCol();
    else
        LoadAllCollision();
}

// 0x410E60
void CColStore::LoadAllCollision() {
    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        CStreaming::RequestModel(COLToModelId(i), 0);
        CStreaming::LoadAllRequestedModels(false);
        CStreaming::RemoveModel(COLToModelId(i));
    }
}

// 0x410690
void CColStore::LoadCol(int32 colSlot, const char* filename)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    CFileLoader::LoadCollisionFile(filename, colSlot);
    def->m_bActive = true;
}

// 0x4106D0
bool CColStore::LoadCol(int32 colSlot, uint8* data, int32 dataSize)
{
    assert(colSlot <= 255); // Otherwise it would wrap when casted to `uint8` (As that's the type it's stored as in CColModel)

    auto* def = ms_pColPool->GetAt(colSlot);

    bool bLoaded;
    if (def->m_nModelIdStart <= def->m_nModelIdEnd)
        bLoaded = CFileLoader::LoadCollisionFile(data, dataSize, colSlot);
    else
        bLoaded = CFileLoader::LoadCollisionFileFirstTime(data, dataSize, colSlot);

    if (!bLoaded)
        return false;

    def->m_bActive = true;
    return true;
}

// 0x410860
void CColStore::LoadCollision(CVector pos, bool bIgnorePlayerVeh)
{
    if (CStreaming::ms_disableStreaming)
        return;

    auto* player = FindPlayerPed();
    if (player && !bIgnorePlayerVeh)
    {
        if (auto* vehicle = FindPlayerVehicle()) {
            // Como el original: solo X e Y (la Z no se toca).
            pos.x += vehicle->m_vecMoveSpeed.x * 20.0F;
            pos.y += vehicle->m_vecMoveSpeed.y * 20.0F;
        }
    }

    SetCollisionRequired(pos, AREA_CODE_NONE);
    if (ms_bCollisionNeeded)
        SetCollisionRequired(ms_vecCollisionNeeded, CGame::GetCurrentAreaCode());

    for (auto& obj : CTheScripts::MissionCleanUp.m_Objects)
    {
        CEntity* entity = nullptr;
        if (obj.type == MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_VEHICLE)
        {
            entity = GetVehiclePool()->GetAtRef(obj.handle);
            if (!entity || entity->GetStatus() == STATUS_WRECKED)
                continue;
        }
        else if (obj.type == MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_PED)
        {
            entity = GetPedPool()->GetAtRef(obj.handle);
            if (!entity || entity->AsPed()->m_nPedState == PEDSTATE_DIE || entity->AsPed()->m_nPedState == PEDSTATE_DEAD)
                continue;
        }

        if (!entity || entity->AsPhysical()->physicalFlags.bDontLoadCollision || entity->AsPhysical()->physicalFlags.bDontApplySpeed)
            continue;

        ms_EntityAreaCode = EntityAreaCode(entity);
        ms_pQuadTree->ForAllMatching(entity->GetPosition(), SetIfCollisionIsRequiredReducedBB);
    }

    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        // Como el original: el contador de referencias se compara con signo (cmp word, 0 / jg).
        if (def->m_bCollisionIsRequired || def->m_bProcedural || static_cast<int16>(def->m_nRefCount) > 0)
        {
            if (!def->m_bActive)
                CStreaming::RequestModel(COLToModelId(i), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);

            def->m_bCollisionIsRequired = false;
        }
        else if (def->m_bActive)
            CStreaming::RemoveModel(COLToModelId(i));
    }

    if (!bIgnorePlayerVeh)
        ms_bCollisionNeeded = false;
}

// 0x410E00
void CColStore::RemoveAllCollision() {
    ZoneScoped;

    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def)
            continue;

        if (!CStreaming::GetInfo(COLToModelId(i)).IsMissionOrGameRequired())
            CStreaming::RemoveModel(COLToModelId(i));
    }
}

// 0x410730
void CColStore::RemoveCol(int32 colSlot)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    def->m_bActive = false;
    // Como el original: índice de 32 bits (con int16 un final de 32767 no terminaría nunca).
    for (int32 i = def->m_nModelIdStart; i <= def->m_nModelIdEnd; ++i) {
        auto* mi = CModelInfo::GetModelInfo(i);
        if (!mi)
            continue;

        auto* cm = mi->GetColModel();
        if (!cm)
            continue;

        if (mi->IsLod() && cm->m_nColSlot == colSlot)
            cm->RemoveCollisionVolumes();
    }
}

// 0x411330
void CColStore::RemoveColSlot(int32 colSlot)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    if (def->m_bActive)
        RemoveCol(colSlot);

    ms_pQuadTree->DeleteItem(def);
    delete def;
}

// 0x4107D0
void CColStore::RemoveRef(int32 colNum)
{
    auto* def = ms_pColPool->GetAt(colNum);
    --def->m_nRefCount; // BUG: We don't check whether the GetAt returned nullptr, which it can
}

// 0x410C00
void CColStore::RequestCollision(const CVector& pos, eAreaCodes areaCode)
{
    SetCollisionRequired(pos, areaCode);
    for (auto i = 1; i < COL_SLOTS; i++) {
        auto* def = ms_pColPool->GetAt(i);
        if (!def || !def->m_bCollisionIsRequired)
            continue;

        if (def->m_Area.IsPointInside(pos, -110.0F))
            CStreaming::RequestModel(COLToModelId(i), STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY);

        def->m_bCollisionIsRequired = false;
    }
}

// 0x4104E0
void CColStore::SetCollisionRequired(const CVector& pos, eAreaCodes areaCode) {
    if (areaCode == AREA_CODE_NONE) {
        auto* const player = FindPlayerPed();
        areaCode = player ? EntityAreaCode(player) : CGame::GetCurrentAreaCode();
    }
    ms_EntityAreaCode = areaCode;
    if (ms_EntityAreaCode == CGame::GetCurrentAreaCode()) {
        ms_pQuadTree->ForAllMatching(pos, SetIfCollisionIsRequired);
    } else {
        ms_pQuadTree->ForAllMatching(pos, SetIfCollisionIsRequiredReducedBB);
    }
}

ColDef* CColStore::GetInSlot(int32 slot) {
    return ms_pColPool->GetAt(slot);
}

CColPool* CColStore::GetPool() {
    return ms_pColPool;
}

void CColStore::InjectHooks()
{
    RH_ScopedClass(CColStore);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Initialise, 0x4113F0);
    RH_ScopedInstall(Shutdown, 0x4114D0);
    RH_ScopedInstall(AddCollisionNeededAtPosn, 0x4103A0);
    RH_ScopedInstall(AddColSlot, 0x411140);
    RH_ScopedInstall(AddRef, 0x4107A0);
    RH_ScopedInstall(RemoveRef, 0x4107D0);
    RH_ScopedInstall(EnsureCollisionIsInMemory, 0x410AD0);
    RH_ScopedInstall(SetCollisionRequired, 0x4104E0);
    RH_ScopedInstall(RequestCollision, 0x410C00);
    RH_ScopedInstall(RemoveColSlot, 0x411330);
    RH_ScopedInstall(RemoveCol, 0x410730);
    RH_ScopedOverloadedInstall(LoadCol, "", 0x410690, void(*)(int32, const char*));
    RH_ScopedOverloadedInstall(LoadCol, "2", 0x4106D0, bool(*)(int32, uint8*, int32));
    RH_ScopedInstall(IncludeModelIndex, 0x410820);
    RH_ScopedInstall(RemoveAllCollision, 0x410E00);
    RH_ScopedInstall(LoadAllCollision, 0x410E60);
    RH_ScopedInstall(LoadAllBoundingBoxes, 0x4113D0);
    RH_ScopedInstall(BoundingBoxesPostProcess, 0x410EC0);
    RH_ScopedInstall(GetBoundingBox, 0x410800);
    RH_ScopedInstall(HasCollisionLoaded, 0x410CE0);
    RH_ScopedInstall(LoadCollision, 0x410860);

    RH_ScopedGlobalInstall(SetIfCollisionIsRequired, 0x4103D0);
    RH_ScopedGlobalInstall(SetIfCollisionIsRequiredReducedBB, 0x410470);
}
