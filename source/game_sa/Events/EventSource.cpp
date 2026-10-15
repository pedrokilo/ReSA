#include "StdInc.h"

#include "EventSource.h"
#include "Event.h"
#include "Ped.h"
#include "PedIntelligence.h"
#include "X87Math.h"

// Como el original (0x4ABAC0): ECX/EDX de salida son residuo de GetSourceEntity y de los colaboradores (los bancos los comparan).
// El EXE no escribe ECX/EDX tras ninguna de las cuatro llamadas salvo en la rama de tipo distinto de ped (CL = tipo & 7), asi que:
//  - Las llamadas se declaran __fastcall y devuelven uint64 (EDX:EAX): el EDX de salida de cada una se recoge en la mitad alta y se
//    pasa como EDX a la siguiente (el EXE no lo toca entre llamadas). La prueba de AL se hace sobre la copia en memoria
//    (`mov eax,[..]; and eax,0FFh`) y no con `movzx ecx,al; test ecx,ecx`, que pisaba ECX; es sobre AL y no sobre EAX porque los
//    colaboradores solo fijan AL.
//  - El EDX de entrada se lee con CarEnterExitEntryEdxCpp (lo primero que se ejecuta) y se pasa a GetSourceEntity: el virtual normal
//    cargaba la tabla en EDX y dejaba esa direccion como residuo en la rama de fuente nula.
//  - La rama de tipo distinto de ped termina con ResidueRegsCpp(CL, EDX de GetSourceEntity, 0).
// IsThreatenedBy (0x601C30): thiscall(intelligence, source) pero el EXE entra con EDX = victim; IsFriendlyWith (0x601BC0) e IsPlayer
// (0x5DF8F0) son thiscall que no leen EDX. Los tres se llaman como __fastcall con ese EDX explicito (mismos `ret 4`/`ret`).
using ExeGetSourceEntity = uint64(__fastcall*)(const CEvent* event, uint32 edx);                                          // vtable[10] (thiscall)
using ExeIsThreatenedBy  = uint64(__fastcall*)(const CPedIntelligence*, const CPed* edxVictim, const CPed* source);       // 0x601C30
using ExeIsFriendlyWith  = uint64(__fastcall*)(const CPedIntelligence*, uint32 edx, const CPed* source);                 // 0x601BC0
using ExeIsPlayer        = uint64(__fastcall*)(const CPed*, uint32 edx);                                                 // 0x5DF8F0

// 0x4ABAC0
#pragma runtime_checks("", off) // /RTC1 añade __RTC_CheckEsp tras las llamadas y cambia ECX/EDX
int32 CEventSource::ComputeEventSourceType(const CEvent& event, const CPed& ped) {
    // Lo primero: ninguna instruccion anterior escribe EDX (lo vigila contrib/audit_register_residue.py)
    const uint32 entryEdx = uint32(CarEnterExitEntryEdxCpp() >> 32);

    const auto         getSource    = reinterpret_cast<ExeGetSourceEntity>((*reinterpret_cast<void* const* const*>(&event))[10]);
    const uint64       sourceResult = getSource(&event, entryEdx);
    const CEntity* const source     = reinterpret_cast<const CEntity*>(uint32(sourceResult));
    if (!source) {
        return 0; // ECX/EDX tal como los deja GetSourceEntity
    }

    // Tipo en CL & 7 (byte +0x36), no GetIsTypePed(): el EXE deja CL = tipo & 7 en la rama de salida
    const uint32 typeBits = reinterpret_cast<const uint8*>(source)[0x36] & 7u;
    if (typeBits != ENTITY_TYPE_PED) {
        return int32(ResidueRegsCpp(typeBits, uint32(sourceResult >> 32), 0));
    }

    const auto* const sourcePed    = reinterpret_cast<const CPed*>(source);
    const auto* const intelligence = ped.GetIntelligence();

    const uint64 threatened = reinterpret_cast<ExeIsThreatenedBy>(0x601C30)(intelligence, &ped, sourcePed);
    if ((uint32(threatened) & 0xFFu) != 0) {
        return 3;
    }
    const uint32 edxAfterThreat = uint32(threatened >> 32);

    const uint64 friendly = reinterpret_cast<ExeIsFriendlyWith>(0x601BC0)(intelligence, edxAfterThreat, sourcePed);
    if ((uint32(friendly) & 0xFFu) != 0) {
        return 2;
    }
    const uint32 edxAfterFriendly = uint32(friendly >> 32);

    const uint64 player = reinterpret_cast<ExeIsPlayer>(0x5DF8F0)(sourcePed, edxAfterFriendly);
    if ((uint32(player) & 0xFFu) != 0) {
        return 1;
    }
    return 0;
}
#pragma runtime_checks("", restore)

void CEventSource::InjectHooks() {
    RH_ScopedClass(CEventSource);
    RH_ScopedCategory("Events");
    RH_ScopedInstall(ComputeEventSourceType, 0x4ABAC0);
}
