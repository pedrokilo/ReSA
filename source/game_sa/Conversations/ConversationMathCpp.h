#pragma once
// Cálculos x87 de CConversationForPed::IsPlayerInPositionForConversation (0x43AC40).
// Los punteros apuntan a tres floats consecutivos (x, y, z).
extern "C" bool __cdecl ConversationTooFarCpp(const float* ped, const float* player);
extern "C" bool __cdecl ConversationFacingAwayCpp(const float* ped, const float* player);
extern "C" bool __cdecl ConversationMovingTooFastCpp(const float* speed);
