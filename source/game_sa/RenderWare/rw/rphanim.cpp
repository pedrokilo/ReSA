/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpHAnim
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rphanim.h"

void RpHAnimInjectHooks() {
    RH_ScopedNamespaceName("RpHAnim");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RpHAnimFrameGetHierarchy, 0x7C5160);
}

void RpHAnimHierarchySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7C45E0)(blockSize, numBlocksToPrealloc);
}

RpHAnimHierarchy* RpHAnimHierarchyCreate(RwInt32 numNodes, RwUInt32* nodeFlags, RwInt32* nodeIDs, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) {
    return ((RpHAnimHierarchy*(__cdecl *)(RwInt32, RwUInt32*, RwInt32*, RpHAnimHierarchyFlag, RwInt32))0x7C4C30)(numNodes, nodeFlags, nodeIDs, flags, maxInterpKeyFrameSize);
}

RpHAnimHierarchy* RpHAnimHierarchyCreateFromHierarchy(RpHAnimHierarchy* hierarchy, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*, RpHAnimHierarchyFlag, RwInt32))0x7C4ED0)(hierarchy, flags, maxInterpKeyFrameSize);
}

RpHAnimHierarchy* RpHAnimHierarchyDestroy(RpHAnimHierarchy* hierarchy) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*))0x7C4D30)(hierarchy);
}

RpHAnimHierarchy* RpHAnimHierarchyCreateSubHierarchy(RpHAnimHierarchy* parentHierarchy, RwInt32 startNode, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*, RwInt32, RpHAnimHierarchyFlag, RwInt32))0x7C4DB0)(parentHierarchy, startNode, flags, maxInterpKeyFrameSize);
}

RpHAnimHierarchy* RpHAnimHierarchyAttach(RpHAnimHierarchy* hierarchy) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*))0x7C4F40)(hierarchy);
}

RpHAnimHierarchy* RpHAnimHierarchyDetach(RpHAnimHierarchy* hierarchy) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*))0x7C4FF0)(hierarchy);
}

RpHAnimHierarchy* RpHAnimHierarchyAttachFrameIndex(RpHAnimHierarchy* hierarchy, RwInt32 nodeIndex) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*, RwInt32))0x7C5020)(hierarchy, nodeIndex);
}

RpHAnimHierarchy* RpHAnimHierarchyDetachFrameIndex(RpHAnimHierarchy* hierarchy, RwInt32 nodeIndex) {
    return ((RpHAnimHierarchy*(__cdecl *)(RpHAnimHierarchy*, RwInt32))0x7C5100)(hierarchy, nodeIndex);
}

RwBool RpHAnimFrameSetHierarchy(RwFrame* frame, RpHAnimHierarchy* hierarchy) {
    return ((RwBool(__cdecl *)(RwFrame*, RpHAnimHierarchy*))0x7C5130)(frame, hierarchy);
}

// 0x7C5160: lee el desplazamiento del dato de plugin HAnim de los marcos (int32 global del EXE, 0xC9B8C8) y devuelve el puntero guardado en
// +4 de ese dato: `*(RpHAnimHierarchy**)(frame + offset + 4)`. Aritmetica de 32 bits como el EXE (sin comprobar nulos ni rangos).
RpHAnimHierarchy* RpHAnimFrameGetHierarchy(RwFrame* frame) {
    const uint32 frameDataOffset = StaticRef<uint32>(0xC9B8C8);
    return *reinterpret_cast<RpHAnimHierarchy**>(reinterpret_cast<uintptr_t>(frame) + frameDataOffset + 4);
}

RwMatrix* RpHAnimHierarchyGetMatrixArray(RpHAnimHierarchy* hierarchy) {
    return ((RwMatrix*(__cdecl *)(RpHAnimHierarchy*))0x7C5120)(hierarchy);
}

// NOTSA
RwMatrix* RpHAnimHierarchyGetNodeMatrix(RpHAnimHierarchy* hierarchy, RwInt32 nodeID) {
    const auto i = RpHAnimIDGetIndex(hierarchy, nodeID);
    assert(i >= 0 && i < hierarchy->numNodes);
    return &RpHAnimHierarchyGetMatrixArray(hierarchy)[i];
}

RwBool RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy* hierarchy) {
    return ((RwBool(__cdecl *)(RpHAnimHierarchy*))0x7C51D0)(hierarchy);
}

RwInt32 RpHAnimIDGetIndex(RpHAnimHierarchy* hierarchy, RwInt32 ID) {
    return ((RwInt32(__cdecl *)(RpHAnimHierarchy*, RwInt32))0x7C51A0)(hierarchy, ID);
}

RwBool RpHAnimPluginAttach() {
    return ((RwBool(__cdecl *)(void))0x7C4600)();
}

void RpHAnimKeyFrameApply(void* matrix, void* voidIFrame) {
    ((void(__cdecl *)(void*, void*))0x7C5B80)(matrix, voidIFrame);
}

void RpHAnimKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) {
    ((void(__cdecl *)(void*, void*, void*, RwReal))0x7C60C0)(voidOut, voidIn1, voidIn2, alpha);
}

void RpHAnimKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) {
    ((void(__cdecl *)(void*, void*, void*, RwReal, void*))0x7C5CA0)(voidOut, voidIn1, voidIn2, time, customData);
}

void RpHAnimKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) {
    ((void(__cdecl *)(void*, void*, void*))0x7C6720)(voidOut, voidIn1, voidIn2);
}

void RpHAnimKeyFrameMulRecip(void* voidFrame, void* voidStart) {
    ((void(__cdecl *)(void*, void*))0x7C65C0)(voidFrame, voidStart);
}

RtAnimAnimation* RpHAnimKeyFrameStreamRead(RwStream* stream, RtAnimAnimation* animation) {
    return ((RtAnimAnimation*(__cdecl *)(RwStream*, RtAnimAnimation*))0x7C64C0)(stream, animation);
}

RwBool RpHAnimKeyFrameStreamWrite(const RtAnimAnimation* animation, RwStream* stream) {
    return ((RwBool(__cdecl *)(const RtAnimAnimation*, RwStream*))0x7C6540)(animation, stream);
}

RwInt32 RpHAnimKeyFrameStreamGetSize(const RtAnimAnimation* animation) {
    return ((RwInt32(__cdecl *)(const RtAnimAnimation*))0x7C65B0)(animation);
}

RwBool RpHAnimFrameSetID(RwFrame* frame, RwInt32 id) {
    return ((RwBool(__cdecl *)(RwFrame*, RwInt32))0x7C5170)(frame, id);
}

RwInt32 RpHAnimFrameGetID(RwFrame* frame) {
    return ((RwInt32(__cdecl *)(RwFrame*))0x7C5190)(frame);
}
