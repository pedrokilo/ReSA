/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventVehicleOnFire
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventVehicleOnFire.h"

namespace {
// 0x4B1170 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventVehicleOnFire que usan los destructores del EXE. Deja la vtable de CEventVehicleOnFire del EXE (0x85B440), libera la referencia de m_vehicle (+0x14) con CEntity::CleanUpOldReference (0x571A00) si no son nulas
// y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 107A, banco Fn107A.)
void __fastcall VehicleOnFireDestructorBody(CEventVehicleOnFire* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B440;
    CEntity::SafeCleanUpRef(self->m_vehicle);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6570 (lote 192A, sin validar): destructor con borrado de CEventVehicleOnFire del EXE (ranura 0 de su vtable 0x85B440; thiscall, un argumento de pila, `ret 4`, 76 bytes: `call 0x4B1170; test byte [esp+..],1; je ..;`
// y, con el bit 0 del BYTE de banderas, el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice)); devuelve `this`). El EXE conserva ECX tras la llamada al cuerpo (sin pila extra).
// Como el original: destruye con el cuerpo de la clase (0x4B1170), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventVehicleOnFire* __fastcall VehicleOnFireScalarDeletingDestructor(CEventVehicleOnFire* self, void* /*edx*/, uint8 flags) {
    VehicleOnFireDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventVehicleOnFire::InjectHooks()
{
    RH_ScopedVirtualClass(CEventVehicleOnFire, 0x85B440, 17);
    RH_ScopedCategory("Events");
    RH_ScopedVMTInstall(GetEventType, 0x4B1130); // lote 201A (sin validar): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x4B1140); // lote 201A (sin validar): retorno de constante
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4B1150); // lote 201A (sin validar): retorno de constante x87
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1160); // lote 201A (sin validar): retorno de constante
    RH_ScopedNamedGlobalInstall(VehicleOnFireDestructorBody, "Destructor", 0x4B1170);
    RH_ScopedNamedGlobalInstall(VehicleOnFireScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6570); // lote 192A (sin validar)

    RH_ScopedInstall(Constructor, 0x4B10C0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B4FD0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7740);
}

// 0x4B10C0
CEventVehicleOnFire::CEventVehicleOnFire(CVehicle* vehicle)
{
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
}

CEventVehicleOnFire::~CEventVehicleOnFire()
{
    CEntity::SafeCleanUpRef(m_vehicle);
}

CEventVehicleOnFire* CEventVehicleOnFire::Constructor(CVehicle* vehicle)
{
    this->CEventVehicleOnFire::CEventVehicleOnFire(vehicle);
    return this;
}

// 0x4B4FD0
bool CEventVehicleOnFire::AffectsPed(CPed* ped)
{
    if (m_vehicle) {
        if (!ped->m_pVehicle || !ped->bInVehicle || ped->m_pVehicle == m_vehicle) {
            if (!ped->IsPlayer() && ped->IsAlive()) {
                CPedGroup* pedGroup = CPedGroups::GetPedsGroup(ped);
                if (pedGroup && !pedGroup->GetMembership().IsLeader(ped)) {
                    if (ped->bInVehicle)
                        return false;
                }
                if (ped->bInVehicle)
                    return true;
                // Como el original: suma (x² + z²) + y² y un NaN cuenta como "cerca".
                CVector distance = ped->GetPosition() - m_vehicle->GetPosition();
                const float distanceSq = distance.x * distance.x + distance.z * distance.z + distance.y * distance.y;
                return !(distanceSq > 100.0f);
            }
        }
    }
    return false;
}

// 0x4B7740
CEventEditableResponse* CEventVehicleOnFire::CloneEditable() const noexcept
{
    return new CEventVehicleOnFire(m_vehicle);
}

// 0x4B1130 (lote 201A, sin validar; el listado la daba como CEventVehicleOnFire y SI lo es; thiscall, `ret`, 6 bytes: `mov eax,4Fh`): EVENT_VEHICLE_ON_FIRE (79); no lee el objeto (ranura 1 de la vtable 0x85B440).
eEventType CEventVehicleOnFire::GetEventType() const {
    return EVENT_VEHICLE_ON_FIRE;
}

// 0x4B1160 (lote 201A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,36h`): prioridad 54; no lee el objeto (ranura 2).
int32 CEventVehicleOnFire::GetEventPriority() const {
    return 54;
}

// 0x4B1140 (lote 201A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventVehicleOnFire::GetLifeTime() {
    return 0;
}

// 0x4B1150 (lote 201A, sin validar; thiscall, `ret`, 7 bytes: `fld dword [0x858B34]`): nivel de sonido local 60.0f (constante del EXE 0x42700000) en ST(0); no lee el objeto (ranura 12). Un solo float, sin aritmetica.
float CEventVehicleOnFire::GetLocalSoundLevel() {
    return 60.0f;
}
