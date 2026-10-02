"""Substitute dependency providers only; retain both candidate function bodies."""
def instrument(path, text):
    if path.endswith('/Game.cpp'):
        anchor = 'void CGame::Process() {'
        assert text.count(anchor) == 1
        return text.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/ConversationForPed/LiveComparison.hpp"', 1).replace(anchor, anchor+'\n    CForPedLive::Update();')
    text = text.replace('#include "ConversationForPed.h"', '#include "ConversationForPed.h"\n#include "../../../reversing/ConversationForPed/Probe.hpp"', 1)
    text = text.replace(', {.reversed = false}', '')
    text = text.replace('void CConversationForPed::Update() {', 'void CConversationForPed::Update() {\n    if (!CForPedProbe::active) return;\n    ++CForPedProbe::updateCalls;')
    text = text.replace('bool CConversationForPed::IsPlayerInPositionForConversation(bool randomConversation) {', 'bool CConversationForPed::IsPlayerInPositionForConversation(bool randomConversation) {\n    ++CForPedProbe::positionCalls;')
    replacements = {
        'FindPlayerPed()->GetPlayerGroup().GetMembership().CountMembersExcludingLeader()': 'CForPedProbe::Members()',
        'FindPlayerPed()->PedIsReadyForConversation(randomConversation)': 'CForPedProbe::Ready(CForPedProbe::Player(-1), randomConversation)',
        'm_pPed->PedIsReadyForConversation(randomConversation)': 'CForPedProbe::Ready(m_pPed, randomConversation)',
        'FindPlayerPed()': 'CForPedProbe::Player(-1)',
        'FindPlayerCoors()': 'CForPedProbe::Coors(-1)',
        'speaker->Say(': 'CForPedProbe::Say(speaker, ',
        'CConversations::AwkwardSay(': 'CForPedProbe::Awkward(',
        'CMessages::ClearSmallMessagesOnly()': 'CForPedProbe::ClearMessages()',
        'CMessages::AddMessageJump(': 'CForPedProbe::Jump(',
        'CMessages::AddMessageQ(': 'CForPedProbe::Queue(',
        'TheText.Get(': 'CForPedProbe::Text(',
        'CPad::GetPad(0)->ConversationYesJustDown()': 'CForPedProbe::Yes()',
        'CPad::GetPad(0)->ConversationNoJustDown()': 'CForPedProbe::No()',
    }
    for old,new in replacements.items():
        assert old in text, old
        text=text.replace(old,new)
    return text
