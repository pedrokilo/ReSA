from pathlib import Path
p=Path('reversing/ScriptPreload355');p.mkdir(exist_ok=True)
s=Path('reversing/ScriptLinks355/LiveComparison.hpp').read_text().replace('ScriptLinks355Live','ScriptPreload355Live').replace('script-links355','script-preload355').replace('#include "AEAudioHardware.h"','#include "AEAudioHardware.h"\n#include "AEAudioUtility.h"')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline bool __cdecl Map(const eAudioEvents& event,eSoundBankS32& bank,int32& sound,int32 slot){trace.push_back({"map",int(event),slot});const_cast<eAudioEvents&>(event)=static_cast<eAudioEvents>(int(event)+7);bank=static_cast<eSoundBankS32>(50+scenario%9);sound=scenario%4==0?-1:scenario%13;return scenario%5!=0;}
inline void __fastcall LoadBank(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"loadbank",int(bank),int(slot)});}
inline void __fastcall LoadSound(void*,void*,eSoundBank bank,eSoundID sound,eSoundBankSlot slot){trace.push_back({"loadsound",int(bank),int(sound),int(slot)});}
'''+s[a:]
a=s.index('inline const std::array<Fn,6>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","PreloadMissionAudio",0x4EC190,0,-1,1024}}};\n'+s[b:]
s=s.replace('Base*,6>','Base*,1>').replace('bool,6>','bool,1>').replace('n<6','n<1')
a=s.index(' EntryPatch a(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch b(0x4EF520,Playing),bd(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsPlayingInBankSlot),Playing);
 EntryPatch m(0x4D9CC0,Map),md(bit_cast<uintptr_t>(&CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent),Map);
 EntryPatch l(0x4D88A0,LoadBank),ld(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),LoadBank);
 EntryPatch q(0x4D8ED0,LoadSound),qd(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSound),LoadSound);
'''+s[b:]
a=s.index('    switch(n)');b=s.index('    if(!candidate',a)
s=s[:a]+'''    reinterpret_cast<void(__thiscall*)(void*,uint8,int32)>(fn.address)(self,slot,1000+i);
'''+s[b:]
s=s.replace('if((n==1||n==2||n==4)&&!positiveCases)','if(!positiveCases)')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/ScriptLinks355',fn).read_text().replace('ScriptLinks355Live','ScriptPreload355Live').replace('script-links355','script-preload355')
 if fn=='build_probe.py':s=s.replace("'Audio/Hardware/AEAudioHardware.h'","'Audio/Hardware/AEAudioHardware.h','Audio/AEAudioUtility.cpp','Audio/AEAudioUtility.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('PreloadMissionAudio 0x4EC190: 1.024 pares preparados, pendiente de ejecución. Ranuras válidas e inválidas, sonidos activos/inactivos, mapper exitoso/fallido y bancos/sonidos de distintos signos. Mapper y carga de hardware simétricos registran evento, ranura y argumentos; compara bytes de todos los enlaces y exige ramas positivas. EXE canónico y evidencia ../VehicleAudio355Evidence/Script_4EC190.txt.\n')
