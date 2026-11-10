/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventInteriorUseInfo
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
#include "EventInteriorUseInfo.h"


void CEventInteriorUseInfo::InjectHooks() {
    RH_ScopedVirtualClass(CEventInteriorUseInfo, 0x8701A8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedVMTInstall(IsValid, 0x4B1640); // lote 153A (banco Fn153A): ranura 14 de la vtable 0x8701A8
}

CEventInteriorUseInfo::CEventInteriorUseInfo(InteriorInfo_t* interiorInfo, Interior_c* interior, uint32 animTime, int8 loopAction) : CEvent() {
    m_InteriorInfo = interiorInfo;
    m_nLoopAction = loopAction;
    m_Interior = interior;
    m_ActionAnimTime = animTime;
}

// 0x4B1640 (lote 153A, banco Fn153A; thiscall, un puntero en la pila, `ret 4`, 54 bytes): con ped no nulo devuelve CPed::IsAlive (0x5E0170, con hook) con ECX = ped; sin ped, como CEvent::IsValid en linea:
// verdadero si el byte +8 (m_bValid) no es cero; si no, compara m_nTimeActive (+4) con GetLifeTime() (ranura 3, llamada virtual) y es verdadero si no lo supera (jle, con signo).
bool CEventInteriorUseInfo::IsValid(CPed* ped) {
    if (ped)
        return ped->IsAlive();
    return CEvent::IsValid(ped);
}

bool CEventInteriorUseInfo::AffectsPed(CPed* ped) {
    return true;
}

