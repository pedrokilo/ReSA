/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexKillAllThreats
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

#include "TaskComplexKillAllThreats.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleStandStill.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x623750 (lote 170A, banco Fn170A; el listado lo atribuia a CTaskSimpleGangDriveBy): Clone de CTaskComplexKillAllThreats del EXE (ranura 1 de su vtable 0x86DA1C; thiscall, sin argumentos de pila, `ret`, 106 bytes con marco
// SEH que solo desenrolla). La clase aun no esta reconstruida (sin campos): se leen los tres dwords +0xC, +0x10 y +0x14 en crudo, como el EXE (que los guarda en registros ANTES de construir). Como el original: reserva 0x18 bytes
// con CTask::operator new (0x61A5A0, con hook); sin memoria devuelve nulo; si no, base CTaskComplex (0x61A3B0, con hook), +0x10 y +0x14 copiados, vtable de la clase del EXE (0x86DA1C), +0xC copiado, y devuelve el objeto
// nuevo. Se engancha como S (la clase aun no tiene una declaracion virtual que nombre la ranura).
CTask* __fastcall CTaskComplexKillAllThreatsCloneExe(const CTaskComplexKillAllThreats* self, void* /*edx*/) {
    const uint8* const source = reinterpret_cast<const uint8*>(self);
    const uint32 field0C = *reinterpret_cast<const uint32*>(source + 0xC);
    const uint32 field10 = *reinterpret_cast<const uint32*>(source + 0x10);
    const uint32 field14 = *reinterpret_cast<const uint32*>(source + 0x14);
    void* const memory = CTask::operator new(0x18);
    if (!memory) {
        return nullptr;
    }
    CTaskComplexBody::Constructor(memory, nullptr);
    uint8* const bytes = static_cast<uint8*>(memory);
    *reinterpret_cast<uint32*>(bytes + 0x10) = field10;
    *reinterpret_cast<uint32*>(bytes + 0x14) = field14;
    *reinterpret_cast<uint32*>(bytes) = 0x86DA1C;
    *reinterpret_cast<uint32*>(bytes + 0xC) = field0C;
    return static_cast<CTask*>(memory);
}
} // namespace

static_assert(CTaskComplexKillAllThreats::Type == 0x3F6); // 0x6221B0: `mov eax,3F6h`

void CTaskComplexKillAllThreats::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillAllThreats, 0x86DA1C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x622180);                // lote 190A (sin validar)
    RH_ScopedInstall(Destructor, 0x6221C0);                 // lote 190A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x6293A0);   // lote 190A (sin validar)
    RH_ScopedInstall(ScanForThreat, 0x6221D0);              // lote 190A (sin validar)
    RH_ScopedNamedGlobalInstall(CTaskComplexKillAllThreatsCloneExe, "Clone", 0x623750); // lote 170A (banco Fn170A)
    RH_ScopedVMTInstall(GetTaskType, 0x6221B0);             // lote 190A (sin validar)
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6293C0);       // lote 190A (sin validar)
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x629480);      // lote 190A (sin validar)
    RH_ScopedVMTInstall(ControlSubTask, 0x622230);          // lote 190A (sin validar)
}

// 0x622180 (lote 190A, sin validar; thiscall, tres argumentos de pila, `ret 0Ch`, 41 bytes): base CTaskComplex (0x61A3B0), +0xC, vtable de la clase del EXE (0x86DA1C), +0x10 y +0x14; devuelve this.
CTaskComplexKillAllThreats::CTaskComplexKillAllThreats(int32 pedFlags, int32 actionDelay, int32 actionChance) :
    m_pedFlags{pedFlags},
    m_actionDelay{actionDelay},
    m_actionChance{actionChance}
{
}

// 0x6221C0 (lote 190A, sin validar; thiscall, `ret`, 11 bytes: `mov [ecx],86DA1Ch; jmp 0x61A3D0`): cuerpo del destructor; escribe la vtable de la clase y sigue con el cuerpo de ~CTaskComplex (0x61A3D0).
// En C++ lo cubre el destructor de la clase y su base CTaskComplex.
CTaskComplexKillAllThreats::~CTaskComplexKillAllThreats() {
}

// 0x623750 (lote 170A, banco Fn170A): el Clone del EXE esta enganchado como S (CTaskComplexKillAllThreatsCloneExe); la ranura de la vtable de la DLL llama a esa misma funcion para no duplicarla.
CTask* CTaskComplexKillAllThreats::Clone() const {
    return CTaskComplexKillAllThreatsCloneExe(this, nullptr);
}

// 0x6221D0 (lote 190A, sin validar; thiscall, un argumento de pila, `ret 4`, 89 bytes; no lee `this`): busca un conocido. Como el original: un CPedAcquaintanceScanner local con el temporizador a cero
// (inicio, intervalo, arrancado, detenido) y solo el escaneo de peatones de script permitido (+0x10 = 1, +0x11 = 0, +0x12 = 0); una variable local de salida a cero; llama a ScanEntities (0x607A90, con hook)
// con el ped, prioridad 4, los 16 peatones del escaner del ped (intel + 0x130), 16 y las dos variables de salida (la segunda se deja sin escribir: solo la rellena FindAcquaintance al acertar y aqui nadie la lee);
// devuelve la primera salida (el peaton encontrado o nulo).
CPed* CTaskComplexKillAllThreats::ScanForThreat(CPed* ped) {
    int32 foundPed = 0;
    int32 foundId;
    CPedAcquaintanceScanner scanner{};
    scanner.SetOnlyScriptPedAllowed();
    scanner.ScanEntities(*ped, 4, ped->GetIntelligence()->GetPedEntities(), 16, &foundPed, &foundId);
    return reinterpret_cast<CPed*>(foundPed);
}

// 0x6293C0 (lote 190A, sin validar; thiscall, un argumento de pila, `ret 4`, 178 bytes con marco SEH que solo desenrolla; ranura 8 de la vtable 0x86DA1C). Como el original: pide el tipo de la subtarea actual (llamada
// virtual, ranura 4); TASK_SIMPLE_STAND_STILL (0xCB) y cualquier tipo que no sea TASK_COMPLEX_KILL_PED_ON_FOOT (0x3E8) devuelven nulo. Para 0x3E8 busca una amenaza (0x6221D0); sin ella devuelve nulo; si no, reserva
// 0x38 bytes con CTask::operator new (0x61A5A0; sin memoria devuelve nulo) y construye CTaskComplexKillPedOnFoot (0x620E30, con hook) con (amenaza, -1, +0xC, +0x10, +0x14, 1).
CTask* CTaskComplexKillAllThreats::CreateNextSubTask(CPed* ped) {
    const eTaskType subTaskType = m_pSubTask->GetTaskType();
    if (subTaskType != TASK_COMPLEX_KILL_PED_ON_FOOT) { // incluye TASK_SIMPLE_STAND_STILL (0xCB)
        return nullptr;
    }
    CPed* const threat = ScanForThreat(ped);
    if (!threat) {
        return nullptr;
    }
    void* const memory = CTask::operator new(sizeof(CTaskComplexKillPedOnFoot));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskComplexKillPedOnFoot(threat, -1, m_pedFlags, m_actionDelay, m_actionChance, 1);
}

// 0x629480 (lote 190A, sin validar; thiscall, un argumento de pila, `ret 4`, 194 bytes con marco SEH que solo desenrolla; ranura 9). Como el original: busca una amenaza (0x6221D0). Con amenaza, igual que CreateNextSubTask:
// 0x38 bytes y CTaskComplexKillPedOnFoot con (amenaza, -1, +0xC, +0x10, +0x14, 1). Sin amenaza, reserva 0x20 bytes (0x61A5A0) y construye CTaskSimpleStandStill (0x62F310, con hook) con (0, false, false, 8.0f).
// Sin memoria, nulo en ambos casos.
CTask* CTaskComplexKillAllThreats::CreateFirstSubTask(CPed* ped) {
    CPed* const threat = ScanForThreat(ped);
    if (threat) {
        void* const memory = CTask::operator new(sizeof(CTaskComplexKillPedOnFoot));
        if (!memory) {
            return nullptr;
        }
        return ::new (memory) CTaskComplexKillPedOnFoot(threat, -1, m_pedFlags, m_actionDelay, m_actionChance, 1);
    }
    void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CTaskSimpleStandStill(0, false, false, 8.0f);
}

// 0x6293A0 (lote 190A, sin validar): destructor con borrado de CTaskComplexKillAllThreats del EXE (ranura 0 de su vtable 0x86DA1C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6221C0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6221C0; aqui su
// envoltura Destructor()), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexKillAllThreats* CTaskComplexKillAllThreats::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
