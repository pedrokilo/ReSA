/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwPlCore
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rwplcore.h"

namespace {
// 0x8019B0: RwFreeListCreateAndPreallocateSpace real (el simbolo publico 0x801B70 es un JMP a esta direccion); sin reconstruir.
RwFreeList* FreeListCreateAndPreallocateSpaceExe(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return ((RwFreeList*(__cdecl *)(RwInt32, RwInt32, RwInt32, RwInt32, RwFreeList*, RwUInt32))0x8019B0)(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}
} // namespace

// Biblioteca RenderWare (reconstruidas en C++, banco Dependencies25D)
void RwPlCoreInjectHooks25d() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    RH_ScopedGlobalInstall(RwFreeListCreate, 0x801980);
    RH_ScopedGlobalInstall(RwFreeListCreateAndPreallocateSpace, 0x801B70);
}

void RwPlCoreInjectHooks() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwPlCore");

    // Funciones de biblioteca (RenderWare) de <= 40 bytes reconstruidas en C++ (lote 25, grupo g25e).
    RH_ScopedGlobalInstall(_rwPluginRegistryGetPluginOffset, 0x808470);
    RH_ScopedGlobalInstall(_rwSListDestroyEndEntries, 0x809400);
    RH_ScopedGlobalInstall(_rwSListGetNumEntries, 0x8094B0);
    RH_ScopedGlobalInstall(_rwSListGetEntry, 0x809510);
    RH_ScopedGlobalInstall(_rwSListGetBegin, 0x809530);
    RH_ScopedGlobalInstall(_rwSListGetEnd, 0x809540);
    RH_ScopedGlobalInstall(_rwStringClose, 0x80A440);
    RH_ScopedGlobalInstall(_rwStringStreamGetSize, 0x80A470);
}

RwMemoryFunctions* RwOsGetMemoryInterface() {
    return ((RwMemoryFunctions*(__cdecl *)(void))0x802230)();
}

// 0x801980: RwFreeListCreate de la compilacion sin RWDEBUG (4 argumentos de pila; la version con fileCreate/lineCreate es solo de depuracion):
// reserva con un bloque preasignado y sin espacio propio para la estructura.
RwFreeList* RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceExe(entrySize, entriesPerBlock, alignment, 1, nullptr, hint);
}

// 0x801B70: salto directo a 0x8019B0 con los mismos seis argumentos.
RwFreeList* RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceExe(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}

RwBool RwFreeListDestroy(RwFreeList* freelist) {
    return ((RwBool(__cdecl *)(RwFreeList*))0x801B80)(freelist);
}

void RwFreeListSetFlags(RwFreeList* freeList, RwUInt32 flags) {
    ((void(__cdecl *)(RwFreeList*, RwUInt32))0x801C10)(freeList, flags);
}

RwUInt32 RwFreeListGetFlags(RwFreeList* freeList) {
    return ((RwUInt32(__cdecl *)(RwFreeList*))0x801C20)(freeList);
}

RwInt32 RwFreeListPurge(RwFreeList* freelist) {
    return ((RwInt32(__cdecl *)(RwFreeList*))0x801E00)(freelist);
}

RwFreeList* RwFreeListForAllUsed(RwFreeList* freelist, RwFreeListCallBack fpCallBack, void* data) {
    return ((RwFreeList*(__cdecl *)(RwFreeList*, RwFreeListCallBack, void*))0x801E90)(freelist, fpCallBack, data);
}

RwInt32 RwFreeListPurgeAllFreeLists() {
    return ((RwInt32(__cdecl *)(void))0x801F90)();
}

void RwStreamSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EC760)(blockSize, numBlocksToPrealloc);
}

RwStream* _rwStreamInitialize(RwStream* stream, RwBool rwOwned, RwStreamType type, RwStreamAccessType accessType, const void* data) {
    return ((RwStream*(__cdecl *)(RwStream*, RwBool, RwStreamType, RwStreamAccessType, const void*))0x7EC810)(stream, rwOwned, type, accessType, data);
}

RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data) {
    return ((RwStream*(__cdecl *)(RwStreamType, RwStreamAccessType, const void*))0x7ECEF0)(type, accessType, data);
}

RwBool RwStreamClose(RwStream* stream, void* data) {
    return ((RwBool(__cdecl *)(RwStream*, void*))0x7ECE20)(stream, data);
}

RwUInt32 RwStreamRead(RwStream* stream, void* buffer, RwUInt32 length) {
    return ((RwUInt32(__cdecl *)(RwStream*, void*, RwUInt32))0x7EC9D0)(stream, buffer, length);
}

RwStream* RwStreamWrite(RwStream* stream, const void* buffer, RwUInt32 length) {
    return ((RwStream*(__cdecl *)(RwStream*, const void*, RwUInt32))0x7ECB30)(stream, buffer, length);
}

RwStream* RwStreamSkip(RwStream* stream, RwUInt32 offset) {
    return ((RwStream*(__cdecl *)(RwStream*, RwUInt32))0x7ECD00)(stream, offset);
}

RwBool _rwStringOpen() {
    return ((RwBool(__cdecl *)(void))0x80A240)();
}

// 0x80A440: el EXE solo ejecuta `ret` (1 byte, seguido de 15 bytes de relleno 0x90 hasta 0x80A450).
void _rwStringClose() {
}

RwBool _rwStringDestroy(RwChar* string) {
    return ((RwBool(__cdecl *)(RwChar*))0x80A450)(string);
}

// 0x80A470: una cadena nula se sustituye por la cadena vacía del EXE (0x884F6A); la longitud sale de rwstrlen del motor
// ([RwEngineInstance + 0x120], cdecl) y el tamaño en el flujo es (longitud + 4) con los dos bits bajos a cero (`add eax, 4` / `and al, 0FCh`).
RwUInt32 _rwStringStreamGetSize(const RwChar* string) {
    if (!string) {
        string = reinterpret_cast<const RwChar*>(0x884F6A);
    }
    const uint32 length = static_cast<uint32>(RWSRCGLOBAL(stringFuncs).vecStrlen(string));
    return (length + 4u) & ~3u;
}

const RwChar* _rwStringStreamWrite(const RwChar* string, RwStream* stream) {
    return ((const RwChar*(__cdecl *)(const RwChar*, RwStream*))0x80A4A0)(string, stream);
}

RwChar* _rwStringStreamFindAndRead(RwChar* string, RwStream* stream) {
    return ((RwChar*(__cdecl *)(RwChar*, RwStream*))0x80A510)(string, stream);
}

void RwPluginRegistrySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x8087B0)(blockSize, numBlocksToPrealloc);
}

RwBool _rwPluginRegistrySetStaticPluginsSize(RwPluginRegistry* reg, RwInt32 size) {
    return ((RwBool(__cdecl *)(RwPluginRegistry*, RwInt32))0x808430)(reg, size);
}

RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry* reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x8084A0)(reg, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x808470: recorre la lista de plugins del registro (firstRegEntry +0x10, nextRegEntry +0x30) y devuelve el desplazamiento (+0) del primero con ese
// pluginID (+8), o -1 si no está.
RwInt32 _rwPluginRegistryGetPluginOffset(const RwPluginRegistry* reg, RwUInt32 pluginID) {
    for (const RwPluginRegEntry* entry = reg->firstRegEntry; entry; entry = entry->nextRegEntry) {
        if (entry->pluginID == pluginID) {
            return entry->offset;
        }
    }
    return -1;
}

const RwPluginRegistry* _rwPluginRegistryInitObject(const RwPluginRegistry* reg, void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, void*))0x8086E0)(reg, object);
}

const RwPluginRegistry* _rwPluginRegistryDeInitObject(const RwPluginRegistry* reg, void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, void*))0x808740)(reg, object);
}

const RwPluginRegistry* _rwPluginRegistryCopyObject(const RwPluginRegistry* reg, void* dstObject, const void* srcObject) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, void*, const void*))0x808770)(reg, dstObject, srcObject);
}

RwInt32 _rwPluginRegistryAddPluginStream(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x8088E0)(reg, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 _rwPluginRegistryAddPlgnStrmlwysCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkAlwaysCallBack))0x808920)(reg, pluginID, alwaysCB);
}

RwInt32 _rwPluginRegistryAddPlgnStrmRightsCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkRightsCallBack))0x808950)(reg, pluginID, rightsCB);
}

const RwPluginRegistry* _rwPluginRegistryReadDataChunks(const RwPluginRegistry* reg, RwStream* stream, void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwStream*, void*))0x808980)(reg, stream, object);
}

const RwPluginRegistry* _rwPluginRegistryWriteDataChunks(const RwPluginRegistry* reg, RwStream* stream, const void* object) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwStream*, const void*))0x808B40)(reg, stream, object);
}

const RwPluginRegistry* _rwPluginRegistrySkipDataChunks(const RwPluginRegistry* reg, RwStream* stream) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwStream*))0x808C10)(reg, stream);
}

RwInt32 _rwPluginRegistryGetSize(const RwPluginRegistry* reg, const void* object) {
    return ((RwInt32(__cdecl *)(const RwPluginRegistry*, const void*))0x808B00)(reg, object);
}

const RwPluginRegistry* _rwPluginRegistryInvokeRights(const RwPluginRegistry* reg, RwUInt32 id, void* obj, RwUInt32 extraData) {
    return ((const RwPluginRegistry*(__cdecl *)(const RwPluginRegistry*, RwUInt32, void*, RwUInt32))0x808AB0)(reg, id, obj, extraData);
}

RwBool RwEngineGetMatrixTolerances(RwMatrixTolerance * const tolerance) {
    return ((RwBool(__cdecl *)(RwMatrixTolerance * const))0x7F1780)(tolerance);
}

RwBool RwEngineSetMatrixTolerances(const RwMatrixTolerance * const tolerance) {
    return ((RwBool(__cdecl *)(const RwMatrixTolerance*))0x7F17B0)(tolerance);
}

void RwMatrixSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7F16A0)(blockSize, numBlocksToPrealloc);
}

RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    return ((RwBool(__cdecl *)(RwMatrix*))0x7F2A20)(mpMat);
}

RwMatrix* RwMatrixCreate() {
    return ((RwMatrix*(__cdecl *)(void))0x7F2A50)();
}

RwMatrix* RwMatrixMultiply(RwMatrix* matrixOut, const RwMatrix* MatrixIn1, const RwMatrix* matrixIn2) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrix*, const RwMatrix*))0x7F18B0)(matrixOut, MatrixIn1, matrixIn2);
}

RwMatrix* RwMatrixTransform(RwMatrix* matrix, const RwMatrix* transform, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrix*, RwOpCombineType))0x7F25A0)(matrix, transform, combineOp);
}

// 0x7F1920: `jmp 0x7F1930` (salto de incremental linking a la rutina real, x87, todavia sin reconstruir). Se le pasa (matrixOut, matrixIn) por direccion y
// se devuelve su resultado; la rutina real hace todo el calculo (incluido el estado x87 que deja).
RwMatrix* RwMatrixOrthoNormalize(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    return reinterpret_cast<RwMatrix*(__cdecl*)(RwMatrix*, const RwMatrix*)>(0x7F1930)(matrixOut, matrixIn);
}

RwMatrix* RwMatrixInvert(RwMatrix* matrixOut, const RwMatrix* matrixIn) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrix*))0x7F2070)(matrixOut, matrixIn);
}

RwMatrix* RwMatrixScale(RwMatrix* matrix, const RwV3d* scale, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwOpCombineType))0x7F22C0)(matrix, scale, combineOp);
}

RwMatrix* RwMatrixTranslate(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwOpCombineType))0x7F2450)(matrix, translation, combineOp);
}

RwMatrix* RwMatrixRotate(RwMatrix* matrix, const RwV3d* axis, RwReal angle, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwReal, RwOpCombineType))0x7F1FD0)(matrix, axis, angle, combineOp);
}

RwMatrix* RwMatrixRotateOneMinusCosineSine(RwMatrix* matrix, const RwV3d* unitAxis, RwReal oneMinusCosine, RwReal sine, RwOpCombineType combineOp) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwV3d*, RwReal, RwReal, RwOpCombineType))0x7F1D00)(matrix, unitAxis, oneMinusCosine, sine, combineOp);
}

const RwMatrix* RwMatrixQueryRotate(const RwMatrix* matrix, RwV3d* unitAxis, RwReal* angle, RwV3d* center) {
    return ((const RwMatrix*(__cdecl *)(const RwMatrix*, RwV3d*, RwReal*, RwV3d*))0x7F2720)(matrix, unitAxis, angle, center);
}

RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*))0x7F18A0)(matrix);
}

RwMatrix* RwMatrixOptimize(RwMatrix* matrix, const RwMatrixTolerance* tolerance) {
    return ((RwMatrix*(__cdecl *)(RwMatrix*, const RwMatrixTolerance*))0x7F17E0)(matrix, tolerance);
}

RwReal _rwMatrixDeterminant(const RwMatrix* matrix) {
    return ((RwReal(__cdecl *)(const RwMatrix*))0x7F1450)(matrix);
}

RwReal _rwMatrixNormalError(const RwMatrix* matrix) {
    return ((RwReal(__cdecl *)(const RwMatrix*))0x7F1500)(matrix);
}

RwReal _rwMatrixOrthogonalError(const RwMatrix* matrix) {
    return ((RwReal(__cdecl *)(const RwMatrix*))0x7F14A0)(matrix);
}

RwReal _rwMatrixIdentityError(const RwMatrix* matrix) {
    return ((RwReal(__cdecl *)(const RwMatrix*))0x7F1590)(matrix);
}

RwReal RwV3dNormalize(RwV3d* out, const RwV3d* in) {
    return ((RwReal(__cdecl *)(RwV3d*, const RwV3d*))0x7ED9B0)(out, in);
}

RwReal RwV3dLength(const RwV3d* in) {
    return ((RwReal(__cdecl *)(const RwV3d*))0x7EDAC0)(in);
}

RwReal RwV2dLength(const RwV2d* in) {
    return ((RwReal(__cdecl *)(const RwV2d*))0x7EDBF0)(in);
}

RwReal RwV2dNormalize(RwV2d* out, const RwV2d* in) {
    return ((RwReal(__cdecl *)(RwV2d*, const RwV2d*))0x7EDC60)(out, in);
}

RwV3d* RwV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix) {
    return ((RwV3d*(__cdecl *)(RwV3d*, const RwV3d*, const RwMatrix*))0x7EDD60)(pointOut, pointIn, matrix);
}

RwV3d* RwV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix) {
    return ((RwV3d*(__cdecl *)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x7EDD90)(pointsOut, pointsIn, numPoints, matrix);
}

RwV3d* RwV3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix) {
    return ((RwV3d*(__cdecl *)(RwV3d*, const RwV3d*, const RwMatrix*))0x7EDDC0)(vectorOut, vectorIn, matrix);
}

RwV3d* RwV3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numPoints, const RwMatrix* matrix) {
    return ((RwV3d*(__cdecl *)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x7EDDF0)(vectorsOut, vectorsIn, numPoints, matrix);
}

RwReal _rwSqrt(const RwReal num) {
    return ((RwReal(__cdecl *)(const RwReal))0x7EDB30)(num);
}

RwReal _rwInvSqrt(const RwReal num) {
    return ((RwReal(__cdecl *)(const RwReal))0x7EDB90)(num);
}

RwReal _rwV3dNormalize(RwV3d* out, const RwV3d* in) {
    return ((RwReal(__cdecl *)(RwV3d*, const RwV3d*))0x7ED910)(out, in);
}

RwSList* _rwSListCreate(RwInt32 size, RwUInt32 hint) {
    return ((RwSList*(__cdecl *)(RwInt32, RwUInt32))0x809160)(size, hint);
}

RwBool _rwSListDestroy(RwSList* sList) {
    return ((RwBool(__cdecl *)(RwSList*))0x809440)(sList);
}

// 0x809400: resta `amount` a numElementsFilled (+4), con envoltura de 32 bits.
void _rwSListDestroyEndEntries(RwSList* sList, RwInt32 amount) {
    sList->numElementsFilled = static_cast<RwInt32>(static_cast<uint32>(sList->numElementsFilled) - static_cast<uint32>(amount));
}

// 0x809510: listElements + entrySize * entry (imul de 32 bits con envoltura).
void* _rwSListGetEntry(RwSList* sList, RwInt32 entry) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(entry));
}

void* _rwSListGetNewEntry(RwSList* sList, RwUInt32 hint) {
    return ((void*(__cdecl *)(RwSList*, RwUInt32))0x809240)(sList, hint);
}

// 0x8094B0: numElementsFilled (+4).
RwInt32 _rwSListGetNumEntries(const RwSList* sList) {
    return sList->numElementsFilled;
}

// 0x809530: listElements (+0).
void* _rwSListGetBegin(RwSList* sList) {
    return sList->listElements;
}

// 0x809540: listElements + entrySize * numElementsFilled (imul de 32 bits con envoltura).
void* _rwSListGetEnd(RwSList* sList) {
    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(sList->listElements) + static_cast<uint32>(sList->entrySize) * static_cast<uint32>(sList->numElementsFilled));
}

RwUInt32 RwEngineGetVersion() {
    return ((RwUInt32(__cdecl *)(void))0x7F2BA0)();
}

RwBool RwEngineInit(const RwMemoryFunctions* memFuncs, RwUInt32 initFlags, RwUInt32 resArenaSize) {
    return ((RwBool(__cdecl *)(const RwMemoryFunctions*, RwUInt32, RwUInt32))0x7F3170)(memFuncs, initFlags, resArenaSize);
}

// Direcciones del EXE usadas por las funciones del lote 25 (g25c) de este archivo.
namespace {
constexpr uintptr_t kEnginePluginRegistry = 0x8E2298; // RwEngine: lista de plugins de RwGlobals
}

void RwPlCoreEngineStreamInjectHooks() {
    RH_ScopedNamespaceName("RwPlCore");
    RH_ScopedCategory("Renderware");

    RH_ScopedGlobalInstall(RwStreamWriteInt32, 0x7ED460);
    RH_ScopedGlobalInstall(RwMatrixOrthoNormalize, 0x7F1920);
    RH_ScopedGlobalInstall(RwEngineRegisterPlugin, 0x7F2BB0);
    RH_ScopedGlobalInstall(RwEngineGetPluginOffset, 0x7F2BE0);
}

// 0x7F2BB0: registra el plugin en la lista del motor (0x8E2298) con _rwPluginRegistryAddPlugin (0x8084A0); el callback de copia es nulo.
RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor initCB, RwPluginObjectDestructor termCB) {
    return _rwPluginRegistryAddPlugin(reinterpret_cast<RwPluginRegistry*>(kEnginePluginRegistry), size, pluginID, initCB, termCB, nullptr);
}

// 0x7F2BE0: desplazamiento del plugin en la lista del motor (0x8E2298) con _rwPluginRegistryGetPluginOffset (0x808470).
RwInt32 RwEngineGetPluginOffset(RwUInt32 pluginID) {
    return _rwPluginRegistryGetPluginOffset(reinterpret_cast<const RwPluginRegistry*>(kEnginePluginRegistry), pluginID);
}

RwBool RwEngineOpen(RwEngineOpenParams* initParams) {
    return ((RwBool(__cdecl *)(RwEngineOpenParams*))0x7F2F70)(initParams);
}

RwBool RwEngineStart() {
    return ((RwBool(__cdecl *)(void))0x7F2E70)();
}

RwBool RwEngineStop() {
    return ((RwBool(__cdecl *)(void))0x7F2E20)();
}

RwBool RwEngineClose() {
    return ((RwBool(__cdecl *)(void))0x7F2F00)();
}

RwBool RwEngineTerm() {
    return ((RwBool(__cdecl *)(void))0x7F3130)();
}

RwInt32 RwEngineGetNumSubSystems() {
    return ((RwInt32(__cdecl *)(void))0x7F2C00)();
}

RwSubSystemInfo* RwEngineGetSubSystemInfo(RwSubSystemInfo* subSystemInfo, RwInt32 subSystemIndex) {
    return ((RwSubSystemInfo*(__cdecl *)(RwSubSystemInfo*, RwInt32))0x7F2C30)(subSystemInfo, subSystemIndex);
}

RwInt32 RwEngineGetCurrentSubSystem() {
    return ((RwInt32(__cdecl *)(void))0x7F2C60)();
}

RwBool RwEngineSetSubSystem(RwInt32 subSystemIndex) {
    return ((RwBool(__cdecl *)(RwInt32))0x7F2C90)(subSystemIndex);
}

RwInt32 RwEngineGetNumVideoModes() {
    return ((RwInt32(__cdecl *)(void))0x7F2CC0)();
}

RwVideoMode* RwEngineGetVideoModeInfo(RwVideoMode* vmi, RwInt32 modeIndex) {
    return plugin::CallAndReturn<RwVideoMode*, 0x7F2CF0>(vmi, modeIndex);
}

RwInt32 RwEngineGetCurrentVideoMode() {
    return ((RwInt32(__cdecl *)(void))0x7F2D20)();
}

RwBool RwEngineSetVideoMode(RwInt32 modeIndex) {
    return ((RwBool(__cdecl *)(RwInt32))0x7F2D50)(modeIndex);
}

RwInt32 RwEngineGetTextureMemorySize() {
    return ((RwInt32(__cdecl *)(void))0x7F2D80)();
}

RwInt32 RwEngineGetMaxTextureSize() {
    return ((RwInt32(__cdecl *)(void))0x7F2DB0)();
}

RwBool RwEngineSetFocus(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F2DE0)(enable);
}

RwMetrics* RwEngineGetMetrics() {
    return ((RwMetrics*(__cdecl *)(void))0x7F2E10)();
}

//RwFileFunctions* RwOsGetFileInterface() {
//    return ((RwFileFunctions*(__cdecl *)(void))0x804130)();
//}

RwError* RwErrorGet(RwError* code) {
    return ((RwError*(__cdecl *)(RwError*))0x808880)(code);
}

RwError* RwErrorSet(RwError* code) {
    return ((RwError*(__cdecl *)(RwError*))0x808820)(code);
}

RwInt32 _rwerror(RwInt32 code, ...) {
    return code;
}

RwBool RwResourcesSetArenaSize(RwUInt32 size) {
    return ((RwBool(__cdecl *)(RwUInt32))0x8080C0)(size);
}

RwInt32 RwResourcesGetArenaSize() {
    return ((RwInt32(__cdecl *)(void))0x8081B0)();
}

RwInt32 RwResourcesGetArenaUsage() {
    return ((RwInt32(__cdecl *)(void))0x8081C0)();
}

RwBool RwResourcesEmptyArena() {
    return ((RwBool(__cdecl *)(void))0x8081F0)();
}

RwResEntry* RwResourcesAllocateResEntry(void* owner, RwResEntry* ownerRef, RwInt32 size, RwResEntryDestroyNotify destroyNotify) {
    return ((RwResEntry*(__cdecl *)(void*, RwResEntry*, RwInt32, RwResEntryDestroyNotify))0x807ED0)(owner, ownerRef, size, destroyNotify);
}

RwBool RwResourcesFreeResEntry(RwResEntry* entry) {
    return ((RwBool(__cdecl *)(RwResEntry*))0x807DE0)(entry);
}

void _rwResourcesPurge() {
    ((void(__cdecl *)(void))0x807E50)();
}

RwBool RwStreamFindChunk(RwStream* stream, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut) {
    return ((RwBool(__cdecl *)(RwStream*, RwUInt32, RwUInt32*, RwUInt32*))0x7ED2D0)(stream, type, lengthOut, versionOut);
}

RwStream* _rwStreamWriteVersionedChunkHeader(RwStream* stream, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 buildNum) {
    return ((RwStream*(__cdecl *)(RwStream*, RwInt32, RwInt32, RwUInt32, RwUInt32))0x7ED270)(stream, type, size, version, buildNum);
}

RwStream* RwStreamWriteReal(RwStream* stream, const RwReal* reals, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, const RwReal*, RwUInt32))0x7ED3D0)(stream, reals, numBytes);
}

// 0x7ED460: escribe los bytes tal cual con RwStreamWrite (0x7ECB30; sin conversion de orden de bytes en x86), ignora su resultado y devuelve el flujo.
RwStream* RwStreamWriteInt32(RwStream* stream, const RwInt32* ints, RwUInt32 numBytes) {
    RwStreamWrite(stream, ints, numBytes);
    return stream;
}

RwStream* RwStreamWriteInt16(RwStream* stream, const RwInt16* ints, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, const RwInt16*, RwUInt32))0x7ED480)(stream, ints, numBytes);
}

RwStream* RwStreamReadReal(RwStream* stream, RwReal* reals, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, RwReal*, RwUInt32))0x7ED4F0)(stream, reals, numBytes);
}

RwStream* RwStreamReadInt32(RwStream* stream, RwInt32* ints, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, RwInt32*, RwUInt32))0x7ED540)(stream, ints, numBytes);
}

RwStream* RwStreamReadInt16(RwStream* stream, RwInt16* ints, RwUInt32 numBytes) {
    return ((RwStream*(__cdecl *)(RwStream*, RwInt16*, RwUInt32))0x7ED4A0)(stream, ints, numBytes);
}

RwStream* RwStreamReadChunkHeaderInfo(RwStream* stream, RwChunkHeaderInfo* chunkHeaderInfo) {
    return ((RwStream*(__cdecl *)(RwStream*, RwChunkHeaderInfo*))0x7ED590)(stream, chunkHeaderInfo);
}
