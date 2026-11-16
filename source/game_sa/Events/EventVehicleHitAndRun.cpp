/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleHitAndRun
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleHitAndRun.h"

namespace {
// 0x4AEA50 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleHitAndRun que usan los destructores del EXE. Deja la vtable de CEventVehicleHitAndRun del EXE (0x85AFA8), libera las referencias de m_vehicle (+0x10) y despues de m_victim (+0xC) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleHitAndRunDestructorBody(CEventVehicleHitAndRun* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AFA8;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    CEntity::SafeCleanUpRef(self->m_victim);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B5FE0 (lote 204A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable de CEventVehicleHitAndRun; thiscall, un argumento de pila, `ret 4`, 76 bytes): destructor con borrado del EXE. Como el original: llama al cuerpo del destructor
// (0x4AEA50, con hook propio) y, con el bit 0 del BYTE de banderas, libera la ranura en el pool de eventos (0xB744AC) EN LINEA, igual que CEvent::operator delete (0x4B5630, con hook: indice = (this - almacenamiento) / 0x44 con signo,
// estados[indice] |= 0x80, cursor = min con signo); devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
__declspec(noinline) CEventVehicleHitAndRun* __fastcall VehicleHitAndRunScalarDeletingDestructor(CEventVehicleHitAndRun* self, void* /*edx*/, uint8 flags) {
    VehicleHitAndRunDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventVehicleHitAndRun::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleHitAndRun, 0x85AFA8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(VehicleHitAndRunDestructorBody, "Destructor", 0x4AEA50);
    RH_ScopedNamedGlobalInstall(VehicleHitAndRunScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B5FE0); // lote 204A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AE990);
    RH_ScopedVMTInstall(Clone, 0x4B7100);
    RH_ScopedVMTInstall(GetEventType, 0x4AEA00);       // lote 199A (sin validar): retorno de constante, ranura 1
    RH_ScopedVMTInstall(GetEventPriority, 0x4AEA40);   // lote 199A (sin validar): retorno de constante, ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x4AEA10);        // lote 199A (sin validar): retorno de constante, ranura 3
    RH_ScopedVMTInstall(AffectsPed, 0x4AEA20);         // lote 199A (sin validar): ranura 5
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4AEA30); // lote 199A (sin validar): ranura 12
    RH_ScopedVMTInstall(IsCriminalEvent, 0x4B27B0); // lote 142A (banco Fn142A): `m_vehicle->m_pDriver` no nulo y CPed::IsPlayer (el listado la atribuia a CEventDanger), ranura 7
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B27D0);
}

CEventVehicleHitAndRun::CEventVehicleHitAndRun(CPed* victim, CVehicle* vehicle)
{
    m_victim = victim;
    m_vehicle = vehicle;
    m_vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_vehicle)); // *
    m_victim->RegisterReference(reinterpret_cast<CEntity**>(&m_victim));   // ** todo: possible nullptr
}

CEventVehicleHitAndRun::~CEventVehicleHitAndRun()
{
    CEntity::SafeCleanUpRef(m_vehicle);
    CEntity::SafeCleanUpRef(m_victim);
}

// 0x4AE990
CEventVehicleHitAndRun* CEventVehicleHitAndRun::Constructor(CPed* victim, CVehicle* vehicle)
{
    this->CEventVehicleHitAndRun::CEventVehicleHitAndRun(victim, vehicle);
    return this;
}

// 0x4AEA00 (lote 199A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,0Eh`): EVENT_VEHICLE_HIT_AND_RUN (14); no lee el objeto (ranura 1 de la vtable 0x85AFA8).
eEventType CEventVehicleHitAndRun::GetEventType() const {
    return EVENT_VEHICLE_HIT_AND_RUN;
}

// 0x4AEA40 (lote 199A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,0Eh`): prioridad 14; no lee el objeto (ranura 2).
int32 CEventVehicleHitAndRun::GetEventPriority() const {
    return 14;
}

// 0x4AEA10 (lote 199A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventVehicleHitAndRun::GetLifeTime() {
    return 0;
}

// 0x4AEA20 (lote 199A, sin validar; thiscall, un argumento de pila, `ret 4`, 12 bytes: `mov ecx,[esp+4]; call 0x5E0170; ret 4`): devuelve CPed::IsAlive del peaton (estado distinto de 0x36 y 0x37); no lee el objeto (ranura 5).
bool CEventVehicleHitAndRun::AffectsPed(CPed* ped) {
    return ped->IsAlive();
}

// 0x4AEA30 (lote 199A, sin validar; thiscall, `ret`, 7 bytes: `fld dword ptr [0x858628]`): 100.0f en ST0 (el EXE no lo redondea ni lo guarda); no lee el objeto (ranura 12).
float CEventVehicleHitAndRun::GetLocalSoundLevel() {
    return 100.0f;
}

// 0x4B27D0
void CEventVehicleHitAndRun::ReportCriminalEvent(CPed* ped)
{
    if (IsCriminalEvent()) {
        if (m_victim->m_nPedType == PED_TYPE_COP)
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_KILL_COP_PED_WITH_CAR, m_vehicle->GetPosition(), (uint32)m_vehicle->m_pDriver, false);
        else
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_KILL_PED_WITH_CAR, m_vehicle->GetPosition(), (uint32)m_vehicle->m_pDriver, false);
    }
}
