/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCivilianPed
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "CivilianPed.h"

/**
 * Registra construcción y control de peatones civiles.
 */
void CCivilianPed::InjectHooks() {
    RH_ScopedVirtualClass(CCivilianPed, 0x86C0A8, 26);
    RH_ScopedCategory("Entity/CivilianPed");

    RH_ScopedInstall(Constructor, 0x5DDB70);
    RH_ScopedInstall(Destructor, 0x5DDC30);
    RH_ScopedVMTInstall(ProcessControl, 0x5DDBF0);
}


// 0x5DDB70
// Configura modelo y speech sobre la base CPed creada con el ped type indicado.
CCivilianPed::CCivilianPed(ePedType pedType, uint32 modelIndex) : CPed(pedType) {
    SetModelIndex(modelIndex); // V1053 Calling the 'SetModelIndex' virtual function in the constructor may lead to unexpected result at runtime
    m_pedSpeech.Initialise(this);
}

// 0x5DDBF0
// Delega el control normal a CPed y actualiza el arma sólo si el ped sigue procesable.
void CCivilianPed::ProcessControl() {
    if (GetCreatedBy() != (PED_MISSION | PED_GAME)) {
        CPed::ProcessControl();
        if (!GetWasPostponed() && m_nPedState != PEDSTATE_DEAD)
            GetActiveWeapon().Update(this);
    }
}

CCivilianPed* CCivilianPed::Constructor(ePedType pedType, uint32 modelIndex) {
    this->CCivilianPed::CCivilianPed(pedType, modelIndex);
    return this;
}

// 0x5DDC30
// Como el original: 0x5DDC30 es el destructor con borrado (`ret 4`): recibe las banderas, destruye el objeto
// (0x5DDBE0 -> CPed::~CPed) y, si el bit 0 esta activo, libera la memoria con CPed::operator delete (0x5E4760).
CCivilianPed* CCivilianPed::Destructor(uint8 flags) {
    this->CCivilianPed::~CCivilianPed();
    if (flags & 1) {
        CPed::operator delete(this);
    }
    return this;
}
