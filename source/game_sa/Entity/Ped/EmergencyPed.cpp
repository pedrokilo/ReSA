#include "StdInc.h"

#include "EmergencyPed.h"

namespace {
// 0x5DE3B0 (lote 178A, sin validar; el listado de entrada decia CCopPed): cuerpo del destructor de CEmergencyPed del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86C200h; jmp 0x5E8620`). Deja la vtable
// de la clase del EXE (0x86C200) y salta a CPed::Destructor (0x5E8620, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x5DE3B0` del EXE.
__declspec(noinline) void __fastcall EmergencyPedDestructorBody(CEmergencyPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C200;
    self->CPed::Destructor();
}

// 0x5DE3E0 (lote 178A, sin validar): destructor con borrado de CEmergencyPed del EXE (ranura 0 de su vtable 0x86C200; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x5DE3B0;
// test byte [esp+8],1; je ..; push esi; call 0x5E4760; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5DE3B0), libera `this` con CPed::operator delete (0x5E4760,
// con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEmergencyPed* __fastcall EmergencyPedScalarDeletingDestructor(CEmergencyPed* self, void* /*edx*/, uint8 flags) {
    EmergencyPedDestructorBody(self, nullptr);
    if (flags & 1) {
        CPed::operator delete(self);
    }
    return self;
}
} // namespace

void CEmergencyPed::InjectHooks() {
    RH_ScopedVirtualClass(CEmergencyPed, 0x86C200, 26);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedNamedGlobalInstall(EmergencyPedDestructorBody, "DestructorBody", 0x5DE3B0);                         // lote 178A (sin validar)
    RH_ScopedNamedGlobalInstall(EmergencyPedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5DE3E0);     // lote 178A (sin validar)
    RH_ScopedInstall(Constructor, 0x5DE340);                                                                     // lote 178A (sin validar)
    RH_ScopedVMTInstall(ProcessControl, 0x5DE400);                                                               // lote 178A (sin validar)
}

// 0x5DE340 (lote 178A, sin validar; el listado de entrada decia CCopPed; thiscall, dos argumentos de pila, `ret 8`, 97 bytes con marco SEH que solo desenrolla): constructor del EXE. Llama al constructor de CPed
// (0x5E8030, con hook) con el tipo, deja la vtable de la clase del EXE (0x86C200), llama a CPed::SetModelIndex (0x5E4880, con hook, llamada directa: la clase no lo redefine) con el indice de modelo y a
// CAEPedSpeechAudioEntity::Initialise (0x4E68D0, con hook; ECX = +0x294, argumento = this). Devuelve this.
CEmergencyPed::CEmergencyPed(ePedType pedType, uint32 modelIndex) : CPed(pedType) {
    SetModelIndex(modelIndex); // V1053 Calling the 'SetModelIndex' virtual function in the constructor may lead to unexpected result at runtime
    m_pedSpeech.Initialise(this);
}

// 0x5DE340 (banco Fn178A): con la forma del EXE. El constructor de C++ (CEmergencyPed(...) : CPed(...)) llamaria al constructor de CPed generado por el compilador (no a la envoltura con hook 0x5E8030) y dejaria la vtable
// de la DLL; aqui, como el EXE: CPed::Constructor (0x5E8030), vtable del EXE (0x86C200), CPed::SetModelIndex directo (0x5E4880) y CAEPedSpeechAudioEntity::Initialise (0x4E68D0). Devuelve this.
CEmergencyPed* CEmergencyPed::Constructor(ePedType pedType, uint32 modelIndex) {
    CPed::Constructor(pedType);
    *reinterpret_cast<uint32*>(this) = 0x86C200;
    CPed::SetModelIndex(modelIndex);
    m_pedSpeech.Initialise(this);
    return this;
}

void CEmergencyPed::MedicAI() {
    // NOP
}

void CEmergencyPed::FiremanAI() {
    // NOP
}

// 0x5DE400 (lote 178A, sin validar; thiscall, sin argumentos, `ret`, 108 bytes; ranura 10 de la vtable 0x86C200): llama a CPed::ProcessControl (0x5E8CD0, llamada directa); si el bit 6 de las banderas (+0x1C: aplazado)
// esta puesto o el estado del ped es PEDSTATE_DEAD (0x37) termina. Si no, actualiza el arma activa (CWeapon::Update, 0x73DB40, con el propietario nulo; el indice del slot se relee de +0x718 con signo en cada
// llamada: 28 bytes por slot desde +0x5A0); si el estado es PEDSTATE_DIE (0x36) termina; si no, la actualiza otra vez y, segun el tipo (+0x598): MEDIC (18) termina, FIREMAN (19) sale por un `jmp` a
// CPed::IsPedInControl (0x5E3960; el resultado se ignora) y cualquier otro termina.
void CEmergencyPed::ProcessControl() {
    CPed::ProcessControl();
    if (GetWasPostponed() || m_nPedState == PEDSTATE_DEAD)
        return;

    // El EXE relee el slot (BYTE con signo, `movsx`) en cada llamada y no comprueba el rango: indexacion sin comprobar (std::array::operator[] la comprueba) con el slot como int8.
    const auto activeWeapon = [this]() -> CWeapon& { return m_aWeapons.data()[static_cast<int8>(m_nActiveWeaponSlot)]; };
    activeWeapon().Update(nullptr);
    if (m_nPedState != PEDSTATE_DIE) {
        activeWeapon().Update(nullptr);
        if (m_nPedType == PED_TYPE_FIREMAN) { // el EXE: tipo == MEDIC termina; tipo == FIREMAN llama a IsPedInControl
            CPed::IsPedInControl();
        }
    }
}

// 0x5DE470
bool CEmergencyPed::InRange(CPed* ped) {
    if (!m_pVehicle)
        return true;

    return (m_pVehicle->GetPosition() - ped->GetPosition()).Magnitude() <= 30.0f;
}
