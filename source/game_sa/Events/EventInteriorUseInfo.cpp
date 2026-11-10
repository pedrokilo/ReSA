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

