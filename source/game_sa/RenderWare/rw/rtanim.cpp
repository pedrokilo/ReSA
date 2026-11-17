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
} // namespace

void RtAnimAnimationFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7CCC80)(blockSize, numBlocksToPrealloc);
}

// 0x7CCCA0: registra el plugin de motor de RtAnim con RwEngineRegisterPlugin (0x7F2BB0): tamano 0, id 0x1B7, constructor 0x7CCCD0 (crea la lista libre de animaciones con los
// parametros de 0x7CCC80) y destructor 0x7CCD10 (pone a 0 el contador de esquemas y destruye la lista libre); ambos por direccion fija (no reconstruidos). Devuelve (resultado > 0), con signo.
RwBool RtAnimInitialize() {
    const RwInt32 result = RwEngineRegisterPlugin(0, kRtAnimPluginID,
        reinterpret_cast<RwPluginObjectConstructor>(0x7CCCD0), reinterpret_cast<RwPluginObjectDestructor>(0x7CCD10));
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

RtAnimInterpolatorInfo* RtAnimGetInterpolatorInfo(RwInt32 typeID) {
    return ((RtAnimInterpolatorInfo*(__cdecl *)(RwInt32))0x7CCDE0)(typeID);
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

RwBool RtAnimAnimationWrite(const RtAnimAnimation* animation, const RwChar* filename) {
    return ((RwBool(__cdecl *)(const RtAnimAnimation*, const RwChar*))0x7CD160)(animation, filename);
}

RtAnimAnimation* RtAnimAnimationStreamRead(RwStream* stream) {
    return ((RtAnimAnimation*(__cdecl *)(RwStream*))0x7CD220)(stream);
}

RwBool RtAnimAnimationStreamWrite(const RtAnimAnimation* animation, RwStream* stream) {
    return ((RwBool(__cdecl *)(const RtAnimAnimation*, RwStream*))0x7CD410)(animation, stream);
}

RwInt32 RtAnimAnimationStreamGetSize(const RtAnimAnimation* animation) {
    return ((RwInt32(__cdecl *)(const RtAnimAnimation*))0x7CD4D0)(animation);
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

RwBool RtAnimInterpolatorSetKeyFrameCallBacks(RtAnimInterpolator* anim, RwInt32 keyFrameTypeID) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RwInt32))0x7CD660)(anim, keyFrameTypeID);
}

void RtAnimInterpolatorSetAnimLoopCallBack(RtAnimInterpolator* anim, RtAnimCallBack callBack, void* data) {
    ((void(__cdecl *)(RtAnimInterpolator*, RtAnimCallBack, void*))0x7CD6F0)(anim, callBack, data);
}

void RtAnimInterpolatorSetAnimCallBack(RtAnimInterpolator* anim, RtAnimCallBack callBack, RwReal time, void* data) {
    ((void(__cdecl *)(RtAnimInterpolator*, RtAnimCallBack, RwReal, void*))0x7CD710)(anim, callBack, time, data);
}

RwBool RtAnimInterpolatorCopy(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RtAnimInterpolator*))0x7CD730)(outAnim, inAnim);
}

RwBool RtAnimInterpolatorSubAnimTime(RtAnimInterpolator* anim, RwReal time) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RwReal))0x7CD760)(anim, time);
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

RwBool RtAnimInterpolatorSetCurrentTime(RtAnimInterpolator* anim, RwReal time) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RwReal))0x7CDAB0)(anim, time);
}

RwBool RtAnimAnimationMakeDelta(RtAnimAnimation* animation, RwInt32 numNodes, RwReal time) {
    return ((RwBool(__cdecl *)(RtAnimAnimation*, RwInt32, RwReal))0x7CDB00)(animation, numNodes, time);
}

RwBool RtAnimInterpolatorBlend(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim1, RtAnimInterpolator* inAnim2, RwReal alpha) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RtAnimInterpolator*, RtAnimInterpolator*, RwReal))0x7CDBF0)(outAnim, inAnim1, inAnim2, alpha);
}

RwBool RtAnimInterpolatorAddTogether(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim1, RtAnimInterpolator* inAnim2) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RtAnimInterpolator*, RtAnimInterpolator*))0x7CDC50)(outAnim, inAnim1, inAnim2);
}

RtAnimInterpolator* RtAnimInterpolatorCreateSubInterpolator(RtAnimInterpolator* parentAnim, RwInt32 startNode, RwInt32 numNodes, RwInt32 maxInterpKeyFrameSize) {
    return ((RtAnimInterpolator*(__cdecl *)(RtAnimInterpolator*, RwInt32, RwInt32, RwInt32))0x7CDCB0)(parentAnim, startNode, numNodes, maxInterpKeyFrameSize);
}

RwBool RtAnimInterpolatorBlendSubInterpolator(RtAnimInterpolator* outAnim, RtAnimInterpolator* inAnim1, RtAnimInterpolator* inAnim2, RwReal alpha) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RtAnimInterpolator*, RtAnimInterpolator*, RwReal))0x7CDCF0)(outAnim, inAnim1, inAnim2, alpha);
}

RwBool RtAnimInterpolatorAddSubInterpolator(RtAnimInterpolator* outAnim, RtAnimInterpolator* mainAnim, RtAnimInterpolator* subAnim) {
    return ((RwBool(__cdecl *)(RtAnimInterpolator*, RtAnimInterpolator*, RtAnimInterpolator*))0x7CDEF0)(outAnim, mainAnim, subAnim);
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
}
