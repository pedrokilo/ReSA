#pragma once
#include "AESound.h"
namespace BulletProbe {
inline bool active{}, bankLoaded{true};
inline unsigned calls{};
inline json captured = json::array();
inline void Capture(const CAESound& s) {
    const int id = s.m_SoundID;
    captured.push_back({{"id",id},{"group",(id-1)/3},{"bank",int(s.m_BankSlot)},
        {"volume",s.m_Volume},{"rolloff",s.m_RollOffFactor},
        {"position",{s.m_CurrPos.x,s.m_CurrPos.y,s.m_CurrPos.z}},
        {"flags",s.m_Flags},{"variance",s.m_SpeedVariance}});
}
}
