/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: BoneNodeManager_c
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "BoneNodeManager.h"

/**
 * Registra las operaciones del pool fijo de BoneNode_c.
 */
void BoneNodeManager_c::InjectHooks() {
    RH_ScopedClass(BoneNodeManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x617330);
    RH_ScopedInstall(Destructor, 0x617390);

    RH_ScopedInstall(Init, 0x6173F0);
    RH_ScopedInstall(Exit, 0x617420);
    RH_ScopedInstall(Reset, 0x617430);
    RH_ScopedInstall(GetBoneNode, 0x617460);
    RH_ScopedInstall(ReturnBoneNode, 0x617470);
    RH_ScopedInstall(GetNumBoneNodesLeft, 0x617480);
}

// 0x6173F0
// Coloca las 128 entradas preasignadas en la lista libre.
bool BoneNodeManager_c::Init() {
    for (auto & item : m_Space) {
        m_Bones.AddItem(&item);
    }
    return true;
}

// 0x617420
void BoneNodeManager_c::Exit() {
    m_Bones.RemoveAll();
}

// 0x617430
// Reconstruye la lista libre sin reasignar el store de BoneNode_c.
void BoneNodeManager_c::Reset() {
    Exit();
    Init();
}

// 0x617460
// Extrae un nodo de la cabeza de la lista libre; puede devolver nullptr si se agotó.
BoneNode_c* BoneNodeManager_c::GetBoneNode() {
    return m_Bones.RemoveHead();
}

// 0x617470
// Devuelve el nodo al pool enlazándolo de nuevo en la lista libre.
void BoneNodeManager_c::ReturnBoneNode(BoneNode_c* boneNode) {
    m_Bones.AddItem(boneNode);
}

// 0x617480
uint32 BoneNodeManager_c::GetNumBoneNodesLeft() const {
    return m_Bones.GetNumItems();
}

// notsa
const BoneInfo_t* BoneNodeManager_c::GetBoneInfoFromTag(eBoneTag32 tag) {
    for (auto& i : ms_boneInfos) {
        if (i.BoneTag == tag) {
            return &i;
        }
    }
    NOTSA_UNREACHABLE("BoneNodeManager_c::GetBoneInfoFromTag: BoneInfo not found for tag {}", static_cast<int32>(tag));
    return nullptr;
}
