from pathlib import Path
import json
p=Path('reversing/VehicleAudio355Evidence')
main=p/'Bike_ControlFull.txt'
s=main.read_text()
if '006BC92C: ret' not in s:
 s+='\n; Continuation recovered after dumpbin display cap; adjacent instruction ranges.\n'+(p/'Bike_ControlContinuation.txt').read_text()+'\n'+(p/'Bike_ControlContinuation2.txt').read_text()
 main.write_text(s)
(p/'asm-coverage-local.json').write_text(json.dumps({'Bike::ProcessControl':{'start':'0x6B9250','end':'0x6BC92C','segments':['original through 0x6BAD78','0x6BAD7C through 0x6BC8BB','0x6BC8BE through 0x6BC92C'],'complete':True,'note':'Continuation files preserve recovery provenance; final ret 0x6BC92C.'}},indent=2))
(p/'plane-damage-validationlimitations.json').write_text(json.dumps({'class':'CPlane','function':'VehicleDamage','address':'0x6CC4B0','validationlimitations':[{'branch':'accepted explosive weapon followed by damage smoke effect','exe_behavior':'Reads uninitialized stack float [esp+0x14] at 0x6CC915; multiplying zero damage can produce zero or NaN depending on prior stack contents.','native_behavior':'Defined positive zero particle force; avoids an uninitialized C++ read.','evidence':['0x6CC57F collision path initializes factor to 1','0x6CC915 fld dword ptr [esp+14h]','0x6CC919 fmul dword ptr [esp+58h]','0x6CC923 stores particle force'],'comparison_scope':'Exclude explosivetosmoke from deterministic equivalence; accepted explosivetoBlowUpCar remains exercised; collisiontosmoke remains exercised.','status':'explicit limited deviation accepted by parent; no full equivalence claim'}]},indent=2))
(p/'PlaneDamageLimitations.md').write_text('CPlane::VehicleDamage (0x6CC4B0): accepted explosive to smoke has an indeterminate EXE stack factor. Collision initializes [esp+14h] at 0x6CC57F; explosive does not. Instructions 0x6CC915 fld [esp+14h], 0x6CC919 fmul [esp+58h], 0x6CC923 store read this factor with zero explosive intensity. Native C++ supplies defined +0 particle force, avoiding undefined behavior. This is a limited deviation, not complete equivalence of that branch. A deterministic bank must exclude accepted explosive to smoke while exercising accepted explosive to BlowUpCar and collision to smoke. See plane-damage-validationlimitations.json. Runtime bank remains pending.\n')

