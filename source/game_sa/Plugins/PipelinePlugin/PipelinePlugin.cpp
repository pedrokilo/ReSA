/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: PipelinePlugin
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PipelinePlugin.h"

#include <rpworld.h>

auto& gPipelinePluginOffset = StaticRef<RwInt32>(0x8D6080);


/**
 * Registra el plugin que persiste un pipelineId adicional en cada RpAtomic.
 */
void PipelinePlugin::InjectHooks() {
    RH_ScopedNamespace(PipelinePlugin);
    RH_ScopedCategory("Plugins");

    // TODO: Move this into the namespace PipelinePlugin
    RH_ScopedGlobalInstall(PipelinePluginAttach, 0x72FBD0);
    RH_ScopedGlobalInstall(GetPipelineID, 0x72FC40);
    RH_ScopedGlobalInstall(SetPipelineID, 0x72FC50);
}

// internal
typedef struct tPipelinePlugin {
    RwUInt32 pipelineId;
} PipelinePluginInstance;

VALIDATE_SIZE(tPipelinePlugin, 0x4);

// internal
// 0x72FB50
static void* PipelineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (gPipelinePluginOffset > 0) {
        RWPLUGINOFFSET(tPipelinePlugin, object, offsetInObject)->pipelineId = 0;
    }
    return object;
}

// internal
// 0x72FB70
static void* PipelineCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RWPLUGINOFFSET(tPipelinePlugin, dstObject, offsetInObject)->pipelineId = RWPLUGINOFFSET(tPipelinePlugin, srcObject, offsetInObject)->pipelineId;
    return dstObject;
}

// internal
// 0x72FB90
static RwStream* PipelineStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwStreamRead(stream, RWPLUGINOFFSET(tPipelinePlugin, object, gPipelinePluginOffset), binaryLength);
    return stream;
}

// internal
// 0x72FBC0
static RwInt32 PipelineGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return sizeof(tPipelinePlugin);
}

// internal
// RenderWare exige un callback de escritura aunque este plugin no emita payload por esa ruta.
static RwStream* PipelineStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return stream;
}

// 0x72FBD0
// Registra el campo pipelineId y sus callbacks de copia/lectura en RpAtomic.
RwBool PipelinePluginAttach() {
    gPipelinePluginOffset = RpAtomicRegisterPlugin(sizeof(tPipelinePlugin), rwID_PIPELINEPLUGIN, PipelineConstructor, nullptr, PipelineCopy);
    if (gPipelinePluginOffset == -1) {
        return FALSE;
    }
    if (RpAtomicRegisterPluginStream(rwID_PIPELINEPLUGIN, PipelineStreamRead, PipelineStreamWrite, PipelineGetSize) == -1) {
        gPipelinePluginOffset = -1;
        return FALSE;
    }
    return TRUE;
}

// 0x72FC40
// Lee directamente el ID desde el payload anexado al atomic.
RwUInt32 GetPipelineID(RpAtomic* atomic) {
    return RWPLUGINOFFSET(tPipelinePlugin, atomic, gPipelinePluginOffset)->pipelineId;
}

// 0x72FC50
void SetPipelineID(RpAtomic* atomic, RwUInt32 pipelineId) {
    RWPLUGINOFFSET(tPipelinePlugin, atomic, gPipelinePluginOffset)->pipelineId = pipelineId;
}
