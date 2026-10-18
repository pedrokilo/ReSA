/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerRelationshipRecorder
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PlayerRelationshipRecorder.h"
#include "TaskCategories.h"
#include "X87Math.h"
#include "PedIntelligence.h"
#include "TaskManager.h"
#include "Task.h"

// 0x61A130
CPlayerRelationshipRecorder::CPlayerRelationshipRecorder() {
    Flush();
}

// 0x61A2C0
CPlayerRelationshipRecorder::~CPlayerRelationshipRecorder() {
    Flush();
}

// 0x61A2A0
void CPlayerRelationshipRecorder::Flush() {
    for (auto& relationship : m_Relationships) {
        relationship.Flush();
    }
}

// 0x61A180
void CPlayerRelationshipRecorder::AddRelationship(const CPed* ped, int32 value) {
    auto& rel = m_Relationships[0];
    if (rel.Ped) {
        rel.Ped = ped;
        rel.Relationship = value;
    }
}

// 0x61A1D0: RecordRelationshipWithPlayer. Forma normal para el resto del codigo; el hook del EXE usa RecordRelationshipWithPlayerRegs.
void CPlayerRelationshipRecorder::RecordRelationshipWithPlayer(const CPed* ped) {
    RecordRelationshipWithPlayerRegs(this, 0, ped);
}

// 0x61A1D0 (ret 4, this en ECX, EDX de entrada sin usar). Que hace el EXE:
//  1. Borra la PRIMERA entrada cuyo Ped coincide: Ped = 0 y solo el byte bajo de Relationship = 0 (no los tres altos).
//  2. task = CTaskManager::GetActiveTask() de ped->m_pIntelligence + 4 (0x681720). Sin tarea sale.
//  3. Con tarea llama a 0x6985E0 (Kill: pone a2 = a3 = 0, llama a task->GetTaskType() [vtable+0x10] y solo a2 = 1 para 0x3FB..0x3FD),
//     a 0x698610 (Follow: pone a2 = a3 = 0 y nada mas) y otra vez a Kill; luego mira SIEMPRE a3, que nadie escribe distinto de 0
//     (en el EXE y en el banco: a3 es el byte que ambos ponen a 0). Por tanto las escrituras Relationship 3 y 7 (0x61A258 y 0x61A28E)
//     son codigo muerto: el flujo real es "dos llamadas a GetTaskType y salir". Se omiten, igual que lo documenta
//     reversing/DependenciesDecisionRelationship6/README.md.
// Registros al salir (los bancos estrictos comparan EAX/ECX/EDX):
//  - Sin tarea: EAX = 0, ECX = ped->m_pIntelligence + 4 (el CTaskManager), EDX = EDX de entrada.
//  - Con tarea: el ultimo Kill deja EDX = EDX con que salio GetTaskType (el vtable de la tarea en las implementaciones triviales),
//    ECX = task (GetTaskType no lo toca; el EXE lo deja asi porque Kill hace `mov ecx,[esp+4]` y no lo cambia despues) y
//    EAX = eTaskType con el byte bajo puesto a 0 (`mov al,[a3]` con a3 = 0).
// EDX/EAX de la llamada virtual se leen declarandola con retorno de 64 bits (como CEventScriptCommand::Destructor); el residuo de ECX no se
// puede leer desde C++ y se asume = task (cierto en los GetTaskType triviales, que son `mov eax,imm; ret`). Sin comprobaciones de ejecucion
// para que /RTC1 no cambie ECX/EDX tras la ultima llamada.
#pragma runtime_checks("", off)
uint32 __fastcall CPlayerRelationshipRecorder::RecordRelationshipWithPlayerRegs(CPlayerRelationshipRecorder* self, uint32 entryEdx, const CPed* ped) {
    // 1. Primera coincidencia: puntero a null y solo el byte bajo de Relationship a 0.
    for (size_t i = 0; i < self->m_Relationships.size(); ++i) {
        PlayerRelationship& rel = self->m_Relationships.data()[i];
        if (rel.Ped == ped) {
            rel.Ped = nullptr;
            *reinterpret_cast<uint8*>(&rel.Relationship) = 0;
            break;
        }
    }

    // 2. CTaskManager = ped->m_pIntelligence + 4 (m_TaskMgr va justo despues de m_pPed). El EXE usa lea ecx,[eax+4] sin comprobar nulos.
    const uint32 taskMgr = static_cast<uint32>(reinterpret_cast<uintptr_t>(ped->GetIntelligence())) + 4;
    CTask* const task    = reinterpret_cast<CTaskManager*>(static_cast<uintptr_t>(taskMgr))->GetActiveTask();
    if (!task) {
        return ResidueRegsCpp(taskMgr, entryEdx, 0);
    }

    // 3. Dos llamadas a task->GetTaskType() (Kill, Follow, Kill); la primera solo por su efecto, la segunda deja EAX/EDX de salida.
    using GetTaskTypeFn = uint64(__thiscall*)(CTask*);
    const GetTaskTypeFn getTaskType = reinterpret_cast<GetTaskTypeFn*>(*reinterpret_cast<void***>(task))[4];
    (void)getTaskType(task);
    const uint64 residue = getTaskType(task);
    return ResidueRegsCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(task)), static_cast<uint32>(residue >> 32), static_cast<uint32>(residue) & 0xFFFFFF00u);
}
#pragma runtime_checks("", restore)

// 0x61A1A0
int32 CPlayerRelationshipRecorder::GetRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return relationship.Relationship;
    }
    return 0;
}

// 0x61A150
void CPlayerRelationshipRecorder::ClearRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return relationship.Flush();
    }
}

// 0x61A2E0
CPlayerRelationshipRecorder& GetPlayerRelationshipRecorder() {
    static auto& g_sPlayerRelationshipRecorder = StaticRef<CPlayerRelationshipRecorder*>(0xC17084);

    if (!g_sPlayerRelationshipRecorder) {
        g_sPlayerRelationshipRecorder = new CPlayerRelationshipRecorder();
    }
    return *g_sPlayerRelationshipRecorder;
}

void CPlayerRelationshipRecorder::InjectHooks() {
    RH_ScopedClass(CPlayerRelationshipRecorder);
    RH_ScopedCategory("Ped");
    RH_ScopedNamedInstall(RecordRelationshipWithPlayerRegs, "RecordRelationshipWithPlayer", 0x61A1D0);
}
