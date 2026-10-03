/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGunShot
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

#include "EventGunShot.h"

void CEventGunShot::InjectHooks() {
    RH_ScopedVirtualClass(CEventGunShot, 0x85ABE0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AC610);
    RH_ScopedVMTInstall(AffectsPed, 0x4B2CD0);
    RH_ScopedVMTInstall(IsCriminalEvent, 0x4AC810);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4AC780);
    RH_ScopedVMTInstall(CloneEditable, 0x4B6B20);
}

CEventGunShot* CEventGunShot::Constructor(CEntity* entity, CVector startPoint, CVector endPoint, bool bHasNoSound) { this->CEventGunShot::CEventGunShot(entity, startPoint, endPoint, bHasNoSound); return this; }

// 0x4AC610
CEventGunShot::CEventGunShot(CEntity* entity, CVector startPoint, CVector endPoint, bool bHasNoSound) : CEventEditableResponse() {
    m_startPoint = startPoint;
    m_endPoint = endPoint;
    m_firedBy = entity;
    m_bHasNoSound = bHasNoSound;
    CEntity::SafeRegisterRef(m_firedBy);
}

CEventGunShot::~CEventGunShot() {
    CEntity::SafeCleanUpRef(m_firedBy);
}

// 0x4B2CD0
bool CEventGunShot::AffectsPed(CPed* ped) {
    if (!m_firedBy)
        return false;

    if (m_firedBy->GetIsTypePed() && CPedGroups::AreInSameGroup(ped, m_firedBy->AsPed()))
        return false;

    if (ped->IsInVehicleThatHasADriver())
        return false;

    // Como el original: FindPlayerWanted solo se llama para policías y se vuelve a llamar en cada uso;
    // el nivel de búsqueda se compara con signo (jle).
    if (ped->m_nPedType == PED_TYPE_COP && static_cast<int32>(FindPlayerWanted()->GetWantedLevel()) > 0) {
        CCopPed* cop = static_cast<CCopPed*>(ped);
        if (FindPlayerWanted()->IsInPursuit(cop) || FindPlayerWanted()->CanCopJoinPursuit(cop)) {
            if (m_firedBy != FindPlayerPed())
                return false;
        }
    }

    if (!ped->IsAlive() || ped == m_firedBy || ped->IsPlayer())
        return false;

    // Como el original: distancia al cuadrado de (peatón - tirador) con el orden de suma del EXE ((z*z + y*y) + x*x)
    const CVector& firedByPos = m_firedBy->GetPosition();
    const CVector& pedPos     = ped->GetPosition();
    const float    dx         = pedPos.x - firedByPos.x;
    const float    dy         = pedPos.y - firedByPos.y;
    const float    dz         = pedPos.z - firedByPos.z;
    const float    distSq     = dz * dz + dy * dy + dx * dx;

    // Como el original: un rango de disturbios NaN toma la rama de "<= 0"
    float fGunShotRange = 45.0f;
    if (!(CEventGunShot::ms_fGunShotSenseRangeForRiot2 > 0.0f)) {
        if (ped->IsCreatedByMission()) {
            fGunShotRange = ped->GetIntelligence()->GetPedFOVRange();
        }
    } else if (ped->IsCreatedByMission()) {
        fGunShotRange = CEventGunShot::ms_fGunShotSenseRangeForRiot2;
    }

    // Como el original: solo se descarta si la distancia es mayor (una comparación con NaN sigue adelante)
    if (distSq > fGunShotRange * fGunShotRange)
        return false;

    if (!m_bHasNoSound)
        return true;

    // Como el original: producto escalar con el orden del EXE ((y*fy + z*fz) + x*fx); NaN cuenta como positivo
    const CVector  dir = m_startPoint - pedPos;
    const CVector& fwd = ped->GetForward();
    const float    dot = dir.y * fwd.y + dir.z * fwd.z + dir.x * fwd.x;
    if (dot <= 0.0f)
        return false;

    return CWorld::GetIsLineOfSightClear(m_startPoint, pedPos, true, true, true, true, true, false, false);
}

// 0x4AC810
bool CEventGunShot::IsCriminalEvent() {
    return m_firedBy && m_firedBy->GetIsTypePed() && m_firedBy->AsPed()->IsPlayer();
}

// 0x4AC780
bool CEventGunShot::TakesPriorityOver(const CEvent& refEvent) {
    if (refEvent.GetEventType() == GetEventType()) {
        bool bIsPlayer = false;
        bool otherPedIsPlayer = false;
        const auto refEventGunShot = static_cast<const CEventGunShot*>(&refEvent);
        if (m_firedBy && m_firedBy->AsPed()->GetIsTypePed()) {
            bIsPlayer = m_firedBy->AsPed()->IsPlayer();
        }

        CPed* otherPed = refEventGunShot->m_firedBy->AsPed();
        if (otherPed && otherPed->GetIsTypePed())
            otherPedIsPlayer = otherPed->IsPlayer();

        return bIsPlayer && !otherPedIsPlayer;
    }
    return GetEventPriority() >= refEvent.GetEventPriority();
}

// 0x4B6B20
CEventEditableResponse* CEventGunShot::CloneEditable() const noexcept {
    return new CEventGunShot(m_firedBy, m_startPoint, m_endPoint, m_bHasNoSound);
}
