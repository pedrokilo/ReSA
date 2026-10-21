/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwHelper
 * Reconstrucción: COMPLETA — 33/33 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 33/33 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include <intrin.h>
#include "X87Math.h"

#include "RwHelper.h"

void RwHelperInjectHooks() {
    RH_ScopedNamespaceName("RwHelper");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(GetEventGlobalGroup, 0x4ABA50);
    RH_ScopedGlobalInstall(GetNameAndDamage, 0x5370A0);
    RH_ScopedGlobalInstall(GetFirstAtomicCallback, 0x734810);
    RH_ScopedGlobalInstall(GetFirstAtomic, 0x734820);
    // RH_ScopedGlobalInstall(Get2DEffectAtomicCallback, 0x734850);
    RH_ScopedGlobalInstall(Get2DEffectAtomic, 0x734880);
    RH_ScopedGlobalInstall(GetFirstObjectCallback, 0x7348B0);
    RH_ScopedGlobalInstall(GetFirstObject, 0x7348C0);
    RH_ScopedGlobalInstall(GetFirstFrameCallback, 0x7348F0);
    RH_ScopedGlobalInstall(GetFirstChild, 0x734900);
    RH_ScopedGlobalInstall(SkinAtomicGetHAnimHierarchCB, 0x734A20);
    RH_ScopedGlobalInstall(GetAnimHierarchyFromSkinClump, 0x734A40);
    RH_ScopedGlobalInstall(GetAnimHierarchyFromFrameCallback, 0x734A70);
    RH_ScopedNamedGlobalInstall(GetAnimHierarchyFromFrameRegs, "GetAnimHierarchyFromFrame", 0x734AB0);
    RH_ScopedGlobalInstall(RwTexDictionaryFindHashNamedTexture, 0x734E50);
    RH_ScopedGlobalInstall(GetFirstTexture, 0x734940);
    RH_ScopedGlobalInstall(RwTexDictionaryGtaStreamRead, 0x730FC0);
    RH_ScopedGlobalInstall(RwTextureGtaStreamRead, 0x730E60);
    RH_ScopedGlobalInstall(SkinGetBonePositionsToTable, 0x735360);
    RH_ScopedGlobalInstall(RpGeometryReplaceOldMaterialWithNewMaterial, 0x734DE0);
    RH_ScopedGlobalInstall(GetAnimHierarchyFromClump, 0x734B10);
    RH_ScopedGlobalInstall(AtomicRemoveAnimFromSkinCB, 0x734B90);
    RH_ScopedGlobalInstall(RpAtomicConvertGeometryToTL, 0x734BE0);
    RH_ScopedGlobalInstall(RpAtomicConvertGeometryToTS, 0x734C20);
    RH_ScopedGlobalInstall(atomicConvertGeometryToTL, 0x734C60);
    RH_ScopedGlobalInstall(RpClumpConvertGeometryToTL, 0x734CB0);
    RH_ScopedGlobalInstall(atomicConvertGeometryToTS, 0x734CE0);
    RH_ScopedGlobalInstall(RpClumpConvertGeometryToTS, 0x734D30);
    RH_ScopedGlobalInstall(forceLinearFilteringAtomicsCB, 0x734DA0);
    RH_ScopedGlobalInstall(SetFilterModeOnClumpsTextures, 0x734DC0);
    RH_ScopedGlobalInstall(forceLinearFilteringMatTexturesCB, 0x734D60);
    RH_ScopedGlobalInstall(SetFilterModeOnAtomicsTextures, 0x734D80);
    RH_ScopedGlobalInstall(RemoveRefsCB, 0x7226D0);
    RH_ScopedGlobalInstall(RpClumpGetBoundingSphere, 0x734FC0);
}

// 0x4ABA50
CEventGlobalGroup* GetEventGlobalGroup() {
    static auto& globalEvents = StaticRef<CEventGlobalGroup*>(0xA9AF6C);

    if (globalEvents)
        return globalEvents;

    globalEvents = new CEventGlobalGroup(nullptr);
    return globalEvents;
}

// TODO: Check `outName` size (to avoid buffer overflow)
// 0x5370A0
void GetNameAndDamage(const char* name, char* objName, bool& bIsDamageModel) {
    const size_t nodesz = strlen(name);

    // Como el original: strncpy (0x821F40) del prefijo y terminador escrito a mano; strcpy en otro caso.
    const auto TerminatedCopy = [=](size_t offset) {
        strncpy(objName, name, nodesz - offset);
        objName[nodesz - offset] = 0;
    };

    // EndsWith "_dam"
    if (name[nodesz - 4] == '_' && name[nodesz - 3] == 'd' && name[nodesz - 2] == 'a' && name[nodesz - 1] == 'm'
    ) {
        bIsDamageModel = true;
        TerminatedCopy(sizeof("_dam") - 1);
    }
    else {
        bIsDamageModel = false;
        // EndsWith "_l0" or "_L0"
        if (name[nodesz - 3] == '_' &&
            (name[nodesz - 2] == 'L' || name[nodesz - 2] == 'l') && name[nodesz - 1] == '0'
        ) {
            TerminatedCopy(sizeof("_l0") - 1);
        } else
            strcpy(objName, name);
    }
}

// 0x734810
RpAtomic* GetFirstAtomicCallback(RpAtomic* atomic, void* data) {
    *(RpAtomic**)(data) = atomic;
    return nullptr;
}

// 0x734820
RpAtomic* GetFirstAtomic(RpClump* clump) {
    RpAtomic* atomic{};
    RpClumpForAllAtomics(clump, GetFirstAtomicCallback, &atomic);
    return atomic;
}

// 0x734850
RpAtomic* Get2DEffectAtomicCallback(RpAtomic* atomic, void* data) {
    return ((RpAtomic * (__cdecl*)(RpAtomic*, void*))0x734850)(atomic, data);
}

// 0x734880
RpAtomic* Get2DEffectAtomic(RpClump* clump) {
    RpAtomic* atomic{};
    RpClumpForAllAtomics(clump, Get2DEffectAtomicCallback, &atomic);
    return atomic;
}

// 0x7348B0
RwObject* GetFirstObjectCallback(RwObject* object, void* data) {
    *(RwObject**)(data) = object;
    return nullptr;
}

// 0x7348C0
RwObject* GetFirstObject(RwFrame* frame) {
    RwObject* obj{};
    RwFrameForAllObjects(frame, GetFirstObjectCallback, &obj);
    return obj;
}

// 0x7348F0
RwFrame* GetFirstFrameCallback(RwFrame* frame, void* data) {
    *(RwFrame**)(data) = frame;
    return nullptr;
}

// 0x734900
RwFrame* GetFirstChild(RwFrame* frame) {
    RwFrame* child{};
    RwFrameForAllChildren(frame, GetFirstFrameCallback, &child);
    return child;
}

// 0x734A70 (callback de RwFrameForAllChildren, cdecl): si el marco tiene jerarquia (RpHAnimFrameGetHierarchy, 0x7C5160) la guarda en *data y detiene el
// recorrido (nullptr); si no, recorre los hijos del marco con ella misma (RwFrameForAllChildren, 0x7F0DC0) y devuelve el marco para seguir con sus hermanos.
// Enganchada en 0x734A70: con el hook activo el cuerpo original de GetAnimHierarchyFromFrame (0x734AB0, hook apagado) llega a esta version por el salto de la entrada.
// El iterador del EXE (RwFrameForAllChildren, 0x7F0DC0) solo lee EAX a la vuelta (nada lee ECX/EDX).
RwFrame* GetAnimHierarchyFromFrameCallback(RwFrame* frame, void* data) {
    if (RpHAnimHierarchy* const hierarchy = RpHAnimFrameGetHierarchy(frame)) {
        *static_cast<RpHAnimHierarchy**>(data) = hierarchy;
        return nullptr;
    }
    RwFrameForAllChildren(frame, GetAnimHierarchyFromFrameCallback, data);
    return frame;
}

// 0x734AB0
// Como el original: jerarquia del propio marco (RpHAnimFrameGetHierarchy, 0x7C5160) o, si no la tiene, la primera que devuelva el
// recorrido de sus hijos con el callback de 0x734A70 (RwFrameForAllChildren, 0x7F0DC0), que se repite una segunda vez si aun no hay.
RpHAnimHierarchy* GetAnimHierarchyFromFrame(RwFrame* frame) {
    const auto callback = &GetAnimHierarchyFromFrameCallback;
    RpHAnimHierarchy* found = nullptr;
    RpHAnimHierarchy* hierarchy = RpHAnimFrameGetHierarchy(frame);
    if (hierarchy) {
        found = hierarchy;
    } else {
        RwFrameForAllChildren(frame, callback, &found);
        hierarchy = found;
    }
    if (!hierarchy) {
        RwFrameForAllChildren(frame, callback, &found);
        hierarchy = found;
    }
    return hierarchy;
}

// Forma del hook en 0x734AB0: el original conserva ECX de entrada. Se declara __fastcall sin parametros en la pila (el llamador cdecl
// limpia la pila) y el marco se lee bajo la direccion de retorno.
#pragma runtime_checks("", off)
uint32 __fastcall GetAnimHierarchyFromFrameRegs(void* entryEcx, uint32 entryEdx) {
    RwFrame* const frame = reinterpret_cast<RwFrame*>(static_cast<uintptr_t>(static_cast<const uint32*>(_AddressOfReturnAddress())[1]));
    RpHAnimHierarchy* const hierarchy = GetAnimHierarchyFromFrame(frame);
    return ResidueRegsCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(entryEcx)), entryEdx, static_cast<uint32>(reinterpret_cast<uintptr_t>(hierarchy)));
}
#pragma runtime_checks("", restore)

// 0x734B10
RpHAnimHierarchy* GetAnimHierarchyFromClump(RpClump* clump) {
    assert(clump);
    return GetAnimHierarchyFromFrame(RpClumpGetFrame(clump));
}

// 0x734A40
RpHAnimHierarchy* GetAnimHierarchyFromSkinClump(RpClump* clump) {
    RpHAnimHierarchy* anim{};
    RpClumpForAllAtomics(clump, SkinAtomicGetHAnimHierarchCB, &anim);
    return anim;
}

// name not from Android
// 0x734A20
RpAtomic* SkinAtomicGetHAnimHierarchCB(RpAtomic* atomic, void* data) {
    *(RpHAnimHierarchy**)(data) = RpSkinAtomicGetHAnimHierarchy(atomic);
    return nullptr;
}

// 0x734B90
RpAtomic* AtomicRemoveAnimFromSkinCB(RpAtomic* atomic, void* data) {
    if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic))) {
        if (RpHAnimHierarchy* hier = RpSkinAtomicGetHAnimHierarchy(atomic)) {
            RtAnimAnimation*& currAnim = hier->currentAnim->pCurrentAnim;
            if (currAnim) {
                RtAnimAnimationDestroy(currAnim);
            }
            currAnim = nullptr;
        }
    }
    return atomic;
}

// 0x734BE0
bool RpAtomicConvertGeometryToTL(RpAtomic* atomic) {
    RpGeometry* geometry = RpAtomicGetGeometry(atomic);

    auto flags = RpGeometryGetFlags(geometry);
    if (flags & rpGEOMETRYNATIVE || !(flags & rpGEOMETRYTRISTRIP))
        return false;

    RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
    RpGeometrySetFlags(geometry, flags & ~rpGEOMETRYTRISTRIP);
    RpGeometryUnlock(geometry);

    return true;
}

// 0x734C20
bool RpAtomicConvertGeometryToTS(RpAtomic* atomic) {
    RpGeometry* geometry = RpAtomicGetGeometry(atomic);

    auto flags = RpGeometryGetFlags(geometry);
    if (flags & rpGEOMETRYNATIVE || flags & rpGEOMETRYTRISTRIP)
        return false;

    RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
    RpGeometrySetFlags(geometry, flags | rpGEOMETRYTRISTRIP);
    RpGeometryUnlock(geometry);

    return true;
}

// 0x734C60
RpAtomic* atomicConvertGeometryToTL(RpAtomic* atomic, void* data) {
    if (!RpAtomicConvertGeometryToTL(atomic)) {
        *(bool*)(data) = false;
    }
    return atomic;
}

// 0x734CB0
bool RpClumpConvertGeometryToTL(RpClump* clump) {
    bool success{ true };
    RpClumpForAllAtomics(clump, atomicConvertGeometryToTL, &success);
    return success;
}

// 0x734CE0
RpAtomic* atomicConvertGeometryToTS(RpAtomic* atomic, void* data) {
    if (!RpAtomicConvertGeometryToTS(atomic)) {
        *(bool*)(data) = false;
    }
    return atomic;
}

// 0x734D30
bool RpClumpConvertGeometryToTS(RpClump* clump) {
    bool success{ true };
    RpClumpForAllAtomics(clump, atomicConvertGeometryToTS, &success);
    return success;
}

// 0x734D60
RpMaterial* forceLinearFilteringMatTexturesCB(RpMaterial* material, void* data) {
    if (RwTexture* texture = RpMaterialGetTexture(material)) {
        RwTextureSetFilterMode(texture, (RwTextureFilterMode)((unsigned)data));
    }
    return material;
}

// 0x734D80
bool SetFilterModeOnAtomicsTextures(RpAtomic* atomic, RwTextureFilterMode filtering) {
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), forceLinearFilteringMatTexturesCB, (void*)(unsigned)filtering);
    return true;
}

// 0x734DA0
RpAtomic* forceLinearFilteringAtomicsCB(RpAtomic* atomic, void* data) {
    SetFilterModeOnAtomicsTextures(atomic, (RwTextureFilterMode)((unsigned)data));
    return atomic;
}

// 0x734DC0
bool SetFilterModeOnClumpsTextures(RpClump* clump, RwTextureFilterMode filtering) {
    RpClumpForAllAtomics(clump, forceLinearFilteringAtomicsCB, (void*)(unsigned)filtering);
    return true;
}

// 0x734DE0: sustituye oldMaterial por newMaterial en cada malla de la geometria y en su lista de materiales. Devuelve true si cambio alguna malla.
// El EXE ignora RpMeshHeader::firstMeshOffset: las mallas empiezan justo despues de la cabecera de 0x10 bytes. Por cada coincidencia, en este orden:
// indice del material en la lista (_rpMaterialListFindMaterialIndex, 0x74E420; -1 si no esta y entonces materials[-1] se escribe igual que en el EXE),
// RpMaterialDestroy(material viejo) (0x74DA20, resta una referencia), lista y malla pasan a apuntar al nuevo y se suma una referencia al nuevo (16 bits).
bool RpGeometryReplaceOldMaterialWithNewMaterial(RpGeometry* geometry, RpMaterial* oldMaterial, RpMaterial* newMaterial) {
    RpMeshHeader* const header = geometry->mesh;
    bool replaced = false;
    RpMesh* mesh = reinterpret_cast<RpMesh*>(header + 1);
    for (uint32 count = header->numMeshes; count > 0; --count, ++mesh) {
        if (mesh->material != oldMaterial) {
            continue;
        }
        const int32 index = _rpMaterialListFindMaterialIndex(&geometry->matList, mesh->material);
        RpMaterialDestroy(mesh->material);
        geometry->matList.materials[index] = newMaterial;
        mesh->material = newMaterial;
        ++newMaterial->refCount;
        replaced = true;
    }
    return replaced;
}

// 0x734E50
RwTexture* RwTexDictionaryFindHashNamedTexture(RwTexDictionary* dictionary, uint32 hash) {
    auto* const sentinel = &dictionary->texturesInDict.link;
    for (auto* link = sentinel->next; link != sentinel; link = link->next) {
        auto* const texture = reinterpret_cast<RwTexture*>(
            reinterpret_cast<uint8*>(link) - offsetof(RwTexture, lInDictionary));
        if (CKeyGen::GetUppercaseKey(texture->name) == hash) {
            return texture;
        }
    }
    return nullptr;
}

// 0x734FC0 (cdecl, ret): esfera envolvente de un clump. Guarda el byte bajo de bUseLTM en el global 0x8D60BC (lo leen los dos callbacks); sin clump o sin esfera devuelve 0 sin tocar
// nada mas. Pone la esfera a 0; con RpClumpGetNumAtomics < 1 devuelve 0 (la esfera ya a cero). Centro = suma de los centros de los atomos (callback 0x734970: centro de la esfera de cada
// atomo transformado por la LTM del marco del atomo, o por su matriz local si el global es 0) dividida entre float(numero de atomos) (x87: 1/n a 24 bits y un producto por componente, en
// el orden x, y, z); radio = maximo de |centro - centro del atomo| + radio del atomo (callback 0x734ED0, con el radio inicial a 0). Despues el centro se transforma con la inversa (0x7F2070,
// RwMatrixInvert) de la LTM del marco del clump (global distinto de 0) o de su matriz local (marco + 0x10). Los dos callbacks siguen siendo codigo del EXE (se pasan por direccion).
// Ningun llamador del EXE lee ECX/EDX; EAX es el clump (o 0).
RpClump* RpClumpGetBoundingSphere(RpClump* clump, RwSphere* sphere, bool bUseLTM) {
    auto& useLtm = StaticRef<uint8>(0x8D60BC);
    useLtm = std::bit_cast<uint8>(bUseLTM);
    if (!clump || !sphere) {
        return nullptr;
    }
    sphere->center.x = 0.0f;
    sphere->center.y = 0.0f;
    sphere->radius = 0.0f;
    sphere->center.z = 0.0f;
    const RwInt32 numAtomics = RpClumpGetNumAtomics(clump);
    if (numAtomics < 1) {
        return nullptr;
    }
    RwV3d sum{};
    RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(0x734970), &sum);
    Hp66ClumpScaleCenterCpp(&sum.x, numAtomics);
    RwSphere local{ sum, 0.0f };
    RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(0x734ED0), &local);
    auto* const frame = reinterpret_cast<RwFrame*>(clump->object.parent);
    RwMatrix matrix;
    if (useLtm) {
        RwMatrixInvert(&matrix, RwFrameGetLTM(frame));
    } else {
        RwMatrixInvert(&matrix, &frame->modelling);
    }
    RwV3dTransformPoints(&local.center, &local.center, 1, &matrix);
    sphere->radius = local.radius;
    sphere->center.x = local.center.x;
    sphere->center.y = local.center.y;
    sphere->center.z = local.center.z;
    return clump;
}

// 0x735140
void SkinGetBonePositions(RpClump* clump) {
    ((void(__cdecl*)(RpClump*))0x735140)(clump);
}

// 0x7352D0
void SkinSetBonePositions(RpClump* clump) {
    ((void(__cdecl*)(RpClump*))0x7352D0)(clump);
}

// 0x735360
// Como el original: con tabla nula no hace nada. Si no, busca el primer atomo del clump (RpClumpForAllAtomics con el callback 0x734810) y su RpSkin (RpSkinGeometryGetSkin, sin
// comprobar nulos: un clump sin atomos revienta, como en el EXE), pide la jerarquia (callback 0x734A20), pone table[0] a cero y recorre los huesos 1..n-1 (n = RpSkinGetNumBones,
// con signo). Para cada hueso i: copia la matriz piel-a-hueso i, la invierte (RwMatrixInvert), copia la del hueso padre (la matriz se vuelve a pedir con RpSkinGetSkinToBoneMatrices
// antes de cada copia) y table[i] = RwV3dTransformPoints(posicion de la inversa, matriz del padre). El padre sale de las banderas del nodo i de la jerarquia (pNodeInfo[i], 0x10 bytes):
// con PUSH (2) se apila el padre actual, con POP (1) el nuevo padre es el desapilado y, sin POP, el nuevo padre es i. La pila del EXE (32 enteros, nunca inicializada) arranca con el
// puntero en su primera posicion: un POP sin PUSH previo lee basura de la pila (no definido; aqui 0).
void SkinGetBonePositionsToTable(RpClump* clump, RwV3d* table) {
    if (!table) {
        return;
    }

    RpAtomic* atomic = nullptr;
    RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(0x734810), &atomic); // GetFirstAtomicCallback
    RpSkin* const skin = RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic));

    RpHAnimHierarchy* hierarchy = nullptr;
    RpClumpForAllAtomics(clump, reinterpret_cast<RpAtomicCallBack>(0x734A20), &hierarchy); // SkinAtomicGetHAnimHierarchCB

    table[0].x = 0.0f;
    table[0].y = 0.0f;
    table[0].z = 0.0f;

    const int32 numBones = static_cast<int32>(RpSkinGetNumBones(skin));
    int32  parent = 0;
    int32  stack[33]{};
    int32* stackTop = stack;
    for (int32 bone = 1; bone < numBones; ++bone) {
        RwMatrix boneMatrix = RpSkinGetSkinToBoneMatrices(skin)[bone];
        RwMatrix inverse;
        RwMatrixInvert(&inverse, &boneMatrix);

        // Como el original: el indice del padre se multiplica por 64 con 32 bits (shl esi,6) sobre el puntero devuelto
        const auto* const parentSource = reinterpret_cast<const RwMatrix*>(reinterpret_cast<const uint8*>(RpSkinGetSkinToBoneMatrices(skin)) + (static_cast<uint32>(parent) << 6));
        RwMatrix parentMatrix = *parentSource;
        RwV3dTransformPoints(&table[bone], &inverse.pos, 1, &parentMatrix);

        const RwInt32 flags = hierarchy->pNodeInfo[bone].flags;
        if (flags & rpHANIMPUSHPARENTMATRIX) {
            *++stackTop = parent;
        }
        if (flags & rpHANIMPOPPARENTMATRIX) {
            parent = *stackTop--;
        } else {
            parent = bone;
        }
    }
}

// 0x7226D0
RpAtomic* RemoveRefsCB(RpAtomic* atomic, void* data) {
    UNUSED(data);
    auto* modelInfo = CVisibilityPlugins::GetModelInfo(atomic);
    modelInfo->RemoveRef();
    return atomic;
}

// 0x7226F0
void RemoveRefsForAtomic(RpClump* clump) {
    RpClumpForAllAtomics(clump, RemoveRefsCB, nullptr);
}
