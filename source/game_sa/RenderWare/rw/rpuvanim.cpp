/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpUVAnim
 * Reconstrucción: COMPLETA — 42/42 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/42 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpuvanim.h"
#include "rpmatfx.h"
#include "X87Math.h"

// Funciones propias del modulo sin nombre en el SDK (nombres inferidos del desensamblado); solo se enganchan, por eso se declaran aqui.
RwStream* _rpUVAnimMaterialStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RpUVAnimMaterialExtension* _rpUVAnimMaterialUpdateMatrices(RpUVAnimMaterialExtension* extension);
RpUVAnim* _rpUVAnimDefaultAnimCreate(const RwChar* name);
// Callbacks del plugin 0x135 y de los esquemas de interpolacion (lotes S8A y S9A; cdecl). Se registran por su direccion propia y llevan hook en la direccion del EXE.
static void*     RpUVAnimEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                          // 0x7CB9B0
static void*     RpUVAnimEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                           // 0x7CBA20
static void*     RpUVAnimMaterialConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                        // 0x7CBA80
static void*     RpUVAnimMaterialDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                         // 0x7CBAA0
static void*     RpUVAnimMaterialCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);     // 0x7CBB30
static RwStream* RpUVAnimMaterialStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x7CBC20
static RwInt32   RpUVAnimMaterialStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                // 0x7CBF20

void RpUVAnimInjectHooks() {
    RH_ScopedNamespaceName("RpUVAnim");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(_rpUVAnimCustomDataStreamGetSize, 0x7CC010);
    RH_ScopedGlobalInstall(RpUVAnimKeyFrameStreamGetSize, 0x7CC9D0);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameDataFromMatrix, 0x7CCC50);
    // Lote R6A (sin validar)
    RH_ScopedGlobalInstall(RpUVAnimPluginAttach, 0x7CB940);
    RH_ScopedGlobalInstall(_rpUVAnimMaterialStreamWrite, 0x7CBB70);
    RH_ScopedGlobalInstall(_rpUVAnimMaterialUpdateMatrices, 0x7CBD80);
    RH_ScopedGlobalInstall(_rpUVAnimDefaultAnimCreate, 0x7CBE00);
    RH_ScopedGlobalInstall(_rpUVAnimCustomDataStreamRead, 0x7CBF70);
    RH_ScopedGlobalInstall(_rpUVAnimCustomDataStreamWrite, 0x7CBFD0);
    RH_ScopedGlobalInstall(RpUVAnimDestroy, 0x7CC0C0);
    RH_ScopedGlobalInstall(RpMaterialUVAnimApplyUpdate, 0x7CC110);
    RH_ScopedGlobalInstall(RpMaterialSetUVAnim, 0x7CC3B0);
    RH_ScopedGlobalInstall(RpMaterialUVAnimAddAnimTime, 0x7CC4B0);
    RH_ScopedGlobalInstall(RpMaterialUVAnimExists, 0x7CC530);
    RH_ScopedGlobalInstall(RpUVAnimParamKeyFrameDataInitFromMatrix, 0x7CC760);
    RH_ScopedGlobalInstall(RpUVAnimKeyFrameInit, 0x7CC810);
    // Lotes S8A y S9A (sin validar)
    RH_ScopedGlobalInstall(RpUVAnimEngineConstructor, 0x7CB9B0);
    RH_ScopedGlobalInstall(RpUVAnimEngineDestructor, 0x7CBA20);
    RH_ScopedGlobalInstall(RpUVAnimMaterialConstructor, 0x7CBA80);
    RH_ScopedGlobalInstall(RpUVAnimMaterialDestructor, 0x7CBAA0);
    RH_ScopedGlobalInstall(RpUVAnimMaterialCopy, 0x7CBB30);
    RH_ScopedGlobalInstall(RpUVAnimMaterialStreamRead, 0x7CBC20);
    RH_ScopedGlobalInstall(RpUVAnimMaterialStreamGetSize, 0x7CBF20);
    RH_ScopedGlobalInstall(RpUVAnimCreate, 0x7CC020);
    RH_ScopedGlobalInstall(RpUVAnimAddRef, 0x7CC100);
    RH_ScopedGlobalInstall(RpUVAnimGetName, 0x7CC3A0);
    RH_ScopedGlobalInstall(RpMaterialUVAnimGetInterpolator, 0x7CC430);
    RH_ScopedGlobalInstall(RpMaterialUVAnimSetInterpolator, 0x7CC450);
    RH_ScopedGlobalInstall(RpMaterialUVAnimSetCurrentTime, 0x7CC470);
    RH_ScopedGlobalInstall(RpMaterialUVAnimSubAnimTime, 0x7CC4F0);
    RH_ScopedGlobalInstall(RpUVAnimParamKeyFrameApply, 0x7CC560);
    RH_ScopedGlobalInstall(RpUVAnimParamKeyFrameInterpolate, 0x7CC600);
    RH_ScopedGlobalInstall(RpUVAnimParamKeyFrameBlend, 0x7CC6A0);
    RH_ScopedGlobalInstall(RpUVAnimParamKeyFrameMulRecip, 0x7CC740);
    RH_ScopedGlobalInstall(RpUVAnimParamKeyFrameAdd, 0x7CC750);
    RH_ScopedGlobalInstall(RpUVAnimKeyFrameStreamRead, 0x7CC870);
    RH_ScopedGlobalInstall(RpUVAnimKeyFrameStreamWrite, 0x7CC920);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameApply, 0x7CC9F0);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameInterpolate, 0x7CCA40);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameBlend, 0x7CCAC0);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameMulRecip, 0x7CCB30);
    RH_ScopedGlobalInstall(RpUVAnimLinearKeyFrameAdd, 0x7CCBE0);
}

static_assert(offsetof(RwMatrix, up) == 0x10 && offsetof(RwMatrix, pos) == 0x30 && sizeof(RpUVAnimLinearKeyFrameData) == 0x18);
static_assert(sizeof(_rpUVAnimCustomData) == 0x44 && offsetof(_rpUVAnimCustomData, nodeToUVChannelMap) == 0x20 && offsetof(_rpUVAnimCustomData, refCount) == 0x40);
static_assert(sizeof(RpUVAnimMaterialExtension) == 0x28 && offsetof(RpUVAnimMaterialExtension, interp) == 0x08);
static_assert(sizeof(RpUVAnimKeyFrame) == 0x20 && offsetof(RpUVAnimKeyFrame, data) == 0x08 && offsetof(RpUVAnimInterpFrame, data) == 0x08);
static_assert(offsetof(RtAnimInterpolator, currentInterpKeyFrameSize) == 0x24 && offsetof(RtAnimInterpolator, numNodes) == 0x2C && offsetof(RtAnimInterpolator, keyFrameApplyCB) == 0x3C
           && sizeof(RtAnimInterpolator) == 0x4C);
static_assert(offsetof(RtAnimAnimation, customData) == 0x14 && offsetof(RtAnimAnimation, pFrames) == 0x10 && offsetof(RtAnimInterpolatorInfo, animKeyFrameSize) == 0x08);

// Lote R6A (sin validar): estado global del modulo en el EXE (direcciones).
//   0xC978F4 lista libre de _rpUVAnimCustomData (bloques de 0x44 bytes; pista de memoria 0x30135)   0xC978FC contador de aperturas del plugin de motor
//   0xC9B8D0 desplazamiento de la extension de material (RpUVAnimMaterialGlobals.engineOffset)       0x8DED84 / 0x8DEDB4 RtAnimInterpolatorInfo lineal / parametrica (typeID en el primer dword)
//   0x882ED4 pivote (-0.5, -0.5, 0)  0x882EE0 pivote inverso (0.5, 0.5, 0)  0x859878 = 57.2957763671875f (grados por radian)
namespace {
constexpr RwUInt32 kUVAnimPluginID = 0x135;
constexpr RwUInt32 kUVAnimMemHint  = 0x30135;

RwFreeList*& UVAnimFreeList() { return *reinterpret_cast<RwFreeList**>(0xC978F4); }
RwInt32& UVAnimMaterialOffset() { return *reinterpret_cast<RwInt32*>(0xC9B8D0); }
RtAnimInterpolatorInfo& UVAnimLinearInfo() { return *reinterpret_cast<RtAnimInterpolatorInfo*>(0x8DED84); }
RtAnimInterpolatorInfo& UVAnimParamInfo()  { return *reinterpret_cast<RtAnimInterpolatorInfo*>(0x8DEDB4); }
const RwV3d& UVAnimPivot()    { return *reinterpret_cast<const RwV3d*>(0x882ED4); }
const RwV3d& UVAnimInvPivot() { return *reinterpret_cast<const RwV3d*>(0x882EE0); }
constexpr float kDegreesPerRadianExe = 57.2957763671875f; // 0x859878 (el EXE usa este float, no pi exacto)

// material + desplazamiento del plugin: la extension (uv[2] y interp[8]).
RpUVAnimMaterialExtension* MaterialExt(const void* material) {
    return reinterpret_cast<RpUVAnimMaterialExtension*>(const_cast<RwUInt8*>(static_cast<const RwUInt8*>(material)) + UVAnimMaterialOffset());
}

} // namespace

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

// 0x7CBF70 (lote R6A, sin validar; cdecl, un argumento, `ret`, 94 bytes): lee los datos de usuario de una animacion UV. Bloque de la lista libre 0xC978F4 (pista 0x30135; NULL si falla); RwStreamRead(flujo, bloque, 0x20)
// para el nombre (0 -> NULL, sin liberar el bloque); RwStreamReadInt32(flujo, bloque + 0x20, 0x20) para el mapa nodo->canal (NULL si falla); refCount (+0x40) = 1. Devuelve el bloque.
_rpUVAnimCustomData* _rpUVAnimCustomDataStreamRead(RwStream* stream) {
    _rpUVAnimCustomData* const customData = static_cast<_rpUVAnimCustomData*>(RwFreeListAlloc(UVAnimFreeList(), kUVAnimMemHint));
    if (customData == nullptr) {
        return nullptr;
    }
    if (RwStreamRead(stream, customData, 0x20) == 0) {
        return nullptr;
    }
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(customData->nodeToUVChannelMap), 0x20) == nullptr) {
        return nullptr;
    }
    customData->refCount = 1;
    return customData;
}

// 0x7CBFD0 (lote R6A, sin validar; cdecl, 2 argumentos, `ret`, 53 bytes): escribe los datos de usuario: RwStreamWrite(flujo, datos, 0x20) (resultado nulo -> NULL) y RwStreamWriteInt32(flujo, datos + 0x20, 0x20)
// (resultado nulo -> NULL; no nulo -> los datos: `neg eax; sbb eax, eax; and eax, esi`).
const _rpUVAnimCustomData* _rpUVAnimCustomDataStreamWrite(const _rpUVAnimCustomData* customData, RwStream* stream) {
    if (RwStreamWrite(stream, customData, 0x20) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(customData->nodeToUVChannelMap), 0x20) == nullptr) {
        return nullptr;
    }
    return customData;
}

// 0x7CBB70 (lote R6A, sin validar; cdecl, 5 argumentos, `ret`, 170 bytes): escritura de la extension del material en el flujo (RwPluginDataChunkWriteCallBack; solo se usan el flujo y el objeto). Mascara de bits con
// las ranuras de interpolacion (+8..+0x27 de la extension) no nulas y tamano 4 + 0x20 * cuantas haya; cabecera de fragmento (tipo 1, tamano, version 0x36003, build 0xFFFF) con _rwStreamWriteVersionedChunkHeader
// (si falla NULL); la mascara (4 bytes) con RwStreamWriteInt32 (si falla NULL); por cada ranura con interpolacion, los 0x20 bytes de nombre de customData de su animacion con RwStreamWrite (sin mirar el resultado).
// Devuelve el flujo.
RwStream* _rpUVAnimMaterialStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const RpUVAnimMaterialExtension* const extension = MaterialExt(object);
    RwUInt32 mask = 0;
    RwInt32 size = 4;
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (extension->interp[slot] != nullptr) {
            mask |= 1u << slot;
            size += 0x20;
        }
    }
    if (_rwStreamWriteVersionedChunkHeader(stream, 1, size, 0x36003, 0xFFFF) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, reinterpret_cast<const RwInt32*>(&mask), 4) == nullptr) {
        return nullptr;
    }
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (const RtAnimInterpolator* const interp = extension->interp[slot]) {
            RwStreamWrite(stream, interp->pCurrentAnim->customData, 0x20);
        }
    }
    return stream;
}

// 0x7CBD80 (lote R6A, sin validar; cdecl, un argumento, `ret`, 121 bytes; sin nombre en el SDK, llamada desde 0x7CBC20 y 0x7CC3B0): sincroniza las dos matrices UV de la extension con las animaciones. Para cada
// canal (0 y 1): recorre las ranuras 0..7 y, dentro de cada una con interpolacion, sus nodos (numNodes sin signo, como el EXE) buscando uno cuyo nodeToUVChannelMap[nodo] de customData sea el canal. Si lo encuentra:
// crea la matriz del canal con RwMatrixCreate si no existia (el resultado se guarda aunque sea NULL; si es NULL devuelve NULL) y pasa al canal siguiente. Si no hay ninguno: destruye la matriz del canal (si
// existe) y la pone a NULL. Una ranura vacia no reinicia el indice de nodo (ya valia 0). Devuelve la extension.
RpUVAnimMaterialExtension* _rpUVAnimMaterialUpdateMatrices(RpUVAnimMaterialExtension* extension) {
    for (RwUInt32 channel = 0; channel < RP_UVANIM_APPLIEDCHANNELS; ++channel) {
        RwUInt32 slot = 0;
        RwUInt32 node = 0;
        bool found = false;
        while (slot < RP_UVANIM_MAXSLOTS) {
            const RtAnimInterpolator* const interp = extension->interp[slot];
            if (interp == nullptr) {
                ++slot;
                continue;
            }
            const _rpUVAnimCustomData* const customData = static_cast<const _rpUVAnimCustomData*>(interp->pCurrentAnim->customData);
            if (channel == customData->nodeToUVChannelMap[node]) {
                found = true;
                break;
            }
            ++node;
            if (node >= static_cast<RwUInt32>(interp->numNodes)) {
                node = 0;
                ++slot;
            }
        }
        if (found) {
            if (extension->uv[channel] == nullptr) {
                extension->uv[channel] = RwMatrixCreate();
                if (extension->uv[channel] == nullptr) {
                    return nullptr;
                }
            }
        } else if (extension->uv[channel] != nullptr) {
            RwMatrixDestroy(extension->uv[channel]);
            extension->uv[channel] = nullptr;
        }
    }
    return extension;
}

// 0x7CBE00 (lote R6A, sin validar; cdecl, un argumento (nombre de 0x20 bytes), `ret`, 285 bytes; llamada desde 0x7CBC20 cuando el flujo no trae el nombre): crea la animacion por defecto de 2 fotogramas.
// RtAnimAnimationCreate(tipo lineal = [0x8DED84], 2, 0, 1.0f) (NULL si falla); bloque de 0x44 bytes de la lista libre 0xC978F4 (pista 0x30135, NULL si falla; la animacion se pierde); strncpy(nombre, 0x20) +
// byte 0x1F a 0, nodeToUVChannelMap[0] = 0 (el resto del mapa NO se escribe: queda con lo que traiga el bloque) y refCount = 1; anim->customData = bloque. Matriz identidad local (en el EXE flags = basura de pila | 0x20003
// y los rellenos sin escribir; ninguno se lee despues) y dos RpUVAnimKeyFrameInit: (anim, frame0, NULL, 0.0f) y (anim, frame1 = frame0 + animKeyFrameSize, frame0, 1.0f). Devuelve la animacion.
RpUVAnim* _rpUVAnimDefaultAnimCreate(const RwChar* name) {
    RtAnimAnimation* const anim = RtAnimAnimationCreate(UVAnimLinearInfo().typeID, 2, 0, 1.0f);
    if (anim == nullptr) {
        return nullptr;
    }
    _rpUVAnimCustomData* const customData = static_cast<_rpUVAnimCustomData*>(RwFreeListAlloc(UVAnimFreeList(), kUVAnimMemHint));
    if (customData == nullptr) {
        return nullptr;
    }
#pragma warning(suppress : 4996)
    strncpy(customData->name, name, 0x20);
    customData->name[RP_UVANIM_MAXNAME - 1] = 0;
    customData->nodeToUVChannelMap[0] = 0;
    customData->refCount = 1;
    anim->customData = customData;

    RwUInt8* const frame0 = static_cast<RwUInt8*>(anim->pFrames);
    RwUInt8* const frame1 = frame0 + anim->interpInfo->animKeyFrameSize;
    RwMatrix matrix;
    memset(&matrix, 0, sizeof(matrix));
    matrix.flags = 0x20003;
    matrix.right.x = 1.0f;
    matrix.up.y = 1.0f;
    matrix.at.z = 1.0f;
    RpUVAnimKeyFrameInit(anim, reinterpret_cast<RpUVAnimKeyFrame*>(frame0), nullptr, 0.0f, &matrix);
    RpUVAnimKeyFrameInit(anim, reinterpret_cast<RpUVAnimKeyFrame*>(frame1), reinterpret_cast<RpUVAnimKeyFrame*>(frame0), 1.0f, &matrix);
    return anim;
}

// 0x7CC010: `mov eax, 0x40; ret`: tamano en flujo de los datos de usuario de UVAnim (constante, no mira el argumento).
RwUInt32 _rpUVAnimCustomDataStreamGetSize(const _rpUVAnimCustomData* customData) {
    return 0x40;
}

// Elimina una referencia a los datos de usuario de una animacion y, si era la ultima, libera el bloque en la lista libre 0xC978F4 y destruye la animacion (secuencia comun de 0x7CBAA0 y 0x7CBC20).
static void UVAnimReleaseAnim(RtAnimAnimation* anim) {
    _rpUVAnimCustomData* const customData = static_cast<_rpUVAnimCustomData*>(anim->customData);
    if (--customData->refCount == 0) {
        RwFreeListFree(UVAnimFreeList(), anim->customData);
        RtAnimAnimationDestroy(anim);
    }
}

// 0x7CB9B0 (lote S8A, sin validar; cdecl, 3 argumentos, 103 bytes): constructor del plugin de motor 0x135. Crea la lista libre de datos de animacion (RwFreeListCreateAndPreallocateSpace(0x44, [0x8DED48], 4, [0x8DED4C],
// [0xC978F4], 0x40135)) y la guarda en 0xC978F4 (aunque sea NULL); RtDictSchemaInit(esquema 0x8DED50) y RtAnimRegisterInterpolationScheme de los esquemas lineal (0x8DED84) y parametrico (0x8DEDB4) (resultados ignorados);
// sube el contador de 0xC978FC. Devuelve el objeto, o NULL si no se pudo crear la lista.
static void* RpUVAnimEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    UVAnimFreeList() = RwFreeListCreateAndPreallocateSpace(0x44, *reinterpret_cast<const RwInt32*>(0x8DED48), 4, *reinterpret_cast<const RwInt32*>(0x8DED4C), UVAnimFreeList(), kUVAnimMemHint);
    void* const result = UVAnimFreeList() != nullptr ? object : nullptr;
    RtDictSchemaInit(&RpUVAnimDictSchema);
    RtAnimRegisterInterpolationScheme(&UVAnimLinearInfo());
    RtAnimRegisterInterpolationScheme(&UVAnimParamInfo());
    ++*reinterpret_cast<RwInt32*>(0xC978FC);
    return result;
}

// 0x7CBA20 (lote S8A, sin validar; cdecl, 3 argumentos, 59 bytes): destructor del plugin de motor: baja el contador de 0xC978FC, RtDictSchemaDestruct(esquema 0x8DED50) y, si existe la lista libre (0xC978F4), la destruye
// con RwFreeListDestroy y pone el puntero a NULL. Devuelve el objeto.
static void* RpUVAnimEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    --*reinterpret_cast<RwInt32*>(0xC978FC);
    RtDictSchemaDestruct(&RpUVAnimDictSchema);
    if (UVAnimFreeList() != nullptr) {
        RwFreeListDestroy(UVAnimFreeList());
        UVAnimFreeList() = nullptr;
    }
    return object;
}

// 0x7CBA80 (lote S8A, sin validar; cdecl, 3 argumentos, 29 bytes): constructor del plugin de material: pone a cero los diez dwords de la extension en objeto + [0xC9B8D0] (no usa el desplazamiento recibido). Devuelve el objeto.
static void* RpUVAnimMaterialConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    memset(MaterialExt(object), 0, sizeof(RpUVAnimMaterialExtension));
    return object;
}

// 0x7CBAA0 (lote S9A, sin validar; cdecl, 3 argumentos, 130 bytes): destructor del plugin de material. Para cada matriz UV (2): RwMatrixDestroy si existe y la ranura queda a NULL (siempre). Para cada ranura de interpolacion
// (8) no nula: baja el contador de referencias de los datos de usuario de su animacion y, si llega a cero, libera el bloque en la lista libre y destruye la animacion (RtAnimAnimationDestroy); despues
// RtAnimInterpolatorDestroy(interpolacion) (la ranura NO se vacia). Devuelve el objeto.
static void* RpUVAnimMaterialDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    RpUVAnimMaterialExtension* const extension = MaterialExt(object);
    for (RwUInt32 channel = 0; channel < RP_UVANIM_APPLIEDCHANNELS; ++channel) {
        if (extension->uv[channel] != nullptr) {
            RwMatrixDestroy(extension->uv[channel]);
        }
        extension->uv[channel] = nullptr;
    }
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (RtAnimInterpolator* const interp = extension->interp[slot]) {
            UVAnimReleaseAnim(interp->pCurrentAnim);
            RtAnimInterpolatorDestroy(interp);
        }
    }
    return object;
}

// 0x7CBB30 (lote S8A, sin validar; cdecl, 4 argumentos, 61 bytes): copia del plugin de material: copia los diez dwords de la extension (src + [0xC9B8D0] a dst + [0xC9B8D0]) y, por cada una de las 8 ranuras con interpolacion
// (los punteros se comparten, no se clonan), suma uno al contador de referencias de los datos de usuario de su animacion. Devuelve el objeto destino.
static void* RpUVAnimMaterialCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    RpUVAnimMaterialExtension* const dst = MaterialExt(dstObject);
    memcpy(dst, MaterialExt(srcObject), sizeof(RpUVAnimMaterialExtension));
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (const RtAnimInterpolator* const interp = dst->interp[slot]) {
            ++static_cast<_rpUVAnimCustomData*>(interp->pCurrentAnim->customData)->refCount;
        }
    }
    return dstObject;
}

// 0x7CBC20 (lote S9A, sin validar; cdecl, 5 argumentos, 339 bytes): lectura de la extension del material desde el flujo. RwStreamFindChunk(flujo, 1, 0, 0) (0 -> NULL); mascara con RwStreamReadInt32 (fallo -> NULL). Por cada
// ranura (0..7) con su bit: RwStreamRead(flujo, nombre de 0x20 bytes) (fallo -> NULL); animacion = entrada del diccionario actual del esquema (RtDictSchemaGetCurrentDict + RtDictFindNamedEntry) con una referencia mas, o
// la animacion por defecto (_rpUVAnimDefaultAnimCreate) si no hay diccionario o no esta (NULL -> NULL); interpolacion = RtAnimInterpolatorCreate(RtAnimAnimationGetNumNodes(anim), interpKeyFrameSize) guardada en la ranura aunque sea
// NULL; RtAnimInterpolatorSetCurrentAnim (0 -> baja una referencia de la animacion con la misma liberacion que el destructor y devuelve NULL). Al final _rpUVAnimMaterialUpdateMatrices(extension): devuelve el flujo si no es NULL, si no NULL.
// El nombre se lee en un buffer de 0x20 bytes sin terminador garantizado (como el EXE).
static RwStream* RpUVAnimMaterialStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject); UNUSED(sizeInObject);
    RpUVAnimMaterialExtension* const extension = MaterialExt(object);
    if (RwStreamFindChunk(stream, 1, nullptr, nullptr) == 0) {
        return nullptr;
    }
    RwUInt32 mask;
    if (RwStreamReadInt32(stream, reinterpret_cast<RwInt32*>(&mask), 4) == nullptr) {
        return nullptr;
    }
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if ((mask & (1u << slot)) == 0) {
            continue;
        }
        RwChar name[RP_UVANIM_MAXNAME];
        if (RwStreamRead(stream, name, 0x20) == 0) {
            return nullptr;
        }
        RtAnimAnimation* anim;
        RtDict* const dict = RtDictSchemaGetCurrentDict(&RpUVAnimDictSchema);
        if (dict != nullptr) {
            anim = static_cast<RtAnimAnimation*>(RtDictFindNamedEntry(dict, name));
            if (anim != nullptr) {
                ++static_cast<_rpUVAnimCustomData*>(anim->customData)->refCount;
            } else {
                anim = _rpUVAnimDefaultAnimCreate(name);
            }
        } else {
            anim = _rpUVAnimDefaultAnimCreate(name);
        }
        if (anim == nullptr) {
            return nullptr;
        }
        const RwInt32 maxInterpKeyFrameSize = anim->interpInfo->interpKeyFrameSize;
        extension->interp[slot] = RtAnimInterpolatorCreate(RtAnimAnimationGetNumNodes(anim), maxInterpKeyFrameSize);
        if (extension->interp[slot] == nullptr || RtAnimInterpolatorSetCurrentAnim(extension->interp[slot], anim) == 0) {
            UVAnimReleaseAnim(anim);
            return nullptr;
        }
    }
    return _rpUVAnimMaterialUpdateMatrices(extension) != nullptr ? stream : nullptr;
}

// 0x7CBF20 (lote S8A, sin validar; cdecl, 3 argumentos, 65 bytes): tamano de la extension en el flujo: 0 si no hay ninguna interpolacion; si no, 0x10 + 0x20 por cada ranura con interpolacion.
static RwInt32 RpUVAnimMaterialStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    const RpUVAnimMaterialExtension* const extension = MaterialExt(object);
    RwInt32 count = 0;
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (extension->interp[slot] != nullptr) {
            ++count;
        }
    }
    return count == 0 ? 0 : 0x10 + 0x20 * count;
}

// 0x7CB940 (lote R6A, sin validar; cdecl, sin argumentos, `ret`, 111 bytes): registra el plugin 0x135. Motor: RwEngineRegisterPlugin(0, 0x135, ctor 0x7CB9B0, dtor 0x7CBA20) (si < 0 devuelve FALSE sin seguir);
// material: RpMaterialRegisterPlugin(0x28, 0x135, ctor 0x7CBA80, dtor 0x7CBAA0, copia 0x7CBB30), cuyo resultado se guarda SIEMPRE en 0xC9B8D0 antes de registrar el flujo RpMaterialRegisterPluginStream(0x135,
// lectura 0x7CBC20, escritura 0x7CBB70, tamano 0x7CBF20) (si < 0 devuelve FALSE); si el desplazamiento guardado es negativo tambien FALSE; si no, TRUE. Lotes S8A/S9A: los callbacks son funciones propias
// (con hook en su direccion del EXE) y ya no se pasan por direccion fija.
RwBool RpUVAnimPluginAttach() {
    if (RwEngineRegisterPlugin(0, kUVAnimPluginID, RpUVAnimEngineConstructor, RpUVAnimEngineDestructor) < 0) {
        return FALSE;
    }
    UVAnimMaterialOffset() = RpMaterialRegisterPlugin(0x28, kUVAnimPluginID, RpUVAnimMaterialConstructor, RpUVAnimMaterialDestructor, RpUVAnimMaterialCopy);
    if (RpMaterialRegisterPluginStream(kUVAnimPluginID, RpUVAnimMaterialStreamRead, _rpUVAnimMaterialStreamWrite, RpUVAnimMaterialStreamGetSize) < 0) {
        return FALSE;
    }
    return UVAnimMaterialOffset() < 0 ? FALSE : TRUE;
}

// Los fotogramas se tratan como arrays de floats: indice 0 = prevFrame (puntero), 1 = time, 2..7 = datos (RpUVAnimKeyFrame +0x08..+0x1C; en la interpolada los indices 0 y 1 son los dos punteros y 2..7 los datos).
static float* FrameFloats(void* frame) { return static_cast<float*>(frame); }
static const float* FrameFloats(const void* frame) { return static_cast<const float*>(frame); }

// 0x7CC9F0 (lote S9A, sin validar; cdecl, 2 argumentos, `ret`, 75 bytes): matriz UV a partir de los seis datos lineales (copia de dwords, sin x87): right = (uv0, uv1, 0), up = (uv2, uv3, 0), at = (0, 0, 0),
// pos = (uv4, uv5, 0) (los rellenos +0x0C, +0x1C, +0x2C, +0x3C no se tocan) y RwMatrixUpdate(matriz) (0x7F18A0), que fija las banderas.
void RpUVAnimLinearKeyFrameApply(void* matrix, void* voidIFrame) {
    RwUInt32* const m = static_cast<RwUInt32*>(matrix);
    const RwUInt32* const f = static_cast<const RwUInt32*>(voidIFrame);
    m[0] = f[2];
    m[1] = f[3];
    m[2] = 0;
    m[4] = f[4];
    m[5] = f[5];
    m[6] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 0;
    m[12] = f[6];
    m[13] = f[7];
    m[14] = 0;
    RwMatrixUpdate(static_cast<RwMatrix*>(matrix));
}

// 0x7CCAC0 (lote S9A, sin validar; x87, 109 bytes): mezcla de dos fotogramas interpolados con un peso: out[i] = float((in2[i] - in1[i]) * alpha + in1[i]) para los seis datos (X87Math.cpp).
void RpUVAnimLinearKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) {
    UVAnimLinearBlendCpp(FrameFloats(voidOut), FrameFloats(voidIn1), FrameFloats(voidIn2), alpha);
}

// 0x7CCA40 (lote S9A, sin validar; x87, 114 bytes): interpolacion lineal: alpha = (time - in1.time) / (in2.time - in1.time) sin redondear a float y out[i] = float((in2[i] - in1[i]) * alpha + in1[i]).
// El quinto argumento (customData) no se usa.
void RpUVAnimLinearKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) {
    UNUSED(customData);
    UVAnimLinearInterpolateCpp(FrameFloats(voidOut), FrameFloats(voidIn1), FrameFloats(voidIn2), time);
}

// 0x7CCBE0 (lote S9A, sin validar; x87, 99 bytes): suma de fotogramas (producto de matrices 2x2 en los cuatro primeros datos y suma de la traslacion) (X87Math.cpp).
void RpUVAnimLinearKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) {
    UVAnimLinearAddCpp(FrameFloats(voidOut), FrameFloats(voidIn1), FrameFloats(voidIn2));
}

// 0x7CCB30 (lote S9A, sin validar; x87, 173 bytes): frame = frame * inverso(start) (X87Math.cpp; conserva la secuencia exacta del EXE, incluido que con determinante 0 no se invierte).
void RpUVAnimLinearKeyFrameMulRecip(void* voidFrame, void* voidStart) {
    UVAnimLinearMulRecipCpp(FrameFloats(voidFrame), FrameFloats(voidStart));
}

// 0x7CC560 (lote S9A, sin validar; cdecl, 2 argumentos, `ret`, 159 bytes): macro RpUVAnimParamKeyFrameToMatrix: right = (s0, skew, 0), up = (0, s1, 0), at = (0, 0, 0), pos = (x, y, 0) (copia de dwords; los
// rellenos no se tocan) y RwMatrixUpdate; despues, POSTCONCAT: RwMatrixTranslate(pivote 0x882ED4), RwMatrixRotate(eje z = (0, 0, 1), float(theta * 57.2957763671875f) con la x87) y RwMatrixTranslate(pivote inverso 0x882EE0).
void RpUVAnimParamKeyFrameApply(void* matrix, void* voidIFrame) {
    RwUInt32* const m = static_cast<RwUInt32*>(matrix);
    const RwUInt32* const f = static_cast<const RwUInt32*>(voidIFrame);
    const RwV3d zAxis = {0.0f, 0.0f, 1.0f};
    m[0] = f[3];  // right.x = s0
    m[1] = f[5];  // right.y = skew
    m[2] = 0;
    m[4] = 0;
    m[5] = f[4];  // up.y = s1
    m[6] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 0;
    m[12] = f[6]; // pos.x = x
    m[13] = f[7]; // pos.y = y
    m[14] = 0;
    RwMatrixUpdate(static_cast<RwMatrix*>(matrix));
    RwMatrixTranslate(static_cast<RwMatrix*>(matrix), &UVAnimPivot(), rwCOMBINEPOSTCONCAT);
    float theta;
    memcpy(&theta, &f[2], sizeof(theta));
    RwMatrixRotate(static_cast<RwMatrix*>(matrix), &zAxis, UVAnimDegreesCpp(theta, kDegreesPerRadianExe), rwCOMBINEPOSTCONCAT);
    RwMatrixTranslate(static_cast<RwMatrix*>(matrix), &UVAnimInvPivot(), rwCOMBINEPOSTCONCAT);
}

// 0x7CC6A0 (lote S9A, sin validar; x87, 153 bytes): como la mezcla lineal pero la diferencia de angulos (theta) se reduce a [-pi, pi] (si < -pi suma 2 pi; si no, si > pi resta 2 pi; con NaN no cambia) (X87Math.cpp).
void RpUVAnimParamKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) {
    UVAnimParamBlendCpp(FrameFloats(voidOut), FrameFloats(voidIn1), FrameFloats(voidIn2), alpha);
}

// 0x7CC600 (lote S9A, sin validar; x87, 158 bytes): interpolacion parametrica con la reduccion del angulo (X87Math.cpp). El quinto argumento (customData) no se usa.
void RpUVAnimParamKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) {
    UNUSED(customData);
    UVAnimParamInterpolateCpp(FrameFloats(voidOut), FrameFloats(voidIn1), FrameFloats(voidIn2), time);
}

// 0x7CC750 (lote S8A, sin validar): un solo `ret` (no hace nada).
void RpUVAnimParamKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) {
    UNUSED(voidOut); UNUSED(voidIn1); UNUSED(voidIn2);
}

// 0x7CC740 (lote S8A, sin validar): un solo `ret` (no hace nada).
void RpUVAnimParamKeyFrameMulRecip(void* voidFrame, void* voidStart) {
    UNUSED(voidFrame); UNUSED(voidStart);
}

// 0x7CC870 (lote S8A, sin validar; cdecl, 2 argumentos, 163 bytes): lee los fotogramas de una animacion. RwStreamReadInt32(flujo, &entero, 4) (valor que no se usa; 0 -> NULL); customData = _rpUVAnimCustomDataStreamRead(flujo)
// (NULL -> NULL) que se guarda en animation->customData; por cada fotograma (numFrames con signo): RwStreamReadReal(flujo, &fotograma->time, 0x1c) (time + seis datos; fallo -> NULL), RwStreamReadInt32(flujo, &indice, 4)
// (fallo -> NULL) y fotograma->prevFrame = pFrames + (indice << 5) (el indice no se comprueba). Devuelve la animacion.
RtAnimAnimation* RpUVAnimKeyFrameStreamRead(RwStream* stream, RtAnimAnimation* animation) {
    RwInt32 header;
    if (RwStreamReadInt32(stream, &header, 4) == nullptr) {
        return nullptr;
    }
    _rpUVAnimCustomData* const customData = _rpUVAnimCustomDataStreamRead(stream);
    if (customData == nullptr) {
        return nullptr;
    }
    RwUInt8* const frames = static_cast<RwUInt8*>(animation->pFrames);
    animation->customData = customData;
    RwUInt8* frame = frames;
    for (RwInt32 i = 0; i < animation->numFrames; ++i) {
        RpUVAnimKeyFrame* const keyFrame = reinterpret_cast<RpUVAnimKeyFrame*>(frame);
        if (RwStreamReadReal(stream, &keyFrame->time, 0x1c) == nullptr) {
            return nullptr;
        }
        RwInt32 index;
        if (RwStreamReadInt32(stream, &index, 4) == nullptr) {
            return nullptr;
        }
        keyFrame->prevFrame = reinterpret_cast<RpUVAnimKeyFrame*>(static_cast<RwUInt32>(index << 5) + reinterpret_cast<RwUInt32>(frames));
        frame += 0x20;
    }
    return animation;
}

// 0x7CC920 (lote S8A, sin validar; cdecl, 2 argumentos, 168 bytes): escribe los fotogramas. RwStreamWriteInt32(flujo, &0, 4) (fallo -> 0); _rpUVAnimCustomDataStreamWrite(animation->customData, flujo) (NULL -> 0); por cada
// fotograma (numFrames con signo): RwStreamWriteReal(flujo, &fotograma->time, 0x1c) (fallo -> 0) y RwStreamWriteInt32(flujo, &indice, 4) con indice = (prevFrame - pFrames) >> 5 (desplazamiento aritmetico de 32 bits;
// con prevFrame NULL sale -(pFrames >> 5)) (fallo -> 0). Devuelve TRUE.
RwBool RpUVAnimKeyFrameStreamWrite(const RtAnimAnimation* animation, RwStream* stream) {
    RwInt32 zero = 0;
    if (RwStreamWriteInt32(stream, &zero, 4) == nullptr) {
        return FALSE;
    }
    if (_rpUVAnimCustomDataStreamWrite(static_cast<const _rpUVAnimCustomData*>(animation->customData), stream) == nullptr) {
        return FALSE;
    }
    const RwUInt8* const frames = static_cast<const RwUInt8*>(animation->pFrames);
    const RwUInt8* frame = frames;
    for (RwInt32 i = 0; i < animation->numFrames; ++i) {
        const RpUVAnimKeyFrame* const keyFrame = reinterpret_cast<const RpUVAnimKeyFrame*>(frame);
        if (RwStreamWriteReal(stream, &keyFrame->time, 0x1c) == nullptr) {
            return FALSE;
        }
        const RwInt32 index = static_cast<RwInt32>(reinterpret_cast<RwUInt32>(keyFrame->prevFrame) - reinterpret_cast<RwUInt32>(frames)) >> 5;
        if (RwStreamWriteInt32(stream, &index, 4) == nullptr) {
            return FALSE;
        }
        frame += 0x20;
    }
    return TRUE;
}

// 0x7CC9D0: tamano en flujo de los fotogramas clave: numFrames * 32 (cada fotograma de 32 bytes) + _rpUVAnimCustomDataStreamGetSize(customData) + 4.
// El EXE llama a 0x7CC010 con animation->customData (+0x14) y suma en 32 bits sin comprobar desbordes; aqui se suma en uint32 para repetir esa aritmetica.
RwInt32 RpUVAnimKeyFrameStreamGetSize(const RtAnimAnimation* animation) {
    const uint32 customDataSize = _rpUVAnimCustomDataStreamGetSize(static_cast<const _rpUVAnimCustomData*>(animation->customData));
    return static_cast<RwInt32>((static_cast<uint32>(animation->numFrames) << 5) + (customDataSize + 4));
}

// 0x7CC020 (lote S9A, sin validar; cdecl, 6 argumentos, 150 bytes): crea una animacion UV. Tipo 0 -> esquema lineal ([0x8DED84]), tipo 1 -> parametrico ([0x8DEDB4]), otro -> NULL. RtAnimAnimationCreate(typeID, numFrames, 0, duration)
// (NULL -> NULL); bloque de datos de la lista libre 0xC978F4 (pista 0x30135; NULL -> NULL y la animacion se pierde); strncpy(nombre, 0x20) con el byte 0x1F a 0; el mapa nodo->canal se copia con `rep movsd` de numNodes dwords
// SIN limite (mas de 8 nodos desbordan el bloque de 0x44 bytes, como en el EXE); refCount = 1; anim->customData = bloque. Devuelve la animacion.
RpUVAnim* RpUVAnimCreate(const RwChar* name, RwUInt32 numNodes, RwUInt32 numFrames, RwReal duration, RwUInt32* nodeIndexToUVChannelMap, RpUVAnimKeyFrameType keyframeType) {
    RwInt32 typeID;
    if (keyframeType == rpUVANIMLINEARKEYFRAMES) {
        typeID = UVAnimLinearInfo().typeID;
    } else if (keyframeType == rpUVANIMPARAMKEYFRAMES) {
        typeID = UVAnimParamInfo().typeID;
    } else {
        return nullptr;
    }
    RtAnimAnimation* const anim = RtAnimAnimationCreate(typeID, numFrames, 0, duration);
    if (anim == nullptr) {
        return nullptr;
    }
    _rpUVAnimCustomData* const customData = static_cast<_rpUVAnimCustomData*>(RwFreeListAlloc(UVAnimFreeList(), kUVAnimMemHint));
    if (customData == nullptr) {
        return nullptr;
    }
#pragma warning(suppress : 4996)
    strncpy(customData->name, name, 0x20);
    customData->name[RP_UVANIM_MAXNAME - 1] = 0;
    memcpy(customData->nodeToUVChannelMap, nodeIndexToUVChannelMap, static_cast<size_t>(numNodes) * sizeof(RwUInt32));
    customData->refCount = 1;
    anim->customData = customData;
    return anim;
}

// 0x7CC0C0 (lote R6A, sin validar; cdecl, 56 bytes): baja el contador de referencias (customData + 0x40); si llega a cero libera el bloque de datos en la lista libre 0xC978F4 (relee anim->customData) y despues
// destruye la animacion con RtAnimAnimationDestroy (0x7CCF10). Devuelve siempre TRUE.
RwBool RpUVAnimDestroy(RpUVAnim* anim) {
    _rpUVAnimCustomData* const customData = static_cast<_rpUVAnimCustomData*>(anim->customData);
    if (--customData->refCount == 0) {
        RwFreeListFree(UVAnimFreeList(), anim->customData);
        RtAnimAnimationDestroy(anim);
    }
    return TRUE;
}

// 0x7CC100 (lote S8A, sin validar; 11 bytes): suma uno al contador de referencias de los datos de usuario (anim->customData + 0x40) y devuelve la animacion.
RpUVAnim* RpUVAnimAddRef(RpUVAnim* anim) {
    ++static_cast<_rpUVAnimCustomData*>(anim->customData)->refCount;
    return anim;
}

// 0x7CC3A0 (lote S8A, sin validar; 8 bytes): devuelve anim->customData (el nombre es el primer campo de los datos).
const RwChar* RpUVAnimGetName(const RpUVAnim* anim) {
    return static_cast<const RwChar*>(anim->customData);
}

// 0x7CC3B0 (lote R6A, sin validar; cdecl, 3 argumentos, `ret`, 118 bytes): asigna la animacion a una ranura (sin comprobar el rango). Interpolacion nueva con RtAnimInterpolatorCreate(RtAnimAnimationGetNumNodes(anim),
// interpInfo->interpKeyFrameSize) guardada en la ranura (NULL si falla -> NULL); RtAnimInterpolatorSetCurrentAnim(interp, anim); sincroniza las matrices con 0x7CBD80: si devuelve NULL destruye la interpolacion de la
// ranura (sin vaciarla) y devuelve NULL; si no, suma uno a customData->refCount y devuelve el material.
RpMaterial* RpMaterialSetUVAnim(RpMaterial* material, RpUVAnim* anim, RwUInt32 slot) {
    RpUVAnimMaterialExtension* const extension = MaterialExt(material);
    const RwInt32 maxInterpKeyFrameSize = anim->interpInfo->interpKeyFrameSize;
    const RwInt32 numNodes = RtAnimAnimationGetNumNodes(anim);
    extension->interp[slot] = RtAnimInterpolatorCreate(numNodes, maxInterpKeyFrameSize);
    if (extension->interp[slot] == nullptr) {
        return nullptr;
    }
    RtAnimInterpolatorSetCurrentAnim(extension->interp[slot], anim);
    if (_rpUVAnimMaterialUpdateMatrices(extension) == nullptr) {
        RtAnimInterpolatorDestroy(extension->interp[slot]);
        return nullptr;
    }
    ++static_cast<_rpUVAnimCustomData*>(anim->customData)->refCount;
    return material;
}

// 0x7CC430 (lote S8A, sin validar; 22 bytes): devuelve la interpolacion de la ranura (sin comprobar el rango).
RpUVAnimInterpolator* RpMaterialUVAnimGetInterpolator(RpMaterial* material, RwUInt32 slot) {
    return MaterialExt(material)->interp[slot];
}

// 0x7CC450 (lote S8A, sin validar; 26 bytes): guarda la interpolacion en la ranura (sin comprobar el rango ni liberar la anterior) y devuelve el material.
RpMaterial* RpMaterialUVAnimSetInterpolator(RpMaterial* material, RpUVAnimInterpolator* interp, RwUInt32 slot) {
    MaterialExt(material)->interp[slot] = interp;
    return material;
}

// 0x7CC470 (lote S9A, sin validar; 55 bytes): RtAnimInterpolatorSetCurrentTime(interp, time) (0x7CDAB0) en cada ranura con interpolacion. Devuelve el material.
RpMaterial* RpMaterialUVAnimSetCurrentTime(RpMaterial* material, RwReal time) {
    RpUVAnimMaterialExtension* const extension = MaterialExt(material);
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (extension->interp[slot] != nullptr) {
            RtAnimInterpolatorSetCurrentTime(extension->interp[slot], time);
        }
    }
    return material;
}

// 0x7CC4B0 (lote R6A, sin validar; cdecl, 2 argumentos, `ret`, 55 bytes): RtAnimInterpolatorAddAnimTime(interp, deltaTime) (0x7CD8D0) en cada una de las 8 ranuras con interpolacion. Devuelve el material.
RpMaterial* RpMaterialUVAnimAddAnimTime(RpMaterial* material, RwReal deltaTime) {
    RpUVAnimMaterialExtension* const extension = MaterialExt(material);
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (extension->interp[slot] != nullptr) {
            RtAnimInterpolatorAddAnimTime(extension->interp[slot], deltaTime);
        }
    }
    return material;
}

// 0x7CC4F0 (lote S9A, sin validar; 55 bytes): RtAnimInterpolatorSubAnimTime(interp, deltaTime) (0x7CD760) en cada ranura con interpolacion. Devuelve el material.
RpMaterial* RpMaterialUVAnimSubAnimTime(RpMaterial* material, RwReal deltaTime) {
    RpUVAnimMaterialExtension* const extension = MaterialExt(material);
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (extension->interp[slot] != nullptr) {
            RtAnimInterpolatorSubAnimTime(extension->interp[slot], deltaTime);
        }
    }
    return material;
}

// 0x7CC110 (lote R6A, sin validar; cdecl, un argumento, `ret`, 649 bytes): reconstruye las matrices UV del material a partir de las interpolaciones. (1) Cada matriz UV existente pasa a identidad (right.x, up.y,
// at.z = 1; el resto de los doce floats utiles a 0; flags |= 0x20003 = ortonormal | identidad; los rellenos no se tocan). (2) Por cada ranura con interpolacion y por cada nodo (numNodes con signo): canal =
// nodeToUVChannelMap[nodo]; si canal >= 2 o no hay matriz de ese canal, se salta el nodo SIN avanzar el puntero de fotograma (que solo avanza currentInterpKeyFrameSize bytes tras aplicar uno: peculiaridad del EXE).
// Para aplicar se arma una matriz local segun keyFrameApplyCB de la interpolacion: si es 0x7CC9F0 (lineal), los seis floats uv[] a right.xy, up.xy, pos.xy (copia de dwords) y RwMatrixUpdate; si es 0x7CC560
// (parametrica), la macro RpUVAnimParamKeyFrameToMatrix en linea: right.x = s0, right.y = skew, up.y = s1, pos = (x, y), RwMatrixUpdate, RwMatrixTranslate(pivote 0x882ED4), RwMatrixRotate(eje z,
// float(theta * 57.2957763671875f)) y RwMatrixTranslate(pivote inverso 0x882EE0), los tres POSTCONCAT; con otro callback se llama a keyFrameApplyCB(matriz, fotograma). Despues
// RwMatrixTransform(matriz UV, local, PRECONCAT). (3) Al final RpMatFXMaterialSetUVTransformMatrices(material, uv[0], uv[1]) (0x8129E0). Devuelve el material. La matriz local del EXE tiene las banderas
// sin inicializar (pila) hasta RwMatrixUpdate, que las pisa; aqui valen 0.
RpMaterial* RpMaterialUVAnimApplyUpdate(RpMaterial* material) {
    RpUVAnimMaterialExtension* const extension = MaterialExt(material);
    for (RwUInt32 channel = 0; channel < RP_UVANIM_APPLIEDCHANNELS; ++channel) {
        if (RwMatrix* const matrix = extension->uv[channel]) {
            const RwUInt32 flags = matrix->flags;
            matrix->at.z = 1.0f;
            matrix->up.y = 1.0f;
            matrix->right.x = 1.0f;
            matrix->up.x = 0.0f;
            matrix->right.z = 0.0f;
            matrix->right.y = 0.0f;
            matrix->at.y = 0.0f;
            matrix->at.x = 0.0f;
            matrix->up.z = 0.0f;
            matrix->pos.z = 0.0f;
            matrix->pos.y = 0.0f;
            matrix->pos.x = 0.0f;
            matrix->flags = flags | 0x20003;
        }
    }
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        RtAnimInterpolator* const interp = extension->interp[slot];
        if (interp == nullptr) {
            continue;
        }
        const RwInt32 frameSize = interp->currentInterpKeyFrameSize;
        const RtAnimKeyFrameApplyCallBack applyCallBack = interp->keyFrameApplyCB;
        const RwUInt32* const channelMap = static_cast<const _rpUVAnimCustomData*>(interp->pCurrentAnim->customData)->nodeToUVChannelMap;
        RwUInt8* frame = reinterpret_cast<RwUInt8*>(interp + 1);
        for (RwInt32 node = 0; node < interp->numNodes; ++node) {
            const RwUInt32 channel = channelMap[node];
            if (channel >= RP_UVANIM_APPLIEDCHANNELS) {
                continue;
            }
            RwMatrix* const target = extension->uv[channel];
            if (target == nullptr) {
                continue;
            }
            const RwUInt32* const fields = reinterpret_cast<const RwUInt32*>(frame + 8); // RpUVAnimInterpFrame::data (6 dwords)
            RwMatrix local;
            memset(&local, 0, sizeof(local));
            RwUInt32* const localBits = reinterpret_cast<RwUInt32*>(&local);
            if (applyCallBack == UVAnimLinearInfo().keyFrameApplyCB) {
                localBits[0] = fields[0];  // right.x = uv[0]
                localBits[1] = fields[1];  // right.y = uv[1]
                localBits[4] = fields[2];  // up.x = uv[2]
                localBits[5] = fields[3];  // up.y = uv[3]
                localBits[12] = fields[4]; // pos.x = uv[4]
                localBits[13] = fields[5]; // pos.y = uv[5]
                RwMatrixUpdate(&local);
            } else if (applyCallBack == UVAnimParamInfo().keyFrameApplyCB) {
                const RwV3d zAxis = {0.0f, 0.0f, 1.0f};
                localBits[0] = fields[1];  // right.x = s0
                localBits[5] = fields[2];  // up.y = s1
                localBits[1] = fields[3];  // right.y = skew
                localBits[12] = fields[4]; // pos.x = x
                localBits[13] = fields[5]; // pos.y = y
                RwMatrixUpdate(&local);
                RwMatrixTranslate(&local, &UVAnimPivot(), rwCOMBINEPOSTCONCAT);
                float theta;
                memcpy(&theta, &fields[0], sizeof(theta));
                RwMatrixRotate(&local, &zAxis, UVAnimDegreesCpp(theta, kDegreesPerRadianExe), rwCOMBINEPOSTCONCAT);
                RwMatrixTranslate(&local, &UVAnimInvPivot(), rwCOMBINEPOSTCONCAT);
            } else {
                applyCallBack(&local, frame);
            }
            RwMatrixTransform(target, &local, rwCOMBINEPRECONCAT);
            frame += frameSize;
        }
    }
    RpMatFXMaterialSetUVTransformMatrices(material, extension->uv[0], extension->uv[1]);
    return material;
}

// 0x7CC530 (lote R6A, sin validar; cdecl, 39 bytes): TRUE si alguna de las 8 ranuras de interpolacion de la extension no es NULL.
RwBool RpMaterialUVAnimExists(const RpMaterial* material) {
    const RpUVAnimMaterialExtension* const extension = MaterialExt(material);
    for (RwUInt32 slot = 0; slot < RP_UVANIM_MAXSLOTS; ++slot) {
        if (extension->interp[slot] != nullptr) {
            return TRUE;
        }
    }
    return FALSE;
}

// 0x7CC760 (lote R6A, sin validar; cdecl, 2 argumentos, `ret`, 174 bytes): datos parametricos de fotograma a partir de una matriz. Copia la matriz (0x40 bytes) a una local; theta = float(fpatan(-up.x, right.x))
// (se guarda en data->theta antes de seguir); RwMatrixTranslate(pivote 0x882ED4), RwMatrixRotate(eje z, float(theta * -57.2957763671875f) (0x882F2C)) y RwMatrixTranslate(pivote inverso 0x882EE0), los tres
// POSTCONCAT, sobre la copia; despues data->s0 = right.x, s1 = up.y, skew = right.y, x = pos.x, y = pos.y (copia de dwords). Devuelve data.
RpUVAnimParamKeyFrameData* RpUVAnimParamKeyFrameDataInitFromMatrix(RpUVAnimParamKeyFrameData* data, const RwMatrix* matrix) {
    RwMatrix local;
    memcpy(&local, matrix, sizeof(local));
    const RwV3d zAxis = {0.0f, 0.0f, 1.0f};
    data->theta = UVAnimParamThetaCpp(local.up.x, local.right.x);
    RwMatrixTranslate(&local, &UVAnimPivot(), rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(&local, &zAxis, UVAnimDegreesCpp(data->theta, -kDegreesPerRadianExe), rwCOMBINEPOSTCONCAT);
    RwMatrixTranslate(&local, &UVAnimInvPivot(), rwCOMBINEPOSTCONCAT);
    memcpy(&data->s0, &local.right.x, sizeof(RwReal));
    memcpy(&data->skew, &local.right.y, sizeof(RwReal));
    memcpy(&data->s1, &local.up.y, sizeof(RwReal));
    memcpy(&data->x, &local.pos.x, sizeof(RwReal));
    memcpy(&data->y, &local.pos.y, sizeof(RwReal));
    return data;
}

// 0x7CC810 (lote R6A, sin validar; cdecl, 5 argumentos, `ret`, 92 bytes): inicializa un fotograma clave: prevFrame y time (copia de dwords) y, segun el typeID del esquema de la animacion: si es el lineal
// ([0x8DED84]), RpUVAnimLinearKeyFrameDataFromMatrix(&frame->data, matrix) (0x7CCC50, devuelve siempre un puntero no nulo) y solo si eso diera NULL se mira la rama parametrica; si no es el lineal y es el
// parametrico ([0x8DEDB4]), RpUVAnimParamKeyFrameDataInitFromMatrix(&frame->data, matrix) (0x7CC760). Devuelve 0 (`xor eax, eax`) en todos los casos.
RpUVAnimKeyFrame* RpUVAnimKeyFrameInit(const RtAnimAnimation* animation, RpUVAnimKeyFrame* keyFrame, RpUVAnimKeyFrame* prevFrame, RwReal time, const RwMatrix* matrix) {
    keyFrame->prevFrame = prevFrame;
    memcpy(&keyFrame->time, &time, sizeof(time));
    if (animation->interpInfo->typeID != UVAnimLinearInfo().typeID || RpUVAnimLinearKeyFrameDataFromMatrix(&keyFrame->data.linear, matrix) == nullptr) {
        if (animation->interpInfo->typeID == UVAnimParamInfo().typeID) {
            RpUVAnimParamKeyFrameDataInitFromMatrix(&keyFrame->data.param, matrix);
        }
    }
    return nullptr;
}
