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
#include "rpmatfx.h"
#include "X87Math.h"

// Funciones propias del modulo sin nombre en el SDK (nombres inferidos del desensamblado); solo se enganchan, por eso se declaran aqui.
RwStream* _rpUVAnimMaterialStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RpUVAnimMaterialExtension* _rpUVAnimMaterialUpdateMatrices(RpUVAnimMaterialExtension* extension);
RpUVAnim* _rpUVAnimDefaultAnimCreate(const RwChar* name);

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

// Funciones del EXE (por direccion fija) que quedan fuera del lote: constructores, destructores y copia de los plugins, lectura y tamano del flujo del material y callbacks de aplicacion de fotogramas.
constexpr uintptr_t kEngineCtor   = 0x7CB9B0; // constructor del plugin de motor (crea la lista libre y registra los dos esquemas de interpolacion)
constexpr uintptr_t kEngineDtor   = 0x7CBA20;
constexpr uintptr_t kMaterialCtor = 0x7CBA80; // pone a cero 10 dwords en la extension
constexpr uintptr_t kMaterialDtor = 0x7CBAA0;
constexpr uintptr_t kMaterialCopy = 0x7CBB30;
constexpr uintptr_t kMaterialRead = 0x7CBC20;
constexpr uintptr_t kMaterialSize = 0x7CBF20;
constexpr uintptr_t kLinearApply  = 0x7CC9F0; // RpUVAnimLinearKeyFrameApply
constexpr uintptr_t kParamApply   = 0x7CC560; // RpUVAnimParamKeyFrameApply
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

// 0x7CB940 (lote R6A, sin validar; cdecl, sin argumentos, `ret`, 111 bytes): registra el plugin 0x135. Motor: RwEngineRegisterPlugin(0, 0x135, ctor 0x7CB9B0, dtor 0x7CBA20) (si < 0 devuelve FALSE sin seguir);
// material: RpMaterialRegisterPlugin(0x28, 0x135, ctor 0x7CBA80, dtor 0x7CBAA0, copia 0x7CBB30), cuyo resultado se guarda SIEMPRE en 0xC9B8D0 antes de registrar el flujo RpMaterialRegisterPluginStream(0x135,
// lectura 0x7CBC20, escritura 0x7CBB70, tamano 0x7CBF20) (si < 0 devuelve FALSE); si el desplazamiento guardado es negativo tambien FALSE; si no, TRUE. Los callbacks se pasan por direccion fija como en el EXE
// (0x7CBB70 queda enganchado a la version en C++).
RwBool RpUVAnimPluginAttach() {
    if (RwEngineRegisterPlugin(0, kUVAnimPluginID, reinterpret_cast<RwPluginObjectConstructor>(kEngineCtor), reinterpret_cast<RwPluginObjectDestructor>(kEngineDtor)) < 0) {
        return FALSE;
    }
    UVAnimMaterialOffset() = RpMaterialRegisterPlugin(0x28, kUVAnimPluginID, reinterpret_cast<RwPluginObjectConstructor>(kMaterialCtor), reinterpret_cast<RwPluginObjectDestructor>(kMaterialDtor),
                                                      reinterpret_cast<RwPluginObjectCopy>(kMaterialCopy));
    if (RpMaterialRegisterPluginStream(kUVAnimPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(kMaterialRead), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x7CBB70),
                                       reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(kMaterialSize)) < 0) {
        return FALSE;
    }
    return UVAnimMaterialOffset() < 0 ? FALSE : TRUE;
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

RpUVAnim* RpUVAnimAddRef(RpUVAnim* anim) {
    return ((RpUVAnim*(__cdecl *)(RpUVAnim*))0x7CC100)(anim);
}

const RwChar* RpUVAnimGetName(const RpUVAnim* anim) {
    return ((const RwChar*(__cdecl *)(const RpUVAnim*))0x7CC3A0)(anim);
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

RpUVAnimInterpolator* RpMaterialUVAnimGetInterpolator(RpMaterial* material, RwUInt32 slot) {
    return ((RpUVAnimInterpolator*(__cdecl *)(RpMaterial*, RwUInt32))0x7CC430)(material, slot);
}

RpMaterial* RpMaterialUVAnimSetInterpolator(RpMaterial* material, RpUVAnimInterpolator* interp, RwUInt32 slot) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RpUVAnimInterpolator*, RwUInt32))0x7CC450)(material, interp, slot);
}

RpMaterial* RpMaterialUVAnimSetCurrentTime(RpMaterial* material, RwReal time) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwReal))0x7CC470)(material, time);
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

RpMaterial* RpMaterialUVAnimSubAnimTime(RpMaterial* material, RwReal deltaTime) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwReal))0x7CC4F0)(material, deltaTime);
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
            if (applyCallBack == reinterpret_cast<RtAnimKeyFrameApplyCallBack>(kLinearApply)) {
                localBits[0] = fields[0];  // right.x = uv[0]
                localBits[1] = fields[1];  // right.y = uv[1]
                localBits[4] = fields[2];  // up.x = uv[2]
                localBits[5] = fields[3];  // up.y = uv[3]
                localBits[12] = fields[4]; // pos.x = uv[4]
                localBits[13] = fields[5]; // pos.y = uv[5]
                RwMatrixUpdate(&local);
            } else if (applyCallBack == reinterpret_cast<RtAnimKeyFrameApplyCallBack>(kParamApply)) {
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
