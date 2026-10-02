/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CScriptResourceManager
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ScriptResourceManager.h"

void CScriptResourceManager::InjectHooks() {
    RH_ScopedClass(CScriptResourceManager);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470480);
    RH_ScopedInstall(AddToResourceManager, 0x4704B0);
    RH_ScopedInstall(RemoveFromResourceManager, 0x470510);
    RH_ScopedInstall(HasResourceBeenRequested, 0x470620);
    //RH_ScopedInstall(Load, 0x0);
    //RH_ScopedInstall(Save, 0x0);
}

// 0x470480
void CScriptResourceManager::Initialise() {
    for (auto& resource : m_aScriptResources) {
        resource.m_nModelId = -1;
        resource.m_pThread = nullptr;
        resource.m_nType = RESOURCE_TYPE_DEFAULT;
    }
}

// 0x4704B0
void CScriptResourceManager::AddToResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    tScriptResource* freeSlot = nullptr;
    for (auto& resource : m_aScriptResources) {
        if (resource.m_nModelId == modelId && resource.m_nType == type && resource.m_pThread == script) {
            return;
        }
        if (resource.m_nType == RESOURCE_TYPE_DEFAULT) {
            freeSlot = &resource;
        }
    }
    if (freeSlot) {
        freeSlot->m_nModelId = modelId;
        freeSlot->m_nType = static_cast<uint16>(type);
        freeSlot->m_pThread = script;
    }
}

// 0x470510
bool CScriptResourceManager::RemoveFromResourceManager(int32 modelId, eScriptResourceType type, CRunningScript* script) {
    tScriptResource* owned = nullptr;
    bool usedByAnotherScript = false;
    for (auto& resource : m_aScriptResources) {
        if (resource.m_nModelId == modelId && resource.m_nType == type) {
            if (resource.m_pThread == script) {
                owned = &resource;
            } else {
                usedByAnotherScript = true;
            }
        }
    }
    if (owned) {
        owned->m_nModelId = -1;
        owned->m_nType = RESOURCE_TYPE_DEFAULT;
        owned->m_pThread = nullptr;
    }
    return !usedByAnotherScript;
}

// 0x470620
bool CScriptResourceManager::HasResourceBeenRequested(int32 modelId, eScriptResourceType type) {
    return rng::any_of(m_aScriptResources, [=](const auto& resource) {
        return resource.m_nModelId == modelId && resource.m_nType == type;
    });
}

// 0x0
bool CScriptResourceManager::Load() {
    assert(false);
    return true;
}

// 0x0
bool CScriptResourceManager::Save() {
    assert(false);
    return true;
}
