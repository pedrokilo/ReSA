/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CScripted2dEffects
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

#include "Scripted2dEffects.h"
#include "X87Math.h"

void CScripted2dEffects::InjectHooks() {
    RH_ScopedClass(CScripted2dEffects);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x6FA6F0);
    RH_ScopedInstall(GetEffectPairs, 0x6FA840);
    RH_ScopedInstall(GetIndex, 0x6F9F60);
    RH_ScopedInstall(AddScripted2DEffect, 0x6FA7C0);
    RH_ScopedInstall(ReturnScripted2DEffect, 0x6F9E80);

    // lote 132A (banco Fn132A)
    RH_ScopedNamedGlobalInstall(CScriptedEffectPairs::Reset, "ResetEffectPairs", 0x6F9EB0);
}

// 0x6F9EB0 (lote 132A, banco Fn132A): en cada uno de los cuatro pares, Effect, WaitingTask y PartnerUseMode de los dos efectos a -1, el PartnerTask del PRIMER efecto a -1 (el del segundo no se
// toca) y UsePartnerImmediately a 0 (solo ese byte: el relleno no se toca); al final NumPairs = 0. Como el original (el EXE no usa un par inicializado por valor). Contrato de registros: el bucle de
// `Init` del EXE (0x6FA740) conserva ECX = this entre pares (`add ecx, 0x94` tras la llamada), asi que la funcion sale con ECX = this, EDX = 0 y EAX = -1, como la del EXE (ResidueRegsCpp, sin
// comprobaciones de ejecucion para que nada toque los registros tras la ultima llamada).
#pragma runtime_checks("", off)
void CScriptedEffectPairs::Reset() {
    for (auto& pair : Pairs) {
        for (auto& effect : pair.Effects) {
            effect.Effect         = -1;
            effect.WaitingTask    = -1;
            effect.PartnerUseMode = CScriptedEffectPair::eMode::NONE;
        }
        pair.Effects[0].PartnerTask = -1;
        pair.UsePartnerImmediately  = false;
    }
    NumPairs = 0;
    ResidueRegsCpp(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this)), 0u, 0xFFFFFFFFu);
}
#pragma runtime_checks("", restore)

// 0x6FA6F0
void CScripted2dEffects::Init() {
    for (auto i = 0u; i < NUM_SCRIPTED_2D_EFFECTS; i++) {
        ms_activated[i]             = false;
        ScriptReferenceIndex[i]     = 1;
        ms_effectSequenceTaskIDs[i] = -1;
        ms_userLists[i].m_bUseList  = false;
        ms_useAgainFlags[i]         = false;
        ms_radii[i]                 = -1.0f;
        rng::fill(ms_userLists[i].m_UserTypes, -1);
        rng::fill(ms_userLists[i].m_UserTypesByPedType, -1);
        ms_effectPairs[i].Reset();
    }
}

// 0x6FA840
CScriptedEffectPairs* CScripted2dEffects::GetEffectPairs(const C2dEffectPedAttractor* effect) {
    // Como el original: sin comprobar el índice (con -1 queda justo antes del array).
    return &ms_effectPairs.data()[GetIndex(effect)];
}

// 0x6F9F60
int32 CScripted2dEffects::GetIndex(const C2dEffectPedAttractor* effect) {
    // Como el original: solo cuenta la dirección exacta de cada efecto (un puntero interior devuelve -1).
    for (int32 i = 0; i < NUM_SCRIPTED_2D_EFFECTS; i++) {
        if (reinterpret_cast<const C2dEffect*>(effect) == &ms_effects.data()[i]) {
            return i;
        }
    }
    return -1;
}

// 0x6FA7C0
int32 CScripted2dEffects::AddScripted2DEffect(float radius) {
    // Como el original: sin hueco libre devuelve -1; los pares se reinician como 0x6F9EB0 (conserva el
    // PartnerTask del segundo efecto y el relleno) y el resto de campos se escriben en el sitio.
    for (int32 slot = 0; slot < NUM_SCRIPTED_2D_EFFECTS; slot++) {
        if (ms_activated[slot]) {
            continue;
        }

        ms_activated[slot]             = true;
        ms_effectSequenceTaskIDs[slot] = -1;
        auto& list = ms_userLists[slot];
        for (auto i = 0; i < 4; i++) {
            list.m_UserTypes[i]          = -1;
            list.m_UserTypesByPedType[i] = -1;
        }
        list.m_bUseList = false;
        ms_effectPairs[slot].Reset();
        ms_useAgainFlags[slot] = false;
        ms_radii[slot]         = radius;
        return slot;
    }
    return -1;
}

// 0x6F9E80
void CScripted2dEffects::ReturnScripted2DEffect(int32 index) {
    // Como el original: sin comprobar índice ni tipo del efecto.
    ms_activated.data()[index] = false;
    GetPedAttractorManager()->RemoveEffect(reinterpret_cast<C2dEffectPedAttractor*>(&ms_effects.data()[index]));
}

// notsa
auto CScripted2dEffects::IndexOfEffect(const C2dEffectPedAttractor* effect) -> std::optional<size_t> {
    const auto fx = reinterpret_cast<const C2dEffect*>(effect);
    if (ms_effects.data() <= fx && fx < ms_effects.data() + ms_effects.size()) {
        return (size_t)(fx - ms_effects.data());
    }
    return std::nullopt;
}

// notsa
auto CScripted2dEffects::GetEffect(int32 index) -> C2dEffectPedAttractor* {
    return notsa::cast<C2dEffectPedAttractor>(&ms_effects[index]);
}

// notsa
int32 CScripted2dEffects::FindFreeSlot()  {
    for (auto&& [i, activated] : rngv::enumerate(ms_activated)) {
        if (!activated) {
            return (int32)(i);
        }
    }
    return -1;
}
