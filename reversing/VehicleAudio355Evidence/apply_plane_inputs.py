from pathlib import Path
p=Path('source/game_sa/Entity/Vehicle/Plane.cpp');s=p.read_text();a=s.index('void CPlane::ProcessControlInputs(uint8 playerNum)');b=s.index('// 0x6CB7C0',a)
native=Path('reversing/VehicleAudio355Evidence/Plane_InputsNativeDraft.txt').read_text()
s=s[:a]+native+'\n'+s[b:];p.write_text(s)
Path('reversing/VehicleAudio355Evidence/Plane_InputsNative.txt').write_text(native)
p=Path('reversing/VehicleAudio355Evidence/inventory.py');s=p.read_text().replace("'PlaneDamage355']","'PlaneDamage355','PlaneInputs355']");p.write_text(s)
