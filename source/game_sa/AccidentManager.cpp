/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAccidentManager
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AccidentManager.h"

namespace {
// 0x56CE30 (lote 129A, banco Fn129A; el listado de pendientes decia CWorld): constructor de CAccidentManager del EXE (thiscall, `ret`, 51 bytes; lo llama GetInstance, 0x56CF5E, sobre la memoria
// recien reservada). Construye las 16 entradas de CAccident con el iterador del CRT (0x821D30, que llama 16 veces a 0x56CDE0) y despues vuelve a poner a cero en linea los mismos campos de cada
// entrada (m_pPed, m_bIsTreated, m_bIsRevived); los bytes de relleno +6 y +7 de cada entrada no se tocan. Devuelve this.
CAccidentManager* __fastcall AccidentManagerConstructorExe(CAccidentManager* self, void* /*edx*/) {
    for (auto& accident : self->m_Accidents) {
        accident.m_pPed = nullptr;
        accident.m_bIsTreated = false;
        accident.m_bIsRevived = false;
    }
    return self;
}
} // namespace

void CAccidentManager::InjectHooks()
{
    RH_ScopedClass(CAccidentManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(AccidentManagerConstructorExe, "Constructor", 0x56CE30);

    RH_ScopedInstall(GetInstance, 0x56CF20);
    RH_ScopedInstall(ReportAccident, 0x56CE80);
    RH_ScopedInstall(GetNumberOfFreeAccidents, 0x56CEE0);
    RH_ScopedInstall(GetNearestFreeAccidentExceptThisOne, 0x56CF90);
    RH_ScopedInstall(GetNearestFreeAccident, 0x56D050);
}

// 0x56CF20
CAccidentManager* CAccidentManager::GetInstance()
{
    if (!gAccidentManager)
        gAccidentManager = new CAccidentManager();

    return gAccidentManager;
}

// 0x56CE80
void CAccidentManager::ReportAccident(CPed* ped)
{
    if (!ped->IsCreatedBy(PED_MISSION) && ped->bAllowMedicsToReviveMe)
    {
        int32 slotIndex = -1;
        for (int32 i = 0; i < NUM_ACCIDENTS; i++)
        {
            if (m_Accidents[i].m_pPed == ped)
                return;

            if (!m_Accidents[i].m_pPed && slotIndex == -1)
                slotIndex = i;
        }

        if (slotIndex != -1)
        {
            m_Accidents[slotIndex].m_pPed = ped;
            m_Accidents[slotIndex].m_bIsTreated = false;
            m_Accidents[slotIndex].m_bIsRevived = false;
            ped->RegisterReference(reinterpret_cast<CEntity**>(&m_Accidents[slotIndex].m_pPed));
        }
    }
}

// 0x56CEE0
int32 CAccidentManager::GetNumberOfFreeAccidents()
{
    int32 count = 0;

    for (auto& acc : m_Accidents)
        if (acc.IsFree())
            count++;

    return count;
}

// 0x56CF90
CAccident* CAccidentManager::GetNearestFreeAccidentExceptThisOne(CVector& posn, CAccident* thisOne, bool bIgnoreHeadless)
{
    float minDistance = FLT_MAX;
    CAccident* nearestAcc = nullptr;

    for (auto& acc : m_Accidents)
    {
        if (acc.IsFree()
            && !(acc.m_pPed->bRemoveHead && bIgnoreHeadless)
            && !(thisOne && thisOne == &acc)
            )
        {
            float distance = DistanceBetweenPoints(acc.m_pPed->GetPosition(), posn);
            if (distance < minDistance)
            {
                minDistance = distance;
                nearestAcc = &acc;
            }
        }
    }

    return nearestAcc;
}

// 0x56D050
CAccident* CAccidentManager::GetNearestFreeAccident(CVector& posn, bool bIgnoreHeadless)
{
    return GetNearestFreeAccidentExceptThisOne(posn, nullptr, bIgnoreHeadless);
}
