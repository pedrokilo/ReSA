/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CScriptsForBrains
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
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
    RH_ScopedInstall(CheckIfNewEntityNeedsScript, 0x46FF20);
    RH_ScopedInstall(MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded, 0x46AAE0);
    //RH_ScopedInstall(SwitchAllObjectBrainsWithThisID, 0x46A900, {.reversed = false});
    //RH_ScopedInstall(AddNewScriptBrain, 0x46A930, {.reversed = false});
    //RH_ScopedInstall(AddNewStreamedScriptBrainForCodeUse, 0x46A9C0, {.reversed = false});
    RH_ScopedInstall(GetIndexOfScriptBrainWithThisName, 0x46AA30);
    RH_ScopedInstall(HasAttractorScriptBrainWithThisNameLoaded, 0x46AB20);
    RH_ScopedInstall(StartNewStreamedScriptBrain, 0x46B270);
    RH_ScopedInstall(StartAttractorScriptBrainWithThisName, 0x46B390);
    RH_ScopedInstall(StartOrRequestNewStreamedScriptBrain, 0x46CD80);
    //RH_ScopedInstall(StartOrRequestNewStreamedScriptBrainWithThisName, 0x46CED0, {.reversed = false});
    RH_ScopedInstall(IsObjectWithinBrainActivationRange, 0x46B3D0);
    RH_ScopedInstall(RequestAttractorScriptBrainWithThisName, 0x46AA80);
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

// 0x46FF20
void CScriptsForBrains::CheckIfNewEntityNeedsScript(CEntity* entity, int8 attachType, void* unused) {
    UNUSED(unused);
    // Como el original: tipo 0 = ped (0x800000 = ya tiene cerebro, 0x1000000 = esperando a que cargue); el resto, objeto (0x300000).
    if (attachType == 0) {
        const auto* ped = static_cast<CPed*>(entity);
        if (ped->bHasAScriptBrain || ped->bWaitingForScriptBrainToLoad) {
            return;
        }
    } else if (static_cast<CObject*>(entity)->m_nObjectFlags & 0x300000) {
        return;
    }

    // Recorre los 70 cerebros; el primero que coincide en tipo y modelo (16 bits) y supera la tirada arranca/solicita el script y termina.
    for (uint8 i = 0; i < m_aScriptForBrains.size(); i++) {
        const auto& brain = m_aScriptForBrains[i];
        if (brain.m_TypeOfBrain != attachType || static_cast<uint16>(brain.m_PedModelOrPedGeneratorIndex) != static_cast<uint16>(entity->m_nModelIndex)) {
            continue;
        }
        // Como el original: rand del EXE (0x821B1E) & 0xFFFF, * 2^-15 (0x858B14), * 100.0 (0x858628), truncado (_ftol2).
        // El producto es exacto en float (k < 65536, k * 100 < 2^24), asi que no depende de la precision del x87.
        const auto exeRand = reinterpret_cast<int(__cdecl*)()>(0x821B1E);
        const int32 roll = static_cast<int32>(static_cast<float>(static_cast<uint32>(exeRand()) & 0xFFFFu) * std::bit_cast<float>(0x38000000u) * 100.0f);
        if (roll < static_cast<int32>(brain.m_PercentageChance)) {
            StartOrRequestNewStreamedScriptBrain(i, entity, attachType, true);
            return;
        }
    }
}

void CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(const char* name) {
    const auto index = GetIndexOfScriptBrainWithThisName(name, 5);
    if (index < 0) return;
    const auto script = m_aScriptForBrains[index].m_StreamedScriptIndex;
    auto& users = CTheScripts::StreamedScripts.m_aScripts[script].m_NumberOfUsers;
    if (users) --users;
}

// 0x46AA80
void CScriptsForBrains::RequestAttractorScriptBrainWithThisName(const char* name) {
    const auto index = GetIndexOfScriptBrainWithThisName(name, 5);
    if (index < 0) {
        return;
    }
    const auto script  = m_aScriptForBrains[index].m_StreamedScriptIndex;
    const auto modelId = SCMToModelId(script);
    // Como el original: el estado de carga se lee del arreglo de informacion de streaming; si no es LOADED (1) pide el modelo con 0x10.
    if (!CStreaming::IsModelLoaded(modelId)) {
        CStreaming::RequestModel(modelId, STREAMING_PRIORITY_REQUEST);
    }
    CTheScripts::StreamedScripts.m_aScripts[script].m_NumberOfUsers++;
}

// 0x46B270
void CScriptsForBrains::StartNewStreamedScriptBrain(uint8 index, CEntity* entity, bool bHasAScriptBrain) {
    const auto& brain  = m_aScriptForBrains[index];
    auto* const script = CTheScripts::StreamedScripts.StartNewStreamedScript(brain.m_StreamedScriptIndex);
    script->m_ExternalType = brain.m_TypeOfBrain;

    // Tabla de saltos del EXE (0x46B36C): 0, 3 y 5 = ped; 1 y 4 = objeto; 2 y el resto (incluidos negativos) = nada mas.
    switch (brain.m_TypeOfBrain) {
    case 0:
    case 3:
    case 5: {
        auto* const ped = static_cast<CPed*>(entity);
        script->m_LocalVars[0].iParam = GetPedPool()->GetRef(ped);
        ped->bHasAScriptBrain = true; // 0x800000
        if (brain.m_TypeOfBrain == 5) {
            script->m_LocalVars[1].iParam = static_cast<int32>(static_cast<uint8>(bHasAScriptBrain));
        }
        const auto toLoad = ped->m_StreamedScriptBrainToLoad;
        ped->bWaitingForScriptBrainToLoad = false; // 0x1000000
        CTheScripts::RemoveFromWaitingForScriptBrainArray(ped, toLoad);
        ped->m_StreamedScriptBrainToLoad = -1;
        break;
    }
    case 1:
    case 4: {
        auto* const object = static_cast<CObject*>(entity);
        script->m_LocalVars[0].iParam = GetObjectPool()->GetRef(object);
        object->m_nObjectFlags |= 0x300000;
        break;
    }
    default:
        break;
    }
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
