/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowPatrolRoute
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

#include "TaskComplexFollowPatrolRoute.h"
#include "PatrolRoute.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x674930 (lote 175A, banco Fn175A; el listado lo atribuia a CTaskComplexWanderProstitute): constructor de CTaskComplexFollowPatrolRoute del EXE (thiscall, CINCO argumentos de pila, `ret 14h`, 182 bytes, marco SEH que
// solo desenrolla). Argumentos: a (16 bits bajos), ruta origen, b (16 bits bajos), c y d (32 bits). Como el original, en este orden: base CTaskComplex (0x61A3B0, con hook); +0x10 = a; vtable de la clase del EXE (0x870114);
// +0xC = b; +0x20 con el bit 1 a cero (el resto no se toca); reserva una ruta de patrulla con CPatrolRoute::operator new (0x41B810, con hook; el EXE le pasa 0x1A4) y, SOLO si hay memoria, la construye con 0x66D440; guarda
// el EAX de esa llamada (el constructor devuelve this; nulo si no hubo memoria) en +0x1C y llama a 0x66D4B0 con ella (SIN comprobar nulo: con el pool lleno el EXE se cae, como esta funcion) para copiar la ruta origen; +0x14 = c, +0x18 = d, el bit 0 de +0x20 a uno
// (releido) y +0x12 y +0xE a cero. Devuelve `this`. El constructor de la clase no existe aun: el del EXE deja la vtable del EXE.
__declspec(noinline) CTaskComplexFollowPatrolRoute* __fastcall CTaskComplexFollowPatrolRouteConstructorExe(CTaskComplexFollowPatrolRoute* self, void* /*edx*/, int32 a, const CPatrolRoute* route, int32 b, uint32 c, uint32 d) {
    CTaskComplexBody::Constructor(self, nullptr);
    self->m_nParam10 = static_cast<int16>(a);
    *reinterpret_cast<uint32*>(self) = 0x870114;
    self->m_nParamC = static_cast<int16>(b);
    self->m_nFlags &= ~2u;
    auto* const memory   = static_cast<CPatrolRoute*>(CPatrolRoute::operator new(sizeof(CPatrolRoute)));
    CPatrolRoute* newRoute = memory ? memory->Constructor() : nullptr; // el EXE usa el EAX que devuelve el constructor (this), no el de la reserva
    self->m_pRoute         = newRoute;
    newRoute->Assign(route);
    self->m_nParam14 = c;
    self->m_nFlags |= 1u;
    self->m_nParam18 = d;
    self->m_nParam12 = 0;
    self->m_nParamE  = 0;
    return self;
}
} // namespace

void CTaskComplexFollowPatrolRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPatrolRoute, 0x870114, 11); // vtable verificado en gta_sa.exe (GetTaskType en [4]); N por confirmar
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(CTaskComplexFollowPatrolRouteConstructorExe, "ConstructorExe", 0x674930); // lote 175A (banco Fn175A)
    // RH_ScopedVMTInstall(GetTaskType, 0x670A60);
    // TODO: instalar las funciones conforme se reconstruyan.
}
