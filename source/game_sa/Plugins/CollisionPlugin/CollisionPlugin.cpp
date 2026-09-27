/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCollisionPlugin
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CollisionPlugin.h"

auto& gCollisionPluginOffset = StaticRef<RwInt32>(0x9689DC);

/**
 * Registra la integración de datos COL embebidos en RpClump.
 */
void CCollisionPlugin::InjectHooks() {
    RH_ScopedClass(CCollisionPlugin);
    RH_ScopedCategory("Plugins");

    RH_ScopedInstall(PluginAttach, 0x41B310);
    RH_ScopedInstall(SetModelInfo, 0x41B350);
}

// internal
// 0x41B1A0
static void* ClumpCollisionConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

// internal
// 0x41B1C0
static void* ClumpCollisionDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return object;
}

// internal
// 0x41B1B0
static void* ClumpCollisionCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return dstObject;
}

// internal
// 0x41B1D0
/**
 * Callback de lectura del chunk de colisión de un clump.
 *
 * IMPORTANTE: la implementación efectiva continúa delegada al EXE mediante
 * plugin::CallAndReturn. El código situado después del return conserva el trabajo
 * incompleto de reversing y no se ejecuta en esta ruta.
 */
static RwStream* ClumpCollisionStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return plugin::CallAndReturn<RwStream*, 0x41B1D0, RwStream*, RwInt32, void*, RwInt32, RwInt32>(stream, binaryLength, object, offsetInObject, sizeInObject);

    // incomplete
    CMemoryMgr::LockScratchPad();
    RwStreamRead(stream, &PC_Scratch, binaryLength);
    CColModel* model = new CColModel();

    switch (*(uint32*)PC_Scratch) {
    case MakeFourCC("COLL"):
        CFileLoader::LoadCollisionModel((uint8*)(&PC_Scratch[32]), *model);
        break;
    case MakeFourCC("COL2"):
        CFileLoader::LoadCollisionModelVer2((uint8*)(&PC_Scratch[32]), (PC_Scratch[4] - 24), *model, nullptr);
        break;
    case MakeFourCC("COL3"):
        CFileLoader::LoadCollisionModelVer3((uint8*)(&PC_Scratch[32]), (PC_Scratch[4] - 24), *model, nullptr);
        break;
    }

    model->MakeMultipleAlloc();
    CCollisionPlugin::ms_currentModel->SetColModel(model, true);
    CCollisionPlugin::ms_currentModel->bDontWriteZBuffer = true;
    CMemoryMgr::ReleaseScratchPad();
    return stream;
}

// internal
// 0x41B2F0
static RwStream* ClumpCollisionStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return stream;
}

// internal
// 0x41B300
static RwInt32 ClumpCollisionGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return -1;
}

// 0x41B310
// Registra el plugin sin payload propio y conecta los callbacks de stream COL.
bool CCollisionPlugin::PluginAttach() {
    // 0x9689DC unused
    gCollisionPluginOffset = RpClumpRegisterPlugin(
        0,
        rwID_COLLISIONPLUGIN,
        ClumpCollisionConstructor,
        ClumpCollisionDestructor,
        ClumpCollisionCopyConstructor
    );

    RpClumpRegisterPluginStream(
        rwID_COLLISIONPLUGIN,
        ClumpCollisionStreamRead,
        ClumpCollisionStreamWrite,
        ClumpCollisionGetSize
    );

    return TRUE;
}

// 0x41B350
// Establece el CClumpModelInfo que recibirá el CColModel durante la lectura del stream.
void CCollisionPlugin::SetModelInfo(CClumpModelInfo* modelInfo) {
    ms_currentModel = modelInfo;
}
