/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpAnimBlend
 * Reconstrucción: COMPLETA — 44/44 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 44/44 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "RpAnimBlend.h"
#include "X87Math.h"

// Offset del payload CAnimBlendClumpData* registrado dentro de cada RpClump.
static auto& ClumpOffset = StaticRef<uint32>(0xB5F878);

// Devuelve una referencia al puntero almacenado en el payload para permitir inicializarlo o limpiarlo.
CAnimBlendClumpData*& RpAnimBlendClumpGetData(RpClump* clump) {
    return *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
}

// 0x4D5F40
void* ClumpAnimConstruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(sizeInObject);

    // Como el original: usa el offset recibido (no el global 0xB5F878)
    *RWPLUGINOFFSET(CAnimBlendClumpData*, object, offsetInObject) = nullptr;

    return object;
}

// 0x4D6110
/**
 * Retira y destruye todas las asociaciones antes de liberar CAnimBlendClumpData.
 * El RpClump continúa siendo propiedad de RenderWare.
 */
void* ClumpAnimDestruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject);
    UNUSED(sizeInObject);

    const auto clump = static_cast<RpClump*>(object);

    if (const auto bd = RpAnimBlendClumpGetData(clump)) {
        RpAnimBlendClumpRemoveAllAssociations(clump);
        delete bd;
        RpAnimBlendClumpGetData(clump) = nullptr; // Como el original: se anula después de liberar
    }

    return object;
}

// 0x4D5F90
void* ClumpAnimCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(dstObject);
    UNUSED(srcObject);
    UNUSED(offsetInObject);
    UNUSED(sizeInObject);

    return nullptr;
}

// 0x4D5FA0
void RtAnimBlendKeyFrameApply(void* voidMat, void* voidFrame) { // Semantically same as the original `RpHAnimBlendKeyFrameApply`
    const auto mat   = static_cast<RwMatrix*>(voidMat);
    const auto frame = static_cast<RpHAnimBlendInterpFrame*>(voidFrame);

    RtQuatUnitConvertToMatrix(&frame->q, mat);
    RwV3dAssign(RwMatrixGetPos(mat), &frame->t);
}

// 0x4D60C0
void RpAnimBlendKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, float time, void* customData) {
    UNUSED(voidIn1);
    UNUSED(voidIn2);
    UNUSED(time);
    UNUSED(customData);

    const auto out = static_cast<RpHAnimBlendInterpFrame*>(voidOut);
    *out = {};
}

// 0x4D6150
/**
 * Registra en RpClump el puntero a CAnimBlendClumpData y el esquema de interpolación.
 *
 * RenderWare conserva los callbacks de keyframe, mientras el juego administra por
 * separado las asociaciones y secuencias reconstruidas por el plugin.
 */
RwBool RpAnimBlendPluginAttach() { // Como el original: devuelve EAX completo (el llamador 0x53D8AE hace test eax,eax)
    ClumpOffset = RpClumpRegisterPlugin(
        sizeof(CAnimBlendClumpData*),
        rwID_RPANIMBLENDPLUGIN,
        &ClumpAnimConstruct,
        &ClumpAnimDestruct,
        &ClumpAnimCopy
    );

    if (static_cast<int32>(ClumpOffset) < 0) { // Como el original: cualquier offset negativo es un fallo (jge)
        return FALSE;
    }

    RtAnimInterpolatorInfo rtInfo{
        .typeID                  = rwID_RPANIMBLENDPLUGIN,

        .interpKeyFrameSize      = sizeof(RpHAnimBlendInterpFrame),
        .animKeyFrameSize        = sizeof(RpHAnimKeyFrame),

        .keyFrameApplyCB         = RtAnimBlendKeyFrameApply,
        .keyFrameBlendCB         = RpHAnimKeyFrameBlend,
        .keyFrameInterpolateCB   = RpAnimBlendKeyFrameInterpolate,
        .keyFrameAddCB           = RpHAnimKeyFrameAdd,
        .keyFrameMulRecipCB      = RpHAnimKeyFrameMulRecip,
        .keyFrameStreamReadCB    = RpHAnimKeyFrameStreamRead,
        .keyFrameStreamWriteCB   = RpHAnimKeyFrameStreamWrite,
        .keyFrameStreamGetSizeCB = RpHAnimKeyFrameStreamGetSize,

        .customDataSize          = 0
    };
    RtAnimRegisterInterpolationScheme(&rtInfo);

    return TRUE;
}

// 0x4D5F50
CAnimBlendClumpData* RpAnimBlendAllocateData(RpClump* clump) {
    // Como el original: devuelve el puntero guardado (EAX)
    return RpAnimBlendClumpGetData(clump) = new CAnimBlendClumpData;
}

// 0x4D6500 (FrameInitCBskin)
static void FrameInitCBskin(AnimBlendFrameData* fd, void* data) {
    UNUSED(data);
    fd->Flags = 0;
}

// 0x4D6510
/**
 * Construye un AnimBlendFrameData por bone de un clump skinned.
 *
 * Toma las posiciones de los bones de SkinGetBonePositionsToTable y relaciona cada
 * slot con el frame del RtAnimInterpolator correspondiente.
 */
void RpAnimBlendClumpInitSkinned(RpClump* clump) { // Can't hook, `clump` passed in eax
    // Como el original: la propia función reserva los datos del clump
    RpAnimBlendAllocateData(clump);
    const auto bd = RpAnimBlendClumpGetData(clump);

    constexpr size_t MAX_NUM_BONES = 64;

    const auto skin     = RpSkinGeometryGetSkin(RpAtomicGetGeometry(GetFirstAtomic(clump)));
    const auto nBones   = RpSkinGetNumBones(skin);
    bd->SetNumberOfBones(nBones);
    const auto rpHAHier = GetAnimHierarchyFromSkinClump(clump);

    // Como el original: tabla de 64 posiciones a cero rellenada por 0x735360 (SkinGetBonePositionsToTable)
    RwV3d bonePositions[MAX_NUM_BONES]{};
    SkinGetBonePositionsToTable(clump, bonePositions);

    // Como el original: contador con signo
    const auto frames = bd->m_FrameDatas;
    for (int32 i = 0; i < static_cast<int32>(nBones); i++) {
        const auto fd = &frames[i];

        fd->KeyFrame = (RpHAnimBlendInterpFrame*)rtANIMGETINTERPFRAME(rpHAHier->currentAnim, i);
        fd->BoneTag  = rpHAHier->pNodeInfo[i].nodeID;
        fd->BonePos  = CVector{ bonePositions[i].x, bonePositions[i].y, bonePositions[i].z };
    }

    bd->ForAllFrames(FrameInitCBskin, nullptr);
    bd->m_FrameDatas[0].HasVelocity = true;
}

// 0x4D6610
static RwFrame* FrameCountCB(RwFrame* f, void* data) {
    ++*static_cast<int32*>(data);
    RwFrameForAllChildren(f, FrameCountCB, data);
    return f;
}

// 0x4D6670
RwFrame* SetFrameDataFrameCB(RwFrame* f, void* data) {
    const auto fd = static_cast<AnimBlendFrameData**>(data);
    (*fd)->Frame = f;
    (*fd)++;
    RwFrameForAllChildren(f, SetFrameDataFrameCB, fd);
    return f;
}

// 0x4D6640 (FrameInitCBnonskin)
static void FrameInitCBnonskin(AnimBlendFrameData* fd, void* data) {
    UNUSED(data);
    fd->Flags    = 0;
    fd->FramePos = *RwMatrixGetPos(RwFrameGetMatrix(fd->Frame));
    fd->BoneTag  = BONE_UNKNOWN;
}

// 0x4D66A0
/**
 * Inicializa clumps no skinned recorriendo sus RwFrame hijos y conservando la
 * posición base de cada frame para posteriores mezclas de traslación.
 */
void RpAnimBlendClumpInitNonSkinned(RpClump* clump) { // Can't hook, `clump` passed in eax
    int32 numFrames = 0;
    RpAnimBlendAllocateData(clump);
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Como el original: cuenta los descendientes del frame raíz con RwFrameForAllChildren (sin RwFrameCount)
    const auto root = RpClumpGetFrame(clump);
    RwFrameForAllChildren(root, FrameCountCB, &numFrames);
    bd->SetNumberOfBones(numFrames);

    // Assign frames to our frame data recursively
    auto fd = bd->m_FrameDatas;
    RwFrameForAllChildren(root, SetFrameDataFrameCB, &fd);

    bd->ForAllFrames(FrameInitCBnonskin, nullptr);
    bd->m_FrameDatas[0].HasVelocity = true;
}

// 0x4D6720
// Selecciona la ruta skinned/no-skinned; cada una reserva los datos del clump.
void RpAnimBlendClumpInit(RpClump* clump) {
    // Como el original: comprueba el skin antes de reservar los datos
    if (IsClumpSkinned(clump)) {
        RpAnimBlendClumpInitSkinned(clump);
    } else {
        RpAnimBlendClumpInitNonSkinned(clump);
    }
}

// 0x4D6790
CAnimBlendAssociation* RpAnimBlendClumpAddAssociation(RpClump* clump, CAnimBlendAssociation* association, uint32 playFlags, float startTime, float blendAmount) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Como el original: enlaza al principio, arranca y aplica flags (palabra baja) y blend
    bd->m_AnimList.Prepend(&association->GetLink());
    association->Start(startTime);
    association->m_Flags |= static_cast<uint16>(playFlags);
    association->m_BlendAmount = blendAmount;
    return association;
}

//! @notsa
CAnimBlendLink& RpAnimBlendClumpGetAssociations(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    return bd->m_AnimList;
}

// 0x4D58A0
eBoneTag32 ConvertPedNode2BoneTag(ePedNode pedNode) {
    switch (pedNode) {
    case PED_NODE_UPPER_TORSO:     return BONE_SPINE1;
    case PED_NODE_HEAD:            return BONE_HEAD;
    case PED_NODE_LEFT_ARM:        return BONE_L_UPPER_ARM;
    case PED_NODE_RIGHT_ARM:       return BONE_R_UPPER_ARM;
    case PED_NODE_LEFT_HAND:       return BONE_L_HAND;
    case PED_NODE_RIGHT_HAND:      return BONE_R_HAND;
    case PED_NODE_LEFT_LEG:        return BONE_L_THIGH;
    case PED_NODE_RIGHT_LEG:       return BONE_R_THIGH;
    case PED_NODE_LEFT_FOOT:       return BONE_L_FOOT;
    case PED_NODE_RIGHT_FOOT:      return BONE_R_FOOT;
    case PED_NODE_RIGHT_LOWER_LEG: return BONE_R_CALF;
    case PED_NODE_LEFT_LOWER_LEG:  return BONE_L_CALF;
    case PED_NODE_LEFT_LOWER_ARM:  return BONE_L_FORE_ARM;
    case PED_NODE_RIGHT_LOWER_ARM: return BONE_R_FORE_ARM;
    case PED_NODE_LEFT_CLAVICLE:   return BONE_L_CLAVICLE;
    case PED_NODE_RIGHT_CLAVICLE:  return BONE_R_CLAVICLE;
    case PED_NODE_NECK:            return BONE_NECK;
    case PED_NODE_JAW:             return BONE_JAW;
    default:                       return BONE_UNKNOWN;
    }
}

// 0x4D56F0
const char* ConvertBoneTag2BoneName(eBoneTag32 boneTag) {
    switch (boneTag) {
    case BONE_R_BREAST:    return "R Breast";
    case BONE_L_BREAST:    return "L Breast";
    case BONE_BELLY:       return "Belly";
    case BONE_ROOT:        return "Root";
    case BONE_PELVIS:      return "Pelvis";
    case BONE_SPINE:       return "Spine";
    case BONE_SPINE1:      return "Spine1";
    case BONE_NECK:        return "Neck";
    case BONE_HEAD:        return "Head";
    case BONE_L_BROW:      return "L Brow";
    case BONE_R_BROW:      return "R Brow";
    case BONE_JAW:         return "Jaw";
    case BONE_R_CLAVICLE:  return "Bip01 R Clavicle";
    case BONE_R_UPPER_ARM: return "R UpperArm";
    case BONE_R_FORE_ARM:  return "R Forearm";
    case BONE_R_HAND:      return "R Hand";
    case BONE_R_FINGER:    return "R Fingers";
    case BONE_R_FINGER_01: return "R Finger01";
    case BONE_L_CLAVICLE:  return "Bip01 L Clavicle";
    case BONE_L_UPPER_ARM: return "L UpperArm";
    case BONE_L_FORE_ARM:  return "L Forearm";
    case BONE_L_HAND:      return "L Hand";
    case BONE_L_FINGER:    return "L Fingers";
    case BONE_L_FINGER_01: return "L Finger01";
    case BONE_L_THIGH:     return "L Thigh";
    case BONE_L_CALF:      return "L Calf";
    case BONE_L_FOOT:      return "L Foot";
    case BONE_L_TOE_0:     return "L Toe";
    case BONE_R_THIGH:     return "R Thigh";
    case BONE_R_CALF:      return "R Calf";
    case BONE_R_FOOT:      return "R Foot";
    case BONE_R_TOE_0:     return "R Toe";
    default:               return nullptr;
    }
}

// 0x4D6BE0
CAnimBlendAssociation* RpAnimBlendClumpExtractAssociations(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Como el original: no comprueba la lista vacía y anula también el `prev` de la cabeza
    const auto head = bd->m_AnimList.next;
    bd->m_AnimList.next = nullptr;
    head->prev          = nullptr;
    return CAnimBlendAssociation::FromLink(head);
}

// 0x4D6C30
void RpAnimBlendClumpGiveAssociations(RpClump* clump, CAnimBlendAssociation* associations) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Delete all animations of this clump
    RpAnimBlendClumpRemoveAllAssociations(clump);
    
    // Use new list of associations
    bd->m_AnimList.next = &associations->GetLink();
    associations->GetLink().prev = &bd->m_AnimList;
}

// 0x4D6430 (FillFrameArrayIndiciesNonSkinned)
static void FillFrameArrayIndiciesNonSkinnedCB(AnimBlendFrameData* fd, void* data) {
    static_cast<AnimBlendFrameData**>(data)[CVisibilityPlugins::GetFrameHierarchyId(fd->Frame)] = fd;
}

// 0x4D64A0
void RpAnimBlendClumpFillFrameArray(RpClump* clump, AnimBlendFrameData** outFrameData) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    if (IsClumpSkinned(clump)) { // 0x4D6450 (FillFrameArrayIndiciesSkinned)
        // Como el original: jerarquía del skin (0x734A40), índice sin comprobar (-1 apunta al frame anterior)
        const auto ah = GetAnimHierarchyFromSkinClump(clump);
        for (int32 i = PED_NODE_UPPER_TORSO; i < TOTAL_PED_NODES; i++) {
            outFrameData[i] = &bd->m_FrameDatas[RpHAnimIDGetIndex(ah, ConvertPedNode2BoneTag((ePedNode)i))];
        }
    } else {
        bd->ForAllFrames(FillFrameArrayIndiciesNonSkinnedCB, outFrameData);
    }
}

// Como el original: resultado de las búsquedas de frames en un global del EXE (gpAnimBlendFrame)
static auto& gpAnimBlendFrameData = StaticRef<AnimBlendFrameData*>(0xB5F87C);

// 0x4D63E0
static void FrameFindByBoneTagCB(AnimBlendFrameData* fd, void* data) {
    if (fd->BoneTag == reinterpret_cast<uint32>(data)) {
        gpAnimBlendFrameData = fd;
    }
}

// 0x4D6400
AnimBlendFrameData* RpAnimBlendClumpFindBone(RpClump* clump, eBoneTag32 boneTag) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    gpAnimBlendFrameData = nullptr;
    bd->ForAllFrames(FrameFindByBoneTagCB, reinterpret_cast<void*>(static_cast<uint32>(static_cast<eBoneTag>(boneTag))));
    return gpAnimBlendFrameData;
}

// 0x4D6240
static void FrameFindByNameCBskin(AnimBlendFrameData* fd, void* data) {
    const auto boneName = ConvertBoneTag2BoneName((eBoneTag)fd->BoneTag);
    if (boneName && !_stricmp(static_cast<const char*>(data), boneName)) {
        gpAnimBlendFrameData = fd;
    }
}

// 0x4D6270
static void FrameFindByNameCBnonskin(AnimBlendFrameData* fd, void* data) {
    if (!_stricmp(static_cast<const char*>(data), GetFrameNodeName(fd->Frame))) {
        gpAnimBlendFrameData = fd;
    }
}

// 0x4D62A0
AnimBlendFrameData* RpAnimBlendClumpFindFrame(RpClump* clump, const char* needle) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    // Como el original: _stricmp (0x8229B6) y gana la última coincidencia
    gpAnimBlendFrameData = nullptr;
    if (IsClumpSkinned(clump)) {
        bd->ForAllFrames(FrameFindByNameCBskin, const_cast<char*>(needle));
    } else {
        bd->ForAllFrames(FrameFindByNameCBnonskin, const_cast<char*>(needle));
    }
    return gpAnimBlendFrameData;
}

// 0x4D6310
static void FrameFindByKeyCBskin(AnimBlendFrameData* fd, void* data) {
    const auto boneName = ConvertBoneTag2BoneName((eBoneTag)fd->BoneTag);
    if (boneName && CKeyGen::GetUppercaseKey(boneName) == reinterpret_cast<uint32>(data)) {
        gpAnimBlendFrameData = fd;
    }
}

// 0x4D6340
static void FrameFindByKeyCBnonskin(AnimBlendFrameData* fd, void* data) {
    if (CKeyGen::GetUppercaseKey(GetFrameNodeName(fd->Frame)) == reinterpret_cast<uint32>(data)) {
        gpAnimBlendFrameData = fd;
    }
}

// 0x4D6370
AnimBlendFrameData* RpAnimBlendClumpFindFrameFromHashKey(RpClump* clump, uint32 boneNameKey) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    gpAnimBlendFrameData = nullptr;
    if (IsClumpSkinned(clump)) {
        bd->ForAllFrames(FrameFindByKeyCBskin, reinterpret_cast<void*>(boneNameKey));
    } else {
        bd->ForAllFrames(FrameFindByKeyCBnonskin, reinterpret_cast<void*>(boneNameKey));
    }
    return gpAnimBlendFrameData;
}

// notsa
template<typename Fn>
CAnimBlendAssociation* RpAnimBlendClumpFindAssociationIf_N(RpClump* clump, Fn&& Pred, size_t n) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    for (auto l = bd->m_AnimList.next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        if (Pred(*a)) {
            if (n-- == 0) {
                return a;
            }
        }
        l = a->GetLink().next;
    }
    return nullptr;   
}

// 0x4D68E0
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, bool, CAnimBlendHierarchy* h) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [h](const CAnimBlendAssociation& a) {
        return a.GetHier() == h;
    }, 0);
}

// 0x4D6870
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, const char* name) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [nameKey = CKeyGen::GetUppercaseKey(name)](const CAnimBlendAssociation& a){
        return a.GetHashKey() == nameKey;
    }, 0);
}

// AnimationId animId
// 0x4D68B0
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, uint32 animId) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [animId](const CAnimBlendAssociation& a){
        return a.GetAnimId() == animId;
    }, 0);
}

// 0x4D15E0
CAnimBlendAssociation* RpAnimBlendClumpGetFirstAssociation(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    return RpAnimBlendClumpIsInitialized(clump) && bd->m_AnimList.next
        ? CAnimBlendAssociation::FromLink(bd->m_AnimList.next)
        : nullptr;
}

// 0x4D6A70
CAnimBlendAssociation* RpAnimBlendClumpGetFirstAssociation(RpClump* clump, uint32 flags) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [flags](const CAnimBlendAssociation& a) {
        return a.m_Flags & flags;
    }, 0);
}

// 0x4D6910
CAnimBlendAssociation* RpAnimBlendClumpGetMainAssociation(RpClump* clump, CAnimBlendAssociation** pp2ndAnim, float* pBlendVal2nd) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    CAnimBlendAssociation *aA{}, *aB{};
    float                  bA{}, bB{};
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (a->IsPartial()) {
            return;
        }
        const auto blend = a->GetBlendAmount();
        if (blend > bA) { // Found a new main?
            aB = std::exchange(aA, a);
            bB = std::exchange(bA, blend);
        } else if (blend > bB) { // Found a new secondary?
            aB = a;
            bB = blend;
        }
    });

    if (pp2ndAnim) {
        *pp2ndAnim = aB;
    }
    if (pBlendVal2nd) {
        *pBlendVal2nd = bB;
    }

    return aA;

    /*
    * Code works, but i think it I overcomplicated it....
    // Array sorted descending by blend amount
    struct {
        CAnimBlendAssociation* anim{};
        float                  blendAmnt{};
    } sorted[2];

    // Fill array
    for (auto l = bd->m_AnimList.next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        if (!a->IsPartial()) {
            const auto it = rng::upper_bound(
                sorted,
                a->GetBlendAmount(),
                [](float l, float r) { return l > r; }, // Descending
                [](auto& v) { return v.blendAmnt; }     // By blend amount
            );
            if (it != std::end(sorted)) {
                std::shift_right(it, std::end(sorted), 1); // Make space for insert
                *it = { a, a->GetBlendAmount() };
            }
        }
        l = a->GetLink().next;
    }

    if (pp2ndAnim) {
        *pp2ndAnim = sorted[1].anim;
    }
    if (pBlendVal2nd) {
        *pBlendVal2nd = sorted[1].blendAmnt;
    }

    return sorted[0].anim;
    */
}

// 0x4D69F0
CAnimBlendAssociation* RpAnimBlendClumpGetMainAssociation_N(RpClump* clump, uint32 n) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [](const CAnimBlendAssociation& a){
        return !a.IsPartial();
    }, n);
}

// 0x4D69A0
CAnimBlendAssociation* RpAnimBlendClumpGetMainPartialAssociation(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    CAnimBlendAssociation *mA{};
    float                  mB{};
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (!a->IsPartial()) {
            return;
        }
        if (a->GetBlendAmount() > mB) {
            mA = a;
            mB = a->GetBlendAmount();
        }
    });
    return mA;
}

// 0x4D6A30
CAnimBlendAssociation* RpAnimBlendClumpGetMainPartialAssociation_N(RpClump* clump, int32 n) {
    return RpAnimBlendClumpFindAssociationIf_N(clump, [](const CAnimBlendAssociation& a){
        return a.IsPartial();
    }, n);
}

// notsa
template<typename Fn>
uint32 RpAnimBlendClumpCountAssociationsIf(RpClump* clump, Fn&& Pred) {
    // Como el original: se recorre desde `m_AnimList.next` (el iterador de CAnimBlendLink empieza en la propia
    // cabeza y contaba una asociación falsa en `bd - 4`)
    uint32 count = 0;
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (Pred(*a)) {
            count++;
        }
    });
    return count;
}

// 0x4D6B60
uint32 RpAnimBlendClumpGetNumAssociations(RpClump* clump) {
    return RpAnimBlendClumpCountAssociationsIf(clump, [](const CAnimBlendAssociation& a){
        return true; // Count all
    });
}

// 0x4D6BB0
uint32 RpAnimBlendClumpGetNumNonPartialAssociations(RpClump* clump) {
    return RpAnimBlendClumpCountAssociationsIf(clump, [](const CAnimBlendAssociation& a){
        return !a.IsPartial();
    });
}

// 0x4D6B80
uint32 RpAnimBlendClumpGetNumPartialAssociations(RpClump* clump) {
    return RpAnimBlendClumpCountAssociationsIf(clump, [](const CAnimBlendAssociation& a){
        return a.IsPartial();
    });
}

// 0x4D6760
bool RpAnimBlendClumpIsInitialized(RpClump* clump) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    return bd && bd->m_NumFrameData;
}

// 0x4D6B00
void RpAnimBlendClumpPauseAllAnimations(RpClump* clump) {
    // Como el original: desde `m_AnimList.next` (el range-for sobre CAnimBlendLink empezaba en la cabeza)
    RpAnimBlendClumpForEachAssociation(clump, [](CAnimBlendAssociation* a) {
        a->SetFlag(ANIMATION_IS_PLAYING, false);
    });
}

// 0x4D6B30
void RpAnimBlendClumpUnPauseAllAnimations(RpClump* clump) {
    // Como el original: desde `m_AnimList.next` (el range-for sobre CAnimBlendLink empezaba en la cabeza)
    RpAnimBlendClumpForEachAssociation(clump, [](CAnimBlendAssociation* a) {
        a->SetFlag(ANIMATION_IS_PLAYING, true);
    });
}


// 0x4D6C00
void RpAnimBlendClumpRemoveAllAssociations(RpClump* clump) {
    RpAnimBlendClumpForEachAssociation(clump, [](CAnimBlendAssociation* a) { // Musn't use a for loop here, because it doesn't pre-cache `next`
        delete a;
    });
}

// 0x4D6820
void RpAnimBlendClumpRemoveAssociations(RpClump* clump, uint32 flags) {
    RpAnimBlendClumpForEachAssociation(clump, [=](CAnimBlendAssociation* a) { // Musn't use a for loop here, because it doesn't pre-cache `next`
        if (!flags || (a->m_Flags & flags)) {
            delete a;
        }
    });
}

// 0x4D67E0
void RpAnimBlendClumpSetBlendDeltas(RpClump* clump, uint32 flags, float delta) {
    RpAnimBlendClumpForEachAssociation(clump, [=](CAnimBlendAssociation* a) {
        if (!flags || (a->m_Flags & flags)) {
            a->SetBlendDelta(delta);
        }
    });
}

struct AnimBlendUpdateData { // OG name
    bool32          IncludePartial{};      //!< Has non-moving anim (Eg.: Anim with MOVEMENT flag NOT set)
    CAnimBlendNode* BlendNodeArrays[12]{}; //!< Null terminated array of arrays.
                                           //!< Each entry is incremented on each call of the appropriate `FrameUpdateCallBack`
                                           //!< So that on every call they point to the blend node of the given frame
                                           //!< (This works because the sequences are sorted the same way as the frames apprear in the clump)
};

static auto& gpAnimBlendClump = StaticRef<CAnimBlendClumpData*>(0xB4EA0C);

// Aritmética float de las rutinas de animación del EXE (0x4D1680..0x4D32D0): cada `fld a; fadd b; fstp` es x87 (PC24, exponente extendido).
// El SSE de la DLL descarta los subnormales (DAZ/FTZ) y con dos NaN devuelve el primer operando (el x87 el de mayor mantisa), así que toda suma,
// resta o producto de estas rutinas va por X87Math.cpp, con los operandos en el mismo orden que el EXE (`ExeAdd(a, b)` = `fld a; fadd b`).
static float ExeAdd(float a, float b) { return RpNanAddCpp(a, b); }
static float ExeSub(float a, float b) { return RpNanSubCpp(a, b); }
static float ExeMul(float a, float b) { return X87TimesFloatCpp(a, b); }

float CalculateTotalBlendOfPartial(AnimBlendUpdateData* c, AnimBlendFrameData* fd, bool checkDontAddToBlend) {
    float sum{};
    if (c->IncludePartial) {
        for (auto it = c->BlendNodeArrays; *it; it++) {
            const auto node = *it;
            if (!node->IsValid()) {
                continue;
            }

            const auto assoc = node->GetAnimAssoc();
            if (assoc->HasFlag(ANIMATION_IS_PARTIAL) && (!checkDontAddToBlend || !assoc->HasFlag(ANIMATION_DONT_ADD_TO_PARTIAL_BLEND))) {
                sum = ExeAdd(assoc->GetBlendAmount(), sum); // fld blend; fadd suma
            }
        }
    }
    return sum;
}

#define USE_COPY_PASTE_FRAME_UPDATE 0
#ifdef USE_COPY_PASTE_FRAME_UPDATE
    #define DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS 1
#endif

#ifdef USE_COPY_PASTE_FRAME_UPDATE
#pragma region "R* Frame Update Functions"
// 0x4D1610 (CQuaternion::Normalise del EXE; los hooks de CQuaternion no están instalados)
// Como el original: longitud en registros x87 y multiplicación por el recíproco (no división); ver X87Math.cpp.
static void NormaliseQuatExe(CQuaternion* quat) {
    AnimQuaternionNormaliseCpp(&quat->x);
}

/*!
 * @brief Ruta sin extracción de velocidad de 0x4D2B90/0x4D2E40/0x4D30A0/0x4D32D0
 * @details Como el original: el blend parcial suma `blend + suma`, la escala se pasa como `1 - suma`,
 * @details las sumas van en el orden del EXE y el producto escalar de los skinned es `w·w + z·z + y·y + x·x`
 * @details (con NaN se suma, `test ah,5`). Skinned: `t = T·b` y luego `(1 - b)·BonePos + t`.
 * @details No skinned: identidad, quaternion normalizado y `pos = T + (1 - b)·FramePos` (T no se escala).
 */
template<bool IsCompressed, bool IsSkinned>
static void FrameUpdateCallBackNoVelocity(AnimBlendFrameData* fd, AnimBlendUpdateData* c) {
    float partialBlend = 0.f;
    if (c->IncludePartial) {
        for (auto it = c->BlendNodeArrays; *it; it++) {
            const auto node = *it;
            if (node->m_Seq && node->m_BlendAssoc->IsPartial()) {
                partialBlend = ExeAdd(node->m_BlendAssoc->m_BlendAmount, partialBlend);
            }
        }
    }
    const float weight = ExeSub(1.f, partialBlend); // fld 1.0; fsub suma (el EXE lo recalcula en cada nodo, siempre igual)

    CVector     nextT{ 0.f, 0.f, 0.f };
    CQuaternion nextQ{ 0.f, 0.f, 0.f, 0.f };
    float       nextBlendT = 0.f;
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;
        if (!node->m_Seq) {
            continue;
        }

        CVector     t;
        CQuaternion q;
        if constexpr (IsCompressed) {
            node->UpdateCompressed(t, q, weight);
        } else {
            node->Update(t, q, weight);
        }

        if (node->m_Seq->m_bHasTranslation) {
            nextT.x    = ExeAdd(t.x, nextT.x);
            nextT.y    = ExeAdd(t.y, nextT.y);
            nextT.z    = ExeAdd(t.z, nextT.z);
            nextBlendT = ExeAdd(node->m_BlendAssoc->m_BlendAmount, nextBlendT);
        }

        if constexpr (IsSkinned) {
            // w·w + z·z + y·y + x·x < 0 en x87 sin redondear a float (en SSE el producto desborda a inf y pierde el signo); con NaN suma
            if (AnimQuaternionDotNegativeCpp(&nextQ.x, &q.x)) {
                nextQ.x = ExeSub(nextQ.x, q.x);
                nextQ.y = ExeSub(nextQ.y, q.y);
                nextQ.z = ExeSub(nextQ.z, q.z);
                nextQ.w = ExeSub(nextQ.w, q.w);
                continue;
            }
        }
        nextQ.x = ExeAdd(q.x, nextQ.x);
        nextQ.y = ExeAdd(q.y, nextQ.y);
        nextQ.z = ExeAdd(q.z, nextQ.z);
        nextQ.w = ExeAdd(q.w, nextQ.w);
    }

    if constexpr (IsSkinned) {
        const auto kf = fd->KeyFrame;
        if (!fd->KeyFramesIgnoreNodeOrientation) {
            NormaliseQuatExe(&nextQ);
            std::memcpy(&kf->q, &nextQ, sizeof(kf->q));
        }
        if (!fd->KeyFramesIgnoreNodeTranslation) {
            kf->t.x = ExeMul(nextT.x, nextBlendT);
            kf->t.y = ExeMul(nextT.y, nextBlendT);
            kf->t.z = ExeMul(nextT.z, nextBlendT);
            const float inv = ExeSub(1.f, nextBlendT); // queda en ST0 (sin redondeo extra: 1 - b nunca sale del rango de float)
            kf->t.x = RpNanMulAddCpp(inv, fd->BonePos.x, kf->t.x); // fld inv; fmul BonePos; fadd t (el producto no se redondea a float)
            kf->t.y = RpNanMulAddCpp(inv, fd->BonePos.y, kf->t.y);
            kf->t.z = RpNanMulAddCpp(inv, fd->BonePos.z, kf->t.z);
        }
    } else {
        const auto mat = RwFrameGetMatrix(fd->Frame);
        if (!fd->KeyFramesIgnoreNodeOrientation) {
            RwMatrixSetIdentity(mat);
            NormaliseQuatExe(&nextQ);
            RpNanQuatToMatrixCpp(&nextQ.x, reinterpret_cast<float*>(mat)); // CQuaternion::Get del EXE (0x59C080) en x87; el hook de Get está apagado y el de la DLL es SSE
        }
        if (!fd->KeyFramesIgnoreNodeTranslation) {
            const auto pos = RwMatrixGetPos(mat);
            pos->x = nextT.x;
            pos->y = nextT.y;
            pos->z = nextT.z;
            const float inv = ExeSub(1.f, nextBlendT);
            pos->x = RpNanMulAddCpp(inv, fd->FramePos.x, pos->x);
            pos->y = RpNanMulAddCpp(inv, fd->FramePos.y, pos->y);
            pos->z = RpNanMulAddCpp(inv, fd->FramePos.z, pos->z);
        }
        RwMatrixUpdate(mat);
    }
}

/*!
 * @brief Extracción de velocidad de 0x4D1680/0x4D1A50/0x4D1DB0/0x4D2100/0x4D2450/0x4D27F0 (en el EXE `c` llega en EAX y `fd` en la pila; cdecl, el llamador limpia)
 * @details Una sola plantilla para las seis rutinas, que difieren en cuatro cosas (comprobadas contra el desensamblado):
 * @details - Suma de blends parciales: 0x4D1680 y 0x4D1A50 (skinned no comprimidas) excluyen DONT_ADD_TO_PARTIAL_BLEND; las otras cuatro no.
 * @details - 2D (`!Is3D`, 0x4D1680/0x4D2100): salta los nodos con IGNORE_ROOT_TRANSLATION y solo usa y (y x si CAN_EXTRACT_X_VELOCITY);
 * @details   3D: x, y y z, sin mirar IGNORE_ROOT_TRANSLATION.
 * @details - Quaternion: solo 0x4D1680 resta cuando el producto escalar es < 0 (con NaN suma); las demás siempre suman.
 * @details - Traslación final: skinned 2D corrige z con (z·2.5 + 2)·BonePos.z entre -0.8 y -0.4; no skinned 2D no suma FramePos;
 * @details   el resto suma BonePos/FramePos por componente. La posición del ped se ASIGNA (`d - c`), no se acumula.
 * @details Orden de los operandos como el EXE (`fld a; fadd b` = a + b) para que con NaN coincida el resultado.
 * @details `GetEndTranslation` escribe en el mismo `t` (el EXE reutiliza el búfer) y se suma ese `t`.
 */
template<bool IsCompressed, bool IsSkinned, bool Is3D>
static void FrameUpdateCallBackExeVelocity(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    const auto kf  = IsSkinned ? fd->KeyFrame : nullptr;                // [fd + 0x10] leído al entrar
    const auto mat = IsSkinned ? nullptr : RwFrameGetMatrix(fd->Frame); // [fd + 0x10] + 0x10 (no skinned)

    const float partialScale = ExeSub(1.f, CalculateTotalBlendOfPartial(c, fd, IsSkinned && !IsCompressed));

    CVector currV{ 0.f, 0.f, 0.f };
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;
        if (!node->m_Seq || !node->m_Seq->m_bHasTranslation) {
            continue;
        }
        const auto flags = node->m_BlendAssoc->m_Flags;
        if constexpr (!Is3D) {
            if (flags & ANIMATION_IGNORE_ROOT_TRANSLATION) {
                continue;
            }
        }
        if (!(flags & ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        if constexpr (IsCompressed) {
            node->GetCurrentTranslationCompressed(t, partialScale);
        } else {
            node->GetCurrentTranslation(t, partialScale);
        }
        if constexpr (Is3D) {
            currV.x = ExeAdd(t.x, currV.x);
            currV.y = ExeAdd(t.y, currV.y);
            currV.z = ExeAdd(t.z, currV.z);
        } else {
            currV.y = ExeAdd(currV.y, t.y);
            if (flags & ANIMATION_CAN_EXTRACT_X_VELOCITY) {
                currV.x = ExeAdd(currV.x, t.x);
            }
        }
    }

    CQuaternion nextQ{ 0.f, 0.f, 0.f, 0.f };
    CVector     nextT{ 0.f, 0.f, 0.f };
    CVector     deltaV{ 0.f, 0.f, 0.f };
    CVector     loopedDeltaV{ 0.f, 0.f, 0.f };
    bool        hasLoopedVelocity = false;
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;
        if (!node->m_Seq) {
            continue;
        }

        CVector     t;
        CQuaternion q;
        const bool  looped = IsCompressed
            ? node->UpdateCompressed(t, q, partialScale)
            : node->Update(t, q, partialScale);

        bool subtractQ = false;
        if constexpr (IsSkinned && !Is3D) {
            subtractQ = AnimQuaternionDotNegativeCpp(&nextQ.x, &q.x); // w·w + z·z + y·y + x·x < 0 (con NaN suma)
        }
        if (subtractQ) {
            nextQ.x = ExeSub(nextQ.x, q.x);
            nextQ.y = ExeSub(nextQ.y, q.y);
            nextQ.z = ExeSub(nextQ.z, q.z);
            nextQ.w = ExeSub(nextQ.w, q.w);
        } else {
            nextQ.x = ExeAdd(q.x, nextQ.x);
            nextQ.y = ExeAdd(q.y, nextQ.y);
            nextQ.z = ExeAdd(q.z, nextQ.z);
            nextQ.w = ExeAdd(q.w, nextQ.w);
        }

        if (!node->m_Seq->m_bHasTranslation) {
            continue;
        }
        const auto flags = node->m_BlendAssoc->m_Flags;
        if constexpr (!Is3D) {
            if (flags & ANIMATION_IGNORE_ROOT_TRANSLATION) {
                continue;
            }
            nextT.x = ExeAdd(nextT.x, t.x);
            nextT.y = ExeAdd(nextT.y, t.y);
            nextT.z = ExeAdd(t.z, nextT.z);
        } else {
            nextT.x = ExeAdd(t.x, nextT.x);
            nextT.y = ExeAdd(t.y, nextT.y);
            nextT.z = ExeAdd(t.z, nextT.z);
        }
        if (!(flags & ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        if constexpr (Is3D) {
            deltaV.x = ExeAdd(t.x, deltaV.x);
            deltaV.y = ExeAdd(t.y, deltaV.y);
            deltaV.z = ExeAdd(t.z, deltaV.z);
        } else {
            deltaV.y = ExeAdd(deltaV.y, t.y);
            if (flags & ANIMATION_CAN_EXTRACT_X_VELOCITY) {
                deltaV.x = ExeAdd(deltaV.x, t.x);
            }
        }

        hasLoopedVelocity |= looped;
        if (!looped) {
            continue;
        }

        if constexpr (IsCompressed) {
            node->GetEndTranslationCompressed(t, partialScale);
        } else {
            node->GetEndTranslation(t, partialScale);
        }
        if constexpr (Is3D) {
            loopedDeltaV.x = ExeAdd(t.x, loopedDeltaV.x);
            loopedDeltaV.y = ExeAdd(t.y, loopedDeltaV.y);
            loopedDeltaV.z = ExeAdd(t.z, loopedDeltaV.z);
        } else {
            loopedDeltaV.y = ExeAdd(loopedDeltaV.y, t.y);
            if (flags & ANIMATION_CAN_EXTRACT_X_VELOCITY) {
                loopedDeltaV.x = ExeAdd(loopedDeltaV.x, t.x);
            }
        }
    }

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        if constexpr (IsSkinned) {
            NormaliseQuatExe(&nextQ);
            std::memcpy(&kf->q, &nextQ, sizeof(kf->q));
        } else {
            RwMatrixSetIdentity(mat);
            NormaliseQuatExe(&nextQ);
            RpNanQuatToMatrixCpp(&nextQ.x, reinterpret_cast<float*>(mat)); // CQuaternion::Get del EXE (0x59C080) en x87; el hook de Get está apagado y el de la DLL es SSE
        }
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Posición del ped: se asigna `delta - actual` y luego se suma la parte del bucle (no se acumula)
        const auto ws = gpAnimBlendClump->m_PedPosition;
        ws->x = ExeSub(deltaV.x, currV.x);
        ws->y = ExeSub(deltaV.y, currV.y);
        if constexpr (Is3D) {
            ws->z = ExeSub(deltaV.z, currV.z);
        }
        if (hasLoopedVelocity) {
            ws->x = ExeAdd(loopedDeltaV.x, ws->x);
            ws->y = ExeAdd(loopedDeltaV.y, ws->y);
            if constexpr (Is3D) {
                ws->z = ExeAdd(loopedDeltaV.z, ws->z);
            }
        }

        // Traslación del frame: x, y, z consecutivos (kf->t o la posición de la matriz)
        float* const tr = IsSkinned ? &kf->t.x : &RwMatrixGetPos(mat)->x;
        tr[0] = ExeSub(nextT.x, deltaV.x);
        tr[1] = ExeSub(nextT.y, deltaV.y);
        if constexpr (Is3D) {
            tr[2] = ExeSub(nextT.z, deltaV.z);
            tr[0] = ExeAdd(fd->BonePos.x, tr[0]);
            tr[1] = ExeAdd(fd->BonePos.y, tr[1]);
            tr[2] = ExeAdd(fd->BonePos.z, tr[2]);
        } else {
            tr[2] = nextT.z; // copia de bits (mov), sin aritmética
            if constexpr (IsSkinned) {
                const float z = tr[2];
                if (!(z < -0.8f)) { // con NaN también entra (jp del EXE)
                    // entre -0.8 y -0.4: ((z·2.5 + 2)·BonePos.z) + z en una sola cadena x87 (sin redondear a float entre operaciones); si no, BonePos.z + z
                    tr[2] = z < -0.4f ? RpNanSkinZCorrectCpp(z, fd->BonePos.z) : ExeAdd(fd->BonePos.z, z);
                }
                tr[0] = ExeAdd(fd->BonePos.x, tr[0]);
                tr[1] = ExeAdd(fd->BonePos.y, tr[1]);
            }
        }
    }

    if constexpr (!IsSkinned) {
        RwMatrixUpdate(mat);
    }
}

// 0x4D1DB0
void FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    FrameUpdateCallBackExeVelocity<true, true, true>(c, fd); // comprimida, skinned, 3D (0x4D1DB0)
}

// 0x4D2E40
// FrameUpdateCallBackT<true, true, false, false>
void FrameUpdateCallBackCompressedSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(c, fd);
        return;
    }

    FrameUpdateCallBackNoVelocity<true, true>(fd, c);
}

// 0x4D27F0
// FrameUpdateCallBackT<true, false, true, true>
void FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    FrameUpdateCallBackExeVelocity<true, false, true>(c, fd); // comprimida, no skinned, 3D (0x4D27F0)
}

// 0x4D32D0
// FrameUpdateCallBackT<true, false, false, false>
void FrameUpdateCallBackCompressedNonSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned(c, fd);
        return;
    }

    FrameUpdateCallBackNoVelocity<true, false>(fd, c);
}

// 0x4D1A50
// FrameUpdateCallBackT<false, true, true, true>
void FrameUpdateCallBackSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) { // `c` is passed in `eax`, can't hook
    FrameUpdateCallBackExeVelocity<false, true, true>(c, fd); // skinned, 3D (0x4D1A50)
}

// 0x4D1680
// FrameUpdateCallBackT<false, true, true, false>
void FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) { // `c` is passed in `eax`, can't hook
    FrameUpdateCallBackExeVelocity<false, true, false>(c, fd); // skinned, 2D (0x4D1680)
}

// 0x4D2B90
// FrameUpdateCallBackT<false, true, false, false>
void FrameUpdateCallBackSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        if (fd->HasZVelocity) {
            FrameUpdateCallBackSkinnedWith3dVelocityExtraction(c, fd);
        } else {
            FrameUpdateCallBackSkinnedWithVelocityExtraction(c, fd);
        }
        return;
    }

    FrameUpdateCallBackNoVelocity<false, true>(fd, c);
}

// 0x4D2450
// FrameUpdateCallBackT<false, false, true, true>
void FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    FrameUpdateCallBackExeVelocity<false, false, true>(c, fd); // no skinned, 3D (0x4D2450)
}

// 0x4D2100
// FrameUpdateCallBackT<false, false, true, false>
void FrameUpdateCallBackNonSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    FrameUpdateCallBackExeVelocity<false, false, false>(c, fd); // no skinned, 2D (0x4D2100)
}

// 0x4D30A0
// FrameUpdateCallBackT<false, false, false, false>
void FrameUpdateCallBackNonSkinned(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        if (fd->HasZVelocity) {
            FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction(c, fd);
        } else {
            FrameUpdateCallBackNonSkinnedWithVelocityExtraction(c, fd);
        }
        return;
    }

    FrameUpdateCallBackNoVelocity<false, false>(fd, c);
}

#pragma endregion
#endif

// 0x4D2E10 (callback de frames fuera de pantalla; cdecl, el llamador limpia). Solo extrae la velocidad 2D de skinned no comprimida (0x4D1680) y no toca la pose.
void FrameUpdateCallBackOffscreen(AnimBlendFrameData* fd, void* data) {
    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        FrameUpdateCallBackSkinnedWithVelocityExtraction(static_cast<AnimBlendUpdateData*>(data), fd);
    }
}

// 0x4D1570
void RpAnimBlendNodeUpdateKeyFrames(AnimBlendUpdateData* c, AnimBlendFrameData* frames, uint32 nFrames) {
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto nodeArray = *it; // #nodes == nFrames

        const auto assoc = nodeArray[0].GetAnimAssoc();
        for (size_t frameN = 0; frameN < nFrames; frameN++) {
            const auto fd = &frames[frameN];

            if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
                continue;
            }

            const auto node = &nodeArray[frameN];
            if (!node->IsValid()) {
                continue;
            }

            node->FindKeyFrame(assoc->GetCurrentTime());
        }
    }
}

// 0x4D34F0
void RpAnimBlendClumpUpdateAnimations(RpClump* clump, float timeStep, bool isOnScreen) {
    const auto bd = RpAnimBlendClumpGetData(clump);

    gpAnimBlendClump = bd;

    if (bd->m_AnimList.IsEmpty()) {
        return;
    }

    AnimBlendUpdateData ctx{};
    size_t             nodesCnt{};

    float totalTime{}, totalBlendAmnt{};

    // 0x4D351F
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (!a->UpdateBlend(timeStep)) {
            return;
        }
        const auto ah = a->GetHier();
        if (!ah->m_pSequences) {
            return;
        }
        CAnimManager::UncompressAnimation(a->GetHier());
        if (nodesCnt + 1 <= std::size(ctx.BlendNodeArrays) - 1) { // - 1 for null terminator
            ctx.BlendNodeArrays[nodesCnt++] = a->GetNodesPtr();
        }
        if (a->IsSyncronised()) {
            // 0x4D3578: fld total; fdiv speed; fmul blend; fadd totalTime; fstp (la x87 redondea a 24 bits en cada paso; con doubles de SSE solo al final).
            // Se lee m_BlendAmount sin pasar por GetBlendAmount() (que multiplica por 1.0 en SSE: convierte NaN señalizantes y descarta subnormales).
            totalTime      = RpUpdateSyncAccumCpp(ah->GetTotalTime(), a->GetSpeed(), a->m_BlendAmount, totalTime);
            totalBlendAmnt = ExeAdd(totalBlendAmnt, a->m_BlendAmount); // 0x4D3589: fld totalBlend; fadd blend; fstp
        } else {
            ctx.IncludePartial = true;        
        }
    });
    ctx.BlendNodeArrays[nodesCnt] = nullptr; // Null terminator

    // 0x4D35A2 - Update animation's timesteps
    // 0x4D35B0: fcomp 0.0 (comparación ordenada de x87: con NaN el EXE sigue por la rama de la división; un subnormal no es cero)
    const bool totalTimeIsZero = CloudsEqualOrderedCpp(totalTime, 0.f);
    const auto animTimeMult    = totalTimeIsZero ? 1.f : totalBlendAmnt / totalTime; // UpdateTime lo ignora (0x4D3720..0x4D3751)
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (!a->IsPlaying()) {
            return;
        }
        // Como el EXE (0x4D35C4..0x4D3605): una cadena x87 sin guardar entre operaciones (cada una redondea a 24 bits) y un solo fstp.
        // Sincronizada: total/totalTime·totalBlend·timeStep (o total·timeStep con totalTime == 0); libre: speed·timeStep.
        if (!a->IsSyncronised()) {
            a->m_TimeStep = ExeMul(a->GetSpeed(), timeStep);
        } else if (totalTimeIsZero) {
            a->m_TimeStep = ExeMul(a->GetHier()->GetTotalTime(), timeStep);
        } else {
            a->m_TimeStep = RpUpdateSyncStepCpp(a->GetHier()->GetTotalTime(), totalTime, totalBlendAmnt, timeStep);
        }
    });

    // 0x4D360E - Update all animations's frames
    const auto rootFD = &bd->GetRootFrameData();
    rootFD->IsUpdatingFrame = true;

    if (rootFD->IsCompressed) {
        // Respect the independently selectable frame-update hooks. Their C++
        // replacements are disabled by default and are outside this function.
        const auto callback = IsClumpSkinned(clump) ? 0x4D2E40u : 0x4D32D0u;
        bd->ForAllFrames(reinterpret_cast<void(*)(AnimBlendFrameData*, void*)>(callback), &ctx);
    } else if (isOnScreen) {
        if (rootFD->NeedsKeyFrameUpdate) {
            RpAnimBlendNodeUpdateKeyFrames(&ctx, bd->m_FrameDatas, bd->m_NumFrameData);
        }

        const auto callback = IsClumpSkinned(clump) ? 0x4D2B90u : 0x4D30A0u;
        bd->ForAllFrames(reinterpret_cast<void(*)(AnimBlendFrameData*, void*)>(callback), &ctx);

        rootFD->NeedsKeyFrameUpdate = false;
    } else {
        bd->ForAllFrames(reinterpret_cast<void(*)(AnimBlendFrameData*, void*)>(0x4D2E10), &ctx);
        rootFD->NeedsKeyFrameUpdate = true;
    }

    // 0x4D3715 - Update all animation's times
    // Unlike UpdateBlend above, the original reads the next link after the
    // finish callback. Preserve this order if a callback changes the list.
    for (auto* link = bd->m_AnimList.next; link; link = link->next) {
        CAnimBlendAssociation::FromLink(link)->UpdateTime(timeStep, animTimeMult);
    }
    
    // 0x4D3764
    RwFrameUpdateObjects(RpClumpGetFrame(clump));
}

// 0x4D60E0
RtAnimAnimation* RpAnimBlendCreateAnimationForHierarchy(RpHAnimHierarchy* hierarchy) {
    if (!hierarchy) {
        return nullptr;
    }
    const auto rtA = RtAnimAnimationCreate(
        rwID_RPANIMBLENDPLUGIN,
        0,
        0,
        0.f
    );
    if (rtA) {
        rtA->numFrames = 2 * hierarchy->numNodes;
    }
    return rtA;
}

// 0x4D5EF0
const char* RpAnimBlendFrameGetName(RwFrame* frame) {
    return GetFrameNodeName(frame);
}

// 0x4D5F00
RwFrame* RpAnimBlendFrameSetName(RwFrame* frame, const char* name) {
    SetFrameNodeName(frame, name);
    return frame; // Como el original: devuelve el frame (EAX)
}

// 0x4D6AB0
CAnimBlendAssociation* RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association) {
    const auto next = association->GetLink().next;
    return next
        ? CAnimBlendAssociation::FromLink(next)
        : nullptr;
}

// 0x4D6AD0
CAnimBlendAssociation* RpAnimBlendGetNextAssociation(CAnimBlendAssociation* association, uint32 flags) {
    // Como el original: sin assert; con flags 0 recorre la lista y devuelve nulo
    for (auto l = association->GetLink().next; l;) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        l            = a->GetLink().next;
        if (a->m_Flags & flags) {
            return a;
        }
    }
    return nullptr;
}

void RpAnimBlendPlugin::InjectHooks() {
    RH_ScopedNamespaceName("RpAnimBlend");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RpAnimBlendPluginAttach, 0x4D6150);
    RH_ScopedGlobalInstall(RpAnimBlendClumpInit, 0x4D6720);
    RH_ScopedGlobalInstall(ClumpAnimConstruct, 0x4D5F40);
    RH_ScopedGlobalInstall(ClumpAnimDestruct, 0x4D6110);
    RH_ScopedGlobalInstall(ClumpAnimCopy, 0x4D5F90);
    RH_ScopedGlobalInstall(RpAnimBlendAllocateData, 0x4D5F50);
    RH_ScopedGlobalInstall(RtAnimBlendKeyFrameApply, 0x4D5FA0);
    RH_ScopedGlobalInstall(RpAnimBlendKeyFrameInterpolate, 0x4D60C0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpAddAssociation, 0x4D6790);
    RH_ScopedGlobalInstall(RpAnimBlendClumpExtractAssociations, 0x4D6BE0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGiveAssociations, 0x4D6C30);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFillFrameArray, 0x4D64A0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFindBone, 0x4D6400);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFindFrame, 0x4D62A0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpFindFrameFromHashKey, 0x4D6370);
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetAssociation, "Hier", 0x4D68E0, CAnimBlendAssociation*(*)(RpClump*, bool, CAnimBlendHierarchy*));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetAssociation, "AnimName", 0x4D6870, CAnimBlendAssociation*(*)(RpClump*, const char*));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetAssociation, "AnimId", 0x4D68B0, CAnimBlendAssociation*(*)(RpClump*, uint32));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetFirstAssociation, "", 0x4D15E0, CAnimBlendAssociation*(*)(RpClump*));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendClumpGetFirstAssociation, "Flags", 0x4D6A70, CAnimBlendAssociation*(*)(RpClump*, uint32));
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainAssociation, 0x4D6910);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainAssociation_N, 0x4D69F0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainPartialAssociation, 0x4D69A0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetMainPartialAssociation_N, 0x4D6A30);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetNumAssociations, 0x4D6B60);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetNumNonPartialAssociations, 0x4D6BB0);
    RH_ScopedGlobalInstall(RpAnimBlendClumpGetNumPartialAssociations, 0x4D6B80);
    RH_ScopedGlobalInstall(RpAnimBlendClumpIsInitialized, 0x4D6760);
    RH_ScopedGlobalInstall(RpAnimBlendClumpPauseAllAnimations, 0x4D6B00);
    RH_ScopedGlobalInstall(RpAnimBlendClumpUnPauseAllAnimations, 0x4D6B30);
    RH_ScopedGlobalInstall(RpAnimBlendClumpRemoveAllAssociations, 0x4D6C00);
    RH_ScopedGlobalInstall(RpAnimBlendClumpRemoveAssociations, 0x4D6820);
    RH_ScopedGlobalInstall(RpAnimBlendClumpSetBlendDeltas, 0x4D67E0);
    RH_ScopedGlobalInstall(RpAnimBlendCreateAnimationForHierarchy, 0x4D60E0);
    RH_ScopedGlobalInstall(RpAnimBlendFrameGetName, 0x4D5EF0);
    RH_ScopedGlobalInstall(RpAnimBlendFrameSetName, 0x4D5F00);
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendGetNextAssociation, "Any", 0x4D6AB0, CAnimBlendAssociation * (*)(CAnimBlendAssociation * association));
    RH_ScopedGlobalOverloadedInstall(RpAnimBlendGetNextAssociation, "Flags", 0x4D6AD0, CAnimBlendAssociation * (*)(CAnimBlendAssociation * association, uint32 flags));

    RH_ScopedGlobalInstall(RpAnimBlendClumpUpdateAnimations, 0x4D34F0);
#ifdef USE_COPY_PASTE_FRAME_UPDATE
    // Most of these functions aren't hookable (without effort) they take args in <eax> and whatnot, they aren't regular `__cdecl` calls
    //RH_ScopedGlobalInstall(FrameUpdateCallBackWithVelocityExtractionCompressedSkinned, 0x4D1DB0);
    RH_ScopedGlobalInstall(FrameUpdateCallBackCompressedSkinned, 0x4D2E40, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    //RH_ScopedGlobalInstall(FrameUpdateCallBackWithVelocityExtractionCompressedNonSkinned, 0x4D27F0);
    RH_ScopedGlobalInstall(FrameUpdateCallBackCompressedNonSkinned, 0x4D32D0, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    //RH_ScopedGlobalInstall(FrameUpdateCallBackSkinnedWith3dVelocityExtraction, 0x4D1A50);
    //RH_ScopedGlobalInstall(FrameUpdateCallBackSkinnedWithVelocityExtraction, 0x4D1680);
    RH_ScopedGlobalInstall(FrameUpdateCallBackSkinned, 0x4D2B90, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    //RH_ScopedGlobalInstall(FrameUpdateCallBackNonSkinnedWith3dVelocityExtraction, 0x4D2450);
    //RH_ScopedGlobalInstall(FrameUpdateCallBackNonSkinnedWithVelocityExtraction, 0x4D2100);
    RH_ScopedGlobalInstall(FrameUpdateCallBackNonSkinned, 0x4D30A0, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
    RH_ScopedGlobalInstall(FrameUpdateCallBackOffscreen, 0x4D2E10, {.enabled = !DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS});
#endif
}
