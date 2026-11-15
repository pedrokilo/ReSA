/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpUVAnim
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpuvanim.h"

void RpUVAnimInjectHooks() {
    RH_ScopedNamespaceName("RpUVAnim");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(_rpUVAnimCustomDataStreamGetSize, 0x7CC010);
    RH_ScopedGlobalInstall(RpUVAnimKeyFrameStreamGetSize, 0x7CC9D0);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameDataFromMatrix, 0x7CCC50);
}

static_assert(offsetof(RwMatrix, up) == 0x10 && offsetof(RwMatrix, pos) == 0x30 && sizeof(RpUVAnimLinearKeyFrameData) == 0x18);

// 0x7CCC50 (clase RpUVAnim: la llama 0x7CC810, de este modulo; el listado la daba por RtAnim con confianza baja): copia seis floats de la matriz a los datos lineales
// (uv[0] = right.x +0x00, uv[1] = right.y +0x04, uv[2] = up.x +0x10, uv[3] = up.y +0x14, uv[4] = pos.x +0x30, uv[5] = pos.y +0x34) como dwords (sin pasar por la x87) y devuelve data (EAX = primer argumento).
RpUVAnimLinearKeyFrameData* RpUVAnimLinearKeyFrameDataFromMatrix(RpUVAnimLinearKeyFrameData* data, const RwMatrix* matrix) {
    memcpy(&data->uv[0], &matrix->right.x, sizeof(RwReal));
    memcpy(&data->uv[1], &matrix->right.y, sizeof(RwReal));
    memcpy(&data->uv[2], &matrix->up.x, sizeof(RwReal));
    memcpy(&data->uv[3], &matrix->up.y, sizeof(RwReal));
    memcpy(&data->uv[4], &matrix->pos.x, sizeof(RwReal));
    memcpy(&data->uv[5], &matrix->pos.y, sizeof(RwReal));
    return data;
}

_rpUVAnimCustomData* _rpUVAnimCustomDataStreamRead(RwStream* stream) {
    return ((_rpUVAnimCustomData*(__cdecl *)(RwStream*))0x7CBF70)(stream);
}

const _rpUVAnimCustomData* _rpUVAnimCustomDataStreamWrite(const _rpUVAnimCustomData* customData, RwStream* stream) {
    return ((const _rpUVAnimCustomData*(__cdecl *)(const _rpUVAnimCustomData*, RwStream*))0x7CBFD0)(customData, stream);
}

// 0x7CC010: `mov eax, 0x40; ret`: tamano en flujo de los datos de usuario de UVAnim (constante, no mira el argumento).
RwUInt32 _rpUVAnimCustomDataStreamGetSize(const _rpUVAnimCustomData* customData) {
    return 0x40;
}

RwBool RpUVAnimPluginAttach() {
    return ((RwBool(__cdecl *)(void))0x7CB940)();
}

void RpUVAnimLinearKeyFrameApply(void* matrix, void* voidIFrame) {
    ((void(__cdecl *)(void*, void*))0x7CC9F0)(matrix, voidIFrame);
}

void RpUVAnimLinearKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) {
    ((void(__cdecl *)(void*, void*, void*, RwReal))0x7CCAC0)(voidOut, voidIn1, voidIn2, alpha);
}

void RpUVAnimLinearKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) {
    ((void(__cdecl *)(void*, void*, void*, RwReal, void*))0x7CCA40)(voidOut, voidIn1, voidIn2, time, customData);
}

void RpUVAnimLinearKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) {
    ((void(__cdecl *)(void*, void*, void*))0x7CCBE0)(voidOut, voidIn1, voidIn2);
}

void RpUVAnimLinearKeyFrameMulRecip(void* voidFrame, void* voidStart) {
    ((void(__cdecl *)(void*, void*))0x7CCB30)(voidFrame, voidStart);
}

void RpUVAnimParamKeyFrameApply(void* matrix, void* voidIFrame) {
    ((void(__cdecl *)(void*, void*))0x7CC560)(matrix, voidIFrame);
}

void RpUVAnimParamKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) {
    ((void(__cdecl *)(void*, void*, void*, RwReal))0x7CC6A0)(voidOut, voidIn1, voidIn2, alpha);
}

void RpUVAnimParamKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) {
    ((void(__cdecl *)(void*, void*, void*, RwReal, void*))0x7CC600)(voidOut, voidIn1, voidIn2, time, customData);
}

void RpUVAnimParamKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) {
    ((void(__cdecl *)(void*, void*, void*))0x7CC750)(voidOut, voidIn1, voidIn2);
}

void RpUVAnimParamKeyFrameMulRecip(void* voidFrame, void* voidStart) {
    ((void(__cdecl *)(void*, void*))0x7CC740)(voidFrame, voidStart);
}

RtAnimAnimation* RpUVAnimKeyFrameStreamRead(RwStream* stream, RtAnimAnimation* animation) {
    return ((RtAnimAnimation*(__cdecl *)(RwStream*, RtAnimAnimation*))0x7CC870)(stream, animation);
}

RwBool RpUVAnimKeyFrameStreamWrite(const RtAnimAnimation* animation, RwStream* stream) {
    return ((RwBool(__cdecl *)(const RtAnimAnimation*, RwStream*))0x7CC920)(animation, stream);
}

// 0x7CC9D0: tamano en flujo de los fotogramas clave: numFrames * 32 (cada fotograma de 32 bytes) + _rpUVAnimCustomDataStreamGetSize(customData) + 4.
// El EXE llama a 0x7CC010 con animation->customData (+0x14) y suma en 32 bits sin comprobar desbordes; aqui se suma en uint32 para repetir esa aritmetica.
RwInt32 RpUVAnimKeyFrameStreamGetSize(const RtAnimAnimation* animation) {
    const uint32 customDataSize = _rpUVAnimCustomDataStreamGetSize(static_cast<const _rpUVAnimCustomData*>(animation->customData));
    return static_cast<RwInt32>((static_cast<uint32>(animation->numFrames) << 5) + (customDataSize + 4));
}

RpUVAnim* RpUVAnimCreate(const RwChar* name, RwUInt32 numNodes, RwUInt32 numFrames, RwReal duration, RwUInt32* nodeIndexToUVChannelMap, RpUVAnimKeyFrameType keyframeType) {
    return ((RpUVAnim*(__cdecl *)(const RwChar*, RwUInt32, RwUInt32, RwReal, RwUInt32*, RpUVAnimKeyFrameType))0x7CC020)(name, numNodes, numFrames, duration, nodeIndexToUVChannelMap, keyframeType);
}

RwBool RpUVAnimDestroy(RpUVAnim* anim) {
    return ((RwBool(__cdecl *)(RpUVAnim*))0x7CC0C0)(anim);
}

RpUVAnim* RpUVAnimAddRef(RpUVAnim* anim) {
    return ((RpUVAnim*(__cdecl *)(RpUVAnim*))0x7CC100)(anim);
}

const RwChar* RpUVAnimGetName(const RpUVAnim* anim) {
    return ((const RwChar*(__cdecl *)(const RpUVAnim*))0x7CC3A0)(anim);
}

RpMaterial* RpMaterialSetUVAnim(RpMaterial* material, RpUVAnim* anim, RwUInt32 slot) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RpUVAnim*, RwUInt32))0x7CC3B0)(material, anim, slot);
}

RpUVAnimInterpolator* RpMaterialUVAnimGetInterpolator(RpMaterial* material, RwUInt32 slot) {
    return ((RpUVAnimInterpolator*(__cdecl *)(RpMaterial*, RwUInt32))0x7CC430)(material, slot);
}

RpMaterial* RpMaterialUVAnimSetInterpolator(RpMaterial* material, RpUVAnimInterpolator* interp, RwUInt32 slot) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RpUVAnimInterpolator*, RwUInt32))0x7CC450)(material, interp, slot);
}

RpMaterial* RpMaterialUVAnimSetCurrentTime(RpMaterial* material, RwReal time) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwReal))0x7CC470)(material, time);
}

RpMaterial* RpMaterialUVAnimAddAnimTime(RpMaterial* material, RwReal deltaTime) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwReal))0x7CC4B0)(material, deltaTime);
}

RpMaterial* RpMaterialUVAnimSubAnimTime(RpMaterial* material, RwReal deltaTime) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwReal))0x7CC4F0)(material, deltaTime);
}

RpMaterial* RpMaterialUVAnimApplyUpdate(RpMaterial* material)
{
    return plugin::CallAndReturn<RpMaterial*, 0x7CC110, RpMaterial*>(material);
}

RwBool RpMaterialUVAnimExists(const RpMaterial* material) {
    return ((RwBool(__cdecl *)(const RpMaterial*))0x7CC530)(material);
}
