// SilentPatch (MIT, Silent): banco aislado de EscalatorsCrashFix, TempObjectsFreeFix y SkinBuildingPipelineFix; ver SilentPatchPend7aSelfTest.h.
#include "StdInc.h"
#include "SilentPatchPend7aSelfTest.h"

#include <cstdarg>
#include <cstdio>
#include <vector>

#include "extensions/SilentPatchConfig.h"
#include "Escalators.h"
#include "Entity/Object/Object.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "Pipelines/CustomBuilding/CustomBuildingRenderer.h"

namespace SilentPatchPend7aSelfTest {
namespace {
FILE* g_log = nullptr;

void Log(const char* fmt, ...) {
    if (!g_log) {
        g_log = fopen("logs/silentpatch_pend7a_selftest.log", "a");
    }
    if (!g_log) {
        return;
    }
    va_list a;
    va_start(a, fmt);
    vfprintf(g_log, fmt, a);
    va_end(a);
    fputc('\n', g_log);
    fflush(g_log);
}

bool Enabled() {
    static const bool v = [] {
        char b[8]{};
        return GetEnvironmentVariableA("GTASA_SILENTPATCH_SELFTEST_PEND7A", b, sizeof(b)) != 0 && b[0] == '1';
    }();
    return v;
}

int32 UsedObjects() {
    auto* pool = GetObjectPool();
    int32 n    = 0;
    for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
        if (!pool->IsFreeSlotAtIndex(i)) {
            n++;
        }
    }
    return n;
}

int32 UsedTemp() {
    auto* pool = GetObjectPool();
    int32 n    = 0;
    for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
        if (!pool->IsFreeSlotAtIndex(i) && pool->GetAt(i)->IsTemporary()) {
            n++;
        }
    }
    return n;
}

bool StillInPool(CObject* o) {
    auto* pool = GetObjectPool();
    for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
        if (!pool->IsFreeSlotAtIndex(i) && pool->GetAt(i) == o) {
            return true;
        }
    }
    return false;
}

void LoadModel(int32 model) {
    CStreaming::RequestModel(model, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
}

void Opciones() {
    Log("opciones: SilentPatch=%d EscalatorsCrashFix=%d TempObjectsFreeFix=%d SkinBuildingPipelineFix=%d", (int)SilentPatchEnabled(),
        (int)SilentPatchOption("EscalatorsCrashFix", true), (int)SilentPatchOption("TempObjectsFreeFix", true), (int)SilentPatchOption("SkinBuildingPipelineFix", true));
}

// L20: con objetos temporales visibles, TryToFreeUpTempObjects los libera tambien (SilentPatch) o solo los no visibles (EXE).
void TestTempObjects() {
    LoadModel(MODEL_CJ_BEAR_BOTTLE);
    if (!CStreaming::IsModelLoaded(MODEL_CJ_BEAR_BOTTLE)) {
        Log("TEMP: modelo no cargado, omitido");
        return;
    }
    const CVector cam = TheCamera.GetPosition();
    const CVector fwd = TheCamera.GetForward();
    std::vector<CObject*> mine;
    for (int i = 0; i < 3; i++) {
        auto* o = new CObject(MODEL_CJ_BEAR_BOTTLE, true);
        o->m_nObjectType = OBJECT_TEMPORARY;
        const bool visibleOne = i < 2;
        o->SetPosn(visibleOne ? cam + fwd * (6.0f + (float)i) : cam - fwd * 80.0f);
        CWorld::Add(o);
        mine.push_back(o);
    }
    for (size_t i = 0; i < mine.size(); i++) {
        Log("TEMP: objeto %zu IsVisible=%d", i, (int)mine[i]->IsVisible());
    }
    const int32 tempBefore = UsedTemp(), usedBefore = UsedObjects();
    CObject::TryToFreeUpTempObjects(3);
    const int32 tempAfter = UsedTemp(), usedAfter = UsedObjects();
    int survivors = 0;
    for (auto* o : mine) {
        if (StillInPool(o)) {
            survivors++;
            CWorld::Remove(o);
            delete o;
        }
    }
    Log("TEMP: temporales antes=%d despues=%d (liberados=%d de 3 pedidos), objetos antes=%d despues=%d, mios que sobreviven=%d", tempBefore, tempAfter, tempBefore - tempAfter,
        usedBefore, usedAfter, survivors);
}

// L19: RemoveEscalatorsForEntity (descarga del edificio) no debe borrar los peldanos en el acto con el arreglo; se borran en el siguiente CEscalators::Update.
void TestEscalators() {
    LoadModel(ModelIndices::MI_ESCALATORSTEP);
    LoadModel(ModelIndices::MI_ESCALATORSTEP8);
    LoadModel(MODEL_CJ_BEAR_BOTTLE);
    if (!CStreaming::IsModelLoaded(ModelIndices::MI_ESCALATORSTEP) || !CStreaming::IsModelLoaded(ModelIndices::MI_ESCALATORSTEP8) || !CStreaming::IsModelLoaded(MODEL_CJ_BEAR_BOTTLE)) {
        Log("ESCAL: modelos de peldanos no cargados (step=%d step8=%d), omitido", (int)CStreaming::IsModelLoaded(ModelIndices::MI_ESCALATORSTEP),
            (int)CStreaming::IsModelLoaded(ModelIndices::MI_ESCALATORSTEP8));
        return;
    }
    const CVector cam  = TheCamera.GetPosition();
    const CVector base = cam + TheCamera.GetForward() * 8.0f;
    auto*         ent  = new CObject(MODEL_CJ_BEAR_BOTTLE, true);
    ent->m_nObjectType = OBJECT_TEMPORARY;
    ent->SetPosn(base);
    CWorld::Add(ent);

    const int32 used0 = UsedObjects();
    CEscalators::AddOne(base, base + CVector{ 2.0f, 0.0f, 0.0f }, base + CVector{ 6.0f, 0.0f, 2.0f }, base + CVector{ 8.0f, 0.0f, 2.0f }, false, ent);
    CEscalators::Update();
    const int32 usedCreated = UsedObjects();
    ent->RemoveEscalatorsForEntity();
    const int32 usedImmediate = UsedObjects();
    CEscalators::Update();
    const int32 usedAfterUpdate = UsedObjects();
    Log("ESCAL: objetos base=%d con peldanos=%d (peldanos=%d); tras RemoveEscalatorsForEntity=%d (pendientes=%d); tras el siguiente Update=%d (borrados en total=%d)", used0,
        usedCreated, usedCreated - used0, usedImmediate, usedImmediate - used0, usedAfterUpdate, usedCreated - usedAfterUpdate);
    CWorld::Remove(ent);
    delete ent;
}

// L82: el atomo skinned del paracaidas (arma 371) conserva su pipeline de piel (0x116) en vez del de edificios.
RpAtomic* AtomicInfo(RpAtomic* atomic, void*) {
    auto*        geometry = RpAtomicGetGeometry(atomic);
    const auto*  extra    = CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry);
    const uint32 plug     = atomic->pipeline ? atomic->pipeline->pluginId : 0u;
    Log("PARACAIDAS: atomo pipeline.pluginId=0x%X (piel=0x%X, edificios DN=0x53F20098, edificios=0x53F2009C) pipelineID=0x%X nightColors=%d prelit=%d conPiel=%d IsCBPCPipelineAttached=%d", plug,
        (uint32)rwID_SKINPLUGIN, (uint32)GetPipelineID(atomic), (int)(extra && extra->NightColors), (int)(RpGeometryGetPreLightColors(geometry) != nullptr),
        (int)(RpSkinGeometryGetSkin(geometry) != nullptr), (int)CCustomBuildingRenderer::IsCBPCPipelineAttached(atomic));
    return atomic;
}

// Guarda de SkinBuildingPipelineFix: un atomo skinned con colores nocturnos y prelit (rama del pipeline DN de AtomicSetup, 0x5D7F1E) conserva el pipeline de piel con el
// arreglo y pasa a 0x53F20098 sin el. Se prueba sobre un clon del clump del jugador, forzando la condicion (colores nocturnos) y restaurandola despues.
RpAtomic* SkinGuardCB(RpAtomic* atomic, void*) {
    auto* geometry = RpAtomicGetGeometry(atomic);
    auto* extra    = CCustomBuildingDNPipeline::GetExtraVertColourPtr(geometry);
    const bool skinned = atomic->pipeline && atomic->pipeline->pluginId == rwID_SKINPLUGIN;
    const bool prelit  = RpGeometryGetPreLightColors(geometry) != nullptr;
    Log("SKINGUARD: atomo del clon: skinned(pipeline piel)=%d prelit=%d extra=%d nightColors=%d", (int)skinned, (int)prelit, (int)(extra != nullptr), (int)(extra && extra->NightColors));
    if (!skinned || !extra) {
        return atomic;
    }
    // el ped de serie no trae colores prelit ni nocturnos: se fuerza la condicion de la rama DN (punteros ficticios) y se restaura despues
    RwRGBA  dummy{};
    RwRGBA* savedNight  = extra->NightColors;
    RwRGBA* savedPrelit = geometry->preLitLum;
    extra->NightColors  = &dummy;
    geometry->preLitLum = &dummy;
    CCustomBuildingRenderer::AtomicSetup(atomic);
    extra->NightColors  = savedNight;
    geometry->preLitLum = savedPrelit;
    Log("SKINGUARD: tras AtomicSetup (rama DN forzada) pipeline.pluginId=0x%X: %s", atomic->pipeline ? atomic->pipeline->pluginId : 0u,
        (atomic->pipeline && atomic->pipeline->pluginId == rwID_SKINPLUGIN) ? "conserva el pipeline de piel" : "pasa al pipeline de edificios");
    return atomic;
}

void TestSkinGuard(CPed* player) {
    auto* clump = player->GetRpClump();
    if (!clump) {
        return;
    }
    auto* clone = RpClumpClone(clump);
    if (!clone) {
        Log("SKINGUARD: no se pudo clonar el clump del jugador");
        return;
    }
    RpClumpForAllAtomics(clone, SkinGuardCB, nullptr);
    RpClumpDestroy(clone);
}

void TestParachute() {
    LoadModel(MODEL_GUN_PARA);
    if (!CStreaming::IsModelLoaded(MODEL_GUN_PARA)) {
        Log("PARACAIDAS: modelo no cargado, omitido");
        return;
    }
    auto* mi    = CModelInfo::GetModelInfo(MODEL_GUN_PARA);
    auto* clump = reinterpret_cast<RpClump*>(mi->GetRwObject());
    if (!clump) {
        Log("PARACAIDAS: sin clump");
        return;
    }
    RpClumpForAllAtomics(clump, AtomicInfo, nullptr);
}
} // namespace

void OnFrame() {
    if (!Enabled()) {
        return;
    }
    static int    stage      = 0;
    static uint32 startFrame = 0;
    auto* player = FindPlayerPed();
    if (!player || !player->GetRwObject() || CTimer::GetFrameCounter() < 200) {
        return;
    }
    if (stage == 0) {
        startFrame = CTimer::GetFrameCounter();
        stage      = 1;
        return;
    }
    if (CTimer::GetFrameCounter() - startFrame < 600) {
        return;
    }
    if (stage == 1) {
        stage = 2;
        Log("=== SILENTPATCH PEND7A SELFTEST inicio");
        Opciones();
        TestTempObjects();
        TestEscalators();
        TestParachute();
        TestSkinGuard(player);
        Log("=== SILENTPATCH PEND7A SELFTEST FIN");
    }
}
} // namespace SilentPatchPend7aSelfTest
