/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerRelationshipRecorder
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
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
// Como el original: solo se mira la primera entrada (sin comparar el peaton) y de `value` se guarda unicamente el byte bajo (`mov dl,[esp+8]`)
void CPlayerRelationshipRecorder::AddRelationship(const CPed* ped, int32 value) {
    auto& rel = m_Relationships[0];
    if (rel.Ped) {
        rel.Ped = ped;
        *reinterpret_cast<uint8*>(&rel.Relationship) = (uint8)value;
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
// Como el original: la primera entrada cuyo peaton coincide (tambien un peaton nulo) y se devuelve solo el byte bajo (AL)
int32 CPlayerRelationshipRecorder::GetRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return *reinterpret_cast<const uint8*>(&relationship.Relationship);
    }
    return 0;
}

// 0x61A150
// Como el original: borra la primera entrada cuyo peaton coincide (peaton a nulo y solo el byte bajo de la relacion)
void CPlayerRelationshipRecorder::ClearRelationshipWithPlayer(const CPed* ped) {
    for (auto& relationship : m_Relationships) {
        if (relationship.Ped != ped)
            continue;

        return relationship.Flush();
    }
}

// 0x61A2E0 (lote 151A, banco Fn151A; el listado decia CPlayerRelationshipRecorder y lo es): cdecl, sin argumentos, `ret`, 64 bytes; devuelve el puntero al registrador unico (0xC17084) en EAX (sus dos llamadores,
// 0x607328 y 0x6082BA, hacen `mov ecx, eax` y no leen EDX). Como el original: si la variable ya tiene valor, lo devuelve; si no, reserva 0x80 bytes con el operator new del CRT del EXE (0x82119A, sin hook, por esa
// direccion: es `operator new`, que el EXE llama directamente) y, si devuelve algo, pone a cero las 16 entradas con SOLO un dword (el peaton) y el byte bajo de la relacion (los tres bytes altos de cada entrada
// quedan como los dejo la reserva: la imagen del constructor C++, que ponia a cero la relacion entera, no es la del EXE) y guarda el puntero; si no devuelve nada, guarda nulo (y devuelve nulo).
CPlayerRelationshipRecorder& GetPlayerRelationshipRecorder() {
    static auto& g_sPlayerRelationshipRecorder = StaticRef<CPlayerRelationshipRecorder*>(0xC17084);

    if (!g_sPlayerRelationshipRecorder) {
        void* const memory = reinterpret_cast<void*(__cdecl*)(size_t)>(0x82119A)(sizeof(CPlayerRelationshipRecorder));
        auto* const recorder = static_cast<CPlayerRelationshipRecorder*>(memory);
        if (recorder) {
            for (size_t i = 0; i < 16; ++i) {
                recorder->m_Relationships.data()[i].Flush();
            }
        }
        g_sPlayerRelationshipRecorder = recorder;
    }
    return *g_sPlayerRelationshipRecorder;
}

void CPlayerRelationshipRecorder::InjectHooks() {
    RH_ScopedClass(CPlayerRelationshipRecorder);
    RH_ScopedCategory("Ped");
    RH_ScopedGlobalInstall(GetPlayerRelationshipRecorder, 0x61A2E0); // lote 151A (banco Fn151A)
    RH_ScopedNamedInstall(RecordRelationshipWithPlayerRegs, "RecordRelationshipWithPlayer", 0x61A1D0);
    RH_ScopedInstall(ClearRelationshipWithPlayer, 0x61A150);
    RH_ScopedInstall(AddRelationship, 0x61A180);
    RH_ScopedInstall(GetRelationshipWithPlayer, 0x61A1A0);
    RH_ScopedInstall(Flush, 0x61A2A0);
}
