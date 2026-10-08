from pathlib import Path
pairs=[('Audio/Entities/AEScriptAudioEntity.cpp','Script_BankNative.txt'),('Audio/Entities/AEScriptAudioEntity.cpp','Script_PlayLoadedNative.txt'),('Entity/Vehicle/MonsterTruck.cpp','Monster_SetupNative.txt'),('Entity/Vehicle/MonsterTruck.cpp','Monster_SuspensionNative.txt'),('Entity/Vehicle/Bike.cpp','Bike_WheelColNative.txt'),('Entity/Vehicle/Train.cpp','Train_ClosestNative.txt'),('Entity/Vehicle/Train.cpp','Train_AllowedNative.txt'),('Audio/Managers/AERadioTrackManager.cpp','Radio_QueueNative.txt'),('Audio/Managers/AERadioTrackManager.cpp','Radio_DJNative.txt')]
for rel,draft in pairs:
 p=Path('source/game_sa')/rel
 if not p.exists():
  found=list(Path('source/game_sa').rglob(p.name));assert len(found)==1; p=found[0]
 native=Path('reversing/VehicleAudio355Evidence',draft).read_text().strip()
 signature=native.split('{',1)[0].strip(); method=signature.split('::',1)[1].split('(',1)[0]
 s=p.read_text(); import re
 match=re.search(r'[^\n]*::'+re.escape(method)+r'\([^\n]*\)\s*\{',s);assert match,(p,method)
 start=match.start(); a=s.index('{',match.start());depth=1;b=a+1
 while depth:
  depth+=(s[b]=='{')-(s[b]=='}');b+=1
 s=s[:start]+native+s[b:];p.write_text(s);print(p,method)
