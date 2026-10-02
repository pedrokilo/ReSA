/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedToPlayerConversations
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "PedToPlayerConversations.h"

#include "Ragdoll/IKChainManager.h"
#include "Conversations.h"
#include "PedClothesDesc.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"
#include "EventScriptCommand.h"

/**
 * Registra los hooks reconstruidos/originales de las conversaciones ambientales
 * entre peatones y el jugador.
 *
 * Update() conserva la máquina de estados y sus dependencias de audio, tareas e IK.
 */
void CPedToPlayerConversations::InjectHooks() {
    RH_ScopedClass(CPedToPlayerConversations);
    RH_ScopedCategory("Conversations");
    RH_ScopedInstall(Clear, 0x43AAE0);
    RH_ScopedInstall(Update, 0x43B0F0);
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
 * Selecciona un interlocutor y avanza la apertura, respuesta y cierre del diálogo.
 * Los plazos usan sumas uint32, incluidas sus vueltas al desbordar el timer.
 */
void CPedToPlayerConversations::Update() {
    auto* const player = FindPlayerPed();
    if (FindPlayerVehicle(-1, false)) {
        m_LastPlayerVehicle = FindPlayerVehicle(-1, false);
        CEntity::RegisterReference(m_LastPlayerVehicle);
    }

    const auto Say = [](CPed* ped, eGlobalSpeechContext context) {
        ped->Say(context, 0, 1.0f, true, false, false);
    };
    const auto WaitToFinish = [] {
        m_State = eP2pState::WAITINGTOFINISH;
        m_StartTime = CTimer::GetTimeInMS();
    };
    const auto AttackPlayer = [player] {
        auto* sequence = new CTaskComplexSequence{};
        sequence->AddTask(new CTaskSimpleStandStill{2000, false, false, 8.0f});
        sequence->AddTask(new CTaskComplexKillPedOnFoot{player, -1, 0, 0, 0, 0});
        CEventScriptCommand event{TASK_PRIMARY_PRIMARY, sequence, false};
        m_pPed->GetIntelligence()->GetEventGroup().Add(&event, false);
    };

    switch (m_State) {
    case eP2pState::INACTIVE: {
        if (CTimer::GetTimeInMS() <= m_TimeOfLastPlayerConversation + 30000u
            || FindPlayerVehicle(-1, false)
            || FindPlayerPed()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN
            || CConversations::IsConversationGoingOn()
            || !player->PedIsReadyForConversation(true)) {
            return;
        }

        auto* const pool = GetPedPool();
        for (int32 inspected = 0; inspected < 8; ++inspected) {
            if (++m_PedPoolIndex >= static_cast<int32>(pool->GetSize())) {
                m_PedPoolIndex = 0;
            }
            auto* const ped = pool->GetAt(m_PedPoolIndex);
            if (!ped || !ped->IsCreatedBy(PED_GAME)
                || !ped->PedIsReadyForConversation(true) || ped->bHasAScriptBrain
                || !ped->GetIsOnScreen()) {
                continue;
            }
            if (!(DotProduct(ped->GetForwardVector(), player->GetPosition() - ped->GetPosition()) > 0.0f)
                || !(DotProduct(player->GetForwardVector(), ped->GetPosition() - player->GetPosition()) > 0.0f)
                || !((ped->GetPosition() - FindPlayerCoors()).Magnitude() < 7.0f)) {
                continue;
            }

            const auto type = ped->m_nPedType;
            const bool rivalGang = type == PED_TYPE_GANG1 || (type >= PED_TYPE_GANG3 && type <= PED_TYPE_GANG10);
            m_Topic = rivalGang ? CGeneral::GetRandomNumberInRange(8, 10) : CGeneral::GetRandomNumberInRange(0, 7);
            if (!ped->GetSpeechAE().WillPedChatAboutTopic(static_cast<int16>(m_Topic))) {
                continue;
            }

            // El original utiliza la semilla del peatón para variar la valoración.
            const auto AdjustedStat = [ped](eStats stat, uint16 mask, float bias) {
                return static_cast<int32>(CStats::GetStatValue(stat) + (ped->m_nRandomSeed & mask) - bias);
            };
            eGlobalSpeechContext opening = CTX_GLOBAL_UNK;
            bool positive = false;
            switch (m_Topic) {
            case 0:
                if (m_LastPlayerVehicle && (m_LastPlayerVehicle->GetPosition() - ped->GetPosition()).Magnitude2D() < 20.0f) {
                    switch (CModelInfo::GetVehicleModelInfo(m_LastPlayerVehicle->GetModelIndex())->m_nVehicleClass) {
                    case VEHICLE_CLASS_RICHFAMILY:
                    case VEHICLE_CLASS_EXECUTIVE:
                    case VEHICLE_CLASS_LEISUREBOAT:
                        if (m_LastPlayerVehicle->m_fHealth > 800.0f) {
                            positive = true;
                            opening = CTX_GLOBAL_CONV_LIKE_CAR;
                        }
                        break;
                    case VEHICLE_CLASS_POORFAMILY:
                    case VEHICLE_CLASS_WORKER:
                    case VEHICLE_CLASS_BIG:
                    case VEHICLE_CLASS_TAXI:
                        opening = CTX_GLOBAL_CONV_DISL_CAR;
                        break;
                    }
                }
                break;
            case 1: {
                const auto respect = AdjustedStat(STAT_CLOTHES_RESPECT, 511, 256.0f);
                if (respect > 500) { positive = true; opening = CTX_GLOBAL_CONV_LIKE_CLOTHES; }
                else if (respect < 150) { opening = CTX_GLOBAL_CONV_DISL_CLOTHES; }
                break;
            }
            case 2: {
                const auto respect = AdjustedStat(STAT_CLOTHES_RESPECT, 511, 256.0f);
                if (FindPlayerPed()->GetPlayerData()->m_pPedClothesDesc->HasVisibleNewHairCut(0)) {
                    if (respect > 500) { positive = true; opening = CTX_GLOBAL_CONV_LIKE_HAIR; }
                    else if (respect < 150 || CStats::GetStatValue(STAT_HAIRDRESSING_BUDGET) < 5.0f) { opening = CTX_GLOBAL_CONV_DISL_HAIR; }
                }
                break;
            }
            case 3: {
                const auto muscle = AdjustedStat(STAT_MUSCLE, 255, 128.0f);
                const auto fat = AdjustedStat(STAT_FAT, 255, 128.0f);
                if (muscle > 400 && fat < 250) { positive = true; opening = CTX_GLOBAL_CONV_LIKE_PHYS; }
                else if (muscle - fat < 200 && fat > 500) { opening = CTX_GLOBAL_CONV_DISL_PHYS; }
                break;
            }
            case 4: {
                const auto respect = AdjustedStat(STAT_CLOTHES_RESPECT, 1023, 512.0f);
                if (respect > 700) { positive = true; opening = CTX_GLOBAL_CONV_LIKE_SHOES; }
                else if (respect < 300) { opening = CTX_GLOBAL_CONV_DISL_SHOES; }
                break;
            }
            case 5: {
                const auto appeal = AdjustedStat(STAT_SEX_APPEAL, 511, 256.0f);
                if (appeal > 700) { positive = true; opening = CTX_GLOBAL_CONV_LIKE_SMELL; }
                else if (appeal < 300) { opening = CTX_GLOBAL_CONV_DISL_SMELL; }
                break;
            }
            case 6:
                if (FindPlayerPed()->GetPlayerData()->m_pPedClothesDesc->HasVisibleTattoo()) {
                    const auto rating = CGeneral::GetRandomNumberInRange(0, 1000);
                    if (rating > 700) { positive = true; opening = CTX_GLOBAL_CONV_LIKE_TATTOO; }
                    else if (rating < 300) { opening = CTX_GLOBAL_CONV_DISL_TATTOO; }
                }
                break;
            case 7:
                // La variable consultada es Wind, no Rain.
                if (CWeather::Wind < 0.1f) {
                    if (CClock::GetGameClockHours() > 6 && CClock::GetGameClockHours() < 20) {
                        positive = true;
                        opening = CTX_GLOBAL_CONV_LIKE_WEATHER;
                    }
                } else if (CWeather::Wind > 0.5f) {
                    opening = CTX_GLOBAL_CONV_DISL_WEATHER;
                }
                break;
            case 8: opening = CTX_GLOBAL_WHERE_YOU_FROM; positive = true; break;
            case 9: opening = CTX_GLOBAL_GANGBANG; positive = true; break;
            }

            if (opening == CTX_GLOBAL_UNK || !CAEPedSpeechAudioEntity::RequestPlayerConversation(ped)) {
                if ((CGeneral::GetRandomNumber() & 4095) == 3) {
                    ped->Say(CTX_GLOBAL_EYEING_PLAYER);
                    return;
                }
                continue;
            }
            if (m_Topic < 8) {
                m_bPositiveOpening = positive;
            }
            Say(ped, opening);
            if (m_Topic >= 8) {
                m_bPositiveOpening = true;
            }
            m_State = eP2pState::PEDHASOPENED;
            m_pPed = ped;
            CEntity::RegisterReference(m_pPed);
            m_TimeOfLastPlayerConversation = CTimer::GetTimeInMS();
            m_StartTime = CTimer::GetTimeInMS();
            ped->DisablePedSpeech(false);
            g_ikChainMan.LookAt("Ped2Pl_Conversation", ped, player, 100000, BONE_HEAD, nullptr, false, 0.25f, 500, 8, false);
            g_ikChainMan.LookAt("Ped2Pl_ConversationP", player, ped, 100000, BONE_HEAD, nullptr, false, 0.25f, 500, 8, false);
            return;
        }
        return;
    }
    case eP2pState::PEDHASOPENED:
        if (!m_pPed) {
            m_State = eP2pState::INACTIVE;
            CAEPedSpeechAudioEntity::ReleasePlayerConversation();
            if (g_ikChainMan.IsLooking(player)) {
                g_ikChainMan.AbortLookAt(player, 250);
            }
            return;
        }
        if (CTimer::GetTimeInMS() > m_StartTime + 4000u) {
            m_State = eP2pState::INACTIVE;
            CAEPedSpeechAudioEntity::ReleasePlayerConversation();
            m_pPed->EnablePedSpeech();
            if (g_ikChainMan.IsLooking(player)) {
                g_ikChainMan.AbortLookAt(player, 250);
            }
            if (g_ikChainMan.IsLooking(m_pPed)) {
                g_ikChainMan.AbortLookAt(m_pPed, 250);
                if (m_bPositiveOpening) {
                    m_pPed->Say(CTX_GLOBAL_CONV_IGNORED);
                }
            }
            return;
        }
        if (CPad::GetPad(0)->ConversationYesJustDown()) {
            if (!m_bPositiveOpening) {
                Say(player, IsPedTypeFemale(m_pPed->m_nPedType) ? CTX_GLOBAL_LIKE_DISMISS_FEMALE : CTX_GLOBAL_LIKE_DISMISS_MALE);
            } else if (m_Topic == 8 || m_Topic == 9) {
                Say(player, m_Topic == 8 ? CTX_GLOBAL_WHERE_YOU_FROM_POS : CTX_GLOBAL_GANGBANG_YES);
                if (CGeneral::GetRandomNumberInRange(0, 100) < 75) {
                    AttackPlayer();
                }
                EndConversation();
                return;
            } else {
                constexpr eGlobalSpeechContext replies[] = {
                    CTX_GLOBAL_LIKE_CAR_REPLY, CTX_GLOBAL_LIKE_CLOTHES_REPLY,
                    CTX_GLOBAL_LIKE_HAIR_REPLY, CTX_GLOBAL_LIKE_PHYS_REPLY,
                    CTX_GLOBAL_LIKE_SHOES_REPLY, CTX_GLOBAL_LIKE_SMELL_REPLY,
                    CTX_GLOBAL_LIKE_TATTOO_REPLY, CTX_GLOBAL_WEATHER_LIKE_REPLY
                };
                if (m_Topic >= 0 && m_Topic < 8) {
                    Say(player, replies[m_Topic]);
                }
            }
            WaitToFinish();
        } else if (CPad::GetPad(0)->ConversationNoJustDown()) {
            if (m_bPositiveOpening) {
                if (m_Topic == 8 || m_Topic == 9) {
                    Say(player, m_Topic == 8 ? CTX_GLOBAL_WHERE_YOU_FROM_NEG : CTX_GLOBAL_GANGBANG_NO);
                    EndConversation();
                    return;
                }
                Say(player, IsPedTypeFemale(m_pPed->m_nPedType) ? CTX_GLOBAL_LIKE_DISMISS_FEMALE : CTX_GLOBAL_LIKE_DISMISS_MALE);
                m_State = eP2pState::WAITINGFORFINALWORD;
                m_StartTime = CTimer::GetTimeInMS();
            } else {
                Say(player, IsPedTypeFemale(m_pPed->m_nPedType) ? CTX_GLOBAL_LIKE_NEGATIVE_FEMALE : CTX_GLOBAL_LIKE_NEGATIVE_MALE);
                if (m_pPed->m_nPedType != PED_TYPE_GANG2 && CGeneral::GetRandomNumberInRange(0, 100) < 40) {
                    AttackPlayer();
                }
                WaitToFinish();
            }
        }
        return;
    case eP2pState::WAITINGFORFINALWORD:
        if (!m_pPed) {
            m_State = eP2pState::INACTIVE;
            CAEPedSpeechAudioEntity::ReleasePlayerConversation();
            if (g_ikChainMan.IsLooking(player)) {
                g_ikChainMan.AbortLookAt(player, 250);
            }
        } else if (CTimer::GetTimeInMS() > m_StartTime + 3000u && !m_pPed->GetPedTalking()) {
            Say(m_pPed, CTX_GLOBAL_LIKE_DISMISS_REPLY);
            WaitToFinish();
        }
        return;
    case eP2pState::WAITINGTOFINISH:
        if (CTimer::GetTimeInMS() > m_StartTime + 2500u || !m_pPed) {
            EndConversation();
        }
        return;
    }
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
    // Como el original: FindPlayerPed se llama antes de tocar el estado (0x43AB13).
    const auto player = FindPlayerPed(-1);

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
    if (g_ikChainMan.IsLooking(player)) {
        g_ikChainMan.AbortLookAt(player, 250);
    }

    // Hace lo mismo con el peatón participante. El return conserva el flujo del
    // código original/reconstruido aunque EndConversation() devuelva void.
    if (m_pPed && g_ikChainMan.IsLooking(m_pPed)) {
        return g_ikChainMan.AbortLookAt(m_pPed, 250);
    }
}
