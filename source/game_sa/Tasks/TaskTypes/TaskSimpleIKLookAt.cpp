/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleIKLookAt
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
#include "Ragdoll/IKChainManager.h"
#include "TaskSimpleIKLookAt.h"

namespace {
// 0x633EF0 (lote 157A, banco Fn157A): cuerpo del destructor de CTaskSimpleIKLookAt del EXE (con hook (Destructor)). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x633EF0` del EXE.
__declspec(noinline) void __fastcall CTaskSimpleIKLookAtDestructorBody(CTaskSimpleIKLookAt* self, void* /*edx*/) { self->CTaskSimpleIKLookAt::~CTaskSimpleIKLookAt(); }

// 0x639490 (lote 157A, banco Fn157A): destructor con borrado de CTaskSimpleIKLookAt del EXE (ranura 0 de su vtable 0x86E37C; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x633EF0; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleIKLookAt* __fastcall CTaskSimpleIKLookAtScalarDeletingDestructor(CTaskSimpleIKLookAt* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleIKLookAtDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskSimpleIKLookAt() { return reinterpret_cast<uintptr_t>(&CTaskSimpleIKLookAtDestructorBody); } // para el banco Fn157A

void CTaskSimpleIKLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKLookAt, 0x86E37C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleIKLookAtScalarDeletingDestructor, "ScalarDeletingDestructor", 0x639490); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x633E00);
    RH_ScopedOverloadedInstall(Destructor, "", 0x633EF0, CTaskSimpleIKLookAt * (CTaskSimpleIKLookAt::*)());

    RH_ScopedInstall(UpdateLookAtInfo, 0x634050);
    RH_ScopedInstall(GetLookAtEntity, 0x634120);
    RH_ScopedInstall(GetLookAtOffset, 0x634130);
    RH_ScopedVMTInstall(Clone, 0x633F00);
    RH_ScopedVMTInstall(GetTaskType, 0x633EE0);
    RH_ScopedVMTInstall(CreateIKChain, 0x633FC0);
}

// Como el original: el EXE guarda y pasa el hueso de desplazamiento como dword (la palabra alta tambien cuenta)
static eBoneTag32 RawBoneTag32(const eBoneTag16& bone) {
    eBoneTag32 r;
    r.m_Value = *reinterpret_cast<const int32*>(&bone);
    return r;
}

// 0x633E00
CTaskSimpleIKLookAt::CTaskSimpleIKLookAt(Const char* name, CEntity* lookAtEntity, int32 time, eBoneTag32 pedBoneID, CVector lookAtOffset, bool useTorso, float speed,
                                         int32 blendTime, int8 priority
) :
    // Como el original: el constructor de CTaskSimpleIKChain va en linea y no escribe 0x14, 0x16, 0x48, 0x4C ni 0x50
    CTaskSimpleIKChain{ UninitTag{ *reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(this) + 0x16) } }
{
    m_OffsetPos       = lookAtOffset;
    *reinterpret_cast<int32*>(&m_OffsetBone) = pedBoneID.get_underlying(); // Como el original: se escribe como dword
    m_Duration        = time;
    m_BlendDuration   = blendTime;
    m_EffectorOffset  = CVector{ 0.f, 0.05f, 0.f };
    m_bUseTorso       = useTorso;
    m_IKChain         = nullptr;
    m_EffectorBone    = BONE_HEAD;
    m_Speed           = speed;
    m_Blend           = 0.f;
    m_nPriority       = priority;
    m_IsBlendingOut   = 0;
    m_TargetEntity    = lookAtEntity;
    m_TargetEntityExisted = lookAtEntity != nullptr;
}

// 0x633F00
CTaskSimpleIKLookAt* CTaskSimpleIKLookAt::Clone() const {
    // Como el original: el hueso de desplazamiento se pasa como dword
    auto* task = new CTaskSimpleIKLookAt("", m_TargetEntity, m_Duration, RawBoneTag32(m_OffsetBone), m_OffsetPos, m_bUseTorso, m_Speed, m_BlendDuration, m_nPriority);
    if (m_IKChain) {
        task->m_Blend        = m_Blend;
        task->m_EndTime      = m_EndTime;
        task->m_TargetBlend  = m_TargetBlend;
        task->m_TargetTime   = m_TargetTime;
        task->m_PivotBone    = m_PivotBone;
    }
    return task;
}

// 0x634050
void CTaskSimpleIKLookAt::UpdateLookAtInfo(const char* strPurpose, CPed* ped, CEntity* targetPed, int32 time, eBoneTag32 pedBoneID, RwV3d lookAtOffset, bool useTorso, float fSpeed,
                                           int32 blendTime, int32 unused) {
    m_TargetEntity        = targetPed;
    m_TargetEntityExisted = !!targetPed;
    *reinterpret_cast<int32*>(&m_OffsetBone) = pedBoneID.get_underlying(); // Como el original: se escribe como dword
    m_OffsetPos           = lookAtOffset;
    m_bUseTorso           = useTorso;
    m_Duration            = time;
    m_Speed               = fSpeed;
    m_BlendDuration       = blendTime;
    m_EndTime             = CTimer::GetTimeInMS() + time;
    m_TargetBlend         = 1.0f;
    m_TargetTime          = CTimer::GetTimeInMS() + blendTime;
    m_IsBlendingOut       = false;

    if (m_IKChain) {
        m_IKChain->UpdateEntity(m_TargetEntity);
        m_IKChain->UpdateOffset(RawBoneTag32(m_OffsetBone), m_OffsetPos);
        m_IKChain->UpdateTarget(true);
    }
}

// 0x634120
CEntity* CTaskSimpleIKLookAt::GetLookAtEntity() {
    return m_TargetEntity;
}

// 0x634130
CVector CTaskSimpleIKLookAt::GetLookAtOffset() { // We return a vector here.. Hopefully this is ABI compatible.
    return m_OffsetPos;
}

// 0x633FC0

// 0x0
bool CTaskSimpleIKLookAt::CreateIKChain(CPed* ped) {
    m_PivotBone = BONE_NECK;
    if (m_IKChain = g_ikChainMan.AddIKChain(
        "",
        eIKChainSlot::LOOK_AT,
        ped,
        m_EffectorBone,
        m_EffectorOffset,
        BONE_NECK,
        m_TargetEntity,
        RawBoneTag32(m_OffsetBone), // Como el original: se pasa como dword
        m_OffsetPos,
        m_Speed,
        m_nPriority)
    ) {
        m_IKChain->ClampLimits(BONE_HEAD, false, true, false, true);
        return true;
    }
    return false;
}

