/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CScripted2dEffects
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

#include "Scripted2dEffects.h"

void CScripted2dEffects::InjectHooks() {
    RH_ScopedClass(CScripted2dEffects);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Init, 0x6FA6F0);
    RH_ScopedInstall(GetEffectPairs, 0x6FA840);
    RH_ScopedInstall(GetIndex, 0x6F9F60);
    RH_ScopedInstall(AddScripted2DEffect, 0x6FA7C0);
    RH_ScopedInstall(ReturnScripted2DEffect, 0x6F9E80);
}

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
        auto& pairs = ms_effectPairs[i];
        pairs.NumPairs = 0;
        for (auto& pair : pairs.Pairs) {
            for (auto& effect : pair.Effects) {
                effect.Effect = -1;
                effect.WaitingTask = -1;
                effect.PartnerUseMode = CScriptedEffectPair::eMode::NONE;
            }
            // The original reset (0x6F9EB0) preserves the second PartnerTask
            // and structure padding. Do not assign a value-initialized pair.
            pair.Effects[0].PartnerTask = -1;
            pair.UsePartnerImmediately = false;
        }
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
        auto& pairs = ms_effectPairs[slot];
        for (auto& pair : pairs.Pairs) {
            for (auto& e : pair.Effects) {
                e.Effect         = -1;
                e.WaitingTask    = -1;
                e.PartnerUseMode = CScriptedEffectPair::eMode::NONE;
            }
            pair.Effects[0].PartnerTask = -1;
            pair.UsePartnerImmediately  = false;
        }
        pairs.NumPairs         = 0;
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
