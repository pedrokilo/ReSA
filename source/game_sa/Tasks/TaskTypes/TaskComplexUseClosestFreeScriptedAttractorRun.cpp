/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseClosestFreeScriptedAttractorRun
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractorRun.h"

void CTaskComplexUseClosestFreeScriptedAttractorRun::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractorRun, 0x85b904, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x4B8790);
    RH_ScopedInstall(Destructor, 0x4B8840);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4B8820);

    RH_ScopedVMTInstall(Clone, 0x4B87B0);
    RH_ScopedVMTInstall(GetTaskType, 0x4B8810);
}

// 0x4B8790
CTaskComplexUseClosestFreeScriptedAttractorRun::CTaskComplexUseClosestFreeScriptedAttractorRun() :
    CTaskComplexUseClosestFreeScriptedAttractor{PEDMOVE_RUN}
{
}

// 0x4B8820 (lote 127A, banco Fn127A) : destructor con borrado de CTaskComplexUseClosestFreeScriptedAttractorRun del EXE (ranura 0 de su vtable 0x85B904; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4B8840; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x4B8840), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexUseClosestFreeScriptedAttractorRun* CTaskComplexUseClosestFreeScriptedAttractorRun::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
