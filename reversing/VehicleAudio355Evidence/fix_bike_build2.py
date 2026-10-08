from pathlib import Path
import re
p=Path('source/game_sa/Entity/Vehicle/Bike.cpp');s=p.read_text().replace('m_pDamageEntity->IsVehicle()','m_pDamageEntity->GetIsTypeVehicle()').replace('m_pDamageEntity->IsBuilding()','m_pDamageEntity->GetIsTypeBuilding()');a=s.index('static float BikeSetupHeight355');b=s.index('// 0x6B89B0',a);s=s[:a]+re.sub(r'\blength\b','suspensionLength',s[a:b])+s[b:];p.write_text(s)
