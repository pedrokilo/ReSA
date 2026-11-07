/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseEffectRunning
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseEffectRunning.h"

namespace {
// 0x636780 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskComplexUseEffectRunning del EXE (thiscall, dos punteros en la pila, `ret 8`, devuelve this). Constructor de CTaskComplexUseEffect (0x6321F0) con los mismos dos argumentos, vtable 0x86E594 y +0x1C = 6 (estado de movimiento: correr).
CTaskComplexUseEffectRunning* __fastcall UseEffectRunningConstructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) {
    reinterpret_cast<CTaskComplexUseEffect*>(self)->Constructor(effect, entity);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x86E594;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = 6;
    return self;
}
} // namespace

void CTaskComplexUseEffectRunning::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffectRunning, 0x86e594, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(UseEffectRunningConstructorBody, "Constructor", 0x636780);

    RH_ScopedVMTInstall(Clone, 0x6367C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6367B0);
}

// 0x636780
CTaskComplexUseEffectRunning::CTaskComplexUseEffectRunning(C2dEffectPedAttractor* fx, CEntity* e) :
    CTaskComplexUseEffect{ fx, e }
{
    m_MoveState = PEDMOVE_RUN;
}

// 0x6367C0
CTaskComplexUseEffectRunning::CTaskComplexUseEffectRunning(const CTaskComplexUseEffectRunning& o) :
    CTaskComplexUseEffectRunning{ o.m_2dFx, o.m_Entity }
{
}
