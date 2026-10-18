/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CScriptsForBrains
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ScriptsForBrains.h"

void CScriptsForBrains::InjectHooks() {
    RH_ScopedClass(CScriptsForBrains);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Init, 0x46A8C0);
    RH_ScopedInstall(MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded, 0x46AAE0);
    //RH_ScopedInstall(SwitchAllObjectBrainsWithThisID, 0x46A900, {.reversed = false});
    //RH_ScopedInstall(AddNewScriptBrain, 0x46A930, {.reversed = false});
    //RH_ScopedInstall(AddNewStreamedScriptBrainForCodeUse, 0x46A9C0, {.reversed = false});
    RH_ScopedInstall(GetIndexOfScriptBrainWithThisName, 0x46AA30);
    RH_ScopedInstall(HasAttractorScriptBrainWithThisNameLoaded, 0x46AB20);
    //RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270, {.reversed = false});
    RH_ScopedInstall(StartAttractorScriptBrainWithThisName, 0x46B390);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrain, 0x46CD80);
    //RH_ScopedInstall(StartOrRequestNewStreamedScriptBrainWithThisName, 0x46CED0, {.reversed = false});
    RH_ScopedInstall(IsObjectWithinBrainActivationRange, 0x46B3D0);
}


// 0x46A8C0
void CScriptsForBrains::Init() {
    // Como el original: escribe campo a campo los 70 registros de 0x14 bytes; los bytes de
    // relleno (+5..+7) no se tocan y el nombre/modelo (+0xC..+0x13) se pone a cero.
    for (auto& script : m_aScriptForBrains) {
        script.m_StreamedScriptIndex = -1;
        script.m_TypeOfBrain = -1;
        script.m_ObjectGroupingId = -1;
        script.m_bBrainActive = true;
        script.m_ObjectBrainActivationRadius = 5.f; // 0x859D9C
        script.m_PedModelOrPedGeneratorIndex = 0;
        script.m_PercentageChance = 0;
        script.m_Pad = 0;
    }
}

void CScriptsForBrains::CheckIfNewEntityNeedsScript(CEntity* entity, int8 attachType, void* unused) {
    plugin::CallMethod<0x46FF20, CScriptsForBrains*, CEntity*, int8, void*>(this, entity, attachType, unused);
}

void CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name) {
    const auto index = GetIndexOfScriptBrainWithThisName(name, 5);
    if (index < 0) return;
    const auto script = m_aScriptForBrains[index].m_StreamedScriptIndex;
    auto& users = CTheScripts::StreamedScripts.m_aScripts[script].m_NumberOfUsers;
    if (users) --users;
}

void CScriptsForBrains::RequestAttractorScriptBrainWithThisName(const char* name) {
    plugin::CallMethod<0x46AA80, CScriptsForBrains*, const char*>(this, name);
}

void CScriptsForBrains::StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool bHasAScriptBrain) {
    plugin::CallMethodAndReturn<void, 0x46B270, CScriptsForBrains*, uint8, CEntity*, uint8>(this, index, entity, bHasAScriptBrain);
}

// 0x46CD80
void CScriptsForBrains::StartOrRequestNewStreamedScriptBrain(uint8 index, CEntity* entity, int8 attachType, bool bAddToWaitingArray) {
    // Como el original: la comprobacion previa solo se hace si se va a anadir a la lista de espera.
    if (bAddToWaitingArray) {
        if (!m_aScriptForBrains[index].m_bBrainActive) {
            return;
        }
        switch (attachType) {
        case 0:
        case 3: {
            auto* ped = static_cast<CPed*>(entity);
            if (ped->bHasAScriptBrain) { // 0x800000
                return;
            }
            if (ped->bWaitingForScriptBrainToLoad) { // 0x1000000
                return;
            }
            break;
        }
        case 1:
        case 4: {
            if (static_cast<CObject*>(entity)->m_nObjectFlags & 0x300000) {
                return;
            }
            break;
        }
        default:
            break;
        }
    }

    if (attachType == 1) {
        auto* obj = static_cast<CObject*>(entity);
        if (bAddToWaitingArray) {
            obj->m_nObjectFlags = (obj->m_nObjectFlags & ~0x200000u) | 0x100000u;
            obj->m_nStreamedScriptBrainToLoad = (int16)(uint16)index;
            CTheScripts::AddToWaitingForScriptBrainArray(obj, (int16)(uint16)index);
        } else {
            obj->m_nObjectFlags = (obj->m_nObjectFlags & ~0x100000u) | 0x200000u;
        }
    }

    const auto scriptModelId = SCMToModelId(m_aScriptForBrains[index].m_StreamedScriptIndex);
    if (CStreaming::IsModelLoaded(scriptModelId)) {
        StartNewStreamedScriptBrain(index, entity, false);
        return;
    }

    CStreaming::RequestModel(scriptModelId, STREAMING_MISSION_REQUIRED);

    if (attachType == 0 || (attachType > 2 && attachType <= 4)) {
        if (bAddToWaitingArray) {
            auto* ped = static_cast<CPed*>(entity);
            ped->bWaitingForScriptBrainToLoad = true; // 0x1000000
            ped->m_StreamedScriptBrainToLoad = (int16)(uint16)index;
            CTheScripts::AddToWaitingForScriptBrainArray(ped, (int16)(uint16)index);
        }
    }
}

bool CScriptsForBrains::HasAttractorScriptBrainWithThisNameLoaded(const char* name) {
    if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
        return CStreaming::IsModelLoaded(SCMToModelId(m_aScriptForBrains[idx].m_StreamedScriptIndex));
    }
    return false;
}

// 0x46B3D0
bool CScriptsForBrains::IsObjectWithinBrainActivationRange(CObject* entity, const CVector& point) {
    const auto& script = m_aScriptForBrains[entity->m_nStreamedScriptBrainToLoad];
    if (script.m_TypeOfBrain != 1) {
        return false;
    }
    // Como el original: copia la posicion y suma dz^2 + dy^2 + dx^2 (orden x87 del EXE).
    const CVector pos = entity->GetPosition();
    const float dx = point.x - pos.x;
    const float dy = point.y - pos.y;
    const float dz = point.z - pos.z;
    const float dist = std::sqrt((dz * dz + dy * dy) + dx * dx);
    return dist < script.m_ObjectBrainActivationRadius;
}

int16 CScriptsForBrains::GetIndexOfScriptBrainWithThisName(const char* name, int8 type) {
    const auto it = rng::find_if(m_aScriptForBrains, [=](tScriptForBrains& script) {
        return script.m_TypeOfBrain == type && !_stricmp(script.m_ScriptName, name);
    });
    return it != m_aScriptForBrains.end()
        ? rng::distance(m_aScriptForBrains.begin(), it)
        : -1;
}

void CScriptsForBrains::StartAttractorScriptBrainWithThisName(const char* name, CPed* ped, bool bHasAScriptBrain) {
    if (!ped->bWaitingForScriptBrainToLoad && !ped->bHasAScriptBrain) {
        if (const auto idx = GetIndexOfScriptBrainWithThisName(name, 5); idx >= 0) {
            StartNewStreamedScriptBrain(static_cast<uint8>(idx), ped, bHasAScriptBrain);
        }
    }
}
