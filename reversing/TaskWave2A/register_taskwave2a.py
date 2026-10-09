"""Registro historico de 21 funciones de CTaskComplexKillCriminal, CTaskComplexKillPedOnFootArmed y CTaskSimplePlayerOnFoot (parcial) via el banco simple TaskWave2A. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 21 entradas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-024531-244414'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-taskwave2a')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
KC='CTaskComplexKillCriminal::';KA='CTaskComplexKillPedOnFootArmed::';PF='CTaskSimplePlayerOnFoot::'
PAIRS={KC+'Constructor':256,KC+'Destructor':256,KC+'Clone':128,KC+'CreateSubTask':1024,KC+'FindNextCriminalToKill':1024,KC+'ChangeTarget':1024,KC+'MakeAbortable':1024,KC+'CreateNextSubTask':1024,
 KA+'Constructor':128,KA+'Destructor':128,KA+'Clone':128,KA+'MakeAbortable':256,KA+'IsPedInLeaderFiringLine':1024,KA+'LineOfSightClearForAttack':1024,KA+'CreateFirstSubTask':1024,KA+'ControlSubTask':1024,
 PF+'Constructor':64,PF+'Destructor':64,PF+'Clone':64,PF+'MakeAbortable':1024,PF+'ProcessPed':1024}
TOTAL=12736;assert sum(PAIRS.values())==TOTAL and len(PAIRS)==21
# Mapping: class + name against docs/hooks.csv (must be unique and still pending); addresses/types come from the catalogue.
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in PAIRS:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
EXPECT={KC+'Constructor':'0x0068BE70',KC+'Destructor':'0x0068BF30',KC+'CreateSubTask':'0x0068C050',KC+'FindNextCriminalToKill':'0x0068C3C0',KC+'ChangeTarget':'0x0068C6E0',
 KA+'Constructor':'0x00621190',KA+'Destructor':'0x00621250',KA+'IsPedInLeaderFiringLine':'0x00621300',KA+'LineOfSightClearForAttack':'0x00621500',PF+'Constructor':'0x00685750',PF+'Destructor':'0x006857D0'}
for n,a in EXPECT.items():assert ADDR[n]==a and TYPE[n]=='S',n
for n in PAIRS:
    if n not in EXPECT:assert TYPE[n]=='V' and ADDR[n]=='0x00000000',n
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('17b6765e') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};unequal=0;cases=set();complete=None;begin=None;layout=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':assert row['failures']==0 and row['cases']==PAIRS[row['function']],row
        elif ev=='complete':complete=row
        elif ev=='begin':begin=row
        elif ev=='layout':layout=row
        else:assert ev=='waiting_for_player',ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('begin')==1 and events.get('function_summary')==21 and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True and begin['hooks']==21 and layout['ok'] is True
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-taskwave2a-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-taskwave2a-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), 12736 isolated CPU pairs for 21 functions of CTaskComplexKillCriminal (8), CTaskComplexKillPedOnFootArmed (8) and CTaskSimplePlayerOnFoot (5 of 10), zero differences, natural completion event with state_restored, layout check ok, saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (peds incl. a full CCopPed, criminal, partner, extras, group leader, vehicles, intelligence, player data, fake group with the leader at +0x28, fake tasks with a traced vtable (GetSubTask, GetTaskType, MakeAbortable, destructor) and quiet ones, a fake event with a traced vtable, raw EXE offsets) and compares: return value, the whole memory of the tested object (and of the second object, the partner KillCriminal task), new tasks created in the task pool (type + words without vptr), fake task memory, a fingerprint of all buffers (references masked), reference lists before and after deletion, the full trace (order and count of virtual calls, rand and collaborators), the x87 control word (24-bit precision, sqrt at 24 bits) and an empty x87 stack. '
 'Traced stubs, which get NO credit: rand (0x821B1E) and CGeneral::GetRandomNumber, CPed::SetCurrentWeapon(type), SetPedDefaultDecisionMaker, CCarCtrl::JoinCarWithRoadSystem, CPed::Say, CPedGroups::GetPedsGroup, CWorld::GetIsLineOfSightClear, CPed::GetTransformedBonePosition, CCover::FindAndReserveCoverPoint and FindCoordinatesCoverPoint, CPed::ReleaseCoverPoint, CPedIntelligence::SetTaskDuckSecondary, CTaskSimpleDuck::SetDuckTimer and ControlDuckMove, CTaskSimpleUseGun::ControlGunMove, CTaskComplexKillPedOnFootArmed::CreateSubTask (validated separately; returns a marked fake task), CCamera::Using1stPersonWeaponMode and ClearPlayerWeaponMode, CWeaponEffects::ClearCrossHair and CAnimManager::BlendAnimation; for CTaskSimplePlayerOnFoot::ProcessPed the five out-of-bench functions it calls are simulated with traces (which one is called and with what arguments). Run for real, not credited: IsPlayer, RegisterReference/CleanUpOldReference, CCopPed::AddCriminalToKill/SetPartner, CPedIntelligence::FindTaskByType/GetTaskDuck/GetTaskUseGun, CTaskManager::GetTaskSecondary, CPedGroupMembership::GetLeader, CWeapon::IsTypeMelee, CWeaponInfo, CVector::Normalise, CTimer, the task pool and the constructors/destructors of the subtasks created by KillCriminal (EnterCar, CarDrive, LeaveCar, GangDriveBy, KillPedOnFoot) on a full-size fake vehicle. CGame::currArea (0xB72914) and CLoadMonitor::m_eProcLevel (0xB7298C) are changed during LineOfSightClearForAttack and restored. '
 'Limits: of CTaskSimplePlayerOnFoot only 5 of its 10 functions are in the bench; OUT of the bench and with NO credit: PlayerControlDucked 0x687F30, PlayerControlZelda 0x6883D0, PlayerControlZeldaWeapon 0x687C20, ProcessPlayerWeapon 0x6859A0 and PlayIdleAnimations 0x6872C0 (they run the real player pad, camera, animation manager/IK, weapon system and crime reporting; ProcessPed only simulates them with traces). The vptr (word 0) and the return value are masked in the constructors and destructors (the task type through GetTaskType is compared instead of the vptr; the destructor return value, EAX, is undefined in the EXE; the final vptr of the destructor is the EXE base vtable vs the DLL one): this applies to the constructors/destructors of the bench, in particular those of CTaskComplexKillCriminal. Bench fixtures corrected after the first run (bench fixes, not source fixes): KC_ABORT now has the cop registered at +0x10 (before it stayed 0xCDCDCDCD and the EXE dereferenced it in CCopPed::AddCriminalToKill) and P_CLUMP uses a fake clump. The bytes +0x1B..+0x1D of KillCriminal and +0x31 of KillPedOnFootArmed are not written by the EXE constructors, so the comparison depends on the 0xCD fill of the pool slots and of the object buffer. Only the own part of each function is validated. '
 'Source corrected to what the EXE does before the approved run (uncommitted at the time; see reversing/TaskWave2A/README.md for addresses): see source_corrections_before_run. FIX_BUGS branches removed from the tested functions. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 KC+'Constructor':'Corpus 256; source corrected: ped type filter >= 24 (any MISSIONn) cancels the criminal (0x68BEE7); the reference is registered only if the filters pass; +0x1B..+0x1D are not written. Compared type, not vptr.',
 KC+'Destructor':'Corpus 256; source corrected: the siren bit (veh+0x42D & 0x7F) is cleared only inside the driver branch (0x68BFCD). Final vptr and return masked.',
 KC+'Clone':'Corpus 128; virtual slot 1 of vtable 0x870A00 (0x68CE50).',
 KC+'CreateSubTask':'Corpus 1024; source corrected: SetCurrentWeapon(22) goes after building the task, also in GANG_DRIVEBY (shared end 0x68C36A); types without a case return 0.',
 KC+'FindNextCriminalToKill':'Corpus 1024; source corrected: not "the closest": the last valid candidate with d < reference wins (or any if the reference is 0), the reference is not updated, d^2 = (dz*dz + dy*dy) + dx*dx, alive = health > 0.',
 KC+'ChangeTarget':'Corpus 1024; source corrected: the target must be in the list AND alive; the partner task is found with FindTaskByType(0x451) and ChangeTarget is called without further checks.',
 KC+'MakeAbortable':'Corpus 1024; virtual slot 6 (0x68DAD0); source corrected: d^2 < 625 strict, GetSourceEntity re-called at each use, AddCriminalToKill returning -1 counts as success (FIX_BUGS branch removed); boundary d^2 = 625 exercised; fixture KC_ABORT corrected (cop registered).',
 KC+'CreateNextSubTask':'Corpus 1024; virtual slot 8 (0x68E4F0); source corrected: ENTER_CAR_AS_DRIVER outside a vehicle sets m_CantGetInCar = 1, the criminal-in-vehicle logic was inverted (criminal in vehicle => chase by car; on foot => d^2 <= 625 goes on foot), LEAVE_CAR checks m_pVehicle not bInVehicle, subtasks without a case return 0.',
 KA+'Constructor':'Corpus 128; source corrected: m_strafeDir (+0x31) is not written.',
 KA+'Destructor':'Corpus 128; reviewed MATCH (no source change). Final vptr and return masked.',
 KA+'Clone':'Corpus 128; virtual slot 1 of vtable 0x86D918 (0x6234C0); reviewed MATCH (copies m_aimImmediate +0x35).',
 KA+'MakeAbortable':'Corpus 256; virtual slot 6 (0x6212B0); reviewed MATCH.',
 KA+'IsPedInLeaderFiringLine':'Corpus 1024; source corrected: leader->target direction with z = 0 and normalised, projection dir*dot (+ leader) with dot (y+z)+x, strict 4.0 and 100.0 tests, final dot product on the un-normalised vector (z+y)+x; boundaries 2.0 and 10.0.',
 KA+'LineOfSightClearForAttack':'Corpus 1024; source corrected: dynamic thresholds (10000 ms / 9.0 by default; 5000 ms and 4.0 or 9.0 in an interior, or when the target is the player and CGame::currArea != 0 or mission-created, depending on m_eProcLevel >= 2), re-check only if MORE than the threshold has passed, null target tolerated in the movement check, crouched (bit 26 of +0x46C) uses position + 0.25 and standing uses the head (was reversed), positions computed before disabling the target vehicle collision.',
 KA+'CreateFirstSubTask':'Corpus 1024; virtual slot 9 (0x62BF00); source corrected: m_newTarget = false before looking at the target, RNG through 0x821B1E (not RandomBool), distance with sqrt > 30.0 or (> 6.0 and armed), unconditional FindCoordinatesCoverPoint and coverPos.z += 1.0, threshold (range*range)*0.25, FIX_BUGS branch removed.',
 KA+'ControlSubTask':'Corpus 1024; virtual slot 10 (0x62CCE0); source corrected: strict d^2 > 400 first and only then RNG 0x407180 (0..100, unsigned compare), GUN_CTRL uses the distance with sqrt and the EXE thresholds (range*0.5, 4.0, +0x9C4 ms), strafe flag and direction > 3 (no-op instead of NOTSA_UNREACHABLE), SEEK_ENTITY with range*0.5 > dist and up to two LOS calls, FIX_BUGS branch for m_lastDuckTime removed.',
 PF+'Constructor':'Corpus 64; reviewed MATCH (no source change). Compared type, not vptr.',
 PF+'Destructor':'Corpus 64; reviewed MATCH (vptr + jump to 0x61A3A0). Final vptr and return masked.',
 PF+'Clone':'Corpus 64; virtual slot 1 of vtable 0x8708EC (0x68AFF0); reviewed MATCH.',
 PF+'MakeAbortable':'Corpus 1024; virtual slot 6 (0x6857E0); reviewed MATCH.',
 PF+'ProcessPed':'Corpus 1024; virtual slot 7 (0x688810); reviewed MATCH; the five unbenched functions it calls (PlayerControlDucked, Zelda, ZeldaWeapon, ProcessPlayerWeapon, PlayIdleAnimations) are traced simulations and get NO credit.'}
assert set(SPECIFIC)==set(PAIRS)
SOURCE_CHANGES={'source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp':'Constructor (type filter >= 24, reference only if filters pass, +0x1B..+0x1D not written); Destructor (siren only in the driver branch); CreateSubTask (SetCurrentWeapon order, GANG_DRIVEBY, types without a case return 0); FindNextCriminalToKill (last valid candidate wins, (z2+y2)+x2, alive = health > 0); ChangeTarget (in list and alive, FindTaskByType 0x451); MakeAbortable (d2 < 625 strict, GetSourceEntity re-called, -1 counts as success, FIX_BUGS removed); CreateNextSubTask (m_CantGetInCar = 1, inverted criminal-in-vehicle logic, LEAVE_CAR checks m_pVehicle)',
 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp':'Constructor (m_strafeDir not written); IsPedInLeaderFiringLine (z = 0, order of sums, strict thresholds); LineOfSightClearForAttack (dynamic thresholds, crouched/standing, null target); CreateFirstSubTask (m_newTarget = false, coverPos.z += 1, RNG 0x821B1E, thresholds); ControlSubTask (RNG 0x407180, strafe flags, GUN_CTRL and SEEK_ENTITY thresholds); FIX_BUGS branches removed',
 'source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h':'member initialisers removed (+0x1B..+0x1D)','source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h':'member initialiser removed (m_strafeDir)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,begin_events=1,function_summaries=21,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 out_of_bench_not_credited=['CTaskSimplePlayerOnFoot::PlayerControlDucked 0x687F30','CTaskSimplePlayerOnFoot::PlayerControlZelda 0x6883D0','CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon 0x687C20','CTaskSimplePlayerOnFoot::ProcessPlayerWeapon 0x6859A0','CTaskSimplePlayerOnFoot::PlayIdleAnimations 0x6872C0'],
 readme_discrepancies='reversing/TaskWave2A/README.md said "not compiled or run yet" at preparation time; the LiveComparison.hpp header comment (hashed in the manifest, left untouched) lists CTaskSimplePlayerOnFoot as 4 of 10 and ProcessPed as out of the bench, but the executed bank (21 hooks, 1024 ProcessPed pairs, summary.json) includes ProcessPed (5 of 10). The earlier failed run (previous.jsonl, 02:35) is not preserved. README approval section added at registration.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-task-wave2a.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled TaskWave2A probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 12736-row log reaudited offline and preserved gzipped with the original hash. TaskComplexKillCriminal and TaskComplexKillPedOnFootArmed (cpp and h) were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/TaskWave2A/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.task-wave2a.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
