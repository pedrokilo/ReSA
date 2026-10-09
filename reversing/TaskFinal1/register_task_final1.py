"""Registro historico de 10 constructores/destructores de tareas (banco simple TaskFinal1). Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 10 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-005855-477249'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-taskfinal1')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CTaskUtilityLineUpPedWithCar::Constructor':512,'CTaskUtilityLineUpPedWithCar::Destructor':128,'CTaskGoToVehicleAndLean::Constructor':256,'CTaskGoToVehicleAndLean::Destructor':256,
 'CTaskGangHassleVehicle::Constructor':256,'CTaskGangHassleVehicle::Destructor':512,'CTaskGangHasslePed::Constructor':256,'CTaskGangHasslePed::Destructor':256,
 'CTaskComplexPlayHandSignalAnim::Constructor':256,'CTaskComplexPlayHandSignalAnim::Destructor':512}
ADDR={'CTaskUtilityLineUpPedWithCar::Constructor':'0x0064FBB0','CTaskUtilityLineUpPedWithCar::Destructor':'0x0064FC00','CTaskGoToVehicleAndLean::Constructor':'0x00660E60','CTaskGoToVehicleAndLean::Destructor':'0x00660EE0',
 'CTaskGangHassleVehicle::Constructor':'0x0065FAC0','CTaskGangHassleVehicle::Destructor':'0x0065FB60','CTaskGangHasslePed::Constructor':'0x0065FED0','CTaskGangHasslePed::Destructor':'0x0065FF60',
 'CTaskComplexPlayHandSignalAnim::Constructor':'0x0061B2B0','CTaskComplexPlayHandSignalAnim::Destructor':'0x0061BDF0'}
TOTAL=3200;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('74c089e6') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
# Independent offline reaudit of the full log.
events={};per={};unequal=0;cases=set();complete=None;begin=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':assert row['failures']==0 and row['cases']==PAIRS[row['function']],row
        elif ev=='complete':complete=row
        elif ev=='begin':begin=row
        else:assert ev=='waiting_for_player',ev
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('begin')==1 and events.get('function_summary')==10 and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True and begin['known'] is True and all(s['ok'] for s in begin['sites'])
# Build manifest and sources (must be the exact files the probe was built from).
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-taskfinal1-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-taskfinal1-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
# Preserve the run (new path only).
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
COMMON=('Simple pair bench inside the full game (original with the hook off vs candidate with the hook on), 3200 isolated CPU pairs for the 10 constructors/destructors, zero differences, natural completion event with state_restored, saves unchanged, configs restored. '
 'Objects live in a private 0x80-byte buffer pre-filled with 0xCD between 16-byte guards (the task pool is never used: its occupancy is compared and restored); constructors are called at the EXE address, destructors in place at the EXE address on objects with random field contents. '
 'Compared per pair: the whole object word by word (pointers into the fixtures normalised to names), the guards, the constructor return value (this), the task type through GetTaskType instead of the vtable pointer, the reference lists of the fake ped and vehicle, hashes of the fake ped and vehicle (m_pReferences masked), the fake subtask contents, the 155 animation blocks (hash before and after), the four hand model reference counts, CTaskComplexPlayHandSignalAnim::ms_animBlock, the game clock and the call trace. '
 'Traced stubs, patched at the EXE address, at the DLL entry and at the DLL body, so these helpers get NO credit: CBaseModelInfo::RemoveRef (decrements the counter at +8; the TXD counter is not touched) and CStreaming::RemoveModel. Not stubbed (run for real, though not validated here): CAnimManager::RemoveAnimBlockRef, GetAnimationBlockIndex("gangs"), CVehicle::SetHasslePosId, RegisterReference/CleanUpOldReference and the base class constructors/destructors. '
 'Limits: word 0 (vptr) of the task objects is excluded from the compared state (constructors compare GetTaskType instead; destructors receive the EXE vtable and the vptr rewritten by the destructor is not compared); the destructor return value is not compared (undefined in the EXE); the deleting-destructor/operator delete path is not exercised (destructors are called in place); the fake subtask has a traced slot-0 destructor and trapped other slots; float arguments include 0, -0, +-Inf, quiet NaN, a denormal and large values but never a signalling NaN; entities are zeroed fake CPed/CAutomobile buffers (vehicle byte 0x51B random); no real peds, vehicles, pools or files. '
 'Source corrected to what the EXE does before this run: CTaskUtilityLineUpPedWithCar constructor stores m_fTime = current time + time (0x64FBCD); CTaskComplexPlayHandSignalAnim destructor removes a hand reference only if its bit is set (bit 0 left, bit 1 right, byte 0x14), the LEFT/RIGHT enum indices were fixed and the constructor does and byte [esi+14h],0F8h; redundant base initialisers removed in CTaskGoToVehicleAndLean and CTaskGangHassleVehicle (no behaviour change; CTaskGangHasslePed has no source change). '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 'CTaskUtilityLineUpPedWithCar::Constructor':'Corpus 512: position vector from finite values and special floats (0, -0, +-Inf, quiet NaN, 1.0, a denormal, FLT_MAX), time from {0,1,-1,600,INT_MAX,INT_MIN,-600} or random, two more ints (0..7 and 0..23 in 75% of cases, else random), clock from special values or random; checks m_fTime = clock + time. Not a CTask (no vptr): the whole object is compared.',
 'CTaskUtilityLineUpPedWithCar::Destructor':'Corpus 128: random bytes in the whole 0x1C object; no collaborators.',
 'CTaskGoToVehicleAndLean::Constructor':'Corpus 256: fake vehicle (80%) or null, int from 0..99999 or random.',
 'CTaskGoToVehicleAndLean::Destructor':'Corpus 256: random fields, vehicle reference registered (75% of the non-null cases) or not, byte at 0x14 random, gangs animation block reference count 0, 0xFFFF or 1..4, fake subtask present (75%) or null.',
 'CTaskGangHassleVehicle::Constructor':'Corpus 256: fake vehicle (80%) or null, random int, raw byte argument, two floats from specials/finite; vehicle byte 0x51B (harassment positions) random.',
 'CTaskGangHassleVehicle::Destructor':'Corpus 512: vehicle reference (80%), harassment position id from {-1,-2,INT_MIN,0..8,15,31}, second reference null/ped/vehicle, flag byte 0x30, gangs block count 1..4, fake subtask.',
 'CTaskGangHasslePed::Constructor':'Corpus 256: fake ped (80%) or null and three ints (0..99 in half of the cases, else random).',
 'CTaskGangHasslePed::Destructor':'Corpus 256: ped reference (80%, registered 75%), flag byte 0x1C, gangs block count 1..4, fake subtask.',
 'CTaskComplexPlayHandSignalAnim::Constructor':'Corpus 256: animation ids from {-1,0,1,0x140..0x144,INT_MAX,INT_MIN} (50%) or 0..999 and a float from specials/finite; byte 0x14 must end with bits 0..2 cleared (source corrected).',
 'CTaskComplexPlayHandSignalAnim::Destructor':'Corpus 512: byte 0x14 random (bits 0/1 select the hands), ms_animBlock -1 (25%) or a valid block, block reference count in {0,1,2,3,0xFFFF}, the four hand model counters all non-zero (33%) or from {0,1,2,3,0xFFFF}; hand reference removal conditional on the bits (source corrected).'}
SOURCE_CHANGES={'source/game_sa/Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp':'constructor m_fTime = CTimer time + time (0x64FBCD)','source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp':'destructor per-hand bit test, LEFT/RIGHT indices, constructor and byte 0x14','source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp':'redundant base initialiser removed (no behaviour change)','source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp':'redundant base initialiser removed (no behaviour change)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (1.2 GB, not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,begin_events=1,function_summaries=10,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 readme_discrepancies='reversing/TaskFinal1/README.md announced 3072 pairs, "not yet compiled or run", and a source-neutral change in CTaskGangHasslePed; the executed bank has 3200 pairs (sum of the per-function corpus) and git shows no change in TaskGangHasslePed.cpp/.h. README header corrected at registration.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-task-final1.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);assert n==6881,n
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':'S'}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled TaskFinal1 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 3200-row log reaudited offline and preserved gzipped with the original hash. The task sources were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/TaskFinal1/README.md'))
temp=registry.with_suffix('.task-final1.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
