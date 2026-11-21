/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RtAnim
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpdbgerr.h"
#include "rtanim.h"
#include "X87Math.h"

static_assert(sizeof(RtAnimInterpolatorInfo) == 0x30 && offsetof(RtAnimInterpolatorInfo, interpKeyFrameSize) == 0x04 && offsetof(RtAnimInterpolatorInfo, animKeyFrameSize) == 0x08 && offsetof(RtAnimInterpolatorInfo, customDataSize) == 0x2C);
static_assert(sizeof(RtAnimAnimation) == 0x18 && offsetof(RtAnimAnimation, duration) == 0x0C && offsetof(RtAnimAnimation, pFrames) == 0x10 && offsetof(RtAnimAnimation, customData) == 0x14);
static_assert(sizeof(RtAnimInterpolator) == 0x4C && offsetof(RtAnimInterpolator, animCallBackTime) == 0x14 && offsetof(RtAnimInterpolator, maxInterpKeyFrameSize) == 0x20 && offsetof(RtAnimInterpolator, numNodes) == 0x2C);
static_assert(offsetof(RtAnimInterpolator, parentAnimation) == 0x38 && offsetof(RtAnimInterpolator, keyFrameInterpolateCB) == 0x44 && sizeof(RtAnimInterpFrameHeader) == 8);

namespace {
// Tabla de esquemas de interpolacion registrados del EXE: 16 RtAnimInterpolatorInfo de 0x30 bytes en 0xC9B920 y su contador en 0xC97900 (la lista libre de animaciones
// ocupa el dword siguiente a la tabla, 0xC9BC20).
constexpr RwInt32 kMaxInterpolatorInfos = 16;
RtAnimInterpolatorInfo* InterpolatorInfos() { return reinterpret_cast<RtAnimInterpolatorInfo*>(0xC9B920); }
RwInt32& NumInterpolatorInfos() { return *reinterpret_cast<RwInt32*>(0xC97900); }

// Plugin de motor de RtAnim: id 0x1B7; los errores se emiten con ese mismo identificador (RWERROR del SDK: pluginID literal del EXE, errorCode = _rwerror(codigo)).
constexpr RwInt32 kRtAnimPluginID = 0x1B7;
// Pistas de memoria del EXE para animaciones e interpoladores: 0x1B | rwMEMHINTDUR_EVENT (0x30000).
constexpr RwUInt32 kRtAnimMemHint = 0x3001B;

void RaiseRtAnimError(RwInt32 code) {
    RwError error;
    error.pluginID  = kRtAnimPluginID;
    error.errorCode = _rwerror(code);
    RwErrorSet(&error);
}

// Parametros de la lista libre de animaciones: dos dwords del EXE (0x8DEDEC = entradas por bloque, 0x8DEDF0 = bloques a reservar de antemano) y la lista libre en 0xC9BC20 (lista en 0xC9B8E0).
RwInt32& FreeListBlockSize() { return *reinterpret_cast<RwInt32*>(0x8DEDEC); }
RwInt32& FreeListNumBlocks() { return *reinterpret_cast<RwInt32*>(0x8DEDF0); }
RwFreeList*& AnimationFreeList() { return *reinterpret_cast<RwFreeList**>(0xC9BC20); }
} // namespace


static void* RtAnimEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x7CCCD0
static void* RtAnimEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);  // 0x7CCD10

// 0x7CCC80 (lote S5A, sin validar): guarda los dos parametros en los globales 0x8DEDEC (blockSize) y 0x8DEDF0 (numBlocksToPrealloc).
void RtAnimAnimationFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    FreeListBlockSize() = blockSize;
    FreeListNumBlocks() = numBlocksToPrealloc;
}

// 0x7CCCD0 (lote S5A, sin validar): constructor del plugin de motor 0x1B7. Crea la lista libre de animaciones, RwFreeListCreateAndPreallocateSpace(0x18, [0x8DEDEC], 4, [0x8DEDF0], lista en
// 0xC9B8E0, pista 0x4001B), y la guarda en 0xC9BC20 (el EXE lee primero el segundo global y luego el primero). Si falla devuelve NULL; si no, devuelve el objeto recibido.
static void* RtAnimEngineConstructor(void* object, RwInt32, RwInt32) {
    const RwInt32 numBlocks = FreeListNumBlocks();
    const RwInt32 blockSize = FreeListBlockSize();
    AnimationFreeList() = RwFreeListCreateAndPreallocateSpace(0x18, blockSize, 4, numBlocks, reinterpret_cast<RwFreeList*>(0xC9B8E0), 0x4001B);
    if (AnimationFreeList() == nullptr) {
        return nullptr;
    }
    return object;
}

// 0x7CCD10 (lote S5A, sin validar): destructor del plugin de motor 0x1B7. Pone a 0 el contador de esquemas (0xC97900); si hay lista libre la destruye y deja el puntero en 0; devuelve el objeto.
static void* RtAnimEngineDestructor(void* object, RwInt32, RwInt32) {
    NumInterpolatorInfos() = 0;
    if (AnimationFreeList() != nullptr) {
        RwFreeListDestroy(AnimationFreeList());
        AnimationFreeList() = nullptr;
    }
    return object;
}

// 0x7CCCA0: registra el plugin de motor de RtAnim con RwEngineRegisterPlugin (0x7F2BB0): tamano 0, id 0x1B7, constructor 0x7CCCD0 (crea la lista libre de animaciones con los
// parametros de 0x7CCC80) y destructor 0x7CCD10 (pone a 0 el contador de esquemas y destruye la lista libre); desde el lote S5A son funciones propias. Devuelve (resultado > 0), con signo.
RwBool RtAnimInitialize() {
    const RwInt32 result = RwEngineRegisterPlugin(0, kRtAnimPluginID, RtAnimEngineConstructor, RtAnimEngineDestructor);
    return result > 0;
}

// 0x7CCD40: alta de un esquema de interpolacion en la tabla global (0xC9B920, maximo 16). Con la tabla llena: RWERROR(1) y FALSE; con el mismo typeID ya registrado: RWERROR(0) y FALSE
// (el EXE mira primero el limite, luego busca el typeID); si no, copia los 0x30 bytes (12 dwords) al final de la tabla, incrementa el contador y devuelve TRUE.
RwBool RtAnimRegisterInterpolationScheme(RtAnimInterpolatorInfo* interpolatorInfo) {
    const RwInt32 count = NumInterpolatorInfos();
    if (count >= kMaxInterpolatorInfos) {
        RaiseRtAnimError(1);
        return FALSE;
    }
    const RwInt32 typeID = interpolatorInfo->typeID;
    for (RwInt32 i = 0; i < count; ++i) {
        if (InterpolatorInfos()[i].typeID == typeID) {
            RaiseRtAnimError(0);
            return FALSE;
        }
    }
    memcpy(&InterpolatorInfos()[count], interpolatorInfo, sizeof(RtAnimInterpolatorInfo));
    NumInterpolatorInfos() = count + 1;
    return TRUE;
}

// 0x7CCDE0 (lote S5A, sin validar): busca el esquema por typeID en la tabla (0xC9B920, contador 0xC97900); si no esta: RWERROR(2) y NULL.
RtAnimInterpolatorInfo* RtAnimGetInterpolatorInfo(RwInt32 typeID) {
    const RwInt32 count = NumInterpolatorInfos();
    for (RwInt32 i = 0; i < count; ++i) {
        if (InterpolatorInfos()[i].typeID == typeID) {
            return &InterpolatorInfos()[i];
        }
    }
    RaiseRtAnimError(2);
    return nullptr;
}

// 0x7CCE40: busca el esquema por typeID en la tabla (0xC9B920); si no esta: RWERROR(2) y NULL. Reserva animKeyFrameSize * numFrames + customDataSize + 0x18 bytes
// (RwMalloc del motor, pista 0x3001B; el EXE no comprueba el resultado ni inicializa los fotogramas) y rellena la cabecera: interpInfo, numFrames, flags, duration (copiado como dword),
// pFrames = justo tras la cabecera y customData = tras los fotogramas si customDataSize > 0 (con signo), o NULL.
RtAnimAnimation* RtAnimAnimationCreate(RwInt32 typeID, RwInt32 numFrames, RwInt32 flags, RwReal duration) {
    const RwInt32 count = NumInterpolatorInfos();
    RwInt32 index = 0;
    while (index < count && InterpolatorInfos()[index].typeID != typeID) {
        ++index;
    }
    if (index >= count) {
        RaiseRtAnimError(2);
        return nullptr;
    }
    RtAnimInterpolatorInfo* info = &InterpolatorInfos()[index];
    auto* animation = static_cast<RtAnimAnimation*>(RwMalloc(static_cast<RwUInt32>(info->animKeyFrameSize * numFrames + info->customDataSize + static_cast<RwInt32>(sizeof(RtAnimAnimation))), kRtAnimMemHint));
    animation->flags      = flags;
    animation->numFrames  = numFrames;
    animation->duration   = duration;
    animation->interpInfo = info;
    animation->pFrames    = animation + 1;
    if (info->customDataSize > 0) {
        animation->customData = static_cast<RwUInt8*>(animation->pFrames) + info->animKeyFrameSize * numFrames;
    } else {
        animation->customData = nullptr;
    }
    return animation;
}

// 0x7CCF10: RwFree del motor (RwEngineInstance + 0x138) sobre la animacion y devuelve TRUE.
RwBool RtAnimAnimationDestroy(RtAnimAnimation* animation) {
    RwFree(animation);
    return TRUE;
}

// 0x7CCF30 (lote R7A, sin validar; 0x7CCF6C..0x7CD144 es la misma funcion, el desensamblador la parte en dos): abre el archivo (rwSTREAMFILENAME, rwSTREAMREAD) y busca el chunk 0x1B. Si no se abre o
// no hay chunk devuelve NULL SIN cerrar el flujo (el EXE no lo cierra en esos dos casos). Despues lee (RwStreamReadInt32/Real, 4 bytes cada uno): version (debe ser 0x100), typeID, numFrames, flags y
// duracion. Busca el esquema por typeID (RWERROR(2) si no esta) y crea la animacion (el EXE inlina RtAnimAnimationCreate: una segunda busqueda, reserva sin comprobar y cabecera como la de esa
// funcion), llama a interpInfo->keyFrameStreamReadCB(stream, animacion) sin mirar su resultado y devuelve la animacion. Cualquier fallo de lectura o de busqueda tras el chunk deja la animacion en NULL;
// en todos esos casos se cierra el flujo con RwStreamClose(stream, 0) antes de devolver.
RtAnimAnimation* RtAnimAnimationRead(const RwChar* filename) {
    RwStream* const stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (stream == nullptr) {
        return nullptr;
    }
    if (!RwStreamFindChunk(stream, 0x1B, nullptr, nullptr)) {
        return nullptr;
    }
    RtAnimAnimation* animation = nullptr;
    do {
        RwInt32 version;
        if (RwStreamReadInt32(stream, &version, sizeof(RwInt32)) == nullptr || version != 0x100) {
            break;
        }
        RwInt32 typeID;
        if (RwStreamReadInt32(stream, &typeID, sizeof(RwInt32)) == nullptr) {
            break;
        }
        const RwInt32 count = NumInterpolatorInfos();
        RwInt32 index = 0;
        while (index < count && InterpolatorInfos()[index].typeID != typeID) {
            ++index;
        }
        if (index >= count) {
            RaiseRtAnimError(2);
            break;
        }
        RtAnimInterpolatorInfo* const info = &InterpolatorInfos()[index];
        RwInt32 numFrames;
        if (RwStreamReadInt32(stream, &numFrames, sizeof(RwInt32)) == nullptr) {
            break;
        }
        RwInt32 flags;
        if (RwStreamReadInt32(stream, &flags, sizeof(RwInt32)) == nullptr) {
            break;
        }
        RwReal duration;
        if (RwStreamReadReal(stream, &duration, sizeof(RwReal)) == nullptr) {
            break;
        }
        animation = RtAnimAnimationCreate(typeID, numFrames, flags, duration);
        info->keyFrameStreamReadCB(stream, animation);
    } while (false);
    RwStreamClose(stream, nullptr);
    return animation;
}

namespace {
constexpr RwUInt32 kRtAnimStreamVersion = 0x36003; // push 36003h
constexpr RwUInt32 kRtAnimStreamBuild   = 0xFFFF;  // push 0FFFFh
constexpr RwInt32  kRtAnimChunkType     = 0x1B;
constexpr RwInt32  kRtAnimFileVersion   = 0x100;

// Cuerpo comun de escritura de RtAnimAnimationWrite / RtAnimAnimationStreamWrite tras abrir el flujo: cabecera versionada del chunk 0x1B (tamano = getSizeCB + 0x14), version 0x100, y los
// cuatro campos typeID (primer dword de la informacion de interpolacion), numFrames, flags y duracion (RwStreamWriteReal). Con el primer fallo devuelve FALSE; si todo va bien llama a
// interpInfo->keyFrameStreamWriteCB(animation, stream) sin mirar su resultado y devuelve TRUE.
RwBool WriteAnimationBody(const RtAnimAnimation* animation, RwStream* stream) {
    const RwInt32 version = kRtAnimFileVersion;
    const RwInt32 size    = animation->interpInfo->keyFrameStreamGetSizeCB(animation) + 0x14;
    _rwStreamWriteVersionedChunkHeader(stream, kRtAnimChunkType, size, kRtAnimStreamVersion, kRtAnimStreamBuild);
    if (!RwStreamWriteInt32(stream, &version, 4)) {
        return FALSE;
    }
    if (!RwStreamWriteInt32(stream, &animation->interpInfo->typeID, 4)) {
        return FALSE;
    }
    if (!RwStreamWriteInt32(stream, &animation->numFrames, 4)) {
        return FALSE;
    }
    if (!RwStreamWriteInt32(stream, &animation->flags, 4)) {
        return FALSE;
    }
    if (!RwStreamWriteReal(stream, &animation->duration, 4)) {
        return FALSE;
    }
    animation->interpInfo->keyFrameStreamWriteCB(animation, stream);
    return TRUE;
}
} // namespace

// 0x7CD160 (lote S6A, sin validar): abre el archivo para escribir (rwSTREAMFILENAME = 2, rwSTREAMWRITE = 2); si no se abre devuelve FALSE. Escribe el chunk (WriteAnimationBody: un fallo de
// escritura salta directamente al cierre) y SIEMPRE cierra con RwStreamClose(stream, 0) y devuelve TRUE (aunque la escritura haya fallado).
RwBool RtAnimAnimationWrite(const RtAnimAnimation* animation, const RwChar* filename) {
    RwStream* const stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, filename);
    if (stream == nullptr) {
        return FALSE;
    }
    WriteAnimationBody(animation, stream);
    RwStreamClose(stream, nullptr);
    return TRUE;
}

// 0x7CD220 (lote S6A, sin validar): lee de un flujo ya abierto: version (RwStreamReadInt32, 4 bytes; debe ser 0x100), typeID, busqueda del esquema (RWERROR(2) y NULL si no esta), numFrames,
// flags y duracion (RwStreamReadReal). Cualquier fallo devuelve NULL sin cerrar el flujo. Crea la animacion como RtAnimAnimationCreate (el EXE lo inlina, con una segunda busqueda) y llama a
// interpInfo->keyFrameStreamReadCB(stream, animacion) sin mirar su resultado; devuelve la animacion.
RtAnimAnimation* RtAnimAnimationStreamRead(RwStream* stream) {
    RwInt32 version;
    if (!RwStreamReadInt32(stream, &version, 4)) {
        return nullptr;
    }
    if (version != kRtAnimFileVersion) {
        return nullptr;
    }
    RwInt32 typeID;
    if (!RwStreamReadInt32(stream, &typeID, 4)) {
        return nullptr;
    }
    if (RtAnimGetInterpolatorInfo(typeID) == nullptr) {
        return nullptr;
    }
    RwInt32 numFrames;
    if (!RwStreamReadInt32(stream, &numFrames, 4)) {
        return nullptr;
    }
    RwInt32 flags;
    if (!RwStreamReadInt32(stream, &flags, 4)) {
        return nullptr;
    }
    RwReal duration;
    if (!RwStreamReadReal(stream, &duration, 4)) {
        return nullptr;
    }
    RtAnimInterpolatorInfo* const info = RtAnimGetInterpolatorInfo(typeID);
    if (info == nullptr) {
        return nullptr;
    }
    RtAnimAnimation* const animation = RtAnimAnimationCreate(typeID, numFrames, flags, duration);
    info->keyFrameStreamReadCB(stream, animation);
    return animation;
}

// 0x7CD410 (lote S6A, sin validar): como RtAnimAnimationWrite sobre un flujo ya abierto y sin cerrarlo; devuelve FALSE en el primer fallo de escritura y TRUE si llega al final.
RwBool RtAnimAnimationStreamWrite(const RtAnimAnimation* animation, RwStream* stream) {
    return WriteAnimationBody(animation, stream);
}

// 0x7CD4D0 (lote S6A, sin validar): interpInfo->keyFrameStreamGetSizeCB(animation) + 0x14.
RwInt32 RtAnimAnimationStreamGetSize(const RtAnimAnimation* animation) {
    return animation->interpInfo->keyFrameStreamGetSizeCB(animation) + 0x14;
}

// 0x7CD4F0: cuenta los nodos de la animacion recorriendo las cabeceras de fotogramas clave (paso = interpInfo->animKeyFrameSize, +8 de la info):
// si la primera cabecera ya apunta (prevFrame, primer dword) a pFrames devuelve 0; si no avanza de cabecera en cabecera contando hasta que la
// cabecera alcanzada vuelva a apuntar a pFrames. Lecturas de dwords sin comprobar limites (con paso 0 y primera cabecera distinta no termina, como el EXE).
RwUInt32 RtAnimAnimationGetNumNodes(const RtAnimAnimation* animation) {
    const uint32 first = reinterpret_cast<uintptr_t>(animation->pFrames);
    const uint32 step = static_cast<uint32>(animation->interpInfo->animKeyFrameSize);
    RwUInt32 count = 0;
    uint32 current = first;
    if (*reinterpret_cast<const uint32*>(current) != first) {
        uint32 previousOfNext;
        do {
            previousOfNext = *reinterpret_cast<const uint32*>(current + step);
            current += step;
            ++count;
        } while (previousOfNext != first);
    }
    return count;
}

// 0x7CD520: reserva numNodes * maxInterpKeyFrameSize + 0x4C bytes (RwMalloc del motor, pista 0x3001B, sin comprobar el resultado) y deja la cabecera sin animacion:
// numNodes, currentInterpKeyFrameSize = maxInterpKeyFrameSize, todo a 0 salvo animCallBackTime = -1.0f, currentAnimKeyFrameSize = -1, isSubInterpolator = 0, offsetInParent = 0 y parentAnimation = el propio interpolador.
RtAnimInterpolator* RtAnimInterpolatorCreate(RwInt32 numNodes, RwInt32 maxInterpKeyFrameSize) {
    auto* interpolator = static_cast<RtAnimInterpolator*>(RwMalloc(static_cast<RwUInt32>(numNodes * maxInterpKeyFrameSize + static_cast<RwInt32>(sizeof(RtAnimInterpolator))), kRtAnimMemHint));
    interpolator->numNodes                  = numNodes;
    interpolator->currentInterpKeyFrameSize = maxInterpKeyFrameSize;
    interpolator->maxInterpKeyFrameSize     = maxInterpKeyFrameSize;
    interpolator->pCurrentAnim              = nullptr;
    interpolator->pNextFrame                = nullptr;
    interpolator->currentTime               = 0.0f;
    interpolator->pAnimCallBack             = nullptr;
    interpolator->animCallBackTime          = -1.0f;
    interpolator->pAnimCallBackData         = nullptr;
    interpolator->pAnimLoopCallBack         = nullptr;
    interpolator->pAnimLoopCallBackData     = nullptr;
    interpolator->currentAnimKeyFrameSize   = -1;
    interpolator->isSubInterpolator         = FALSE;
    interpolator->offsetInParent            = 0;
    interpolator->parentAnimation           = interpolator;
    interpolator->keyFrameApplyCB           = nullptr;
    interpolator->keyFrameInterpolateCB     = nullptr;
    interpolator->keyFrameBlendCB           = nullptr;
    interpolator->keyFrameAddCB             = nullptr;
    return interpolator;
}

// 0x7CD590: `jmp [RwEngineInstance + 0x138]` con el mismo argumento, es decir RwFree del motor (memoryFuncs.rwfree) sobre la propia interpolacion.
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwfree) == 0x138);
// 0x7CD590
void RtAnimInterpolatorDestroy(RtAnimInterpolator* anim) {
    RwFree(anim);
}
 
/**
 * \ingroup rtanim
 * \ref RtAnimInterpolatorSetCurrentAnim sets the current animation on the
 * animation interpolator. It is assumed that the animation is designed for the
 * animation interpolator it is being set on since no animation interpolator structure
 * comparisons are made. The animation defines the interpolation schemes used
 * and they will be setup on the animation interpolator at this call. The maximum
 * keyframe size of the animation interpolator must be sufficient to support the
 * keyframe size required by the interpolation scheme. The animation is
 * initialized to time zero at this call.
 *
 * \param animI Pointer to the \ref RtAnimInterpolator.
 * \param anim Pointer to the \ref RtAnimAnimation.
 *
 * \return \ref RwBool, TRUE on success, FALSE if an error occurs.
 *
 *
 * @addr 0x7CD5A0
 */
RwBool
RtAnimInterpolatorSetCurrentAnim(RtAnimInterpolator *animI,
                               RtAnimAnimation *anim)
{
    RtAnimInterpolatorInfo     *interpInfo;
    RwInt32                     i;
    void                       *kf1, *kf2, *interpFrame;

    RWAPIFUNCTION(RWSTRING("RtAnimInterpolatorSetCurrentAnim"));
    RWASSERT(animI);
    RWASSERT(anim);
    RWASSERT(anim->numFrames >= animI->numNodes * 2);

    animI->pCurrentAnim = anim;
    animI->currentTime = 0.0f;

    /* Get interpolator functions */
    interpInfo = anim->interpInfo;
    animI->currentInterpKeyFrameSize = interpInfo->interpKeyFrameSize;
    RWASSERT(animI->currentInterpKeyFrameSize <= animI->maxInterpKeyFrameSize);
    animI->currentAnimKeyFrameSize = interpInfo->animKeyFrameSize;

    animI->keyFrameApplyCB = interpInfo->keyFrameApplyCB;
    animI->keyFrameBlendCB = interpInfo->keyFrameBlendCB;
    animI->keyFrameInterpolateCB = interpInfo->keyFrameInterpolateCB;
    animI->keyFrameAddCB = interpInfo->keyFrameAddCB;

    /*
     * Set up initial interpolation frames for time=0
     * First copy the time=0 keyframes to the interpolated frame array
     */
    //TODO: convert these don't copy
    for (i=0; i < animI->numNodes; i++)
    {
        animI->keyFrameInterpolateCB(rtANIMGETINTERPFRAME(animI, i),
                                     ((RwUInt8 *)anim->pFrames) + (i * animI->currentAnimKeyFrameSize),
                                     ((RwUInt8 *)anim->pFrames) + ((i + animI->numNodes) * animI->currentAnimKeyFrameSize),
                                     0.0f, anim->customData);
    }

    /*
     * Now initialize the interpolated frame headers to point to the initial
     * keyframe pairs.
     */
    { // Como el original: también con las animaciones de RpAnimBlend (el EXE pisa sus frames interpolados)
        /*
         * In RW there is an RtAnimAnimation, which holds the raw keyframes of an animation.
         * To play an animation, an RtAnimAnimation is given to an RtAnimInterpolator which
         * then interpolates the keyframes into interpolation frames which are then used to
         * animate e.g. a hierarchy by generating matrices from them.
         * But R* hasn't really used RW's animation system since III.
         * In III and VC R* wrote their own interpolated animation data into a dummy
         * RtAnimAnimation and practically didn't use the RtAnimInterpolator.
         * In SA they went a step further and now write into the interpolation frames of
         * the RtAnimInterpolator instead, almost completely avoid RtAnimAnimation.
         * Almost! They're still attaching an RtAnimAnimation to an RtAnimInterpolator
         * and this is where stuff breaks.
         *
         * (SilentPatch skips this for RpAnimBlend animations; we keep the exe behaviour and overwrite them.)
         *
         * For more info see: https://gtaforums.com/topic/669045-silentpatch/page/154/#comment-1068971599
         */

        interpFrame = rtANIMGETINTERPFRAME(animI, 0);
        kf1 = anim->pFrames;
        kf2 = (RwInt8 *)anim->pFrames + animI->numNodes * animI->currentAnimKeyFrameSize;

        for (i=0; i < animI->numNodes; i++)
        {
            RtAnimInterpFrameHeader  *hdr = (RtAnimInterpFrameHeader *)interpFrame;

            hdr->keyFrame1 = (RtAnimKeyFrameHeader*)kf1;
            hdr->keyFrame2 = (RtAnimKeyFrameHeader*)kf2;
           
            interpFrame = (RwInt8 *)interpFrame + animI->currentInterpKeyFrameSize;
            kf1 = (RwInt8 *)kf1 + animI->currentAnimKeyFrameSize;
            kf2 = (RwInt8 *)kf2 + animI->currentAnimKeyFrameSize;
        }
    }
    animI->pNextFrame = (RwUInt8 *)anim->pFrames + (animI->numNodes * animI->currentAnimKeyFrameSize * 2);

    RWRETURN(TRUE);
}

// 0x7CD660 (lote S6A, sin validar): busca el esquema keyFrameTypeID (RWERROR(2) y FALSE si no esta) y copia a la interpolacion los callbacks apply (+0x0C -> +0x3C), blend (+0x10 -> +0x40),
// interpolate (+0x14 -> +0x44) y add (+0x18 -> +0x48) como dwords. No toca los tamanos de fotograma. Devuelve TRUE.
RwBool RtAnimInterpolatorSetKeyFrameCallBacks(RtAnimInterpolator* anim, RwInt32 keyFrameTypeID) {
    const RtAnimInterpolatorInfo* const info = RtAnimGetInterpolatorInfo(keyFrameTypeID);
    if (info == nullptr) {
        return FALSE;
    }
    anim->keyFrameApplyCB       = info->keyFrameApplyCB;
    anim->keyFrameBlendCB       = info->keyFrameBlendCB;
    anim->keyFrameInterpolateCB = info->keyFrameInterpolateCB;
    anim->keyFrameAddCB         = info->keyFrameAddCB;
    return TRUE;
}

// 0x7CD6F0 (lote S6A, sin validar): pAnimLoopCallBack (+0x18) = callBack; pAnimLoopCallBackData (+0x1C) = data.
void RtAnimInterpolatorSetAnimLoopCallBack(RtAnimInterpolator* anim, RtAnimCallBack callBack, void* data) {
    anim->pAnimLoopCallBack     = callBack;
    anim->pAnimLoopCallBackData = data;
}

// 0x7CD710 (lote S6A, sin validar): pAnimCallBack (+0x0C) = callBack; pAnimCallBackData (+0x10) = data; animCallBackTime (+0x14) = time (copiado como dword).
void RtAnimInterpolatorSetAnimCallBack(RtAnimInterpolator* anim, RtAnimCallBack callBack, RwReal time, void* data) {
    anim->pAnimCallBack     = callBack;
    anim->pAnimCallBackData = data;
    memcpy(&anim->animCallBackTime, &time, sizeof(RwUInt32));
}

// 0x7CD730 (lote S6A, sin validar): copia los fotogramas interpolados de inAnim a outAnim (desde +0x4C): outAnim->numNodes * outAnim->currentInterpKeyFrameSize bytes (rep movsd + rep movsb).
// Devuelve TRUE.
RwBool RtAnimInterpolatorCopy(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim) {
    const RwUInt32 bytes = static_cast<RwUInt32>(outAnim->numNodes * outAnim->currentInterpKeyFrameSize);
    memcpy(outAnim + 1, inAnim + 1, bytes);
    return TRUE;
}

// 0x7CD760 (lote S6A, sin validar): retrocede el tiempo de la animacion. Aritmetica en float (x87 a 24 bits). Comparaciones del EXE (fnstsw + test ah):
//   disparo puntual (fired): currentTime > animCallBackTime (ordenado) y (currentTime - time) <= animCallBackTime (ordenado); con NaN no se dispara.
//   newTime = currentTime - time se guarda en currentTime. Si newTime < 0 (ordenado; con 0, positivo o NaN va a la otra rama): suma la duracion hasta que deje de ser < 0, guarda el tiempo,
//   reinicia con RtAnimInterpolatorSetCurrentAnim(anim, pCurrentAnim) y avanza con RtAnimInterpolatorAddAnimTime(anim, tiempo corregido); looped = TRUE y fired |= currentTime < animCallBackTime.
//   Si no: por cada nodo retrocede keyFrame1/keyFrame2 mientras currentTime < keyFrame1->time (ordenado) y llama a keyFrameInterpolateCB(frame, kf1, kf2, currentTime, customData); pNextFrame = 0.
// Despues dispara pAnimCallBack(anim, data) si fired (la anula si devuelve NULL) y, si looped, pAnimLoopCallBack(anim, data) (idem). Devuelve TRUE.
RwBool RtAnimInterpolatorSubAnimTime(RtAnimInterpolator* anim, RwReal time) {
    bool looped = false;
    bool fired  = false;
    if (anim->currentTime > anim->animCallBackTime) {
        fired = (anim->currentTime - time) <= anim->animCallBackTime;
    }

    RtAnimAnimation* const currentAnim = anim->pCurrentAnim;
    const RwReal newTime = anim->currentTime - time;
    anim->currentTime = newTime;
    if (newTime < 0.0f) {
        RwReal corrected = anim->currentTime;
        const RwReal duration = currentAnim->duration;
        do {
            corrected = corrected + duration;
        } while (corrected < 0.0f);
        anim->currentTime = corrected;
        RtAnimInterpolatorSetCurrentAnim(anim, currentAnim);
        RtAnimInterpolatorAddAnimTime(anim, corrected);
        looped = true;
        if (anim->currentTime < anim->animCallBackTime) {
            fired = true;
        }
    } else {
        const RwInt32 numNodes = anim->numNodes;
        if (numNodes != 0) {
            auto* frame = reinterpret_cast<RtAnimInterpFrameHeader*>(anim + 1);
            RwInt32 remaining = numNodes;
            do {
                while (anim->currentTime < frame->keyFrame1->time) {
                    RtAnimKeyFrameHeader* const old = frame->keyFrame1;
                    frame->keyFrame2 = old;
                    frame->keyFrame1 = *reinterpret_cast<RtAnimKeyFrameHeader**>(old);
                }
                anim->keyFrameInterpolateCB(frame, frame->keyFrame1, frame->keyFrame2, anim->currentTime, anim->pCurrentAnim->customData);
                frame = reinterpret_cast<RtAnimInterpFrameHeader*>(reinterpret_cast<uint8*>(frame) + anim->currentInterpKeyFrameSize);
            } while (--remaining != 0);
        }
        anim->pNextFrame = nullptr;
    }

    if (fired && anim->pAnimCallBack) {
        if (!anim->pAnimCallBack(anim, anim->pAnimCallBackData)) {
            anim->pAnimCallBack = nullptr;
        }
    }
    if (looped && anim->pAnimLoopCallBack) {
        if (!anim->pAnimLoopCallBack(anim, anim->pAnimLoopCallBackData)) {
            anim->pAnimLoopCallBack = nullptr;
        }
    }
    return TRUE;
}

// 0x7CD8D0: avanza el tiempo de la animacion. Con time <= 0 (o NaN) no hace nada y devuelve TRUE. Si no: dispara la devolucion de llamada puntual (callbackFired) cuando
// currentTime < animCallBackTime y currentTime + time >= animCallBackTime; guarda currentTime += time y, si supera la duracion, resta la duracion las veces que haga falta (acumulando callbackFired
// si algun valor intermedio alcanza animCallBackTime), marca el bucle (looped) y reinicia con RtAnimInterpolatorSetCurrentAnim(anim, pCurrentAnim) restaurando el tiempo restante.
// Sin pNextFrame tambien reinicia (conservando el tiempo). Luego avanza pNextFrame por los fotogramas con prevFrame->time <= currentTime: en cada paso busca (desde el nodo siguiente al ultimo, con vuelta a 0) el
// fotograma interpolado cuyo keyFrame2 es prevFrame, le pone keyFrame1 = keyFrame2 y keyFrame2 = pNextFrame (sin limite de busqueda, como el EXE). Despues llama a keyFrameInterpolateCB(interp, kf1, kf2, currentTime,
// customData) por nodo, a pAnimCallBack(anim, data) si se disparo (lo anula si devuelve NULL) y a pAnimLoopCallBack(anim, data) si hubo bucle (lo anula si devuelve NULL). Siempre devuelve TRUE.
// Aritmetica en float: la x87 del EXE (24 bits) redondea cada operacion igual que un float; las comparaciones son ordenadas (falsas con NaN), como `fnstsw + test ah,41h`.
RwBool RtAnimInterpolatorAddAnimTime(RtAnimInterpolator* anim, RwReal time) {
    if (!(time > 0.0f)) {
        return TRUE;
    }
    RtAnimAnimation* const currentAnim = anim->pCurrentAnim;
    const RwReal callbackTime = anim->animCallBackTime;
    const RwReal duration     = currentAnim->duration;
    const RwInt32 numNodes    = anim->numNodes;
    bool looped = false;

    bool callbackFired = false;
    if (anim->currentTime < callbackTime) {
        callbackFired = (time + anim->currentTime) >= callbackTime;
    }

    RwReal newTime = time + anim->currentTime;
    anim->currentTime = newTime;
    if (newTime > duration) {
        looped = true;
        do {
            newTime = newTime - duration;
            callbackFired |= (newTime >= callbackTime);
        } while (newTime > duration);
        anim->currentTime = newTime;
        RtAnimInterpolatorSetCurrentAnim(anim, currentAnim);
        anim->currentTime = newTime;
    }

    if (!anim->pNextFrame) {
        const RwReal keptTime = anim->currentTime;
        RtAnimInterpolatorSetCurrentAnim(anim, currentAnim);
        anim->currentTime = keptTime;
    }

    uint8* const endOfFrames = static_cast<uint8*>(currentAnim->pFrames) + currentAnim->numFrames * anim->currentAnimKeyFrameSize;
    RwInt32 nodeIndex = 0;
    while (static_cast<uint8*>(anim->pNextFrame) < endOfFrames) {
        auto* const prevFrame = *reinterpret_cast<RtAnimKeyFrameHeader**>(anim->pNextFrame);
        if (prevFrame->time > anim->currentTime) {
            break;
        }
        RtAnimInterpFrameHeader* interpFrame;
        do {
            interpFrame = reinterpret_cast<RtAnimInterpFrameHeader*>(reinterpret_cast<uint8*>(anim + 1) + anim->currentInterpKeyFrameSize * nodeIndex);
            ++nodeIndex;
            if (nodeIndex >= numNodes) {
                nodeIndex = 0;
            }
        } while (interpFrame->keyFrame2 != prevFrame);
        interpFrame->keyFrame1 = interpFrame->keyFrame2;
        interpFrame->keyFrame2 = static_cast<RtAnimKeyFrameHeader*>(anim->pNextFrame);
        anim->pNextFrame = static_cast<uint8*>(anim->pNextFrame) + anim->currentAnimKeyFrameSize;
    }

    uint8* interpFrame = reinterpret_cast<uint8*>(anim + 1);
    for (RwInt32 node = numNodes; node > 0; --node) {
        auto* const header = reinterpret_cast<RtAnimInterpFrameHeader*>(interpFrame);
        anim->keyFrameInterpolateCB(interpFrame, header->keyFrame1, header->keyFrame2, anim->currentTime, anim->pCurrentAnim->customData);
        interpFrame += anim->currentInterpKeyFrameSize;
    }

    if (callbackFired && anim->pAnimCallBack) {
        if (!anim->pAnimCallBack(anim, anim->pAnimCallBackData)) {
            anim->pAnimCallBack = nullptr;
        }
    }
    if (looped && anim->pAnimLoopCallBack) {
        if (!anim->pAnimLoopCallBack(anim, anim->pAnimLoopCallBackData)) {
            anim->pAnimLoopCallBack = nullptr;
        }
    }
    return TRUE;
}

namespace {
// Fotograma interpolado `index` de una interpolacion: justo tras la cabecera (+0x4C), de currentInterpKeyFrameSize bytes cada uno (sin comprobar limites, como el EXE).
void* InterpFrame(RtAnimInterpolator* anim, RwInt32 index) {
    return reinterpret_cast<uint8*>(anim + 1) + anim->currentInterpKeyFrameSize * index;
}
} // namespace

// 0x7CDAB0 (lote S6A, sin validar): diferencia = time - currentTime (float). Si diferencia < 0 (ordenado) llama a RtAnimInterpolatorSubAnimTime(anim, -diferencia); si no (incluidos 0 y NaN)
// a RtAnimInterpolatorAddAnimTime(anim, diferencia). Devuelve TRUE.
RwBool RtAnimInterpolatorSetCurrentTime(RtAnimInterpolator* anim, RwReal time) {
    const RwReal difference = time - anim->currentTime;
    if (difference < 0.0f) {
        RtAnimInterpolatorSubAnimTime(anim, -difference);
    } else {
        RtAnimInterpolatorAddAnimTime(anim, difference);
    }
    return TRUE;
}

// 0x7CDB00 (lote S6A, sin validar): crea una interpolacion temporal (RtAnimInterpolatorCreate(numNodes, interpInfo->interpKeyFrameSize); NULL -> FALSE), le pone la animacion con
// RtAnimInterpolatorSetCurrentAnim y la lleva al tiempo `time` (diferencia = time - currentTime; < 0 ordenado -> SubAnimTime(-diferencia), si no AddAnimTime). Despues, por nodo, llama a
// interpInfo->keyFrameMulRecipCB(fotograma, fotograma interpolado) y, mientras el tiempo del fotograma no sea EXACTAMENTE igual a la duracion (== ordenado; con NaN tampoco), pasa al
// siguiente fotograma del mismo nodo (el que tiene como anterior al actual; sin limite de busqueda) y repite. Cada nodo avanza currentAnimKeyFrameSize / currentInterpKeyFrameSize.
// Libera la interpolacion con el RwFree del motor y devuelve TRUE.
RwBool RtAnimAnimationMakeDelta(RtAnimAnimation* animation, RwInt32 numNodes, RwReal time) {
    RtAnimInterpolator* const interpolator = RtAnimInterpolatorCreate(numNodes, animation->interpInfo->interpKeyFrameSize);
    if (interpolator == nullptr) {
        return FALSE;
    }
    RtAnimInterpolatorSetCurrentAnim(interpolator, animation);
    const RwReal difference = time - interpolator->currentTime;
    if (difference < 0.0f) {
        RtAnimInterpolatorSubAnimTime(interpolator, -difference);
    } else {
        RtAnimInterpolatorAddAnimTime(interpolator, difference);
    }

    uint8* keyFrames   = static_cast<uint8*>(animation->pFrames);
    uint8* interpFrame = reinterpret_cast<uint8*>(interpolator + 1);
    for (RwInt32 node = numNodes; node > 0; --node) {
        uint8* frame = keyFrames;
        for (;;) {
            animation->interpInfo->keyFrameMulRecipCB(frame, interpFrame);
            if (*reinterpret_cast<RwReal*>(frame + 4) == animation->duration) {
                break;
            }
            uint8* next = frame;
            if (*reinterpret_cast<uint8**>(frame) != frame) {
                uint8* link;
                do {
                    const RwInt32 step = interpolator->currentAnimKeyFrameSize;
                    link = *reinterpret_cast<uint8**>(next + step);
                    next += step;
                } while (link != frame);
            }
            frame = next;
        }
        interpFrame += interpolator->currentInterpKeyFrameSize;
        keyFrames += interpolator->currentAnimKeyFrameSize;
    }
    RwFree(interpolator);
    return TRUE;
}

// 0x7CDBF0 (lote S6A, sin validar): por cada nodo de outAnim (numNodes se relee tras cada llamada) llama a outAnim->keyFrameBlendCB(salida[i], entrada1[i], entrada2[i], alpha); cada
// fotograma se direcciona con el tamano de SU interpolacion (+0x24). Devuelve TRUE.
RwBool RtAnimInterpolatorBlend(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim1, RtAnimInterpolator* inAnim2, RwReal alpha) {
    for (RwInt32 i = 0; i < outAnim->numNodes; ++i) {
        outAnim->keyFrameBlendCB(InterpFrame(outAnim, i), InterpFrame(inAnim1, i), InterpFrame(inAnim2, i), alpha);
    }
    return TRUE;
}

// 0x7CDC50 (lote S6A, sin validar): por cada nodo de outAnim llama a outAnim->keyFrameAddCB(salida[i], entrada1[i], entrada2[i]). Devuelve TRUE.
RwBool RtAnimInterpolatorAddTogether(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim1, RtAnimInterpolator* inAnim2) {
    for (RwInt32 i = 0; i < outAnim->numNodes; ++i) {
        outAnim->keyFrameAddCB(InterpFrame(outAnim, i), InterpFrame(inAnim1, i), InterpFrame(inAnim2, i));
    }
    return TRUE;
}

// 0x7CDCB0 (lote S6A, sin validar): maxInterpKeyFrameSize == -1 -> el de la interpolacion padre (+0x20). Crea la interpolacion con RtAnimInterpolatorCreate(numNodes, tamano) y le pone
// isSubInterpolator = 1, offsetInParent = startNode y parentAnimation = parentAnim (sin comprobar NULL, como el EXE). Devuelve la nueva interpolacion.
RtAnimInterpolator* RtAnimInterpolatorCreateSubInterpolator(RtAnimInterpolator* parentAnim, RwInt32 startNode, RwInt32 numNodes, RwInt32 maxInterpKeyFrameSize) {
    if (maxInterpKeyFrameSize == -1) {
        maxInterpKeyFrameSize = parentAnim->maxInterpKeyFrameSize;
    }
    RtAnimInterpolator* const sub = RtAnimInterpolatorCreate(numNodes, maxInterpKeyFrameSize);
    sub->parentAnimation   = parentAnim;
    sub->offsetInParent    = startNode;
    sub->isSubInterpolator = TRUE;
    return sub;
}

// 0x7CDCF0 (lote S6A, sin validar): mezcla en el espacio de nodos del padre (offsetInParent). Tres casos segun isSubInterpolator == 1 (comparacion exacta con 1):
//  - outAnim es sub: para cada nodo k de [out.off, out.off + out.numNodes) (numNodes y offset se releen tras cada llamada): outAnim->keyFrameBlendCB(out[k - out.off], in1[k - in1.off], in2[k - in2.off], alpha)
//    con los desplazamientos de in1 e in2 leidos al principio.
//  - si no y inAnim1 es sub: copia in2 -> out para los in1.off primeros nodos (in1.off * out.size bytes), mezcla los nodos [in1.off, in1.off + in1.numNodes) con
//    keyFrameBlendCB(out[k], in1[k - in1.off], in2[k], alpha) y copia el resto (out.numNodes - in1.numNodes - in1.off nodos de tamano out.size; el origen se direcciona con el tamano de in2).
//  - si no: igual con inAnim1 e inAnim2 intercambiados (copia in1 -> out en los in2.off primeros nodos, mezcla [in2.off, ...) con keyFrameBlendCB(out[k], in1[k], in2[k - in2.off], alpha), el resto con el tamano de in1 en el origen).
// Devuelve TRUE.
RwBool RtAnimInterpolatorBlendSubInterpolator(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim1, RtAnimInterpolator* inAnim2, RwReal alpha) {
    if (outAnim->isSubInterpolator == 1) {
        RwInt32 k = outAnim->offsetInParent;
        if (k >= outAnim->numNodes + k) {
            return TRUE;
        }
        const RwInt32 in1Off = inAnim1->offsetInParent;
        const RwInt32 in2Off = inAnim2->offsetInParent;
        const RwInt32 outOff = k;
        do {
            outAnim->keyFrameBlendCB(InterpFrame(outAnim, k - outOff), InterpFrame(inAnim1, k - in1Off), InterpFrame(inAnim2, k - in2Off), alpha);
            ++k;
        } while (k < outAnim->numNodes + outAnim->offsetInParent);
        return TRUE;
    }

    uint8* const outFrames = reinterpret_cast<uint8*>(outAnim + 1);
    if (inAnim1->isSubInterpolator == 1) {
        memcpy(outFrames, inAnim2 + 1, static_cast<RwUInt32>(inAnim1->offsetInParent * outAnim->currentInterpKeyFrameSize));
        for (RwInt32 k = inAnim1->offsetInParent; k < inAnim1->numNodes + inAnim1->offsetInParent; ++k) {
            outAnim->keyFrameBlendCB(InterpFrame(outAnim, k), InterpFrame(inAnim1, k - inAnim1->offsetInParent), InterpFrame(inAnim2, k), alpha);
        }
        const RwInt32 first = inAnim1->offsetInParent + inAnim1->numNodes;
        const RwInt32 rest  = (outAnim->numNodes - inAnim1->numNodes - inAnim1->offsetInParent) * outAnim->currentInterpKeyFrameSize;
        memcpy(outFrames + first * outAnim->currentInterpKeyFrameSize, reinterpret_cast<uint8*>(inAnim2 + 1) + inAnim2->currentInterpKeyFrameSize * first, static_cast<RwUInt32>(rest));
        return TRUE;
    }

    memcpy(outFrames, inAnim1 + 1, static_cast<RwUInt32>(outAnim->currentInterpKeyFrameSize * inAnim2->offsetInParent));
    for (RwInt32 k = inAnim2->offsetInParent; k < inAnim2->numNodes + inAnim2->offsetInParent; ++k) {
        outAnim->keyFrameBlendCB(InterpFrame(outAnim, k), InterpFrame(inAnim1, k), InterpFrame(inAnim2, k - inAnim2->offsetInParent), alpha);
    }
    const RwInt32 first = inAnim2->offsetInParent + inAnim2->numNodes;
    const RwInt32 rest  = (outAnim->numNodes - inAnim2->numNodes - inAnim2->offsetInParent) * outAnim->currentInterpKeyFrameSize;
    memcpy(outFrames + first * outAnim->currentInterpKeyFrameSize, reinterpret_cast<uint8*>(inAnim1 + 1) + inAnim1->currentInterpKeyFrameSize * first, static_cast<RwUInt32>(rest));
    return TRUE;
}

// 0x7CDEF0 (lote S6A, sin validar): para cada nodo i de outAnim (numNodes se relee): outAnim->keyFrameAddCB(salida[base + i], principal[sub.off + i], sub[i]) con base = sub.off si
// outAnim NO es sub (isSubInterpolator == 0) y 0 si lo es. Cada fotograma se direcciona con el tamano de su interpolacion. Devuelve TRUE.
RwBool RtAnimInterpolatorAddSubInterpolator(RtAnimInterpolator* outAnim, RtAnimInterpolator* mainAnim, RtAnimInterpolator* subAnim) {
    RwInt32 base = 0;
    if (outAnim->isSubInterpolator == 0) {
        base = subAnim->offsetInParent;
    }
    for (RwInt32 i = 0; i < outAnim->numNodes; ++i) {
        outAnim->keyFrameAddCB(InterpFrame(outAnim, base + i), InterpFrame(mainAnim, subAnim->offsetInParent + i), InterpFrame(subAnim, i));
    }
    return TRUE;
}

void RtAnim::InjectHooks() {
    RH_ScopedNamespaceName("RtAnim");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtAnimInitialize, 0x7CCCA0);
    RH_ScopedGlobalInstall(RtAnimRegisterInterpolationScheme, 0x7CCD40);
    RH_ScopedGlobalInstall(RtAnimAnimationCreate, 0x7CCE40);
    RH_ScopedGlobalInstall(RtAnimAnimationDestroy, 0x7CCF10);
    RH_ScopedGlobalInstall(RtAnimAnimationRead, 0x7CCF30); // lote R7A (sin validar)
    RH_ScopedGlobalInstall(RtAnimAnimationGetNumNodes, 0x7CD4F0);
    RH_ScopedGlobalInstall(RtAnimInterpolatorCreate, 0x7CD520);
    RH_ScopedGlobalInstall(RtAnimInterpolatorDestroy, 0x7CD590);
    RH_ScopedGlobalInstall(RtAnimInterpolatorSetCurrentAnim, 0x7CD5A0);
    RH_ScopedGlobalInstall(RtAnimInterpolatorAddAnimTime, 0x7CD8D0);
    RH_ScopedGlobalInstall(RtAnimAnimationFreeListCreateParams, 0x7CCC80);          // lote S5A (sin validar)
    RH_ScopedGlobalInstall(RtAnimEngineConstructor, 0x7CCCD0);                      // lote S5A (sin validar)
    RH_ScopedGlobalInstall(RtAnimEngineDestructor, 0x7CCD10);                       // lote S5A (sin validar)
    RH_ScopedGlobalInstall(RtAnimGetInterpolatorInfo, 0x7CCDE0);                    // lote S5A (sin validar)
    RH_ScopedGlobalInstall(RtAnimAnimationWrite, 0x7CD160);                         // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimAnimationStreamRead, 0x7CD220);                    // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimAnimationStreamWrite, 0x7CD410);                   // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimAnimationStreamGetSize, 0x7CD4D0);                 // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorSetKeyFrameCallBacks, 0x7CD660);       // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorSetAnimLoopCallBack, 0x7CD6F0);        // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorSetAnimCallBack, 0x7CD710);            // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorCopy, 0x7CD730);                       // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorSubAnimTime, 0x7CD760);                // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorSetCurrentTime, 0x7CDAB0);             // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimAnimationMakeDelta, 0x7CDB00);                     // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorBlend, 0x7CDBF0);                      // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorAddTogether, 0x7CDC50);                // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorCreateSubInterpolator, 0x7CDCB0);      // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorBlendSubInterpolator, 0x7CDCF0);       // lote S6A (sin validar)
    RH_ScopedGlobalInstall(RtAnimInterpolatorAddSubInterpolator, 0x7CDEF0);         // lote S6A (sin validar)
}
