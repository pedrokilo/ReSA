/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPhoneInfo
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "DatosExe/DataBssG4.h"
#include "DatosExe/DataBssG2.h"
#include "DatosExe/DataBssG1.h"

#include "PhoneInfo.h"
#include "AnimBlendAssociation.h"
#include "Pad.h"
#include "Ped.h"
#include "Messages.h"
#include "AnimManager.h"
#include "Timer.h"

void CPhoneInfo::InjectHooks() {
    RH_ScopedClass(CPhoneInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4541E0);
    RH_ScopedInstall(PhoneAnimFinishCallback, 0x453F40);
    RH_ScopedInstall(PhoneAnimAnswerCallback, 0x453E40); // lote P6B (sin validar)
}

// 0x4541E0 (thiscall, sin argumentos de pila, `ret`, 30 bytes): constructor de CPhoneInfo del EXE. Llama a `eh vector constructor iterator` (0x821D30) sobre +8 con 50 elementos de 0x34 bytes,
// el constructor de cada elemento 0x453FA0 (`mov eax,ecx; ret`, no escribe nada) y el destructor 0x453FB0 (`ret`, solo desenrolla si un constructor lanzara), y devuelve this. Lo llama el inicializador
// estatico de ThePhoneInfo (0x84A225, ECX = 0x977BE8). Los 50 constructores no hacen nada, asi que no hay nada que escribir; `m_nMax` y `m_nCount` los deja el inicializador del juego.
CPhoneInfo* CPhoneInfo::Constructor() {
    return this;
}

// 0x453F40 (cdecl, dos argumentos de pila, `ret`, 81 bytes): callback de fin de la animacion de telefono (se registra con CAnimBlendAssociation::SetFinishCallback en 0x4543F1, dentro de la funcion de 0x454340 que termina la llamada; `data` es el peaton).
// En el orden del EXE: banderas de la asociacion (byte bajo de +0x2E) |= 4 (ANIMATION_IS_BLEND_AUTO_REMOVE) y m_BlendDelta = -1000.0f (0xC47A0000); CPad::GetPad(0) (0x53FB70, push 0) y se limpia el bit 6 del
// byte +0x10E del pad (bPlayerTalksOnPhone, byte bajo de DisablePlayerControls); si m_BlendAmount > 0.5f (fcomp con 0x858B8C = 0.5f, `test ah,41h; jne`: NaN y <= 0.5 no entran) se pone bUpdateAnimHeading
// (dword +0x46C |= 0x4000); y si el estado del peaton (+0x530) es 0x13 (PEDSTATE_MAKE_PHONECALL) se llama a CPed::SetPedState (0x5E4500) con PEDSTATE_IDLE (1).
// m_BlendAmount se lee DESPUES de la llamada a GetPad (el EXE hace fld tras el call), y la comparacion es de 24 bits sobre el float almacenado.
void CPhoneInfo::PhoneAnimFinishCallback(CAnimBlendAssociation* assoc, void* data) {
    auto* const ped = static_cast<CPed*>(data);

    reinterpret_cast<uint8*>(&assoc->m_Flags)[0] |= (uint8)ANIMATION_IS_BLEND_AUTO_REMOVE;
    assoc->m_BlendDelta = -1000.0f;

    CPad* const pad = CPad::GetPad(0);
    reinterpret_cast<uint8*>(&pad->DisablePlayerControls)[0] &= 0xBF;

    if (assoc->m_BlendAmount > 0.5f) {
        ped->bUpdateAnimHeading = true;
    }

    if (ped->m_nPedState == PEDSTATE_MAKE_PHONECALL) {
        ped->SetPedState(PEDSTATE_IDLE);
    }
}

namespace {
// Estado de la llamada de telefono del EXE (.data): 0x977BB8 peaton que llama, 0x977BBC y 0x977BC4 banderas, 0x977BC0 cabina en uso y 0x977BC8 instante en que acaban los mensajes.
auto& s_phoneCallingPed   = StaticRef<CPed*>(0x977BB8);
auto& s_phoneFlagBC       = StaticRef<uint8>(0x977BBC);
auto& s_phoneActivePhone  = StaticRef<void*>(0x977BC0);
auto& s_phoneFlagC4       = StaticRef<uint8>(0x977BC4);
auto& s_phoneMessagesEnd  = StaticRef<uint32>(0x977BC8);
// Campos de CPhone usados por el callback: seis punteros a texto en +0x0C, instante en +0x24 y estado en +0x2C.
auto& PhoneMessage(void* phone, size_t index) { return reinterpret_cast<const GxtChar**>(static_cast<uint8*>(phone) + 0x0C)[index]; }
auto& PhoneTime(void* phone) { return *reinterpret_cast<uint32*>(static_cast<uint8*>(phone) + 0x24); }
auto& PhoneState(void* phone) { return *reinterpret_cast<int32*>(static_cast<uint8*>(phone) + 0x2C); }
}

// 0x453E40 (lote P6B, sin validar; hueco 0x452F90: la pieza viva de esa rutina; nombre inferido: callback de fin de la animacion de descolgar, registrado con CAnimBlendAssociation::SetFinishCallback en 0x4546xx
// por CPhoneInfo::Update (el listado de huecos la daba como una rutina de 4 KB con tabla de saltos: los 4 KB de 0x453000..0x453DCB son una busqueda de ruta sobre rejilla sin llamadores, codigo muerto); cdecl, dos
// argumentos de pila (asociacion, cabina), `ret`, 242 bytes): por cada uno de los seis textos de la cabina (+0x0C) no nulos CMessages::AddMessageQ(texto, 3000, 0, true) y se suman 3000 ms; el instante de fin
// (0x977BC8) = CTimer::m_snTimeInMilliseconds + (suma como 16 bits); 0x977BC4 = 1, cabina activa (0x977BC0) = cabina, 0x977BBC = 0; el estado de la cabina (+0x2C) pasa a 7 si era 4 y si no a 8 (y entonces
// +0x24 = instante actual). El peaton que llama (0x977BB8) queda con m_nMoveState = 1 (PEDMOVE_STILL) y se le mezcla la animacion 3 de su grupo (+0x4D4) con retardo 8.0. Si m_BlendAmount de la asociacion es
// > 0.5f (fcomp con 0x858B8C, `test ah,41h; jne`: NaN y <= 0.5 no entran) y hay peaton, se le mezcla la animacion 0x93 del grupo 0 con retardo 8.0; en cualquier caso 0x977BB8 queda a NULL.
void CPhoneInfo::PhoneAnimAnswerCallback(CAnimBlendAssociation* assoc, void* phone) {
    uint32 totalTime = 0;
    for (size_t i = 0; i < 6; i++) {
        if (const GxtChar* const text = PhoneMessage(phone, i)) {
            CMessages::AddMessageQ(text, 3000, 0, true);
            totalTime += 3000;
        }
    }
    s_phoneMessagesEnd = static_cast<uint16>(totalTime) + CTimer::GetTimeInMS();
    const bool wasState4 = PhoneState(phone) == 4;
    s_phoneFlagC4 = 1;
    s_phoneActivePhone = phone;
    s_phoneFlagBC = 0;
    if (wasState4) {
        PhoneState(phone) = 7;
    } else {
        PhoneState(phone) = 8;
        PhoneTime(phone) = CTimer::GetTimeInMS();
    }
    CPed* const ped = s_phoneCallingPed;
    ped->m_nMoveState = PEDMOVE_STILL;
    CAnimManager::BlendAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), ped->m_nAnimGroup, static_cast<AnimationId>(3), 8.0f);
    if (assoc->m_BlendAmount > 0.5f) {
        if (s_phoneCallingPed) {
            CAnimManager::BlendAnimation(reinterpret_cast<RpClump*>(s_phoneCallingPed->GetRwObject()), static_cast<AssocGroupId>(0), static_cast<AnimationId>(0x93), 8.0f);
        }
    }
    s_phoneCallingPed = nullptr;
}
