from pathlib import Path
p=Path('source/game_sa/Entity/Vehicle/Plane.cpp');s=p.read_text();a=s.index('void CPlane::DoPlaneGenerationAndRemoval()');b=s.index('// 0x6C9140',a);native=Path('reversing/VehicleAudio355Evidence/Plane_GenerationNativeDraft.txt').read_text();s=s[:a]+native+'\n'+s[b:]
headers=['Streaming.h','TheScripts.h','GameLogic.h','PedGroups.h','PedGroup.h','PedGroupIntelligence.h','PedIntelligence.h','EventDanger.h','EventGroup.h','PlayerPed.h']
marker='#include "FireManager.h"';s=s.replace(marker,marker+'\n'+'\n'.join('#include "'+h+'"' for h in headers if '#include "'+h+'"' not in s))
p.write_text(s)
Path('reversing/VehicleAudio355Evidence/Plane_GenerationNative.txt').write_text(native)
p=Path('reversing/PlaneGeneration355/README.md');s=p.read_text().replace('pending source application/runtime','native source applied, pending runtime');p.write_text(s)
p=Path('reversing/VehicleAudio355Evidence/inventory.py');s=p.read_text().replace("'PlaneInputs355']","'PlaneInputs355','PlaneGeneration355']");p.write_text(s)
