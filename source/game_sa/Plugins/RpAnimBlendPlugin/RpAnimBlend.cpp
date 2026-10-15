/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpAnimBlend
 * Reconstrucción: COMPLETA — 43/43 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 43/43 entradas.
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
                sum += assoc->GetBlendAmount();
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
                partialBlend = node->m_BlendAssoc->m_BlendAmount + partialBlend;
            }
        }
    }

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
            node->UpdateCompressed(t, q, 1.f - partialBlend);
        } else {
            node->Update(t, q, 1.f - partialBlend);
        }

        if (node->m_Seq->m_bHasTranslation) {
            nextT.x    = t.x + nextT.x;
            nextT.y    = t.y + nextT.y;
            nextT.z    = t.z + nextT.z;
            nextBlendT = node->m_BlendAssoc->m_BlendAmount + nextBlendT;
        }

        if constexpr (IsSkinned) {
            const float dot = q.w * nextQ.w + q.z * nextQ.z + q.y * nextQ.y + q.x * nextQ.x;
            if (dot < 0.f) {
                nextQ.x = nextQ.x - q.x;
                nextQ.y = nextQ.y - q.y;
                nextQ.z = nextQ.z - q.z;
                nextQ.w = nextQ.w - q.w;
                continue;
            }
        }
        nextQ.x = q.x + nextQ.x;
        nextQ.y = q.y + nextQ.y;
        nextQ.z = q.z + nextQ.z;
        nextQ.w = q.w + nextQ.w;
    }

    if constexpr (IsSkinned) {
        const auto kf = fd->KeyFrame;
        if (!fd->KeyFramesIgnoreNodeOrientation) {
            NormaliseQuatExe(&nextQ);
            std::memcpy(&kf->q, &nextQ, sizeof(kf->q));
        }
        if (!fd->KeyFramesIgnoreNodeTranslation) {
            kf->t.x = nextT.x * nextBlendT;
            kf->t.y = nextT.y * nextBlendT;
            kf->t.z = nextT.z * nextBlendT;
            const float inv = 1.f - nextBlendT;
            kf->t.x = inv * fd->BonePos.x + kf->t.x;
            kf->t.y = inv * fd->BonePos.y + kf->t.y;
            kf->t.z = inv * fd->BonePos.z + kf->t.z;
        }
    } else {
        const auto mat = RwFrameGetMatrix(fd->Frame);
        if (!fd->KeyFramesIgnoreNodeOrientation) {
            RwMatrixSetIdentity(mat);
            NormaliseQuatExe(&nextQ);
            nextQ.Get(mat);
        }
        if (!fd->KeyFramesIgnoreNodeTranslation) {
            const auto pos = RwMatrixGetPos(mat);
            pos->x = nextT.x;
            pos->y = nextT.y;
            pos->z = nextT.z;
            const float inv = 1.f - nextBlendT;
            pos->x = inv * fd->FramePos.x + pos->x;
            pos->y = inv * fd->FramePos.y + pos->y;
            pos->z = inv * fd->FramePos.z + pos->z;
        }
        RwMatrixUpdate(mat);
    }
}

// 0x4D1DB0
void FrameUpdateCallBackWithVelocityExtractionCompressedSkinned(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    // Como el original: sin asserts de HasVelocity/HasZVelocity (las comprimidas se llaman sin mirar HasZVelocity)

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D1DB0
        call ecx
        add esp, 4 // Como el original: cdecl, el llamador limpia el argumento
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslationCompressed(t, partialScale);
        currV += t;
    }

    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->UpdateCompressed(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslationCompressed(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        fd->KeyFrame->t = nextT - deltaV + fd->BonePos;
    }
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
    // Como el original: sin asserts de HasVelocity/HasZVelocity (las comprimidas se llaman sin mirar HasZVelocity)

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D27F0
        call ecx
        add esp, 4 // Como el original: cdecl, el llamador limpia el argumento
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector currV{}; // 3D Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslationCompressed(t, partialScale);
        currV += t;
    }

    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->UpdateCompressed(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslationCompressed(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        CVector t = nextT - deltaV + fd->BonePos;
        RwV3dAssign(RwMatrixGetPos(fmat), &t);
    }

    RwMatrixUpdate(fmat);
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
    // Como el original: sin asserts de HasVelocity/HasZVelocity (las comprimidas se llaman sin mirar HasZVelocity)

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D1A50
        call ecx
        add esp, 4 // Como el original: cdecl, el llamador limpia el argumento
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, true);

    // 0x4D1B32
    CVector currV{}; // 3D Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV += t;
    }

    // 0x4D1B97
    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    // 0x4D1CB2
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    // 0x4D1CD7
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        fd->KeyFrame->t = nextT - deltaV + fd->BonePos;
    }
}

// 0x4D1680
// FrameUpdateCallBackT<false, true, true, false>
void FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) { // `c` is passed in `eax`, can't hook
    // Como el original: sin asserts de HasVelocity/HasZVelocity (las comprimidas se llaman sin mirar HasZVelocity)

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D1680
        call ecx
        add esp, 4 // Como el original: cdecl, el llamador limpia el argumento
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, true);

    // 0x4D174C
    CVector2D currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION) || !assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            currV.x += t.x;
        }
    }

    // 0x4D17C4
    CQuaternion nextQ{}; // Rotation
    CVector     nextT{}; // Translation
    CVector2D   deltaV{}, loopedDeltaV{}; // 2D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, partialScale);

        nextQ = q.Dot(nextQ) >= 0.f
            ? nextQ + q
            : nextQ - q;

        if (!node->GetRootKF()->HasTranslation() || assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            deltaV.x += t.x;
        }

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, partialScale);
            loopedDeltaV.y += t2.y;
            if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
                loopedDeltaV.x += t2.x;
            }
        }
    }

    // 0x4D195C
    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();
        fd->KeyFrame->q = nextQ;
    }

    // 0x4D1980
    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        wsPos->x = deltaV.x - currV.x;
        wsPos->y = deltaV.y - currV.y;
        if (hasLoopedVelocity) {
            wsPos->x += loopedDeltaV.x;
            wsPos->y += loopedDeltaV.y;
        }

        // Update key-frame translation
        const auto kfT = &fd->KeyFrame->t;
        *kfT = nextT - CVector{deltaV};
        kfT->x += fd->BonePos.x;
        kfT->y += fd->BonePos.y;
        if (kfT->z >= -0.8f) {
            kfT->z += kfT->z >= -0.4f
                ? fd->BonePos.z
                : (kfT->z * 2.5f + 2.f) * fd->BonePos.z;
        }
    }
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
    // Como el original: sin asserts de HasVelocity/HasZVelocity (las comprimidas se llaman sin mirar HasZVelocity)

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D2450
        call ecx
        add esp, 4 // Como el original: cdecl, el llamador limpia el argumento
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV += t;
    }

    CQuaternion nextQ{};
    CVector     nextT{}; // Translation
    CVector     deltaV{}, loopedDeltaV{}; // 3D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, blendScale);

        nextQ = nextQ + q;

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV += t;

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, blendScale);
            loopedDeltaV += t;
        }
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        *wsPos += deltaV - currV;
        if (hasLoopedVelocity) {
            *wsPos += loopedDeltaV;
        }

        // Update key-frame translation
        CVector t = nextT - deltaV + fd->BonePos;
        RwV3dAssign(RwMatrixGetPos(fmat), &t);
    }

    RwMatrixUpdate(fmat);
}

// 0x4D2100
// FrameUpdateCallBackT<false, false, true, false>
void FrameUpdateCallBackNonSkinnedWithVelocityExtraction(AnimBlendUpdateData* c, AnimBlendFrameData* fd) {
    // Como el original: sin asserts de HasVelocity/HasZVelocity (las comprimidas se llaman sin mirar HasZVelocity)

#if DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS
    _asm {
        mov eax, c
        push fd
        mov ecx, 0x4D2100
        call ecx
        add esp, 4 // Como el original: cdecl, el llamador limpia el argumento
    };
    return;
#endif

    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, false);

    CVector2D currV{}; // Velocity
    for (auto it = c->BlendNodeArrays; *it; it++) {
        const auto node = *it;

        if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();
        if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION) || !assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector t;
        node->GetCurrentTranslation(t, partialScale);
        currV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            currV.x += t.x;
        }
    }

    CQuaternion nextQ{}; // Rotation
    CVector     nextT{}; // Translation
    CVector2D   deltaV{}, loopedDeltaV{}; // 2D Velocity
    bool        hasLoopedVelocity{};
    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        CVector t;
        CQuaternion q;
        const auto looped = node->Update(t, q, partialScale);

        nextQ = q.Dot(nextQ) >= 0.f
            ? nextQ + q
            : nextQ - q;

        if (!node->GetRootKF()->HasTranslation() || assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) {
            continue;
        }

        nextT += t;

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        deltaV.y += t.y;
        if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
            deltaV.x += t.x;
        }

        if (hasLoopedVelocity |= looped) {
            CVector t2;
            node->GetEndTranslation(t2, partialScale);
            loopedDeltaV.y += t2.y;
            if (assoc->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY)) {
                loopedDeltaV.x += t2.x;
            }
        }
    }

    const auto fmat = RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        RwMatrixSetIdentity(fmat);
        nextQ.Normalise();
        nextQ.Get(fmat);
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Update world positions (This moves the ped around the world)
        const auto wsPos = gpAnimBlendClump->m_PedPosition;
        wsPos->x = deltaV.x - currV.x;
        wsPos->y = deltaV.y - currV.y;
        if (hasLoopedVelocity) {
            wsPos->x += loopedDeltaV.x;
            wsPos->y += loopedDeltaV.y;
        }

        // Update key-frame translation
        CVector t = nextT - CVector{deltaV};
        t.x += fd->BonePos.x;
        t.y += fd->BonePos.y;
        RwV3dAssign(RwMatrixGetPos(fmat), &t);

        //if (kfT->z >= -0.8f) {
        //    kfT->z += kfT->z >= -0.4f
        //        ? fd->BonePos.z
        //        : (kfT->z * 2.5f + 2.f) * fd->BonePos.z;
        //}
    }

    RwMatrixUpdate(fmat);
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

template<bool IsCompressed, bool IsSkinned, bool ExtractVelocity, bool Extract3DVelocity>
struct NeedsRemoveQuatFlips : std::false_type {};
template<>
struct NeedsRemoveQuatFlips<true, true, false, false> : std::true_type {}; // FrameUpdateCallBackCompressedSkinned
template<>
struct NeedsRemoveQuatFlips<false, true, false, false> : std::true_type {}; // FrameUpdateCallBackSkinned
template<>
struct NeedsRemoveQuatFlips<false, true, true, false> : std::true_type {}; // FrameUpdateCallBackSkinnedWithVelocityExtraction
template<>
struct NeedsRemoveQuatFlips<false, false, true, false> : std::true_type {}; // FrameUpdateCallBackNonSkinnedWithVelocityExtraction

/*!
 * @brief Copy-paste eliminated per-tick frame update
 * @tparam IsCompressed If the anim data is compressed
 * @tparam IsSkinned If the frame is skinned or not
 * @tparam ExtractVelocity Whenever to use the translation to update the ped's position
 * @tparam Extract3DVelocity Whenever to use Z translation too for updating the ped's position
 * @param fd Frame Data
 * @param c Context
*/
template<bool IsCompressed, bool IsSkinned, bool ExtractVelocity, bool Extract3DVelocity>
void FrameUpdateCallBackT(AnimBlendFrameData* fd, AnimBlendUpdateData* c) {
    const auto partialScale = 1.f - CalculateTotalBlendOfPartial(c, fd, true);

    constexpr bool IsFixBugs = true;

    const auto GetVelocityFromTranslation = [](CAnimBlendNode* node, CVector t) {
        return CVector{
            !IsFixBugs && Extract3DVelocity || node->GetAnimAssoc()->HasFlag(ANIMATION_CAN_EXTRACT_X_VELOCITY) // BUGFIX: Check this flag for 3D too (Like they did for 2D)
                ? t.x
                : 0.f,
            t.y,
            Extract3DVelocity
                ? t.z
                : 0.f
        };
    };

    // (if `ExtractVelocity`): Extract current velocity
    CVector currV{}; // Current velocity (before update), Z only used if `ExtractZVelocity`
    if constexpr (ExtractVelocity) {
        for (auto it = c->BlendNodeArrays; *it; it++) {
            const auto node = *it;

            if (!node->IsValid() || !node->GetRootKF()->HasTranslation()) {
                continue;
            }

            const auto assoc = node->GetAnimAssoc();
            if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
                continue;
            }

            if (IsFixBugs || !Extract3DVelocity) {
                if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) { // BUGFIX: Check this flag for 3D too (Like they did for 2D)
                    continue;
                }
            }

            CVector t;
            node->I_GetCurrentTranslation<IsCompressed>(t, partialScale);
            currV += GetVelocityFromTranslation(node, t);
        }
    }

    CQuaternion nextQ{}; // Rotation after update
    CVector     nextT{}; // Translation after update

    // if `ExtractVelocity`:
    CVector nextV{}, loopedNextV{}; // Velocity after update, Z only used if `ExtractZVelocity`
    bool    hasLoopedVelocity{};
    // else:
    float   nextBlendT{}; // Key-frame blend

    for (auto it = c->BlendNodeArrays; *it; ++*it, it++) { // NOTE: Increments NodeArray pointer too!
        const auto node = *it;

        if (!node->IsValid()) {
            continue;
        }

        const auto assoc = node->GetAnimAssoc();

        const auto blendScale = partialScale;
        CVector t;
        CQuaternion q;
        const auto looped = node->I_Update<IsCompressed>(t, q, blendScale);

        // NOTE: Not sure if doing this at all is necessary, but let's keep it the way it was.
        if constexpr (NeedsRemoveQuatFlips<IsCompressed, IsSkinned, ExtractVelocity, Extract3DVelocity>{}) {
            nextQ = nextQ + (q.Dot(nextQ) >= 0.f ? q : -q);
        } else {
            nextQ = nextQ + q;
        }

        if (!node->GetRootKF()->HasTranslation()) {
            continue;
        }

        if (IsFixBugs || ExtractVelocity && !Extract3DVelocity) {
            if (assoc->HasFlag(ANIMATION_IGNORE_ROOT_TRANSLATION)) { // BUGFIX: Check this flag for 3D too (Like they did for 2D)
                continue;
            }
        }

        nextT += t;
        if constexpr (!ExtractVelocity) {
            nextBlendT += node->GetAnimAssoc()->GetBlendAmount();
        }

        if constexpr (!ExtractVelocity) {
            continue;
        }

        if (!assoc->HasFlag(ANIMATION_CAN_EXTRACT_VELOCITY)) {
            continue;
        }

        CVector cT;
        node->I_GetCurrentTranslation<IsCompressed>(cT, partialScale);
        nextV += GetVelocityFromTranslation(node, cT);

        if (hasLoopedVelocity |= looped) {
            CVector eT;
            node->I_GetEndTranslation<IsCompressed>(eT, blendScale);
            loopedNextV += GetVelocityFromTranslation(node, eT);
        }
    }

    const auto fmat = IsSkinned
        ? nullptr
        : RwFrameGetMatrix(fd->Frame);

    if (!fd->KeyFramesIgnoreNodeOrientation) {
        nextQ.Normalise();

        if constexpr (IsSkinned) {
            fd->KeyFrame->q = nextQ;
        } else {
            RwMatrixSetIdentity(fmat);
            nextQ.Get(fmat); // Set delta rotation as the rotation of the frame
        }
    }

    if (!fd->KeyFramesIgnoreNodeTranslation) {
        // Apply velocity
        if constexpr (ExtractVelocity) {
            const auto wsPos = gpAnimBlendClump->m_PedPosition;

            wsPos->x = nextV.x - currV.x;
            wsPos->y = nextV.y - currV.y;
            if constexpr (Extract3DVelocity) {
                wsPos->z = nextV.z - currV.z;
            }

            if (hasLoopedVelocity) {
                wsPos->x += loopedNextV.x;
                wsPos->y += loopedNextV.y;
                if constexpr (Extract3DVelocity) {
                    wsPos->z += loopedNextV.z;
                }
            }
        }

        // Apply translation
        RwV3d* t = IsSkinned
            ? &fd->KeyFrame->t
            : RwMatrixGetPos(fmat);

        const CVector nodePos = IsSkinned
            ? fd->BonePos
            : fd->FramePos;

        if constexpr (ExtractVelocity) {
            t->x = nextT.x - nextV.x + nodePos.x;
            t->y = nextT.y - nextV.y + nodePos.y;

            if constexpr (Extract3DVelocity) {
                t->z = nextT.z - nextV.z + nodePos.z;
            } else if constexpr (IsSkinned && !IsCompressed) { // FrameUpdateCallBackSkinnedWithVelocityExtraction
                t->z = nextT.z;
                if (t->z >= -0.8f) {
                    t->z += t->z >= -0.4f
                        ? fd->BonePos.z
                        : (t->z * 2.5f + 2.f) * fd->BonePos.z;
                }
            } // otherwise `z` remains untouched
        } else {
            *t = lerp<CVector>(nodePos, nextT, nextBlendT);
        }
    }

    if constexpr (!IsSkinned) {
        RwMatrixUpdate(fmat);
    }
}

/*!
 * @brief Per-tick frame update wrapper
 * @tparam IsCompressed If the anim data is compressed
 * @tparam IsSkinned If the frame is skinned or not
 * @param fd Frame Data
 * @param c Context (Real type: `AnimBlendUpdateData*`)
*/
template<bool IsCompressed, bool IsSkinned>
void FrameUpdateCallBackW(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

#if USE_COPY_PASTE_FRAME_UPDATE // Templated function
    if constexpr (IsCompressed) {
        if constexpr (IsSkinned) {
            FrameUpdateCallBackCompressedSkinned(fd, c);
        } else {
            FrameUpdateCallBackCompressedNonSkinned(fd, c);
        }
    } else {
        if constexpr (IsSkinned) {
            FrameUpdateCallBackSkinned(fd, c);
        } else {
            FrameUpdateCallBackNonSkinned(fd, c);
        }
    }
#else
    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
        if (fd->HasZVelocity) { // NOTE: Originally compressed anims immediately did 3D velocity, so this flag may not be set on compressed anims?
            FrameUpdateCallBackT<IsCompressed, IsSkinned, true, true>(fd, c);
        } else {
            FrameUpdateCallBackT<IsCompressed, IsSkinned, true, false>(fd, c);
        }
    } else {
        FrameUpdateCallBackT<IsCompressed, IsSkinned, false, false>(fd, c);
    }
#endif
}

// 0x4D2E10
void FrameUpdateCallBackOffscreen(AnimBlendFrameData* fd, void* data) {
    const auto c = static_cast<AnimBlendUpdateData*>(data);

    if (fd->HasVelocity && gpAnimBlendClump->m_PedPosition) {
#if USE_COPY_PASTE_FRAME_UPDATE
        FrameUpdateCallBackSkinnedWithVelocityExtraction(c, fd);
#else
        FrameUpdateCallBackT<false, true, true, false>(fd, c);
#endif
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
            totalTime = float(double(ah->GetTotalTime()) / a->GetSpeed() * a->GetBlendAmount() + totalTime);
            totalBlendAmnt += a->GetBlendAmount();
        } else {
            ctx.IncludePartial = true;        
        }
    });
    ctx.BlendNodeArrays[nodesCnt] = nullptr; // Null terminator

    // 0x4D35A2 - Update animation's timesteps
    const auto animTimeMult = totalTime == 0.f ? 1.f : totalBlendAmnt / totalTime;
    RpAnimBlendClumpForEachAssociation(clump, [&](CAnimBlendAssociation* a) {
        if (!a->IsPlaying()) {
            return;
        }
        // The executable divides the hierarchy duration before multiplying the
        // total blend and timestep; calling UpdateTimeStep would round earlier.
        const double speed = a->IsSyncronised()
            ? (totalTime == 0.f
                ? double(a->GetHier()->GetTotalTime())
                : double(a->GetHier()->GetTotalTime()) / totalTime * totalBlendAmnt)
            : double(a->GetSpeed());
        a->m_TimeStep = float(speed * timeStep);
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
#endif
}
