/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedToPlayerConversations
 * Reconstrucción: INCOMPLETA — 2/3 funciones reconstruidas.
 * Pendientes de reconstruir: 1.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Update | 0x0043B0F0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "PedToPlayerConversations.h"

#include "Ragdoll/IKChainManager.h"

/**
 * Registra los hooks reconstruidos/originales de las conversaciones ambientales
 * entre peatones y el jugador.
 *
 * Update() continúa delegando en el ejecutable original y por ello permanece
 * marcado como no reconstruido.
 */
void CPedToPlayerConversations::InjectHooks() {
    RH_ScopedClass(CPedToPlayerConversations);
    RH_ScopedCategory("Conversations");
    RH_ScopedInstall(Clear, 0x43AAE0);
    RH_ScopedInstall(Update, 0x43B0F0, {.reversed = false});
    RH_ScopedInstall(EndConversation, 0x43AB10);
}

// 0x43AAE0
/**
 * Devuelve el subsistema a su estado inactivo inicial.
 *
 * Si existe un intercambio activo, primero se libera la reserva de voz de
 * conversación del jugador. Después se reinician el puntero al peatón participante
 * y la marca temporal de la última conversación.
 */
void CPedToPlayerConversations::Clear() {
    ZoneScoped;

    if (m_State != eP2pState::INACTIVE) {
        // El sistema de voz reserva estado/recursos especiales mientras el jugador
        // participa en una conversación. Sólo se liberan cuando es necesario.
        CAEPedSpeechAudioEntity::ReleasePlayerConversation();
        m_State = eP2pState::INACTIVE;
    }

    // Descarta el participante y el historial temporal mantenido por el subsistema.
    m_pPed                         = nullptr;
    m_TimeOfLastPlayerConversation = 0;
}

// 0x43B0F0
/**
 * Ejecuta la máquina de estados original de conversación peatón-jugador.
 *
 * La rutina todavía no ha sido reconstruida, así que este wrapper llama
 * deliberadamente al ejecutable original para conservar su comportamiento.
 */
void CPedToPlayerConversations::Update() {
    plugin::Call<0x43B0F0>();
}

// 0x43AB10
/**
 * Finaliza la conversación ambiental activa y elimina sus efectos de presentación.
 *
 * Además de liberar el estado especial de conversación del sistema de voz, vuelve
 * a habilitar el habla normal del peatón y cancela suavemente el IK de mirada de
 * ambos participantes.
 */
void CPedToPlayerConversations::EndConversation() {
    // Hace que el subsistema pase inmediatamente a considerarse inactivo.
    m_State = eP2pState::INACTIVE;

    // Devuelve la entidad de voz del jugador al funcionamiento normal fuera de conversación.
    CAEPedSpeechAudioEntity::ReleasePlayerConversation();

    if (m_pPed) {
        // Es posible que el habla normal del peatón se haya deshabilitado durante
        // el intercambio especial.
        m_pPed->EnablePedSpeech();
    }

    // Durante la conversación puede forzarse al jugador a mirar al peatón.
    // Si esa mirada sigue activa, se cancela suavemente durante 250 ms.
    const auto player = FindPlayerPed(-1);
    if (g_ikChainMan.IsLooking(player)) {
        g_ikChainMan.AbortLookAt(player, 250);
    }

    // Hace lo mismo con el peatón participante. El return conserva el flujo del
    // código original/reconstruido aunque EndConversation() devuelva void.
    if (m_pPed && g_ikChainMan.IsLooking(m_pPed)) {
        return g_ikChainMan.AbortLookAt(m_pPed, 250);
    }
}
