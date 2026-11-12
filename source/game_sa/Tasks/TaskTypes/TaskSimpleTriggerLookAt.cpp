/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTriggerLookAt
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleTriggerLookAt.h"
#include "Ragdoll/IKChainManager.h"

namespace {
// 0x634500 (lote 163A, banco Fn163A; thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla): cuerpo del destructor de CTaskSimpleTriggerLookAt del EXE. Deja la vtable de la clase del
// EXE (0x86E3CC), libera con CEntity::CleanUpOldReference (0x571A00, con hook) la referencia no nula de la entidad (+8, se pasa su direccion) y termina con CTask::Destructor (0x61A3A0, con hook). El marco SEH se omite.
// Antes la DLL llamaba a esta direccion del EXE con plugin::CallMethod. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall CTaskSimpleTriggerLookAtDestructorBody(CTaskSimpleTriggerLookAt* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E3CC;
    CEntity::SafeCleanUpRef(self->m_pEntity);
    self->CTask::Destructor();
}

// 0x6394D0 (lote 157A, banco Fn157A): destructor con borrado de CTaskSimpleTriggerLookAt del EXE (ranura 0 de su vtable 0x86E3CC; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x634500; test byte [esp+8],1; je ..; push esi;
// call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta
// puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleTriggerLookAt* __fastcall CTaskSimpleTriggerLookAtScalarDeletingDestructor(CTaskSimpleTriggerLookAt* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleTriggerLookAtDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote157ABody_CTaskSimpleTriggerLookAt() { return reinterpret_cast<uintptr_t>(&CTaskSimpleTriggerLookAtDestructorBody); } // para el banco Fn157A

void CTaskSimpleTriggerLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTriggerLookAt, 0x86E3CC, 9); // desde el lote 182A la clase define constructor y metodos virtuales, asi que la DLL emite su propia vtable
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleTriggerLookAtDestructorBody, "DestructorBody", 0x634500); // lote 163A (banco Fn163A)
    RH_ScopedNamedGlobalInstall(CTaskSimpleTriggerLookAtScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6394D0); // lote 157A (banco Fn157A)

    RH_ScopedInstall(Constructor, 0x634440); // lote 182A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x634560);
    RH_ScopedVMTInstall(GetTaskType, 0x6344F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x634610);
    RH_ScopedVMTInstall(ProcessPed, 0x634620);
}

static_assert(CTaskSimpleTriggerLookAt::Type == 0x10D); // el EXE (0x6344F0) devuelve `mov eax,10Dh`

// 0x634440 (lote 182A, sin validar; thiscall, 10 dwords en la pila, `ret 0x28`, 163 bytes con marco SEH que solo desenrolla, devuelve this): CTask::CTask (0x61A3B0) y los campos en el orden del EXE:
// tiempo (+0xC), hueso (+0x10), desplazamiento (+0x14..+0x1C, tres dwords), uso del torso (+0x20, byte), velocidad (+0x24, dword), tiempo de mezcla (+0x28), prioridad (+0x2D, byte bajo), vtable 0x86E3CC (en la DLL
// la suya), entidad (+8). Con entidad no nula: CEntity::RegisterReference (0x571B70, con la direccion del campo +8) y +0x2C = 1; si es nula, +0x2C = 0 y nada mas. El marco SEH se omite.
CTaskSimpleTriggerLookAt::CTaskSimpleTriggerLookAt(CEntity* entity, int32 time, int32 offsetBoneTag, RwV3d offsetPos, bool bUseTorso, float speed, int32 blendTime, int32 priority) :
    CTaskSimple{}
{
    m_time           = time;
    m_nOffsetBoneTag = offsetBoneTag;
    m_vOffsetPos     = offsetPos;
    m_bUseTorso      = bUseTorso;
    m_Speed          = speed;
    m_nBlendTime     = blendTime;
    m_nPriority      = static_cast<int8>(priority);
    m_pEntity        = entity;
    if (entity) {
        CEntity::RegisterReference(m_pEntity);
        m_bEntityExist = true;
    } else {
        m_bEntityExist = false;
    }
}

// Como el EXE (cuerpo 0x634500): suelta la referencia a la entidad si no es nula. Para los objetos creados por la DLL (vtable propia); los del EXE usan el DestructorBody enganchado.
CTaskSimpleTriggerLookAt::~CTaskSimpleTriggerLookAt() {
    CEntity::SafeCleanUpRef(m_pEntity);
}

// 0x634560 (lote 182A, sin validar; thiscall, sin argumentos, `ret`, 176 bytes con marco SEH que solo desenrolla): copia de la tarea. Con hueso >= 0 (con signo) y SIN entidad, copia con hueso = -1 y tiempo = 100 (0x64);
// si no, con el hueso y el tiempo propios. Reserva 0x30 bytes con CTask::operator new (0x61A5A0; nulo -> devuelve nulo sin construir) y llama al constructor con (entidad, tiempo, hueso, desplazamiento, torso (byte
// ampliado a cero), velocidad (dword), tiempo de mezcla, prioridad (byte con signo)); devuelve el resultado del constructor.
CTask* CTaskSimpleTriggerLookAt::Clone() const {
    int32 time = m_time;
    int32 bone = m_nOffsetBoneTag;
    if (bone > -1 && !m_pEntity) {
        bone = -1;
        time = 100;
    }
    void* const memory = CTask::operator new(sizeof(CTaskSimpleTriggerLookAt));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CTaskSimpleTriggerLookAt*>(memory)->Constructor(m_pEntity, time, bone, m_vOffsetPos, m_bUseTorso, m_Speed, m_nBlendTime, m_nPriority);
}

// 0x634620 (lote 182A, sin validar; thiscall, un puntero en la pila, `ret 4`, 76 bytes): si la entidad existio (+0x2C) pero ya es nula (+8) no hace nada; si no, IKChainManager_c::LookAt (0x618970, con hook) sobre g_ikChainMan
// (0xC15448) con ("TaskTriggerLookAt" 0x86E3F0, ped, entidad, tiempo, hueso, &desplazamiento, torso (byte ampliado a cero), velocidad, tiempo de mezcla, prioridad (byte con signo), false). Siempre devuelve true.
bool CTaskSimpleTriggerLookAt::ProcessPed(CPed* ped) {
    if (!m_bEntityExist || m_pEntity) {
        eBoneTag32 bone;
        bone.m_Value = m_nOffsetBoneTag; // el dword del EXE tal cual, sin pasar por el enum
        g_ikChainMan.LookAt(
            "TaskTriggerLookAt",
            ped,
            m_pEntity,
            m_time,
            bone,
            reinterpret_cast<CVector*>(&m_vOffsetPos),
            m_bUseTorso,
            m_Speed,
            m_nBlendTime,
            m_nPriority,
            false
        );
    }
    return true;
}
