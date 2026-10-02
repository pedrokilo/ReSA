/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMouseControllerState
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

#include "MouseControllerState.h"

void CMouseControllerState::InjectHooks() {
    RH_ScopedClass(CMouseControllerState);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x53F220);
    RH_ScopedInstall(Clear, 0x53F250);
    RH_ScopedInstall(CheckForInput, 0x53F270);
}

// 0x53F220
CMouseControllerState::CMouseControllerState() {
    m_fWheelMoved = 0.f;
    m_AmountMoved.Reset();
    Clear();
}

CMouseControllerState* CMouseControllerState::Constructor() {
    this->CMouseControllerState::CMouseControllerState();
    return this;
}

// 0x53F250
void CMouseControllerState::Clear() {
    isMouseLeftButtonPressed = false;
    isMouseRightButtonPressed = false;
    isMouseMiddleButtonPressed = false;
    isMouseWheelMovedUp = false;
    isMouseWheelMovedDown = false;
    isMouseFirstXPressed = false;
    isMouseSecondXPressed = false;
}

// 0x53F270
bool CMouseControllerState::CheckForInput() const {
    return (
        isMouseLeftButtonPressed
        || isMouseRightButtonPressed
        || isMouseMiddleButtonPressed
        || isMouseWheelMovedUp
        || isMouseWheelMovedDown
        || isMouseFirstXPressed
        || isMouseSecondXPressed
        || !m_AmountMoved.IsZero()
    );
}
