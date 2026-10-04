/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: IKChain_c
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 20/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "IKChain.h"
#include "BoneNodeManager.h"

#include "Enums/eIKChainSlot.h"

void IKChain_c::InjectHooks() {
    RH_ScopedClass(IKChain_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x618370);
    RH_ScopedInstall(Exit, 0x617870);
    RH_ScopedInstall(Update, 0x6184B0);
    RH_ScopedInstall(MoveBonesToTarget, 0x6178B0);
    RH_ScopedInstall(SetBlend, 0x617C10);
    RH_ScopedInstall(SetOffsetBoneTag, 0x617C20);
    RH_ScopedInstall(SetOffsetPos, 0x617C30);
    RH_ScopedInstall(ClampLimits, 0x618520);
    RH_ScopedInstall(GetLimits, 0x618590);
    RH_ScopedInstall(SetLimits, 0x6185E0);
    RH_ScopedInstall(GetBoneSpeed, 0x618630);
    RH_ScopedInstall(SetBoneSpeed, 0x618680);
    RH_ScopedInstall(GetPriority, 0x617C50);
    RH_ScopedInstall(GetBoneNodeFromTag, 0x617C60);
    RH_ScopedInstall(SetupBones, 0x617CA0);
    RH_ScopedInstall(UpdateEntity, 0x617E00);
    RH_ScopedInstall(UpdateOffset, 0x617E20);
    RH_ScopedInstall(UpdateTarget, 0x617E50);
    RH_ScopedInstall(IsFacingTarget, 0x617E60);
    RH_ScopedInstall(IsAtTarget, 0x617F30);
}

// Como el original: las referencias de la cadena (m_Ped y m_TargetEntity) se escriben en bruto, sin
// CleanUpOldReference del valor anterior; solo se llama a RegisterReference donde lo hace el EXE.
template<typename T>
static T*& RawRefSlot(notsa::EntityRef<T>& ref) {
    return *reinterpret_cast<T**>(std::addressof(ref));
}

// Como el original (0x617AA3..0x617AC5): GetSpeed() * acos(dot) * m_Speed * 57.29578f en el x87.
// acos es el del CRT del juego (0x822380): fpatan(sqrt((1 + x)(1 - x)), x) con la palabra de control
// (anterior & 0x300) | 0x7F; con la palabra del juego (!= 0x27F, precision enmascarada) y el indicador de
// inexactitud activo, el CRT redondea el resultado a double (0x828EA7) antes de devolverlo, salvo que el
// indicador del CRT 0xC9ABFC (nunca escrito por el EXE) no sea cero.
static float s_IKChainRadToDeg = std::bit_cast<float>(0x42652EE0u); // 0x859878 (no es el float más cercano a 180/pi)
static float __declspec(naked) __cdecl IKChainRotationAngleX87(float boneSpeed, float dot, float chainSpeed) {
    __asm {
        fld    dword ptr [esp + 4]
        fld    dword ptr [esp + 8]
        sub    esp, 0Ch
        fnstcw word ptr [esp + 8]
        mov    ax, word ptr [esp + 8]
        and    ax, 300h
        or     ax, 7Fh
        mov    word ptr [esp + 0Ah], ax
        cmp    word ptr [esp + 8], 27Fh
        je     ik_compute
        fldcw  word ptr [esp + 0Ah]
    ik_compute:
        fld1
        fadd   st, st(1)
        fld1
        fsub   st, st(2)
        fmulp  st(1), st
        fsqrt
        fxch   st(1)
        fpatan
        mov    ax, word ptr [esp + 8]
        cmp    dword ptr ds:[0C9ABFCh], 0
        jne    ik_fast
        cmp    ax, 27Fh
        je     ik_done
        test   ax, 20h
        je     ik_restore
        fnstsw ax
        test   ax, 20h
        je     ik_restore
        fstp   qword ptr [esp]
        fld    qword ptr [esp]
        jmp    ik_restore
    ik_fast:
        cmp    ax, 27Fh
        je     ik_done
    ik_restore:
        fldcw  word ptr [esp + 8]
    ik_done:
        add    esp, 0Ch
        fmulp  st(1), st
        fmul   dword ptr [esp + 0Ch]
        fmul   dword ptr [s_IKChainRadToDeg]
        ret
    }
}

// 0x618370
bool IKChain_c::Init(
    const char*  name, // unused
    eIKChainSlot ikSlot,
    CPed*        ped,
    eBoneTag32   effectorBone,
    CVector      effectorPos,
    eBoneTag32   pivotBone,
    CEntity*     entity,
    eBoneTag32   offsetBone,
    CVector      offsetPos,
    float        speed,
    int32        priority
) {
    // Como el original: m_Ped se escribe en bruto y la referencia solo se registra si la cadena es válida.
    auto& rawPed = RawRefSlot(m_Ped);
    rawPed = ped;

    const auto frames = ped->GetAnimBlendData().GetFrames();

    if (frames[0].NeedsKeyFrameUpdate || !frames[0].IsUpdatingFrame) {
        return false;
    }

    if (!ped->bHasBeenRendered) { // Bone data isn't present yet in this case
        return false;
    }

    // Check if frame of this bone has non-zero translation (Como el original: NaN cuenta como distinto de cero)
    {
        const auto& boneFrame = frames[RpHAnimIDGetIndex(&ped->GetAnimHierarchy(), (RwInt32)effectorBone)].KeyFrame;
        if (boneFrame->t.x == 0.f && boneFrame->t.y == 0.f && boneFrame->t.z == 0.f) {
            return false;
        }
    }

    rawPed->RegisterReference(reinterpret_cast<CEntity**>(&rawPed));

    m_Bones = nullptr; // Como el original: se limpia antes de SetupBones
    SetupBones(effectorBone, effectorPos, pivotBone, frames);

    m_EffectorPos  = effectorPos;
    m_Blend        = 0.f; // Como el original: la mezcla empieza en 0
    m_EffectorBone = effectorBone;
    UpdateEntity(entity); // Inlined en el EXE
    m_OffsetBone   = offsetBone;
    m_OffsetPos    = offsetPos;
    m_Speed        = speed;
    m_UpdateTarget = true;
    m_IKSlot       = ikSlot;
    m_Priority     = priority;

    return true;
}

// 0x617870
void IKChain_c::Exit() {
    for (auto i = 0; i < m_BonesCount; i++) {
        g_boneNodeMan.ReturnBoneNode(m_Bones[i]);
    }
    delete[] std::exchange(m_Bones, nullptr);
}

// 0x6184B0
void IKChain_c::Update(float deltaTime) {
    UNUSED(deltaTime);

    // Como el original: jerarquía de la piel del clump (0x734A40) y matriz por índice, sin comprobar nulos
    auto* const hier = GetAnimHierarchyFromSkinClump(reinterpret_cast<RpClump*>(m_Ped->GetRwObject()));
    const auto  idx  = RpHAnimIDGetIndex(hier, (RwInt32)static_cast<eBoneTag>(m_PivotBone));
    m_PivotBoneMatrix = RpHAnimHierarchyGetMatrixArray(hier) + idx;
    m_Bones[m_BonesCount - 1]->CalcWldMat(m_PivotBoneMatrix);
    MoveBonesToTarget();
    for (auto i = 0; i < m_BonesCount; i++) {
        m_Bones[i]->BlendKeyframe(m_Blend);
    }
}

// 0x617F30
bool IKChain_c::IsAtTarget(float range, float* outDist) const {
    // Como el original: RwV3dLength del EXE (raíz por tabla), no CVector::Magnitude
    const CVector rootPos = m_Bones[0]->GetPosition();
    CVector       delta{ m_OffsetPosWS.x - rootPos.x, m_OffsetPosWS.y - rootPos.y, m_OffsetPosWS.z - rootPos.z };
    const float   len = RwV3dLength(&delta);
    if (outDist) {
        *outDist = len;
    }
    return len <= range && m_Blend > 0.98f;
}

// 0x617E60
bool IKChain_c::IsFacingTarget() const {
    const auto* const root    = m_Bones[0];
    const CVector     rootPos = root->GetPosition();

    CVector effectorDir;
    RwV3dTransformVector(&effectorDir, &m_EffectorPos, &root->GetMatrix());
    RwV3dNormalize(&effectorDir, &effectorDir);

    // Como el original: la dirección al objetivo parte de la posición del hueso raíz (no del efector transformado)
    CVector targetDir{ m_OffsetPosWS.x - rootPos.x, m_OffsetPosWS.y - rootPos.y, m_OffsetPosWS.z - rootPos.z };
    RwV3dNormalize(&targetDir, &targetDir);

    // Como el original: (y*y + x*x) + z*z
    const float dot = effectorDir.y * targetDir.y + effectorDir.x * targetDir.x + effectorDir.z * targetDir.z;
    return dot >= 0.95f && m_Blend > 0.98f;
}

// 0x617E50
void IKChain_c::UpdateTarget(bool update) {
    m_UpdateTarget = update;
}

// 0x617E20
void IKChain_c::UpdateOffset(eBoneTag32 offsetBone, CVector offsetPos) {
    m_OffsetBone = offsetBone;
    m_OffsetPos = offsetPos;
}

// 0x618520
void IKChain_c::ClampLimits(eBoneTag32 boneTag, bool limitX, bool limitY, bool limitZ, bool current) {
    auto& boneNode = *GetBoneNodeFromTag(boneTag);
    if (current) {
        boneNode.ClampLimitsCurrent(limitX, limitY, limitZ);
    } else {
        boneNode.ClampLimitsDefault(limitX, limitY, limitZ);
    }
}

// 0x617E00
void IKChain_c::UpdateEntity(CEntity* entity) {
    // Como el original: no se llama a CleanUpOldReference de la entidad anterior
    auto& rawTarget = RawRefSlot(m_TargetEntity);
    rawTarget = entity;
    if (entity) {
        entity->RegisterReference(&rawTarget);
    }
}

// inline
// 0x617C60
inline BoneNode_c* IKChain_c::GetBoneNodeFromTag(eBoneTag32 boneTag) {
    for (auto&& bone : GetBones()) {
        if (bone->GetBoneTag() == boneTag) {
            return bone;
        }
    }
    NOTSA_LOG_WARN("IKChain_c::GetBoneNodeFromTag: Bone tag {} not found in chain", (int32)(boneTag));
    return nullptr;
}

// 0x617C50
int8 IKChain_c::GetPriority() const {
    return m_Priority;
}

// 0x617C30
void IKChain_c::SetOffsetPos(CVector offsetPos) {
    m_OffsetPos = offsetPos;
}

// 0x617C20
void IKChain_c::SetOffsetBoneTag(eBoneTag32 offsetBone) {
    m_OffsetBone = offsetBone;
}

// 0x617C10
void IKChain_c::SetBlend(float blend) {
    m_Blend = blend;
}

// 0x6178B0
void IKChain_c::MoveBonesToTarget() {
    static auto& s_bLimitBones = StaticRef<bool>(0x8D2BD2); // true; Como el original: Limit solo si está activo

    const auto rootBone = m_Bones[0]; // Como el original: se lee antes de mirar el número de huesos

    if (m_UpdateTarget) {
        if (CEntity* const target = m_TargetEntity) {
            const auto mat = target->GetModellingMatrix();
            if (m_OffsetBone == BONE_UNKNOWN) {
                if (mat) {
                    RwV3dTransformPoint(&m_OffsetPosWS, &m_OffsetPos, mat);
                }
            } else {
                // Como el original: 0x5E4280 escribe directamente en m_OffsetPosWS (sin comprobar el tipo)
                target->AsPed()->GetBonePosition(&m_OffsetPosWS, (eBoneTag)m_OffsetBone, false);
                if (mat) {
                    CVector transformed;
                    RwV3dTransformVector(&transformed, &m_OffsetPos, mat);
                    m_OffsetPosWS.x = transformed.x + m_OffsetPosWS.x;
                    m_OffsetPosWS.y = transformed.y + m_OffsetPosWS.y;
                    m_OffsetPosWS.z = transformed.z + m_OffsetPosWS.z;
                }
            }
        } else {
            m_OffsetPosWS = m_OffsetPos;
        }
    }

    constexpr auto DIST_TOLERANCE = 0.00001f; // 0x858C14

    for (auto i = 0; i < m_BonesCount; i++) {
        const auto    bone    = m_Bones[i];
        const CVector bonePos = bone->GetPosition();
        const CVector rootPos = rootBone->GetPosition();

        CVector transformedEffector;
        RwV3dTransformVector(&transformedEffector, &m_EffectorPos, &rootBone->GetMatrix()); // Se transforma en cada iteración, como en el EXE

        // Como el original: (efector + raíz) - hueso, longitud con RwV3dLength y NaN descartado
        CVector dirBoneToEffector{
            (transformedEffector.x + rootPos.x) - bonePos.x,
            (transformedEffector.y + rootPos.y) - bonePos.y,
            (transformedEffector.z + rootPos.z) - bonePos.z
        };
        if (!(RwV3dLength(&dirBoneToEffector) > DIST_TOLERANCE)) {
            continue;
        }
        RwV3dNormalize(&dirBoneToEffector, &dirBoneToEffector);

        CVector dirBoneToOffset{ m_OffsetPosWS.x - bonePos.x, m_OffsetPosWS.y - bonePos.y, m_OffsetPosWS.z - bonePos.z };
        if (!(RwV3dLength(&dirBoneToOffset) > DIST_TOLERANCE)) {
            continue;
        }
        RwV3dNormalize(&dirBoneToOffset, &dirBoneToOffset);

        // Como el original: (y*y + x*x) + z*z; solo sigue si dot < 0.997 (NaN se descarta)
        const float dot = dirBoneToOffset.y * dirBoneToEffector.y + dirBoneToOffset.x * dirBoneToEffector.x + dirBoneToOffset.z * dirBoneToEffector.z;
        if (!(dot < 0.997f)) { // Make sure they don't point in the same direction
            continue;
        }

        const float angle = IKChainRotationAngleX87(bone->GetSpeed(), dot, m_Speed);

        // Como el original: efector x objetivo con los productos en el orden del EXE
        CVector cross{
            dirBoneToOffset.z * dirBoneToEffector.y - dirBoneToOffset.y * dirBoneToEffector.z,
            dirBoneToOffset.x * dirBoneToEffector.z - dirBoneToOffset.z * dirBoneToEffector.x,
            dirBoneToOffset.y * dirBoneToEffector.x - dirBoneToOffset.x * dirBoneToEffector.y
        };

        const RwMatrix* const boneParentMat = bone->GetParent()
            ? &bone->GetParent()->GetMatrix()
            : m_PivotBoneMatrix;

        RtQuat quat;
        RtQuatConvertFromMatrix(&quat, boneParentMat);

        // Como el original: inverso en otro cuaternión, norma ((y*y + x*x) + z*z) + w*w; si no es
        // positiva el EXE usa ese cuaternión sin inicializar
        RtQuat invQuat;
        {
            const float mag = quat.imag.y * quat.imag.y + quat.imag.x * quat.imag.x + quat.imag.z * quat.imag.z + quat.real * quat.real;
            if (mag > 0.f) {
                const float recp = 1.f / mag;
                invQuat.real   = quat.real * recp;
                invQuat.imag.x = quat.imag.x * -recp;
                invQuat.imag.y = quat.imag.y * -recp;
                invQuat.imag.z = quat.imag.z * -recp;
            }
        }

        CVector axis;
        RtQuatTransformVectors(&axis, &cross, 1, &invQuat);
        RtQuatRotate(&bone->GetOrientation(), &axis, angle, rwCOMBINEPOSTCONCAT);

        if (s_bLimitBones) {
            bone->Limit(m_Blend);
        }
        bone->CalcWldMat(bone->GetParent() ? &bone->GetParent()->GetMatrix() : m_PivotBoneMatrix);
    }
}

// 0x617CA0
void IKChain_c::SetupBones(eBoneTag32 effector, CVector effectorPos, eBoneTag32 pivot, AnimBlendFrameData* frames) {
    UNUSED(effectorPos);

    m_PivotBoneMatrix = m_Ped->GetBoneMatrix(pivot);
    m_PivotBone       = pivot;

    const auto GetBoneLinkPrev = [](eBoneTag32 b) {
        const auto* info = BoneNodeManager_c::GetBoneInfoFromTag(b);
        return info
            ? info->ParentBoneTag
            : BONE_UNKNOWN; // BUGFIX: Handle case where `info` is null (Bone tag not found, logging done by `GetBoneInfoFromTag`)
    };

    // Allocate bone nodes we use (Going from effectorBone -> pivotBone)
    BoneNode_c* bones[MAX_BONE_NUM];
    m_BonesCount = 0;
    for (auto boneIt = effector; boneIt != pivot && boneIt != BONE_UNKNOWN; boneIt = GetBoneLinkPrev(boneIt)) { // BUGFIX: Handle `BONE_UNKNOWN` case in loop condition
        auto* const node = g_boneNodeMan.GetBoneNode(); // Allocate new node
        node->Init(boneIt, frames[RpHAnimIDGetIndex(&m_Ped->GetAnimHierarchy(), (RwInt32)boneIt)].KeyFrame);
        bones[m_BonesCount++] = node;
    }

    // Now we know the exact size to allocate, so do that
    m_Bones = new BoneNode_c*[m_BonesCount];
    for (auto i = 0; i < m_BonesCount; i++) {
        m_Bones[i] = bones[i];
    }

    // Link bones together
    for (auto& bone : GetBones()) {
        const auto prevBoneId = GetBoneLinkPrev(bone->GetBoneTag());
        // The pivot anchors the chain but is deliberately excluded from m_Bones.
        // Its child is the chain root, so no in-chain parent link is needed.
        if (prevBoneId == pivot || prevBoneId == BONE_UNKNOWN) {
            continue;
        }
        const auto prevBoneNode = GetBoneNodeFromTag(prevBoneId);
        if (!prevBoneNode) {
            continue;
        }
        prevBoneNode->AddChild(bone);
    }
}

// unused
// 0x6185E0
void IKChain_c::SetLimits(eBoneTag32 boneTag, eRotationAxis axis, float min, float max) {
    GetBoneNodeFromTag(boneTag)->SetLimits(axis, min, max);
}

// unused
// 0x618630
float IKChain_c::GetBoneSpeed(eBoneTag32 boneTag) {
    return GetBoneNodeFromTag(boneTag)->GetSpeed();
}

// unused
// 0x618680
void IKChain_c::SetBoneSpeed(eBoneTag32 boneTag, float speed) {
    GetBoneNodeFromTag(boneTag)->SetSpeed(speed);
}

// 0x618590
void IKChain_c::GetLimits(eBoneTag32 boneTag, eRotationAxis axis, float& min, float& max) {
    GetBoneNodeFromTag(boneTag)->GetLimits(axis, min, max);
}
