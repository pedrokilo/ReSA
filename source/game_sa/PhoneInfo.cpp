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

#include "PhoneInfo.h"
#include "AnimBlendAssociation.h"
#include "Pad.h"
#include "Ped.h"

void CPhoneInfo::InjectHooks() {
    RH_ScopedClass(CPhoneInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4541E0);
    RH_ScopedInstall(PhoneAnimFinishCallback, 0x453F40);
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
