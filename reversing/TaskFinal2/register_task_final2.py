"""Registro historico de 7 funciones restantes de CTaskSimpleFightingControl (3) y CTaskComplexProstituteSolicit (4) via el banco simple TaskFinal2. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 7 entradas (dependency_review=null)."""
import gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-010844-400424'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-taskfinal2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
PAIRS={'CTaskSimpleFightingControl::CalcMoveCommand':1024,'CTaskSimpleFightingControl::Clone':128,'CTaskSimpleFightingControl::MakeAbortable':1024,'CTaskComplexProstituteSolicit::Clone':128,'CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute':256,'CTaskComplexProstituteSolicit::IsTaskValid':1024,'CTaskComplexProstituteSolicit::MakeAbortable':256}
# Direcciones/tipos tal como constan en docs/hooks.csv (Clone y MakeAbortable de Fighting son virtuales con direccion 0).
ADDR={'CTaskSimpleFightingControl::CalcMoveCommand':('0x00624B50','S'),'CTaskSimpleFightingControl::Clone':('0x00000000','V'),'CTaskSimpleFightingControl::MakeAbortable':('0x00000000','V'),
 'CTaskComplexProstituteSolicit::Clone':('0x006622F0','V'),'CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute':('0x00661D30','S'),'CTaskComplexProstituteSolicit::IsTaskValid':('0x00661BB0','S'),'CTaskComplexProstituteSolicit::MakeAbortable':('0x00661B80','V')}
TOTAL=3840;assert sum(PAIRS.values())==TOTAL
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['pairs_per_function']==PAIRS
assert summary['executed_probe_sha256'].startswith('49fc5a50') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
# Independent offline reaudit of the full log.
events={};per={};unequal=0;cases=set();complete=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True:unequal+=1
        elif ev=='complete':complete=row
        elif ev=='layout':assert row['ok'] is True
        elif ev=='function_summary':assert row['failures']==0
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('function_summary')==7 and events.get('begin')==1 and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True
# Build manifest and sources (must be the exact files the probe was built from).
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-taskfinal2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-taskfinal2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
previous=SRC/'previous.jsonl'
COMMON=('Simple pair bench (original with hook off vs candidate with hook on), 3840 isolated CPU pairs for 7 functions of CTaskSimpleFightingControl (3) and CTaskComplexProstituteSolicit (4) with a loaded save game, zero differences, natural completion event with state_restored, layout ok, saves unchanged, configs restored. '
 'Fixtures: peds, target entity, vehicles, intelligence, player data, fake tasks (traced vtable) and a fake event (traced vtable with priority and type) in private buffers with raw EXE offsets; x87 forced to 24 bits. '
 'Compared: return value, full memory of the tested object, new pool tasks (type + words without vptr), memory of the fake tasks, fingerprint of every buffer (references masked), reference lists before and after deletion, the full trace (order and number of virtual calls and rand calls), x87 control word and empty x87 stack (FpuTag); pool m_LastFreeSlot and the 0xCD fill of free slots, patched entries, vtable slots and the time step are checked at start and restored at the end. '
 'Traced stubs, patched at the EXE address and at the DLL body and acting only on the bank thread, so these helpers get NO credit: rand 0x821B1E, FindPlayerPed 0x56E210, CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes 0x5F3660, ComputeEntityHitSide 0x5F3AC0, IsPlayerOnAMission 0x464D50, IsCoopGameGoingOn 0x441390. '
 'Run for real: FindTaskByType, GetSimplestActiveTask, GetTaskFighting, GetVehicleAppearance, IsUpsideDown, IsOnItsSide, Normalise, the task pool (tasks created by Clone are found in the pool and deleted) and references. '
 'Limits: the ProstituteSolicit destructor runs with FindPlayerPed returning null when the pool tasks created by Clone are deleted (early exit), so that deletion only proves the pool ends clean; CTaskComplexProstituteSolicit::Clone with a null client is NOT tested (the EXE and DLL constructors dereference the client). GetTaskPool/IsFreeSlotAtIndex/GetAt and the 0x80 per-slot offsets are copied from TasksBatch4. '
 'Source corrected to what the EXE does before this run: CTaskSimpleFightingControl::CalcMoveCommand (7 differences: bearing subtracted from the rotation before wrapping with a single-turn wrap; strict threshold 0x8D2E60 (0.2617993950843811) instead of DegreesToRadians(15); the EXE rand is always consumed; the 0.8 threshold compares the magnitude; vehicle sum z^2+x^2+y^2 with strict >0.7; 64-bit _ftol2 for the time step; fpatan) and its vehicle branch now calls ComputeEntityBoundingBoxPlanes (0x5F3660) instead of the UncachedAll variant; CTaskSimpleFightingControl::MakeAbortable (priorities other than 0/1/2 fall to the end instead of NOTSA_UNREACHABLE, GetEventType repeated per comparison, dot product (y*ny+z*nz)+x*nx and threshold 0.707). CTaskComplexProstituteSolicit: no source changes. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 'CTaskSimpleFightingControl::CalcMoveCommand':'Corpus 1024 (EXE 0x624B50): return values -1,0,3,7,8,9,10 observed, angle boundaries (+y axis, exact subtraction), wraps, rand with controlled low bits, extreme time steps (5e8: 32-bit overflow, negative, 0) and vehicles with fixed planes (sum-order sensitive mode); source changes listed above.',
 'CTaskSimpleFightingControl::Clone':'Corpus 128 via vtable 0x86D700 slot 1 (0x622EB0); new pool task compared and deleted.',
 'CTaskSimpleFightingControl::MakeAbortable':'Corpus 1024 via vtable 0x86D700 slot 6 (0x61DD00): priorities out of range, null event, event types 0..6 and others, event priorities 21/22/23, normals on the 0.707 boundary; source changes listed above.',
 'CTaskComplexProstituteSolicit::Clone':'Corpus 128 via vtable 0x86FB88 slot 1 (0x6622F0); non-null client only; new pool task compared and deleted.',
 'CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute':'Corpus 256 (EXE 0x661D30); void return.',
 'CTaskComplexProstituteSolicit::IsTaskValid':'Corpus 1024 (EXE 0x661BB0): cases generated by failing stage (17 conditions in EXE order) plus a second random failure, with exact boundaries (squared distance 100, up.z = -0.9, right.z = +-0.8).',
 'CTaskComplexProstituteSolicit::MakeAbortable':'Corpus 256 via vtable 0x86FB88 slot 6 (0x661B80).'}
SOURCE_CHANGES={'source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp':'CalcMoveCommand and MakeAbortable corrected to the EXE (see scope); ComputeEntityBoundingBoxPlanes in the vehicle branch','source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp':'no source change'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=summary['pairs_per_function'],covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,function_summaries=7,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 other_files_in_source_run_dir={'previous.jsonl':sha(previous) if previous.exists() else None})
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-task-final2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions'])
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');addr,typ=ADDR[name];hook={'class':cls,'fn_name':fn,'address':addr,'type':typ}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled TaskFinal2 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 3840-row log reaudited offline and preserved gzipped with the original hash. TaskSimpleFightingControl.cpp was corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/TaskFinal2/README.md'))
# Relee justo antes de escribir: otro agente puede haber modificado el registro.
assert sha(registry)==before,'el registro cambio durante la preparacion; reintentar'
temp=registry.with_suffix('.task-final2.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions'])),indent=2),json.dumps(dict(registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
