"""Registro historico de quince funciones de CRoadBlocks, CPools y CIdleCam via el banco simple WorldWave2. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 15 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-023948-282056'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-worldwave2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CRoadBlocks::RegisterScriptRoadBlock':512,'CRoadBlocks::ClearScriptRoadBlocks':128,'CRoadBlocks::GetRoadBlockNodeInfo':768,'CRoadBlocks::ClearSpaceForRoadBlockObject':1024,'CRoadBlocks::GenerateRoadBlocks':1024,'CPools::MakeSureSlotInObjectPoolIsEmpty':512,'CPools::CheckPoolsEmpty':128,'CIdleCam::Init':64,'CIdleCam::Reset':128,'CIdleCam::ProcessIdleCamTicker':256,'CIdleCam::GetLookAtPositionOnTarget':512,'CIdleCam::IdleCamGeneralProcess':256,'CIdleCam::SetTarget':384,'CIdleCam::SetTargetPlayer':128,'CIdleCam::IsTargetValid':512}
ADDR={'CRoadBlocks::RegisterScriptRoadBlock':'0x00460DF0','CRoadBlocks::ClearScriptRoadBlocks':'0x00460EC0','CRoadBlocks::GetRoadBlockNodeInfo':'0x00460EE0','CRoadBlocks::ClearSpaceForRoadBlockObject':'0x00461020','CRoadBlocks::GenerateRoadBlocks':'0x004629E0','CPools::MakeSureSlotInObjectPoolIsEmpty':'0x00550080','CPools::CheckPoolsEmpty':'0x00551950','CIdleCam::Init':'0x0050E6D0','CIdleCam::Reset':'0x0050A160','CIdleCam::ProcessIdleCamTicker':'0x0050A200','CIdleCam::GetLookAtPositionOnTarget':'0x0050EAE0','CIdleCam::IdleCamGeneralProcess':'0x0050E690','CIdleCam::SetTarget':'0x0050A280','CIdleCam::SetTargetPlayer':'0x0050EB50','CIdleCam::IsTargetValid':'0x00517770'}
TOTAL=6336;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('1552ba83') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};unequal=0;cases=set();complete=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True:unequal+=1
        elif ev=='complete':complete=row
        elif ev=='error':raise SystemExit('error event in log')
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('function_summary')==len(PAIRS) and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-worldwave2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-worldwave2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON='Simple pair bench inside the full game (original with hook off vs candidate with hook on, hooked in CGame::Process), 6336 isolated CPU pairs for the 15 functions, zero differences, natural completion event with state_restored, saves unchanged, configs restored. Each case rebuilds the same fixture from the same seed in both legs and compares the result, the fixture state (hash or bytes), the x87 control word (24-bit precision) and the ordered trace of every outgoing call with raw arguments; the hook jump state is checked. CRoadBlocks: the global block 0xA43438..0xA43C78 is saved/restored; ThePaths areas 61 (unloaded), 62 and 63 are replaced by fake nodes (0x1C), navigation links and CCarPathLink (0xE) with pointers and m_anNumAddresses restored; fake 0xA40-byte entities with a fake vtable (slot 0 = traced deleting destructor). CPools: the object pool pointer (0xB7449C) is swapped for a fake 4-slot pool of 0x19C bytes and restored. CIdleCam: fake 0x9C-byte object with guards and a fake 0x300-byte camera; CPad 0, CTimer, the gIdleCam ticker, the InterestingEvents byte, the active camera of TheCamera and CWorld::pIgnoreEntity are saved/restored. Traced stubs, which get NO credit: FindObjectsIntersectingCube, CWorld::Remove/Add, CanBeDeleted of vehicle/ped/object, rand (0x821B1E) and CGeneral::GetRandomNumber (same sequence), FindPlayerWanted/Vehicle/Coors/Ped, CreateRoadBlockBetween2Points (trace only), CWorld::ProcessLineOfSight, CProjectileInfo::RemoveIfThisIsAProjectile, CObject::operator new and constructor, the EXE printf (0x821982; the DLL does not call it, so CheckPoolsEmpty is compared by state only: nothing must change), RegisterReference, CleanUpOldReference and GetIsLineOfSightClear. Only the own part of each function is validated. Bench noise declared: CIdleCam::SetTarget with denormal floats (SSE DAZ/FTZ in the DLL vs x87 in the EXE) fails only when m_Target is null and a component is a small integer (denormal) added to 0.0; the camera vectors are normalised in the fixture (FillIdle) so no denormals reach the function. Excluded from the bench and NOT credited: CRoadBlocks::Init, CRoadBlocks::GenerateRoadBlockPedsForCar, CPools::Initialise, ShutDown, Save, Load, LoadPedPool, LoadObjectPool and LoadVehiclePool, CIdleCam::FinaliseIdleCamera, ProcessSlerp, ProcessTargetSelection, Run and Process, and the whole of CCam. Historical evidence approved with explicit limits; no current dependency accreditation.'
SPECIFIC={'CRoadBlocks::RegisterScriptRoadBlock':'Corpus 512 over the aScriptRoadBlocks table and NumRoadBlocks.','CRoadBlocks::ClearScriptRoadBlocks':'Corpus 128; compared by state of the global block.','CRoadBlocks::GetRoadBlockNodeInfo':'Corpus 768; calls the two real hooks with 0xDEADBEEF sentinels to check that nothing is written on return false.','CRoadBlocks::ClearSpaceForRoadBlockObject':'Corpus 1024; source corrected before the run: an entity of a different type returns false and objectType is checked.','CRoadBlocks::GenerateRoadBlocks':'Corpus 1024; first run 317/1024 exposed a real source difference, fixed before the approved run: indices, IsAreaLoaded with m_wAreaId (0x462AC4 movzx eax,di), inner loop up to NumRoadBlocks creating the barricade of the own node with mrbDir (0x4631F7..0x4632AE), |dot| <= 0.7 NaN-safe as fcomp + test ah,41h. GeneratePeds (initial model 0, order of sums, pistol if CITYCOP, exact -pi/2, GetRandomIntInRange(0,8), gang weapon) was corrected in the source; GenerateRoadBlockPedsForCar itself is out of the bench.','CPools::MakeSureSlotInObjectPoolIsEmpty':'Corpus 512 with the fake 4-slot object pool; the function was implemented in the source before the run. LoadVehiclePool, LoadPedPool, LoadObjectPool and ShutDown were also corrected in the source but are out of the bench and get no credit; CEntity::ClearRwObjectPointer was added.','CPools::CheckPoolsEmpty':'Corpus 128; state-only comparison (the EXE printf is not called by the DLL).','CIdleCam::Init':'Corpus 64.','CIdleCam::Reset':'Corpus 128.','CIdleCam::ProcessIdleCamTicker':'Corpus 256; source corrected to use _ftol2.','CIdleCam::GetLookAtPositionOnTarget':'Corpus 512; source corrected to take vecUp by reference.','CIdleCam::IdleCamGeneralProcess':'Corpus 256.','CIdleCam::SetTarget':'Corpus 384; denormal float noise normalised in the fixture (see scope).','CIdleCam::SetTargetPlayer':'Corpus 128.','CIdleCam::IsTargetValid':'Corpus 512; source corrected to sum z, y, x in that order.'}
SOURCE_CHANGES={'source/game_sa/RoadBlocks.cpp':'ClearSpaceForRoadBlockObject (different type returns false, objectType check); GeneratePeds (initial model 0, order of sums, pistol if CITYCOP, exact -pi/2, GetRandomIntInRange(0,8), gang weapon); GenerateRoadBlocks (indices, IsAreaLoaded with m_wAreaId, inner loop to NumRoadBlocks with the own-node barricade, NaN-safe |dot|<=0.7)','source/game_sa/IdleCam.cpp':'vecUp by reference, ProcessIdleCamTicker with _ftol2, IsTargetValid sums z,y,x','source/game_sa/Pools.cpp':'MakeSureSlotInObjectPoolIsEmpty implemented; LoadVehiclePool/LoadPedPool/LoadObjectPool/ShutDown corrected (outside the bench)','source/game_sa/Entity/Entity.h':'CEntity::ClearRwObjectPointer added'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=len(PAIRS),unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-world-wave2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':'S'}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled WorldWave2 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 6336-row log reaudited offline and preserved gzipped with the original hash. RoadBlocks.cpp, IdleCam.cpp, Pools.cpp and Entity.h were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/WorldWave2/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.world-wave2.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
