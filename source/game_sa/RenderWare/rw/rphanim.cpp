/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpHAnim
 * Reconstrucción: COMPLETA — 35/35 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/35 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rphanim.h"
#include "X87Math.h"

// Callbacks del plugin 0x11E, de los marcos y de los esquemas de interpolacion (lotes S10A y S11A; cdecl). Se registran por su direccion propia y llevan hook en la direccion del EXE.
static void*     RpHAnimEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                             // 0x7C4670
static void*     RpHAnimEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                              // 0x7C4720
static void*     RpHAnimFrameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                              // 0x7C4750
static void*     RpHAnimFrameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                               // 0x7C4770
static void*     RpHAnimFrameCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);           // 0x7C4830
static RwStream* RpHAnimFrameStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x7C48D0
static RwStream* RpHAnimFrameStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);       // 0x7C4A00
static RwInt32   RpHAnimFrameStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                      // 0x7C4BF0
static RwFrame*  RpHAnimAttachChildCallBack(RwFrame* frame, void* data);                                                          // 0x7C4F90
static RwFrame*  RpHAnimAttachIndexChildCallBack(RwFrame* frame, void* data);                                                     // 0x7C50A0

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

// Dato del plugin en cada marco (8 bytes en marco + [0xC9B8C8]): identificador del nodo (-1 sin asignar) y jerarquia asociada.
struct HAnimFrameData {
    RwInt32 id;
    RpHAnimHierarchy* hierarchy;
};
static_assert(sizeof(HAnimFrameData) == 8 && offsetof(HAnimFrameData, hierarchy) == 4);

HAnimFrameData& FrameData(const void* frame) {
    return *reinterpret_cast<HAnimFrameData*>(reinterpret_cast<uintptr_t>(frame) + static_cast<uint32>(HAnimGlobals().engineOffset));
}

// Copia nodeID, nodeIndex y flags de cada nodo de `src` a `dst` (numNodes de `dst`, releido en cada vuelta) y pone pFrame a NULL (comun de 0x7C4830 y 0x7C4ED0).
void CopyNodeInfo(RpHAnimHierarchy* dst, const RpHAnimHierarchy* src) {
    for (RwInt32 i = 0; i < dst->numNodes; ++i) {
        dst->pNodeInfo[i].pFrame = nullptr;
        dst->pNodeInfo[i].flags = src->pNodeInfo[i].flags;
        dst->pNodeInfo[i].nodeIndex = src->pNodeInfo[i].nodeIndex;
        dst->pNodeInfo[i].nodeID = src->pNodeInfo[i].nodeID;
    }
}

// Libera las matrices (si no son compartidas ni vacias), la tabla de nodos, la interpolacion y el bloque de la jerarquia (comun de 0x7C4770 y 0x7C4D30). Subjerarquia (flag 1): no libera
// matrices ni nodos; NOMATRICES (flag 2): no libera el bloque de matrices.
void ReleaseHierarchy(RpHAnimHierarchy* hierarchy) {
    const RwUInt32 flags = static_cast<RwUInt32>(hierarchy->flags);
    if (!(flags & rpHANIMHIERARCHYSUBHIERARCHY)) {
        if (!(flags & rpHANIMHIERARCHYNOMATRICES)) {
            RwFree(hierarchy->pMatrixArrayUnaligned);
        }
        RwFree(hierarchy->pNodeInfo);
    }
    RtAnimInterpolator* const currentAnim = hierarchy->currentAnim;
    hierarchy->pMatrixArrayUnaligned = nullptr;
    hierarchy->pMatrixArray = nullptr;
    hierarchy->pNodeInfo = nullptr;
    RtAnimInterpolatorDestroy(currentAnim);
    RwFreeListFree(HAnimGlobals().HAnimFreeList, hierarchy);
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
    // Lotes S10A y S11A (sin validar)
    RH_ScopedGlobalInstall(RpHAnimHierarchySetFreeListCreateParams, 0x7C45E0);
    RH_ScopedGlobalInstall(RpHAnimEngineConstructor, 0x7C4670);
    RH_ScopedGlobalInstall(RpHAnimEngineDestructor, 0x7C4720);
    RH_ScopedGlobalInstall(RpHAnimFrameConstructor, 0x7C4750);
    RH_ScopedGlobalInstall(RpHAnimFrameDestructor, 0x7C4770);
    RH_ScopedGlobalInstall(RpHAnimFrameCopy, 0x7C4830);
    RH_ScopedGlobalInstall(RpHAnimFrameStreamWrite, 0x7C48D0);
    RH_ScopedGlobalInstall(RpHAnimFrameStreamRead, 0x7C4A00);
    RH_ScopedGlobalInstall(RpHAnimFrameStreamGetSize, 0x7C4BF0);
    RH_ScopedGlobalInstall(RpHAnimHierarchyDestroy, 0x7C4D30);
    RH_ScopedGlobalInstall(RpHAnimHierarchyCreateSubHierarchy, 0x7C4DB0);
    RH_ScopedGlobalInstall(RpHAnimHierarchyCreateFromHierarchy, 0x7C4ED0);
    RH_ScopedGlobalInstall(RpHAnimHierarchyAttach, 0x7C4F40);
    RH_ScopedGlobalInstall(RpHAnimAttachChildCallBack, 0x7C4F90);
    RH_ScopedGlobalInstall(RpHAnimHierarchyDetach, 0x7C4FF0);
    RH_ScopedGlobalInstall(RpHAnimHierarchyAttachFrameIndex, 0x7C5020);
    RH_ScopedGlobalInstall(RpHAnimAttachIndexChildCallBack, 0x7C50A0);
    RH_ScopedGlobalInstall(RpHAnimHierarchyDetachFrameIndex, 0x7C5100);
    RH_ScopedGlobalInstall(RpHAnimFrameSetHierarchy, 0x7C5130);
    RH_ScopedGlobalInstall(RpHAnimFrameSetID, 0x7C5170);
    RH_ScopedGlobalInstall(RpHAnimFrameGetID, 0x7C5190);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameApply, 0x7C5B80);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameInterpolate, 0x7C5CA0);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameBlend, 0x7C60C0);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameStreamWrite, 0x7C6540);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameStreamGetSize, 0x7C65B0);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameMulRecip, 0x7C65C0);
    RH_ScopedGlobalInstall(RpHAnimKeyFrameAdd, 0x7C6720);
}

// 0x7C45E0 (lote S10A, sin validar; 20 bytes): guarda el tamano de bloque de la lista libre de jerarquias en 0x8DECD8 y el numero de bloques a reservar en 0x8DECDC.
void RpHAnimHierarchySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    StaticRef<RwInt32>(0x8DECD8) = blockSize;
    StaticRef<RwInt32>(0x8DECDC) = numBlocksToPrealloc;
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

// 0x7C4ED0 (lote S10A, sin validar; cdecl, 3 argumentos, 110 bytes): crea una jerarquia con el mismo numero de nodos (RpHAnimHierarchyCreate(numNodes, NULL, NULL, flags, maxInterpKeyFrameSize), 0x7C4C30; sin comprobar el
// resultado) y copia de la original nodeID, nodeIndex y flags de cada nodo dejando pFrame a NULL. Devuelve la nueva.
RpHAnimHierarchy* RpHAnimHierarchyCreateFromHierarchy(RpHAnimHierarchy* hierarchy, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) {
    RpHAnimHierarchy* const created = RpHAnimHierarchyCreate(hierarchy->numNodes, nullptr, nullptr, flags, maxInterpKeyFrameSize);
    CopyNodeInfo(created, hierarchy);
    return created;
}

// 0x7C4D30 (lote S11A, sin validar; cdecl, 1 argumento, 119 bytes): destruye una jerarquia. Sin el flag de subjerarquia (1) libera el bloque de matrices sin alinear (salvo con NOMATRICES, 2) y la tabla de nodos; pone a NULL
// matrices y tabla; RtAnimInterpolatorDestroy(interpolacion); devuelve el bloque a la lista libre 0xC9B8CC; el dato HAnim del marco padre (parentFrame, leido antes de liberar) queda sin jerarquia si existe. Devuelve NULL.
RpHAnimHierarchy* RpHAnimHierarchyDestroy(RpHAnimHierarchy* hierarchy) {
    RwFrame* const parentFrame = hierarchy->parentFrame;
    ReleaseHierarchy(hierarchy);
    if (parentFrame != nullptr) {
        FrameData(parentFrame).hierarchy = nullptr;
    }
    return nullptr;
}

// 0x7C4DB0 (lote S11A, sin validar; cdecl, 4 argumentos, 274 bytes): crea una subjerarquia que empieza en el nodo startNode. Cuenta los nodos del subarbol (1 + los siguientes hasta que la profundidad, +1 con flag de apilar y -1 con
// flag de desapilar, vuelve a 0; el primer nodo solo se cuenta aparte si tiene el flag de desapilar; sin limite de numNodes del padre). Bloque de la lista libre 0xC9B8CC (pista 0x3011E; sin comprobar), flags = flags | 1,
// numNodes = cuenta, currentAnim = RtAnimInterpolatorCreateSubInterpolator(padre.currentAnim, startNode, cuenta, maxInterpKeyFrameSize) (0x7CDCB0). rootParentOffset: -1 si startNode == 0; si no se recorren los nodos 0..startNode-1
// con una pila de indices (apilar el indice padre actual con el flag 2; el padre pasa a ser el nodo (flag 0) o se desapila (flag 1)). parentFrame = pFrame del nodo de inicio, matrices = las del padre desde startNode (sin bloque
// propio), pNodeInfo = tabla del padre desde startNode, parentHierarchy = padre. La pila del EXE es de 28 enteros en la pila local (mas profundidad pisaria registros guardados); aqui 1024.
RpHAnimHierarchy* RpHAnimHierarchyCreateSubHierarchy(RpHAnimHierarchy* parentHierarchy, RwInt32 startNode, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize) {
    RpHAnimNodeInfo* const startInfo = &parentHierarchy->pNodeInfo[startNode];
    RwInt32 count = 1;
    if (!(startInfo->flags & rpHANIMPOPPARENTMATRIX)) {
        RwInt32 depth = 1;
        const RpHAnimNodeInfo* info = startInfo + 1;
        do {
            const RwUInt32 nodeFlags = static_cast<RwUInt32>(info->flags);
            if (nodeFlags & rpHANIMPUSHPARENTMATRIX) {
                ++depth;
            }
            if (nodeFlags & rpHANIMPOPPARENTMATRIX) {
                --depth;
            }
            ++count;
            ++info;
        } while (depth != 0);
    }
    RpHAnimHierarchy* const sub = static_cast<RpHAnimHierarchy*>(RwFreeListAlloc(HAnimGlobals().HAnimFreeList, kHAnimMemHint));
    sub->flags = static_cast<RwInt32>(static_cast<RwUInt32>(flags) | 1u);
    sub->numNodes = count;
    sub->currentAnim = RtAnimInterpolatorCreateSubInterpolator(parentHierarchy->currentAnim, startNode, count, maxInterpKeyFrameSize);
    if (startNode == 0) {
        sub->rootParentOffset = -1;
    } else {
        RwInt32 parentIndex = -1;
        RwInt32 stack[1024];
        RwInt32* top = stack;
        const RpHAnimNodeInfo* info = parentHierarchy->pNodeInfo;
        for (RwInt32 i = 0; i < startNode; ++i, ++info) {
            const RwUInt32 nodeFlags = static_cast<RwUInt32>(info->flags);
            if (nodeFlags & rpHANIMPUSHPARENTMATRIX) {
                *top++ = parentIndex;
            }
            if (nodeFlags & rpHANIMPOPPARENTMATRIX) {
                parentIndex = *--top;
            } else {
                parentIndex = i;
            }
        }
        sub->rootParentOffset = parentIndex;
    }
    sub->parentFrame = parentHierarchy->pNodeInfo[startNode].pFrame;
    sub->pMatrixArrayUnaligned = nullptr;
    sub->pMatrixArray = parentHierarchy->pMatrixArray + startNode;
    sub->parentHierarchy = parentHierarchy;
    sub->pNodeInfo = parentHierarchy->pNodeInfo + startNode;
    return sub;
}

// 0x7C4F90 (lote S10A, sin validar; cdecl, 2 argumentos, 80 bytes; callback de RwFrameForAllChildren de 0x7C4F40): para cada nodo cuyo nodeID es el identificador HAnim del marco, su pFrame pasa a ser el marco;
// despues RwFrameUpdateObjects(marco) y recursion por los hijos con el mismo callback. Devuelve el marco.
static RwFrame* RpHAnimAttachChildCallBack(RwFrame* frame, void* data) {
    RpHAnimHierarchy* const hierarchy = static_cast<RpHAnimHierarchy*>(data);
    for (RwInt32 i = 0; i < hierarchy->numNodes; ++i) {
        RpHAnimNodeInfo* const info = &hierarchy->pNodeInfo[i];
        if (FrameData(frame).id == info->nodeID) {
            info->pFrame = frame;
        }
    }
    RwFrameUpdateObjects(frame);
    RwFrameForAllChildren(frame, RpHAnimAttachChildCallBack, hierarchy);
    return frame;
}

// 0x7C4F40 (lote S10A, sin validar; cdecl, 1 argumento, 80 bytes): enlaza los marcos con la jerarquia. Los nodos cuyo nodeID coincide con el identificador HAnim del marco raiz (parentFrame) reciben ese marco;
// RwFrameUpdateObjects(raiz) y RwFrameForAllChildren(raiz, 0x7C4F90, jerarquia). Devuelve la jerarquia.
RpHAnimHierarchy* RpHAnimHierarchyAttach(RpHAnimHierarchy* hierarchy) {
    RwFrame* const parentFrame = hierarchy->parentFrame;
    for (RwInt32 i = 0; i < hierarchy->numNodes; ++i) {
        RpHAnimNodeInfo* const info = &hierarchy->pNodeInfo[i];
        if (FrameData(parentFrame).id == info->nodeID) {
            info->pFrame = parentFrame;
        }
    }
    RwFrameUpdateObjects(parentFrame);
    RwFrameForAllChildren(parentFrame, RpHAnimAttachChildCallBack, hierarchy);
    return hierarchy;
}

// 0x7C4FF0 (lote S10A, sin validar; cdecl, 1 argumento, 40 bytes): pone a NULL el pFrame de todos los nodos. Devuelve la jerarquia.
RpHAnimHierarchy* RpHAnimHierarchyDetach(RpHAnimHierarchy* hierarchy) {
    for (RwInt32 i = 0; i < hierarchy->numNodes; ++i) {
        hierarchy->pNodeInfo[i].pFrame = nullptr;
    }
    return hierarchy;
}

// Datos del callback 0x7C50A0 (en la pila del EXE: jerarquia y nodeID buscado).
struct HAnimAttachIndexData {
    RpHAnimHierarchy* hierarchy;
    RwInt32 nodeID;
};

// 0x7C50A0 (lote S10A, sin validar; cdecl, 2 argumentos, 99 bytes; callback de 0x7C5020): si el identificador HAnim del marco es el nodeID buscado, los nodos con ese nodeID reciben el marco; RwFrameUpdateObjects(marco) y
// recursion por los hijos. Devuelve el marco.
static RwFrame* RpHAnimAttachIndexChildCallBack(RwFrame* frame, void* data) {
    HAnimAttachIndexData* const attach = static_cast<HAnimAttachIndexData*>(data);
    if (FrameData(frame).id == attach->nodeID) {
        RpHAnimHierarchy* const hierarchy = attach->hierarchy;
        for (RwInt32 i = 0; i < hierarchy->numNodes; ++i) {
            RpHAnimNodeInfo* const info = &hierarchy->pNodeInfo[i];
            if (info->nodeID == attach->nodeID) {
                info->pFrame = frame;
            }
        }
    }
    RwFrameUpdateObjects(frame);
    RwFrameForAllChildren(frame, RpHAnimAttachIndexChildCallBack, attach);
    return frame;
}

// 0x7C5020 (lote S10A, sin validar; cdecl, 2 argumentos, 119 bytes): como Attach pero solo para el nodo nodeIndex: nodeID = pNodeInfo[nodeIndex].nodeID; si la raiz tiene ese identificador HAnim, los nodos con ese nodeID reciben la
// raiz; RwFrameUpdateObjects(raiz) y RwFrameForAllChildren(raiz, 0x7C50A0, {jerarquia, nodeID}). Devuelve la jerarquia.
RpHAnimHierarchy* RpHAnimHierarchyAttachFrameIndex(RpHAnimHierarchy* hierarchy, RwInt32 nodeIndex) {
    RwFrame* const parentFrame = hierarchy->parentFrame;
    HAnimAttachIndexData attach = {hierarchy, hierarchy->pNodeInfo[nodeIndex].nodeID};
    if (FrameData(parentFrame).id == attach.nodeID) {
        for (RwInt32 i = 0; i < hierarchy->numNodes; ++i) {
            RpHAnimNodeInfo* const info = &hierarchy->pNodeInfo[i];
            if (info->nodeID == attach.nodeID) {
                info->pFrame = parentFrame;
            }
        }
    }
    RwFrameUpdateObjects(parentFrame);
    RwFrameForAllChildren(parentFrame, RpHAnimAttachIndexChildCallBack, &attach);
    return hierarchy;
}

// 0x7C5100 (lote S10A, sin validar; 23 bytes): pFrame del nodo nodeIndex = NULL (sin comprobar el rango). Devuelve la jerarquia.
RpHAnimHierarchy* RpHAnimHierarchyDetachFrameIndex(RpHAnimHierarchy* hierarchy, RwInt32 nodeIndex) {
    hierarchy->pNodeInfo[nodeIndex].pFrame = nullptr;
    return hierarchy;
}

// 0x7C5130 (lote S10A, sin validar; cdecl, 2 argumentos, 47 bytes): asocia una jerarquia al marco. La jerarquia anterior (si habia) pierde su parentFrame (NULL); el dato HAnim del marco guarda la nueva y su parentFrame pasa a ser
// el marco (si no es NULL). Devuelve TRUE.
RwBool RpHAnimFrameSetHierarchy(RwFrame* frame, RpHAnimHierarchy* hierarchy) {
    HAnimFrameData& data = FrameData(frame);
    if (data.hierarchy != nullptr) {
        data.hierarchy->parentFrame = nullptr;
    }
    data.hierarchy = hierarchy;
    if (hierarchy != nullptr) {
        hierarchy->parentFrame = frame;
    }
    return TRUE;
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

// 0x7C4670 (lote S10A, sin validar; cdecl, 3 argumentos, 172 bytes): constructor del plugin de motor 0x11E. Crea la lista libre de jerarquias (RwFreeListCreateAndPreallocateSpace(0x24, [0x8DECD8], 4, [0x8DECDC], espacio
// propio 0xC97054, pista 0x4011E)) y la guarda en 0xC9B8CC (aunque sea NULL); registra el esquema de interpolacion 1 (fotogramas de 0x24 bytes, tanto interpolados como de animacion, con los callbacks de este modulo, sin datos
// de usuario). Devuelve el objeto, o NULL si no se pudo crear la lista.
static void* RpHAnimEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    HAnimGlobals().HAnimFreeList = RwFreeListCreateAndPreallocateSpace(0x24, StaticRef<RwInt32>(0x8DECD8), 4, StaticRef<RwInt32>(0x8DECDC), reinterpret_cast<RwFreeList*>(0xC97054), 0x4011E);
    void* const result = HAnimGlobals().HAnimFreeList != nullptr ? object : nullptr;
    RtAnimInterpolatorInfo info;
    info.typeID                  = 1;
    info.interpKeyFrameSize      = 0x24;
    info.animKeyFrameSize        = 0x24;
    info.keyFrameApplyCB         = RpHAnimKeyFrameApply;
    info.keyFrameBlendCB         = RpHAnimKeyFrameBlend;
    info.keyFrameInterpolateCB   = RpHAnimKeyFrameInterpolate;
    info.keyFrameAddCB           = RpHAnimKeyFrameAdd;
    info.keyFrameMulRecipCB      = RpHAnimKeyFrameMulRecip;
    info.keyFrameStreamReadCB    = RpHAnimKeyFrameStreamRead;
    info.keyFrameStreamWriteCB   = RpHAnimKeyFrameStreamWrite;
    info.keyFrameStreamGetSizeCB = RpHAnimKeyFrameStreamGetSize;
    info.customDataSize          = 0;
    RtAnimRegisterInterpolationScheme(&info);
    return result;
}

// 0x7C4720 (lote S10A, sin validar; cdecl, 3 argumentos, 33 bytes): destructor del plugin de motor: si existe la lista libre (0xC9B8CC) la destruye con RwFreeListDestroy y pone el puntero a NULL. Devuelve el objeto.
static void* RpHAnimEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    if (HAnimGlobals().HAnimFreeList != nullptr) {
        RwFreeListDestroy(HAnimGlobals().HAnimFreeList);
        HAnimGlobals().HAnimFreeList = nullptr;
    }
    return object;
}

// 0x7C4750 (lote S10A, sin validar; cdecl, 3 argumentos, 26 bytes): constructor del dato del marco: jerarquia = NULL e identificador = -1 en marco + [0xC9B8C8] (no usa el desplazamiento recibido). Devuelve el marco.
static void* RpHAnimFrameConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    HAnimFrameData& data = FrameData(object);
    data.hierarchy = nullptr;
    data.id = -1;
    return object;
}

// 0x7C4770 (lote S11A, sin validar; cdecl, 3 argumentos, 188 bytes): destructor del dato del marco. Con jerarquia: pone a NULL el pFrame de todos sus nodos; si su parentFrame es este marco, la libera (ReleaseHierarchy: sin el
// flag de subjerarquia libera matrices y tabla de nodos, destruye la interpolacion y devuelve el bloque a la lista libre) y limpia el dato del marco padre; si es otro marco solo desvincula. Siempre deja jerarquia = NULL e
// identificador = -1. Devuelve el marco.
static void* RpHAnimFrameDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    HAnimFrameData& data = FrameData(object);
    if (RpHAnimHierarchy* const hierarchy = data.hierarchy) {
        for (RwInt32 i = 0; i < hierarchy->numNodes; ++i) {
            hierarchy->pNodeInfo[i].pFrame = nullptr;
        }
        RwFrame* const parentFrame = hierarchy->parentFrame;
        if (parentFrame == object) {
            ReleaseHierarchy(hierarchy);
            if (parentFrame != nullptr) {
                FrameData(parentFrame).hierarchy = nullptr;
            }
        }
        data.hierarchy = nullptr;
    }
    data.id = -1;
    return object;
}

// 0x7C4830 (lote S10A, sin validar; cdecl, 4 argumentos, 154 bytes): copia del dato del marco: identificador copiado; si el origen tiene jerarquia y no es subjerarquia, crea una nueva (RpHAnimHierarchyCreate(numNodes, NULL, NULL,
// flags, maxInterpKeyFrameSize del origen)), le copia los nodos (pFrame a NULL), la deja en el dato del destino y su parentFrame pasa a ser el marco destino. Devuelve el marco destino.
static void* RpHAnimFrameCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    HAnimFrameData& dst = FrameData(dstObject);
    const HAnimFrameData& src = FrameData(srcObject);
    dst.id = src.id;
    RpHAnimHierarchy* const srcHierarchy = src.hierarchy;
    if (srcHierarchy == nullptr || (static_cast<RwUInt32>(srcHierarchy->flags) & rpHANIMHIERARCHYSUBHIERARCHY)) {
        return dstObject;
    }
    RpHAnimHierarchy* const created = RpHAnimHierarchyCreate(srcHierarchy->numNodes, nullptr, nullptr, static_cast<RpHAnimHierarchyFlag>(srcHierarchy->flags), srcHierarchy->currentAnim->maxInterpKeyFrameSize);
    CopyNodeInfo(created, srcHierarchy);
    dst.hierarchy = created;
    created->parentFrame = static_cast<RwFrame*>(dstObject);
    return dstObject;
}

// 0x7C48D0 (lote S10A, sin validar; cdecl, 5 argumentos, 290 bytes): escribe el dato del marco. RwStreamWriteInt32 de la version 0x100 y del identificador (fallo -> NULL). Sin jerarquia o con subjerarquia (flag 1): un entero 0
// (fallo -> NULL). Si no: numNodes, flags, maxInterpKeyFrameSize de la interpolacion y, por cada nodo (numNodes releido), nodeID, nodeIndex y flags (cada fallo -> NULL). Devuelve el flujo.
static RwStream* RpHAnimFrameStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject); UNUSED(sizeInObject);
    const RwInt32 version = 0x100;
    if (RwStreamWriteInt32(stream, &version, 4) == nullptr) {
        return nullptr;
    }
    const HAnimFrameData& data = FrameData(object);
    if (RwStreamWriteInt32(stream, &data.id, 4) == nullptr) {
        return nullptr;
    }
    RpHAnimHierarchy* const hierarchy = data.hierarchy;
    if (hierarchy == nullptr || (static_cast<RwUInt32>(hierarchy->flags) & rpHANIMHIERARCHYSUBHIERARCHY)) {
        const RwInt32 zero = 0;
        return RwStreamWriteInt32(stream, &zero, 4) != nullptr ? stream : nullptr;
    }
    if (RwStreamWriteInt32(stream, &hierarchy->numNodes, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, &hierarchy->flags, 4) == nullptr) {
        return nullptr;
    }
    if (RwStreamWriteInt32(stream, &hierarchy->currentAnim->maxInterpKeyFrameSize, 4) == nullptr) {
        return nullptr;
    }
    const RpHAnimNodeInfo* info = hierarchy->pNodeInfo;
    for (RwInt32 i = 0; i < hierarchy->numNodes; ++i, ++info) {
        if (RwStreamWriteInt32(stream, &info->nodeID, 4) == nullptr || RwStreamWriteInt32(stream, &info->nodeIndex, 4) == nullptr || RwStreamWriteInt32(stream, &info->flags, 4) == nullptr) {
            return nullptr;
        }
    }
    return stream;
}

// 0x7C4A00 (lote S11A, sin validar; cdecl, 5 argumentos, 482 bytes): lee el dato del marco. Version (debe ser 0x100; fallo de lectura o version distinta -> NULL), identificador (al dato del marco), numNodes (si <= 0 devuelve el
// flujo sin crear nada), flags y maxInterpKeyFrameSize (fallos -> NULL). Bloque de la lista libre 0xC9B8CC (pista 0x3011E) a cero (0x24 bytes); si es NULL termina dejando la jerarquia del marco en NULL y devuelve el flujo.
// currentAnim = RtAnimInterpolatorCreate(numNodes, maxInterpKeyFrameSize), flags, parentFrame = el marco, numNodes, parentHierarchy = ella misma; con NOMATRICES (2) matrices NULL, si no RwMalloc(numNodes * 64 + 15) alineado a 16;
// pNodeInfo = RwMalloc(numNodes * 16); por cada nodo (numNodes releido): nodeID, nodeIndex y flags del flujo (fallo -> NULL sin liberar) y pFrame = NULL. Al final la jerarquia queda en el dato del marco. Devuelve el flujo.
// rootParentOffset queda a 0 (el bloque se pone a cero; no se escribe -1).
static RwStream* RpHAnimFrameStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject); UNUSED(sizeInObject);
    HAnimFrameData& data = FrameData(object);
    RwInt32 version;
    if (RwStreamReadInt32(stream, &version, 4) == nullptr || version != 0x100) {
        return nullptr;
    }
    if (RwStreamReadInt32(stream, &data.id, 4) == nullptr) {
        return nullptr;
    }
    RwInt32 numNodes;
    if (RwStreamReadInt32(stream, &numNodes, 4) == nullptr) {
        return nullptr;
    }
    if (numNodes <= 0) {
        return stream;
    }
    RwInt32 flags;
    if (RwStreamReadInt32(stream, &flags, 4) == nullptr) {
        return nullptr;
    }
    RwInt32 maxInterpKeyFrameSize;
    if (RwStreamReadInt32(stream, &maxInterpKeyFrameSize, 4) == nullptr) {
        return nullptr;
    }
    RpHAnimHierarchy* const hierarchy = static_cast<RpHAnimHierarchy*>(RwFreeListAlloc(HAnimGlobals().HAnimFreeList, kHAnimMemHint));
    if (hierarchy != nullptr) {
        memset(hierarchy, 0, 0x24);
        hierarchy->currentAnim = RtAnimInterpolatorCreate(numNodes, maxInterpKeyFrameSize);
        hierarchy->flags = flags;
        hierarchy->parentFrame = static_cast<RwFrame*>(object);
        hierarchy->numNodes = numNodes;
        hierarchy->parentHierarchy = hierarchy;
        if (static_cast<RwUInt32>(flags) & rpHANIMHIERARCHYNOMATRICES) {
            hierarchy->pMatrixArray = nullptr;
            hierarchy->pMatrixArrayUnaligned = nullptr;
        } else {
            void* const block = RwMalloc(static_cast<RwUInt32>(numNodes) * 64u + 15u, kHAnimMemHint);
            hierarchy->pMatrixArrayUnaligned = block;
            hierarchy->pMatrixArray = reinterpret_cast<RwMatrix*>((reinterpret_cast<uintptr_t>(block) + 15u) & ~static_cast<uintptr_t>(15u));
        }
        hierarchy->pNodeInfo = static_cast<RpHAnimNodeInfo*>(RwMalloc(static_cast<RwUInt32>(numNodes) * 16u, kHAnimMemHint));
        RpHAnimNodeInfo* info = hierarchy->pNodeInfo;
        for (RwInt32 i = 0; i < hierarchy->numNodes; ++i, ++info) {
            if (RwStreamReadInt32(stream, &info->nodeID, 4) == nullptr || RwStreamReadInt32(stream, &info->nodeIndex, 4) == nullptr || RwStreamReadInt32(stream, &info->flags, 4) == nullptr) {
                return nullptr;
            }
            info->pFrame = nullptr;
        }
    }
    data.hierarchy = hierarchy;
    return stream;
}

// 0x7C4BF0 (lote S10A, sin validar; cdecl, 3 argumentos, 59 bytes): tamano en flujo del dato del marco. Con identificador -1 y sin jerarquia: 0. Sin jerarquia o con subjerarquia (flag 1): 0xC. Si no: numNodes * 12 + 0x14.
static RwInt32 RpHAnimFrameStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    const HAnimFrameData& data = FrameData(object);
    if (data.id == -1 && data.hierarchy == nullptr) {
        return 0;
    }
    const RpHAnimHierarchy* const hierarchy = data.hierarchy;
    if (hierarchy == nullptr || (static_cast<RwUInt32>(hierarchy->flags) & rpHANIMHIERARCHYSUBHIERARCHY)) {
        return 0xC;
    }
    return hierarchy->numNodes * 12 + 0x14;
}

// 0x7C4600: registra el plugin 0x11E. Motor: RwEngineRegisterPlugin(0, 0x11E, ctor 0x7C4670, dtor 0x7C4720); si el resultado es < 0 devuelve FALSE sin seguir. Marcos:
// RwFrameRegisterPlugin(8, 0x11E, ctor 0x7C4750, dtor 0x7C4770, copia 0x7C4830) -> 0xC9B8C8 (se guarda aunque sea negativo); RwFrameRegisterPluginStream(0x11E, lectura 0x7C4A00,
// escritura 0x7C48D0, tamano 0x7C4BF0): resultado < 0 -> FALSE; despues, offset en 0xC9B8C8 < 0 -> FALSE; si no, TRUE. Lotes S10A/S11A: las rutinas del plugin son funciones propias (con hook en su direccion del EXE) y ya no se pasan por direccion fija.
RwBool RpHAnimPluginAttach() {
    if (RwEngineRegisterPlugin(0, kHAnimPluginID,
            RpHAnimEngineConstructor, RpHAnimEngineDestructor) < 0) {
        return FALSE;
    }
    HAnimGlobals().engineOffset = RwFrameRegisterPlugin(8, kHAnimPluginID,
        RpHAnimFrameConstructor, RpHAnimFrameDestructor, RpHAnimFrameCopy);
    if (RwFrameRegisterPluginStream(kHAnimPluginID,
            RpHAnimFrameStreamRead, RpHAnimFrameStreamWrite, RpHAnimFrameStreamGetSize) < 0) {
        return FALSE;
    }
    if (HAnimGlobals().engineOffset < 0) {
        return FALSE;
    }
    return TRUE;
}

// 0x7C5B80 (lote S11A, sin validar; x87, 281 bytes): fotograma interpolado -> matriz: cuaternion (x, y, z, w = +0x08..+0x14) a matriz de rotacion con los productos xx, yy, zz, yz, zx, xy, wx redondeados a float y wy, wz sin
// redondear (X87Math.cpp); banderas = 3 (ortonormal); posicion = traslacion (+0x18..+0x20) copiada como dwords; los huecos de relleno de la matriz no se tocan.
void RpHAnimKeyFrameApply(void* matrix, void* voidIFrame) {
    HAnimKeyFrameApplyCpp(static_cast<float*>(matrix), static_cast<const float*>(voidIFrame));
}

// 0x7C60C0 (lote S11A, sin validar; x87, 1024 bytes): mezcla esferica de dos fotogramas interpolados con un peso (cuaterniones por slerp con acos y seno en linea; traslacion lineal) (X87Math.cpp). Puede dejar el cuaternion de voidIn2 negado.
void RpHAnimKeyFrameBlend(void* voidOut, void* voidIn1, void* voidIn2, RwReal alpha) {
    HAnimKeyFrameBlendCpp(static_cast<float*>(voidOut), static_cast<float*>(voidIn1), static_cast<float*>(voidIn2), alpha, _rwSqrt);
}

// 0x7C5CA0 (lote S11A, sin validar; x87, 1043 bytes): interpolacion esferica entre dos fotogramas clave en el instante `time` (alpha = (time - t1) / (t2 - t1)) (X87Math.cpp). El quinto argumento (customData) no se usa.
void RpHAnimKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, RwReal time, void* customData) {
    UNUSED(customData);
    HAnimKeyFrameInterpolateCpp(static_cast<float*>(voidOut), static_cast<float*>(voidIn1), static_cast<float*>(voidIn2), time, _rwSqrt);
}

// 0x7C6720 (lote S11A, sin validar; x87, 196 bytes): suma de fotogramas (producto de cuaterniones y suma de traslaciones) (X87Math.cpp).
void RpHAnimKeyFrameAdd(void* voidOut, void* voidIn1, void* voidIn2) {
    HAnimKeyFrameAddCpp(static_cast<float*>(voidOut), static_cast<const float*>(voidIn1), static_cast<const float*>(voidIn2));
}

// 0x7C65C0 (lote S11A, sin validar; x87, 338 bytes): frame = frame * inverso(start) (cuaternion inverso por la norma y traslacion restada) (X87Math.cpp). Con norma <= 0 o NaN el EXE usa valores de pila sin inicializar;
// aqui ceros.
void RpHAnimKeyFrameMulRecip(void* voidFrame, void* voidStart) {
    HAnimKeyFrameMulRecipCpp(static_cast<float*>(voidFrame), static_cast<const float*>(voidStart));
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

// 0x7C6540 (lote S11A, sin validar; cdecl, 2 argumentos, 107 bytes): escribe los fotogramas. Por cada uno (numFrames releido en cada vuelta): RwStreamWriteReal(flujo, &fotograma->time, 0x20) y
// RwStreamWriteInt32(flujo, &desplazamiento, 4) con desplazamiento = prevFrame - pFrames en BYTES (la lectura lo divide entre 36); un fallo devuelve 0. Devuelve 1.
RwBool RpHAnimKeyFrameStreamWrite(const RtAnimAnimation* animation, RwStream* stream) {
    const RwUInt8* const frames = static_cast<const RwUInt8*>(animation->pFrames);
    const RwUInt8* frame = frames;
    for (RwInt32 i = 0; i < animation->numFrames; ++i, frame += sizeof(RpHAnimKeyFrame)) {
        const RpHAnimKeyFrame* const keyFrame = reinterpret_cast<const RpHAnimKeyFrame*>(frame);
        if (RwStreamWriteReal(stream, &keyFrame->time, 0x20) == nullptr) {
            return FALSE;
        }
        const RwInt32 offset = static_cast<RwInt32>(reinterpret_cast<RwUInt32>(keyFrame->prevFrame) - reinterpret_cast<RwUInt32>(frames));
        if (RwStreamWriteInt32(stream, &offset, 4) == nullptr) {
            return FALSE;
        }
    }
    return TRUE;
}

// 0x7C65B0 (lote S11A, sin validar; 14 bytes): tamano en flujo de los fotogramas: numFrames * 36 (aritmetica de 32 bits).
RwInt32 RpHAnimKeyFrameStreamGetSize(const RtAnimAnimation* animation) {
    return static_cast<RwInt32>(static_cast<RwUInt32>(animation->numFrames) * 36u);
}

// 0x7C5170 (lote S11A, sin validar; 23 bytes): identificador HAnim del marco (dato del plugin, primer dword). Devuelve TRUE.
RwBool RpHAnimFrameSetID(RwFrame* frame, RwInt32 id) {
    FrameData(frame).id = id;
    return TRUE;
}

// 0x7C5190 (lote S11A, sin validar; 14 bytes): identificador HAnim del marco.
RwInt32 RpHAnimFrameGetID(RwFrame* frame) {
    return FrameData(frame).id;
}
