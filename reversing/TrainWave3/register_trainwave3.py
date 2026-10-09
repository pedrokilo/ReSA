"""Registro historico de CTrain::ProcessControl (virtual, hueco 10 de la vtable 0x872370 = 0x6F86A0) via el banco simple TrainWave3. Re-audita en streaming la corrida aprobada, conserva la evidencia en runs/ y anade 1 entrada (dependency_review=null). CTrain queda completa."""
import csv,gzip,hashlib,json,os,shutil,subprocess,datetime
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-110357-858237'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-trainwave3')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
NAME='CTrain::ProcessControl';PAIRS={NAME:1024};TOTAL=1024
assert summary['pairs_per_function']==PAIRS
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
cls,fn=NAME.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
assert len(m)==1 and m[0]['validated']=='0' and m[0]['address']=='0x00000000' and m[0]['type']=='V';ADDR=m[0]['address'];TYPE=m[0]['type']
assert [r['fn_name'] for r in rows if r['class']==cls and r['validated']=='0']==[fn]
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith('1d1240d8') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};unequal=0;cases=set();complete=None;layout=None;fsum={}
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='layout':layout=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('layout')==1 and events.get('function_summary')==1 and events.get('case_start')==TOTAL and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert fsum[NAME]['cases']==TOTAL and fsum[NAME]['failures']==0
assert complete['pairs']==TOTAL and complete['state_restored'] is True and complete['layout_ok'] is True and layout['ok'] is True
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-trainwave3-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-trainwave3-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
# TrainNode.cpp (GetDistanceFromStart * 0.33333334f) was edited BEFORE the probe build but is not listed in the build manifest: its hash is recorded as current, provenance inferred from mtimes.
TN='source/game_sa/TrainNode.cpp';tn_sha=sha(ROOT/TN)
tn_mtime=datetime.datetime.fromtimestamp((ROOT/TN).stat().st_mtime);asi_mtime=datetime.datetime.fromtimestamp((HERE/'artifacts/gta_reversed-trainwave3-probe.asi').stat().st_mtime)
assert tn_mtime<asi_mtime,(tn_mtime,asi_mtime)
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
COMMON=('Simple pair bench inside the full game with a loaded save (original with the virtual hook off, EXE body 0x6F86A0 = vtable 0x872370 slot 10, vs candidate with the vtable hook on), 1024 isolated CPU pairs for CTrain::ProcessControl (hooks.csv row with address 0x00000000, virtual), zero differences, natural completion event with state_restored and layout_ok, layout event ok, saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake train of 0x6C0 bytes with fake vtable and guards, chain of 1 to 4 fake carriages, fake closed track of 6 to 72 nodes with strictly increasing cumulative distances and the global track tables 0xC38024/0xC38014/0xC37FEC, fake model info with CColModel and fake handling) and compares the full hash of the 8 fake vehicles and the guard, the subject matrix, the track, the touched globals, the x87 control word (24 bits) and the whole trace of outgoing calls with raw arguments (floats as bit patterns); foreign threads ignored (Mine()). '
 'Scenes: gbModelViewer 3%, rear carriage with predecessor 22%, rear carriage without predecessor 6%, head on autopilot 25%, head driven by the player 18%, off-rail (bNotOnARailRoad) 26%; exact edge values of the rail position. '
 'Limits: about 25 collaborators are replaced by traced doubles WITHOUT credit (CAEVehicleAudioEntity::Service/AddAudioEvent, CPad::GetAccelerate/GetBrake/StartShake, CPed::IsPlayer, CPlayerPed::GetPadFromPlayer, CTrain::FindNumCarriagesPulled/FindMaximumSpeedToStopAtStations/IsInTunnel/RemoveRandomPassenger/AddNearbyPedAsRandomPassenger, CGameLogic::CalcDistanceToForbiddenTrainCrossing, CCamera::CamShake, rand and CGeneral::GetRandomNumber, FindPlayerPed, CCarCtrl::ScanForPedDanger, CPhysical::ProcessControl/ApplyMoveSpeed/SkipPhysics/ApplyMoveForce/ApplyTurnForce/RemoveAndAdd, CEntity::UpdateRwFrame, cBuoyancy::ProcessBuoyancy, MarkSurroundingEntitiesForCollisionWithTrain, CTrainNode::GetLightingFromCollision, tColLighting::GetCurrentLighting). '
 'OUT of the bench: carriage with a null m_matrix in the on-rail branch (the EXE writes through it unchecked), real m_pRwObject (null in the bench), GetLightingFromCollision without found lighting (real CWorld::ProcessVerticalLine), out-of-domain values (rail distance < -total or >= 2*total, node index outside the track) and NaN/inf. '
 'The GetLightingFromCollision stub has a different version for the DLL (hidden return pointer) than for the EXE. Only the own part of the function is validated. Source corrected to what the EXE does before the approved run (uncommitted at the time; see reversing/TrainWave3/README.md). Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={NAME:'Corpus 1024; source corrected (EXE addresses): (1) 0x6F88D0/0x6F8910/0x6F8940 or/and [eax+5B8h] stopped-at-station and passengers flags written on each carriage, not on this; (2) 0x6F8DF0 bNotOnARailRoad on each carriage; (3) 0x6F8991 fmul 0.02f stop speed; (4) 0x6F8BF2, 0x6F8E49, 0x6F9D1B EXE pow 0x822130 with double base and no float rounding; (5) 0x6F8D93 node dot product in z,x,y order; (6) 0x6F9626 fpatan minus old heading with the EXE pi constants (0x858CB8/BC/C0); (7) 0x6F9910 m_fMovingSpeed = sqrt((dz^2+dy^2)+dx^2); (8) 0x6F9CA6..0x6F9DA2 buoyancy with the force returned by ProcessBuoyancy; (9) 0x6F8EAD and 0x6F8F13 FindPlayerPed(-1) only in passenger states 0 and 2; (10) node distance uint16*0.33333334f (0x859040) inlined in the EXE and also fixed in CTrainNode::GetDistanceFromStart (TrainNode.cpp, 0x6F54B0).'}
SOURCE_CHANGES={'source/game_sa/Entity/Vehicle/Train.cpp':'ProcessControl: per-carriage flags (0x6F88D0, 0x6F8910, 0x6F8940), per-carriage bNotOnARailRoad (0x6F8DF0), stop speed *0.02f (0x6F8991), EXE pow 0x822130 (0x6F8BF2, 0x6F8E49, 0x6F9D1B), node dot product z,x,y (0x6F8D93), heading with fpatan and EXE pi (0x6F9626), moving speed sqrt((dz2+dy2)+dx2) (0x6F9910), buoyancy with the returned force (0x6F9CA6..0x6F9DA2), FindPlayerPed(-1) only in states 0 and 2 (0x6F8EAD, 0x6F8F13), node distance uint16*0.33333334f (0x859040)',
 TN:'GetDistanceFromStart: uint16 * 0.33333334f as the EXE 0x6F54B0 (not hashed by the probe build manifest; mtime before the probe build)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],extra_sources_not_in_manifest={TN:dict(current_sha256=tn_sha,mtime=tn_mtime.isoformat(timespec='seconds'),probe_asi_mtime=asi_mtime.isoformat(timespec='seconds'))},game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=PAIRS,covered_functions={NAME:True},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,layout_events=1,layout_ok=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=1,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 out_of_bench_not_credited=['null m_matrix on-rail branch','real m_pRwObject','GetLightingFromCollision without found lighting','out-of-domain rail values','NaN/inf'],
 notes='With this function CTrain has no pending entries in docs/hooks.csv (it was the only one). docs/hooks.csv keeps address 0x00000000 for this virtual; the mapping was done by class + name (unique row) and the EXE body 0x6F86A0 = vtable 0x872370 slot 10 (README). TrainNode.cpp (CTrainNode::GetDistanceFromStart fix) is not in the probe build manifest; its current hash is recorded in manifest.extra_sources_not_in_manifest and the equality with the tested build is inferred from mtimes.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-train-wave3.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
hook={'class':cls,'fn_name':fn,'address':ADDR,'type':TYPE}
assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions'])
data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',NAME],pairs=TOTAL,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
 source_provenance='Exact compiled TrainWave3 probe (ASI/PDB hashes in build-manifest.json); Train.cpp/.h hashed from the probe build manifest and equal to the working tree at registration; full 1024-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[NAME],review_reference='reversing/TrainWave3/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.train-wave3.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
import time
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
