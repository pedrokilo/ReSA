"""Registro historico de 51 funciones de CTrain (19), CPlane (14) y CBike (18) via el banco simple VehWave2. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 51 entradas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-033721-461118'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-vehwave2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
PAIRS=summary['pairs_per_function'];TOTAL=12864;NFUN=51
assert sum(PAIRS.values())==TOTAL and len(PAIRS)==NFUN
assert sum(1 for n in PAIRS if n.startswith('CTrain::'))==19 and sum(1 for n in PAIRS if n.startswith('CPlane::'))==14 and sum(1 for n in PAIRS if n.startswith('CBike::'))==18
OUT_OF_BENCH=['CTrain::ProcessControl','CBike::Constructor','CBike::Destructor','CBike::DamageKnockOffRider','CBike::ProcessEntityCollision']
assert not any(n in PAIRS for n in OUT_OF_BENCH)
# Mapping: class + name against docs/hooks.csv (must be unique and still pending); virtual functions have address 0.
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in PAIRS:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
    assert (TYPE[name]=='V')==(ADDR[name]=='0x00000000'),name
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith('67a86a6e') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};unequal=0;cases=set();complete=None;layout=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':assert row['failures']==0 and row['cases']==PAIRS[row['function']],row
        elif ev=='complete':complete=row
        elif ev=='layout':layout=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('function_summary')==NFUN and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True and complete['layout_ok'] is True and layout['ok'] is True
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-vehwave2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-vehwave2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes()
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
COMMON=('Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), 12864 isolated CPU pairs for 51 functions of CTrain (19), CPlane (14) and CBike (18), zero differences, natural completion event with state_restored, layout check ok, saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake 0xA18-byte vehicles in a fake 8-slot vehicle pool that replaces the real pool pointer 0xB74494 and is restored, fake vtable with traced slots, 32-byte guards) and compares: the full hash of every vehicle, the guard, the return value, the x87 control word and the trace of outgoing calls with arguments. Virtual functions are called through the EXE vtable slot (with the hook on it points to the DLL). '
 'Traced stubs, which get NO credit: CWorld::Add/Remove, FindPlayerVehicle, CWorld::ProcessVerticalLine, CTrain::FindPositionOnTrackFromCoors, operator delete/delete[] (Shutdown only), CWorld::ClearScanCodes, TrainHitStuff, CVehicle::Render, CAutomobile::OpenDoor, CVehicle::SetModelIndex, CClumpModelInfo::FillFrameArray, CBike::ProcessRiderAnims, GetFirstObject, CAutomobile::CAutomobile, CVehicleModelInfo::GetWheelPosn, CEntity::GetColModel, CPlaceable::SetOrientation, RwFrameGetLTM, CPhysical::ApplyMoveSpeed/ApplyTurnSpeed/SkipPhysics/CheckCollision/ApplyMoveForce/ApplyTurnForce, cBuoyancy::ProcessBuoyancy, CBike::CalculateLeanMatrix and DoHeadLightBeam (Render only) and the RenderState pointers of RwGlobals. '
 'Limits: OUT of the bench and with NO credit: CTrain::ProcessControl (full train physics), CBike::Constructor, CBike::Destructor, CBike::DamageKnockOffRider (corrected in source but not benched: needs fake peds, events and CEventGroup::Add) and CBike::ProcessEntityCollision. CBike::ProcessBuoyancy is compared only without driver and passengers (m_pDriver = null, m_nMaxPassengers = 0); CBike::GetComponentWorldPosition only with present nodes (the missing-node path differs in logging); CPlane::OpenDoor with the dead-code global flag 0xC1CAFC off. Masked bench noise: in CBike::CalculateLeanMatrix the padding of the lean matrix (+0xC/+0x1C/+0x2C/+0x3C), which is stack garbage in the EXE (0x59BE30 temp + rep movs); CMatrix comparisons ignore the padding between vectors. Fixture floats are finite or quiet NaN. Only the own part of each function is validated. '
 'Source corrected to what the EXE does before the approved run (uncommitted at the time; see reversing/VehWave2/README.md for addresses): see source_corrections_before_run. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPEC_TXT={
 'CTrain::Shutdown':'source corrected: delete[] (0x8213AE) only when the pointer is not null and every entry of 0xC38024 is zeroed (was scalar delete on a copy).',
 'CTrain::SetTrainSpeed':'source corrected: fmul 0.02f (0x858B38), not speed/50.',
 'CTrain::SetTrainCruiseSpeed':'source corrected: _ftol2 and low byte (mov [ecx+3D0h],al), no assert with values > 255; tested up to |1e6|.',
 'CTrain::IsInTunnel':'source corrected: CColPoint/CEntity* locals left uninitialised as in the EXE.',
 'CTrain::RemoveMissionTrains':'source corrected: pool walked from the last slot to the first (dec edi, sub ebp,0A18h).',
 'CTrain::ReleaseMissionTrains':'source corrected: pool walked from the last slot to the first.',
 'CTrain::MarkSurroundingEntitiesForCollisionWithTrain':'source corrected: sector = floor(v*0.02f + 60.0f); positions in [-3300, 3300], radius up to 6000, scan counter 0, 1, 0xFFFE, 0xFFFF.',
 'CPlane::Constructor':'source corrected: door angles with the EXE constants 0x3FF1463B (HYDRA/RUSTLER/CROPDUST/STUNT), 0xC016CBE4 (SHAMAL) and 0xBFA0D97C (NEVADA).',
 'CPlane::AreWeInNoPlaneZone':'source corrected: distance summed as (dz*dz + dy*dy) + dx*dx; NaN camera and exact zone edges included.',
 'CPlane::AreWeInNoBigPlaneZone':'NaN camera and exact zone edges included.',
 'CPlane::OpenDoor':'virtual slot 27; the global flag 0xC1CAFC branch is dead code in the EXE (source keeps it as if(false)) and is not exercised.',
 'CPlane::SetupDamageAfterLoad':'virtual slot 51; the fake vtable slot 51 holds the real effect.',
 'CBike::CalculateLeanMatrix':'source corrected: component = (minZ * axis) rounded to float then * (1 - fcos(angle)), added to the position in x, y, z order; the lean matrix padding is masked (stack garbage in the EXE; first run 516/1024 differed only in the full vehicle hash).',
 'CBike::ProcessBuoyancy':'source corrected: force Z = force.z/((t*mass)*0.008), ((usedMass/constant)*force)*1.05, multiplier 1 - force*0.05, pow of the EXE (0x822130) in ST0 without rounding, NaN-safe comparisons (fcomp + test ah,41h); first run found 5/1024 real differences with NaN; compared without driver or passengers only.',
 'CBike::GetAllWheelsOffGround':'source corrected: the EXE reads byte +0x805 (source read +0x804); first run 51/128 real differences.',
 'CBike::ProcessDrivingAnims':'virtual slot 46; source corrected: the EXE returns when bOffscreen and status is NOT STATUS_PLAYER (condition was inverted).',
 'CBike::GetComponentWorldPosition':'virtual slot 25; only nodes present (the missing-node path calls printf in the EXE vs spdlog in the DLL).'}
SPECIFIC={n:'Corpus %d; '%PAIRS[n]+SPEC_TXT.get(n,'virtual slot of the EXE vtable; compared as is.' if TYPE[n]=='V' else 'compared against the EXE as is.') for n in PAIRS}
SOURCE_CHANGES={'source/game_sa/Entity/Vehicle/Train.cpp':'Shutdown (delete[] and table zeroing); SetTrainSpeed (fmul 0.02f); SetTrainCruiseSpeed (_ftol2, low byte); IsInTunnel (uninitialised locals); RemoveMissionTrains/ReleaseMissionTrains (last slot to first); MarkSurroundingEntitiesForCollisionWithTrain (floor(v*0.02+60))',
 'source/game_sa/Entity/Vehicle/Plane.cpp':'Constructor (EXE door-angle constants); AreWeInNoPlaneZone ((dz2+dy2)+dx2)',
 'source/game_sa/Entity/Vehicle/Bike.cpp':'CalculateLeanMatrix; ProcessBuoyancy (EXE formulas, NaN-safe comparisons); DamageKnockOffRider (out of bench); ProcessDrivingAnims (inverted condition); GetAllWheelsOffGround (+0x805); SetupModelNodes (no GetRpClump)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=PAIRS,covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=NFUN,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 out_of_bench_not_credited=OUT_OF_BENCH,
 readme_discrepancies='reversing/VehWave2/README.md says "written but NOT compiled or run" (pre-run state at preparation). The earlier failed run (previous.jsonl) is not preserved. README approval section added at registration.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-veh-wave2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled VehWave2 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 12864-row log reaudited offline and preserved gzipped with the original hash. Train.cpp, Plane.cpp and Bike.cpp were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/VehWave2/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.veh-wave2.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
