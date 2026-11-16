/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimplePlayHandSignalAnim
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimplePlayHandSignalAnim.h"

#include "Ragdoll/IKChainManager.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x61AE50 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimplePlayHandSignalAnim del EXE (thiscall, cuatro dwords en la pila, `ret 10h`, devuelve this). Base CTaskSimple (0x61A390); id de animacion en +0x10, factor (patron de bits tal cual) en +0x14, byte +0x18 = (byte bajo del tercer argumento != 0), bit 2 del byte +0xC = bit 0 del cuarto, +8/+0x1C/+0x20 = 0 y vtable 0x86D5B8.
CTaskSimplePlayHandSignalAnim* __fastcall HandSignalConstructorBody(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint32 animId, uint32 blendBits, uint32 fatHands, uint32 holdLastFrame) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    bytes[0xC] = static_cast<uint8>(((holdLastFrame & 1) << 2) | (bytes[0xC] & 0xF8));
    *reinterpret_cast<uint32*>(bytes + 0x10) = animId;
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x20) = 0;
    *reinterpret_cast<uint32*>(bytes) = 0x86D5B8;
    *reinterpret_cast<uint32*>(bytes + 0x14) = blendBits;
    bytes[0x18] = static_cast<uint8>(fatHands) != 0 ? 1 : 0;
    return self;
}

// 0x61BD80 (lote 202A, sin validar): destructor con borrado de CTaskSimplePlayHandSignalAnim del EXE (ranura 0 de su vtable 0x86D5B8; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x61AEB0; test byte [esp+8],1; je ..;
// push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x61AEB0, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas
// esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimplePlayHandSignalAnim* __fastcall HandSignalScalarDeletingDestructor(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/, uint8 flags) {
    CTaskSimplePlayHandSignalAnim::DestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimplePlayHandSignalAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayHandSignalAnim, 0x86D5B8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(HandSignalConstructorBody, "Constructor", 0x61AE50);
    RH_ScopedNamedGlobalInstall(DestructorBodyExe, "DestructorBody", 0x61AEB0);                            // lote 202A (sin validar)
    RH_ScopedNamedGlobalInstall(HandSignalScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61BD80); // lote 202A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x61AEA0);    // lote 202A (sin validar): ranura 4
    RH_ScopedVMTInstall(Clone, 0x61B980);          // lote 202A (sin validar): ranura 1
    RH_ScopedVMTInstall(MakeAbortable, 0x61AF50);  // lote 202A (sin validar): ranura 6, `jmp 0x61A790`

    RH_ScopedVMTInstall(ProcessPed, 0x61BDA0);
    RH_ScopedInstall(StartAnim, 0x61AF60);
}

// 0x61AE50
CTaskSimplePlayHandSignalAnim::CTaskSimplePlayHandSignalAnim(AnimationId animationId, float fBlendFactor, bool bFatHands, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame) {
    m_PlayerIdlesAnimBlockId = animationId;
    m_LeftHand = nullptr;
    m_RightHand = nullptr;
    m_AnimBlenDelta = fBlendFactor;
    m_DoUseFatHands = bFatHands;
}

CTaskSimplePlayHandSignalAnim::~CTaskSimplePlayHandSignalAnim() {
    if (m_LeftHand) {
        CWorld::Remove(m_LeftHand);
        delete m_LeftHand;
        --CObject::nNoTempObjects;
    }

    if (m_RightHand) {
        CWorld::Remove(m_RightHand);
        delete m_RightHand;
        --CObject::nNoTempObjects;
    }
}

// 0x61AEA0 (lote 202A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,1A9h`): TASK_SIMPLE_HANDSIGNAL_ANIM (425); no lee el objeto (ranura 4 de la vtable 0x86D5B8).
eTaskType CTaskSimplePlayHandSignalAnim::GetTaskType() const {
    return Type;
}

// 0x61AF50 (lote 202A, sin validar; 5 bytes: `jmp 0x61A790`; ranura 6 de la vtable 0x86D5B8): salto puro a CTaskSimpleAnim::MakeAbortable (0x61A790, con hook, `ret 0Ch` con tres argumentos de pila: ped, prioridad, evento): no toca los registros ni
// la pila, asi que los tres argumentos pasan tal cual y el valor de retorno es el de esa funcion. Aqui es la llamada calificada (no virtual) a la base.
bool CTaskSimplePlayHandSignalAnim::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return CTaskSimpleAnim::MakeAbortable(ped, priority, event);
}

// 0x61B980 (lote 202A, sin validar; ranura 1 (Clone) de la vtable 0x86D5B8; thiscall, sin argumentos, `ret`, 118 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CTask::operator new (0x61A5A0, con hook) con 0x24 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x61AE50 (HandSignalConstructorBody, con hook) con cuatro argumentos de pila: (identificador de animacion
// +0x10, factor +0x14 (patron de bits tal cual), byte +0x18 ampliado a cero, bit 2 del byte +0xC desplazado a bit 0) y devuelve la tarea nueva (el constructor devuelve `this`). El constructor del EXE deja la vtable 0x86D5B8.
CTask* CTaskSimplePlayHandSignalAnim::Clone() const {
    void* const memory = CTask::operator new(sizeof(CTaskSimplePlayHandSignalAnim));
    if (!memory) {
        return nullptr;
    }
    const uint8* const bytes = reinterpret_cast<const uint8*>(this);
    return HandSignalConstructorBody(
        static_cast<CTaskSimplePlayHandSignalAnim*>(memory), nullptr,
        *reinterpret_cast<const uint32*>(bytes + 0x10),
        *reinterpret_cast<const uint32*>(bytes + 0x14),
        uint32{ bytes[0x18] },
        uint32{ (bytes[0xC] & 4u) >> 2 }
    );
}

// 0x61AEB0 (lote 202A, sin validar; thiscall, sin argumentos de pila, `ret`, 146 bytes con marco SEH que solo desenrolla (se omite)): cuerpo del destructor de CTaskSimplePlayHandSignalAnim del EXE. Como el original: deja la vtable de la clase del EXE (0x86D5B8);
// con la mano izquierda (+0x1C) no nula: CWorld::Remove (0x563280, con hook, cdecl un argumento), vuelve a leer +0x1C y, si no es nulo, llama al destructor con borrado de la mano (ranura 0, flag 1), y baja en uno CObject::nNoTempObjects (word en 0xBB4A70)
// AUNQUE el puntero ya fuera nulo; lo mismo con la mano derecha (+0x20); y sigue con el cuerpo de ~CTaskSimpleAnim (0x61A6F0, con hook). Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall CTaskSimplePlayHandSignalAnim::DestructorBodyExe(CTaskSimplePlayHandSignalAnim* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D5B8;
    if (self->m_LeftHand) {
        CWorld::Remove(self->m_LeftHand);
        delete self->m_LeftHand;
        --CObject::nNoTempObjects;
    }
    if (self->m_RightHand) {
        CWorld::Remove(self->m_RightHand);
        delete self->m_RightHand;
        --CObject::nNoTempObjects;
    }
    CTaskSimpleAnim::DestructorBodyExe(self, nullptr);
}

// 0x61BDA0
bool CTaskSimplePlayHandSignalAnim::ProcessPed(CPed* ped) {
    if (g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_LEFT, ped) || m_bIsFinished) {
        return true;
    }

    if (!m_pAnim) {
        if (CObject::nNoTempObjects + 1 >= 150) {
            return true;
        }

        CTaskSimplePlayHandSignalAnim::StartAnim(ped);
    }

    return m_bIsFinished;
}

// 0x61AF60
void CTaskSimplePlayHandSignalAnim::StartAnim(CPed* ped) {
    // Como el original: GSIGN1 - trunc(rand * (1/32768) * -5), es decir [GSIGN1, GSIGN5]
    const auto animId = static_cast<AnimationId>(CGeneral::GetRandomIntInRange((int32)ANIM_ID_GSIGN1, (int32)ANIM_ID_GSIGN5 + 1));

    //
    // Pointing / weapon logic
    //
    if (ped->GetEntityThatThisPedIsHolding() || g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_RIGHT, ped) || ped->GetActiveWeapon().m_Type != eWeaponType::WEAPON_UNARMED) {
        m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_HANDSIGNALL, animId, m_AnimBlenDelta);
        m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);

        if (m_PlayerIdlesAnimBlockId == -1) {
            return;
        }

        const auto handIndex = m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL;
        m_LeftHand    = new CHandObject(handIndex, ped, true);
        if (!m_LeftHand) {
            return;
        }

        CWorld::Add(m_LeftHand);
        auto* animHierarchy = CAnimManager::GetAnimAssociation(ANIM_GROUP_LHAND, m_PlayerIdlesAnimBlockId)->m_BlendHier;
        CAnimManager::AddAnimation(m_LeftHand->GetRpClump(), animHierarchy, 0);
        ++CObject::nNoTempObjects;
        return;
    }

    //
    // Both arms logic
    //
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_HANDSIGNAL, animId, m_AnimBlenDelta);
    m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    if (m_PlayerIdlesAnimBlockId == -1) {
        return;
    }

    const auto CreateHandObject = [ped, this](eModelID handModel, AssocGroupId animGrpId, bool isLeftHand) {
        const auto handObj = new CHandObject(handModel, ped, isLeftHand);
        if (handObj) {
            CWorld::Add(handObj);
            CAnimManager::AddAnimation(
                handObj->GetRpClump(),
                CAnimManager::GetAnimAssociation(animGrpId, m_PlayerIdlesAnimBlockId)->GetAnimHierarchy(), // Como el original: LHAND (0x4F) o RHAND (0x50)
                0
            );
            CObject::nNoTempObjects++;
        }
        return handObj;
    };
    m_LeftHand = CreateHandObject(m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL, ANIM_GROUP_LHAND, true);
    m_RightHand = CreateHandObject(m_DoUseFatHands ? MODEL_FHANDR : MODEL_SHANDR, ANIM_GROUP_RHAND, false);
}
