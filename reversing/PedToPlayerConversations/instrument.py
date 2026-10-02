def instrument(path, text):
    if path.endswith('/Game.cpp'):
        anchor = 'void CGame::Process() {'
        assert text.count(anchor) == 1
        return text.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/PedToPlayerConversations/LiveComparison.hpp"', 1).replace(anchor, anchor+'\n    P2PLive::Update();')
    text = text.replace('#include "PedToPlayerConversations.h"', '#include "PedToPlayerConversations.h"\n#include "../../../reversing/PedToPlayerConversations/Probe.hpp"', 1)
    text = text.replace('void CPedToPlayerConversations::Update() {', 'void CPedToPlayerConversations::Update() {\n    if (!P2PProbe::active) return;\n    ++P2PProbe::calls;')
    text = text.replace('void CPedToPlayerConversations::EndConversation() {', 'void CPedToPlayerConversations::EndConversation() {\n    if (!P2PProbe::active) return;')
    replacements = {
        'FindPlayerPed()->GetWantedLevel()': 'P2PProbe::Wanted(P2PProbe::Player(-1))',
        'FindPlayerPed()': 'P2PProbe::Player(-1)',
        'FindPlayerPed(-1)': 'P2PProbe::Player(-1)',
        'FindPlayerVehicle(-1, false)': 'P2PProbe::Vehicle(-1, false)',
        'FindPlayerCoors()': 'P2PProbe::Coors(-1)',
        'CConversations::IsConversationGoingOn()': 'P2PProbe::Conversation()',
        'player->PedIsReadyForConversation(true)': 'P2PProbe::Ready(player, true)',
        'ped->PedIsReadyForConversation(true)': 'P2PProbe::Ready(ped, true)',
        'ped->GetIsOnScreen()': 'P2PProbe::OnScreen(ped)',
        'ped->GetSpeechAE().WillPedChatAboutTopic(': 'P2PProbe::Topic(ped, ',
        'CGeneral::GetRandomNumberInRange(': 'P2PProbe::RandomRange(',
        'CGeneral::GetRandomNumber()': 'P2PProbe::Random()',
        'CStats::GetStatValue(': 'P2PProbe::Stat(',
        'P2PProbe::Player(-1)->GetPlayerData()->m_pPedClothesDesc->HasVisibleNewHairCut(0)': 'P2PProbe::Hair(nullptr, 0)',
        'P2PProbe::Player(-1)->GetPlayerData()->m_pPedClothesDesc->HasVisibleTattoo()': 'P2PProbe::Tattoo(nullptr)',
        'CAEPedSpeechAudioEntity::RequestPlayerConversation(': 'P2PProbe::Request(',
        'CAEPedSpeechAudioEntity::ReleasePlayerConversation()': 'P2PProbe::Release()',
        'm_pPed->Say(': 'P2PProbe::Say(m_pPed, ',
        'ped->Say(': 'P2PProbe::Say(ped, ',
        'ped->DisablePedSpeech(false)': 'P2PProbe::Disable(ped, false)',
        'm_pPed->EnablePedSpeech()': 'P2PProbe::Enable(m_pPed)',
        'm_pPed->GetPedTalking()': 'P2PProbe::Talking(m_pPed)',
        'g_ikChainMan.IsLooking(': 'P2PProbe::Looking(',
        'g_ikChainMan.AbortLookAt(': 'P2PProbe::Abort(',
        'g_ikChainMan.LookAt(': 'P2PProbe::LookAt(',
        'CPad::GetPad(0)->ConversationYesJustDown()': 'P2PProbe::Yes()',
        'CPad::GetPad(0)->ConversationNoJustDown()': 'P2PProbe::No()',
        'm_pPed->GetIntelligence()->GetEventGroup().Add(&event, false)': 'P2PProbe::AddEvent(nullptr, &event, false)',
    }
    for old,new in replacements.items():
        assert old in text, old
        text = text.replace(old,new)
    return text
