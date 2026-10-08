from pathlib import Path
p=Path('source/game_sa/Entity/Vehicle/Bike.cpp');s=p.read_text();a=s.index('// 0x6BE310');b=s.index('// 0x6BDEA0',a);c=s[a:b].replace('m_fBrakePedal','m_BrakePedal').replace('CTimer::GetTimeInMSPauseMode()','CTimer::GetTimeInMS()').replace('((CTimer::GetTimeInMS() >> 10) + 6 & 15)','(((CTimer::GetTimeInMS() >> 10) + 6) & 15)');p.write_text(s[:a]+c+s[b:])
