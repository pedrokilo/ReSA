from pathlib import Path
import re
p=Path('source/game_sa/Audio/Managers/AERadioTrackManager.cpp')
s=p.read_text(encoding='utf-8')
x=Path('reversing/VehicleAudio355Evidence/Radio_4E8E40.txt').read_text(encoding='utf-8').split('==== ensamblador')[0]
body=x[x.index('{'):x.rindex('}')+1]
body=body.replace('float10','float').replace('(&DAT_00b62c1c)','(&StaticRef<int8>(0xB62C1C))')
body=re.sub(r'FUN_00558e40\((0x[0-9a-f]+)\)',r'CStats::GetStatValue(static_cast<eStats>(\1))',body)
body=body.replace('FUN_004d9c10','CAEAudioUtility::GetRandomNumberInRange').replace('return;', 'return local_20[iVar1];')
s=s.replace('{\n    return plugin::CallAndReturn<int8, 0x4E8E40>();\n}',body)
for name in ('ChooseIdentIndex','ChooseAdvertIndex','ChooseDJBanterIndexFromList','ChooseMusicTrackIndex','ChooseTalkRadioShow','IsVehicleRadioActive'):
 s=re.sub(r'(RH_ScopedInstall\('+name+r', 0x[0-9A-F]+), \{ \.reversed = false \}',r'\1',s)
p.write_text(s,encoding='utf-8')
