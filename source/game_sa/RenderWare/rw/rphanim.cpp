/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpHAnim
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rphanim.h"

namespace {
// Plugin RpHAnim: id 0x11E; las reservas usan la pista 0x3011E (0x11E | rwMEMHINTDUR_EVENT).
constexpr RwUInt32 kHAnimPluginID = 0x11E;
constexpr RwUInt32 kHAnimMemHint  = 0x3011E;
constexpr RwUInt32 kMatrixIdentityFlags = 0x20003; // rwMATRIXTYPEORTHONORMAL | rwMATRIXINTERNALIDENTITY

// Variables globales del modulo en el EXE (0xC9B8C8: desplazamiento del dato de plugin en los marcos; 0xC9B8CC: lista libre de jerarquias).
RpHAnimAtomicGlobalVars& HAnimGlobals() { return *reinterpret_cast<RpHAnimAtomicGlobalVars*>(0xC9B8C8); }

RwGlobals& EngineGlobals() { return **reinterpret_cast<RwGlobals**>(0xC97B24); }

static_assert(sizeof(RtAnimInterpolator) == 0x4C && offsetof(RtAnimInterpolator, currentInterpKeyFrameSize) == 0x24 && offsetof(RtAnimInterpolator, keyFrameApplyCB) == 0x3C);
static_assert(offsetof(RpHAnimHierarchy, pMatrixArray) == 0x8 && offsetof(RpHAnimHierarchy, pNodeInfo) == 0x10 && offsetof(RpHAnimHierarchy, parentFrame) == 0x14 &&
              offsetof(RpHAnimHierarchy, parentHierarchy) == 0x18 && offsetof(RpHAnimHierarchy, rootParentOffset) == 0x1C && offsetof(RpHAnimHierarchy, currentAnim) == 0x20);
static_assert(sizeof(RpHAnimNodeInfo) == 0x10 && offsetof(RpHAnimNodeInfo, flags) == 8 && offsetof(RpHAnimNodeInfo, pFrame) == 0xC);
static_assert(offsetof(RwFrame, modelling) == 0x10 && offsetof(RwFrame, ltm) == 0x50 && offsetof(RwFrame, root) == 0xA0 && offsetof(RwFrame, inDirtyListLink) == 0x8);
static_assert(offsetof(RwGlobals, dirtyFrameList) == 0xBC && sizeof(RwMatrix) == 0x40);

// Matriz identidad como la monta el EXE: tres vectores unitarios, posicion 0 y flags = (basura de la pila) | 0x20003 (aqui, 0x20003 sin mas bits); los huecos de relleno
// quedan sin escribir en el EXE (aqui a 0).
void SetIdentity(RwMatrix& m) {
    m = RwMatrix{};
    m.right.x = 1.0f;
    m.up.y    = 1.0f;
    m.at.z    = 1.0f;
    m.flags   = kMatrixIdentityFlags;
}

// Matriz del padre del marco `frame` (frame->object.parent) en espacio global: identidad sin padre; con la jerarquia sucia (RwFrameDirty, 0x7F0340) se acumula multiplicando
// las matrices de modelado de la cadena (RwMatrixMultiply, 0x7F18B0); si no, copia la LTM (RwFrameGetLTM, 0x7F0990).
void ParentWorldMatrix(RwFrame* frame, RwMatrix& out) {
    RwFrame* parent = static_cast<RwFrame*>(frame->object.parent);
    if (parent == nullptr) {
        SetIdentity(out);
        return;
    }
    if (RwFrameDirty(parent)) {
        out = parent->modelling;
        for (RwFrame* up = static_cast<RwFrame*>(parent->object.parent); up != nullptr; up = static_cast<RwFrame*>(up->object.parent)) {
            const RwMatrix previous = out;
            RwMatrixMultiply(&out, &previous, &up->modelling);
        }
    } else {
        out = *RwFrameGetLTM(parent);
    }
}

// Alta del marco raiz de la jerarquia en la lista de marcos sucios del motor (insercion a la cabeza de RwGlobals::dirtyFrameList) y bit 2 de su indicador privado, salvo
// que ya tenga alguno de los bits 0..1.
void MarkRootDirty(RpHAnimHierarchy* hierarchy) {
    RwFrame* const root = hierarchy->parentFrame->root;
    if ((root->object.privateFlags & 3) == 0) {
        RwLLLink* const head = &EngineGlobals().dirtyFrameList.link;
        root->inDirtyListLink.next = head->next;
        root->inDirtyListLink.prev = head;
        head->next->prev = &root->inDirtyListLink;
        head->next = &root->inDirtyListLink;
        root->object.privateFlags |= 2;
    }
}

// Marca la LTM del marco como sincronizada: privateFlags = (privateFlags & ~4) | 8.
void MarkFrameLtmSynced(RwFrame* frame) {
    frame->object.privateFlags = static_cast<RwUInt8>((frame->object.privateFlags & 0xFB) | 0x08);
}
} // namespace

void RpHAnimInjectHooks() {
    RH_ScopedNamespaceName("RpHAnim");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RpHAnimFrameGetHierarchy, 0x7C5160);
    // Lote R4A (sin validar)
    RH_ScopedGlobalInstall(RpHAnimPluginAttach, 0x7C4600);
    RH_ScopedGlobalInstall(RpHAnimHierarchyCreate, 0x7C4C30);
    RH_ScopedGlobalInstall(RpHAnimHierarchyGetMatrixArray, 0x7C5120);
    RH_ScopedGlobalInstall(RpHAnimIDGetIndex, 0x7C51A0);
    RH_ScopedGlobalInstall(RpHAnimHierarchyUpdateMatrices, 0x7C51D0);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameStreamRead, 0x7C64C0);
}

void RpHAnimHierarchySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7C45E0)(blockSize, numBlocksToPrealloc);
}

// 0x7C4C30: crea una jerarquia. Orden exacto del EXE: (1) RwFreeListAlloc de la lista 0xC9B8CC (pista 0x3011E), sin comprobar el resultado; (2) RtAnimInterpolatorCreate
// (0x7CD520) -> currentAnim; (3) flags, numNodes, parentFrame = NULL; (4) si flags no tiene NOMATRICES (2): RwMalloc(numNodes*64 + 15) -> pMatrixArrayUnaligned (+0x0C) y
// pMatrixArray (+0x08) alineado a 16 con `& ~15` del puntero + 15; con NOMATRICES ambos a NULL; (5) RwMalloc(numNodes*16) -> pNodeInfo; (6) por nodo: pFrame = NULL, nodeID = nodeIDs[i]
// SOLO si nodeIDs != NULL, nodeIndex = i, flags = nodeFlags[i] SOLO si nodeFlags != NULL (sin escribir se queda lo que traiga el bloque); (7) parentHierarchy = la propia jerarquia.
RpHAnimHierarchy* RpHAnimHierarchyCreate(RwInt32 numNodes, RwUInt32* nodeFlags, RwInt32* nodeIDs, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) {
    auto* hierarchy = static_cast<RpHAnimHierarchy*>(RwFreeListAlloc(HAnimGlobals().HAnimFreeList, kHAnimMemHint));
    hierarchy->currentAnim = RtAnimInterpolatorCreate(numNodes, maxInterpKeyFrameSize);
    hierarchy->flags       = flags;
    hierarchy->numNodes    = numNodes;
    hierarchy->parentFrame = nullptr;
    if (!(flags & rpHANIMHIERARCHYNOMATRICES)) {
        void* const block = RwMalloc(static_cast<RwUInt32>(numNodes) * 64u + 15u, kHAnimMemHint);
        hierarchy->pMatrixArrayUnaligned = block;
        hierarchy->pMatrixArray = reinterpret_cast<RwMatrix*>((reinterpret_cast<uintptr_t>(block) + 15u) & ~static_cast<uintptr_t>(15u));
    } else {
        hierarchy->pMatrixArray = nullptr;
        hierarchy->pMatrixArrayUnaligned = nullptr;
    }
    hierarchy->pNodeInfo = static_cast<RpHAnimNodeInfo*>(RwMalloc(static_cast<RwUInt32>(numNodes) * 16u, kHAnimMemHint));
    for (RwInt32 i = 0; i < numNodes; ++i) {
        RpHAnimNodeInfo& info = hierarchy->pNodeInfo[i];
        info.pFrame = nullptr;
        if (nodeIDs != nullptr) {
            info.nodeID = nodeIDs[i];
        }
        info.nodeIndex = i;
        if (nodeFlags != nullptr) {
            info.flags = static_cast<RwInt32>(nodeFlags[i]);
        }
    }
    hierarchy->parentHierarchy = hierarchy;
    return hierarchy;
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

// 0x7C5120: `mov eax,[esp+4]; mov eax,[eax+8]; ret`.
RwMatrix* RpHAnimHierarchyGetMatrixArray(RpHAnimHierarchy* hierarchy) {
    return hierarchy->pMatrixArray;
}

// NOTSA
RwMatrix* RpHAnimHierarchyGetNodeMatrix(RpHAnimHierarchy* hierarchy, RwInt32 nodeID) {
    const auto i = RpHAnimIDGetIndex(hierarchy, nodeID);
    assert(i >= 0 && i < hierarchy->numNodes);
    return &RpHAnimHierarchyGetMatrixArray(hierarchy)[i];
}

// 0x7C51D0: recalcula las matrices de la jerarquia a partir de los fotogramas interpolados de currentAnim. Flags usados: SUBHIERARCHY 0x1, NOMATRICES 0x2, UPDATEMODELLINGMATRICES 0x1000,
// UPDATELTMS 0x2000, LOCALSPACEMATRICES 0x4000; flags de nodo (nodeInfo[i].flags & 3): 0 nada, 1 pop, 2 push, 3 push+pop. Siempre devuelve TRUE.
// El EXE trae `RpHAnimKeyFrameApply` (0x7C5B80) incrustada cuando el callback de la animacion es esa direccion; aqui se llama siempre al callback (con 0x7C5B80 es el mismo codigo).
// Diferencias asumidas con el EXE: los flags de las matrices identidad que no se escriben (basura de pila | 0x20003) salen como 0x20003, y los huecos de relleno de las matrices
// temporales (+0x1C, +0x2C, +0x3C), que el EXE copia sin inicializar, salen a 0. Las pilas de matrices/punteros del EXE son de 30 matrices y ~540 punteros; aqui 64 y 512.
RwBool RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy* hierarchy) {
    const RwUInt32 flags = static_cast<RwUInt32>(hierarchy->flags);
    RtAnimInterpolator* const anim = hierarchy->currentAnim;
    const RwInt32 keyFrameStride = anim->currentInterpKeyFrameSize;
    const RtAnimKeyFrameApplyCallBack applyCB = anim->keyFrameApplyCB;
    RwUInt8* keyFrame = reinterpret_cast<RwUInt8*>(anim) + sizeof(RtAnimInterpolator);
    const RwUInt32 updateLTMs = flags & rpHANIMHIERARCHYUPDATELTMS;
    const RwUInt32 updateModelling = flags & rpHANIMHIERARCHYUPDATEMODELLINGMATRICES;

    if (flags & rpHANIMHIERARCHYNOMATRICES) {
        // Sin matrices propias: cada nodo se multiplica por la matriz "actual" parentWorld (pila de copias de matrices).
        RwMatrix parentWorld{};
        ParentWorldMatrix(hierarchy->parentFrame, parentWorld);
        if (updateLTMs) {
            MarkRootDirty(hierarchy);
        }

        RwMatrix matrixStack[64];
        RwMatrix* stackTop = matrixStack;
        const RpHAnimNodeInfo* info = hierarchy->pNodeInfo;
        for (RwInt32 i = 0; i < hierarchy->numNodes; ++i, ++info, keyFrame += keyFrameStride) {
            RwMatrix local{};
            applyCB(&local, keyFrame);

            RwMatrix world{};
            RwMatrixMultiply(&world, &local, &parentWorld);

            if (RwFrame* const frame = info->pFrame) {
                bool syncLtm = false;
                if (updateModelling) {
                    frame->modelling = local;
                    if (updateLTMs) {
                        syncLtm = true;
                    } else {
                        RwFrameUpdateObjects(frame);
                    }
                } else if (updateLTMs) {
                    syncLtm = true;
                }
                if (syncLtm) {
                    frame->ltm = world;
                    MarkFrameLtmSynced(frame);
                }
            }

            switch (info->flags & 3) {
            case 0:
                parentWorld = world;
                break;
            case 1:
                --stackTop;
                parentWorld = *stackTop;
                break;
            case 2:
                *stackTop = parentWorld;
                ++stackTop;
                parentWorld = world;
                break;
            default: // 3: ni apila ni desapila
                break;
            }
        }
        return TRUE;
    }

    // Con matrices propias: pMatrixArray[i] = local * (*base), `base` apunta a la matriz del nodo padre (pila de punteros).
    RwMatrix identity{};
    RwMatrix worldOfParent{};
    RwMatrix* base;
    RwFrame* parentFrameForWorld = nullptr;
    bool computeParentWorld = true;
    if ((flags & rpHANIMHIERARCHYSUBHIERARCHY) && hierarchy->rootParentOffset != -1) {
        base = &hierarchy->parentHierarchy->pMatrixArray[hierarchy->rootParentOffset];
        if ((flags & rpHANIMHIERARCHYUPDATELTMS) && (flags & rpHANIMHIERARCHYLOCALSPACEMATRICES)) {
            parentFrameForWorld = hierarchy->parentHierarchy->parentFrame;
        } else {
            computeParentWorld = false;
        }
    } else if (flags & rpHANIMHIERARCHYLOCALSPACEMATRICES) {
        SetIdentity(identity);
        base = &identity;
        if (updateLTMs) {
            parentFrameForWorld = hierarchy->parentFrame;
        } else {
            computeParentWorld = false;
        }
    } else {
        base = &worldOfParent;
        parentFrameForWorld = hierarchy->parentFrame;
    }
    if (computeParentWorld) {
        if (parentFrameForWorld == nullptr || parentFrameForWorld->object.parent == nullptr) {
            SetIdentity(worldOfParent);
        } else {
            ParentWorldMatrix(parentFrameForWorld, worldOfParent);
        }
    }
    if (updateLTMs) {
        MarkRootDirty(hierarchy);
    }

    RwMatrix* pointerStack[512];
    RwMatrix** stackTop = pointerStack;
    RwMatrix* matrix = hierarchy->pMatrixArray;
    const RpHAnimNodeInfo* info = hierarchy->pNodeInfo;
    for (RwInt32 i = 0; i < hierarchy->numNodes; ++i, ++info, ++matrix, keyFrame += keyFrameStride) {
        RwMatrix local{};
        applyCB(&local, keyFrame);
        RwMatrixMultiply(matrix, &local, base);

        if (RwFrame* const frame = info->pFrame) {
            bool syncLtm = false;
            if (updateModelling) {
                frame->modelling = local;
                if (updateLTMs) {
                    syncLtm = true;
                } else {
                    RwFrameUpdateObjects(frame);
                }
            } else if (updateLTMs) {
                syncLtm = true;
            }
            if (syncLtm) {
                if (flags & rpHANIMHIERARCHYLOCALSPACEMATRICES) {
                    RwMatrixMultiply(&frame->ltm, matrix, &worldOfParent);
                } else {
                    frame->ltm = *matrix;
                }
                MarkFrameLtmSynced(frame);
            }
        }

        switch (info->flags & 3) {
        case 0:
            base = matrix;
            break;
        case 1:
            --stackTop;
            base = *stackTop;
            break;
        case 2:
            *stackTop = base;
            ++stackTop;
            base = matrix;
            break;
        default: // 3
            break;
        }
    }
    return TRUE;
}

// 0x7C51A0: posicion (contador del bucle, no nodeIndex) del primer nodo cuyo nodeID es `ID`; -1 si no esta o no hay nodos.
RwInt32 RpHAnimIDGetIndex(RpHAnimHierarchy* hierarchy, RwInt32 ID) {
    const RpHAnimNodeInfo* info = hierarchy->pNodeInfo;
    for (RwInt32 i = 0; i < hierarchy->numNodes; ++i, ++info) {
        if (info->nodeID == ID) {
            return i;
        }
    }
    return -1;
}

// 0x7C4600: registra el plugin 0x11E. Motor: RwEngineRegisterPlugin(0, 0x11E, ctor 0x7C4670, dtor 0x7C4720); si el resultado es < 0 devuelve FALSE sin seguir. Marcos:
// RwFrameRegisterPlugin(8, 0x11E, ctor 0x7C4750, dtor 0x7C4770, copia 0x7C4830) -> 0xC9B8C8 (se guarda aunque sea negativo); RwFrameRegisterPluginStream(0x11E, lectura 0x7C4A00,
// escritura 0x7C48D0, tamano 0x7C4BF0): resultado < 0 -> FALSE; despues, offset en 0xC9B8C8 < 0 -> FALSE; si no, TRUE. Las rutinas del plugin se pasan por direccion fija.
RwBool RpHAnimPluginAttach() {
    if (RwEngineRegisterPlugin(0, kHAnimPluginID,
            reinterpret_cast<RwPluginObjectConstructor>(0x7C4670), reinterpret_cast<RwPluginObjectDestructor>(0x7C4720)) < 0) {
        return FALSE;
    }
    HAnimGlobals().engineOffset = RwFrameRegisterPlugin(8, kHAnimPluginID,
        reinterpret_cast<RwPluginObjectConstructor>(0x7C4750), reinterpret_cast<RwPluginObjectDestructor>(0x7C4770), reinterpret_cast<RwPluginObjectCopy>(0x7C4830));
    if (RwFrameRegisterPluginStream(kHAnimPluginID,
            reinterpret_cast<RwPluginDataChunkReadCallBack>(0x7C4A00), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x7C48D0), reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x7C4BF0)) < 0) {
        return FALSE;
    }
    if (HAnimGlobals().engineOffset < 0) {
        return FALSE;
    }
    return TRUE;
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

// 0x7C64C0: lee los fotogramas de la animacion. Por cada uno (numFrames se relee del objeto en cada vuelta): RwStreamReadReal(stream, &frame->time, 0x20) (tiempo, cuaternion y traslacion; 8 floats a
// partir de +4) y RwStreamReadInt32(stream, &desplazamiento, 4); prevFrame = pFrames + (desplazamiento / 36) * 36 bytes (division SIN signo, via multiplicacion por 0x38E38E39). Cualquier lectura
// fallida (resultado NULL) devuelve NULL sin deshacer nada; si no, devuelve la animacion.
RtAnimAnimation* RpHAnimKeyFrameStreamRead(RwStream* stream, RtAnimAnimation* animation) {
    auto* const frames = static_cast<RwUInt8*>(animation->pFrames);
    for (RwInt32 i = 0; i < animation->numFrames; ++i) {
        auto* const frame = reinterpret_cast<RpHAnimKeyFrame*>(frames + static_cast<size_t>(i) * sizeof(RpHAnimKeyFrame));
        if (RwStreamReadReal(stream, &frame->time, 0x20) == nullptr) {
            return nullptr;
        }
        RwInt32 offset;
        if (RwStreamReadInt32(stream, &offset, 4) == nullptr) {
            return nullptr;
        }
        frame->prevFrame = reinterpret_cast<RpHAnimKeyFrame*>(frames + static_cast<size_t>(static_cast<RwUInt32>(offset) / 36u) * 36u);
    }
    return animation;
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
