from pathlib import Path
adds={'PlaneInputs355':['Entity/Vehicle/Automobile.cpp','Entity/Vehicle/Automobile.h','tFlyingHandlingData.h'],'PlaneDamage355':['tHandlingData.h']}
for bank,extra in adds.items():
 p=Path('reversing',bank,'build_probe.py');s=p.read_text();s=s.replace("'Entity/Vehicle/Plane.h',","'Entity/Vehicle/Plane.h',"+','.join(repr(x) for x in extra)+',');p.write_text(s)
