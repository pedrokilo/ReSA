"""Registro historico de 44 funciones de CCarEnterExit (22) y CRunningScript (22) via el banco simple MiscWave2. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 51 entradas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-034057-598047'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-miscwave2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
PAIRS=summary['pairs_per_function'];TOTAL=19264;NFUN=44
assert sum(PAIRS.values())==TOTAL and len(PAIRS)==NFUN
assert sum(1 for n in PAIRS if n.startswith('CCarEnterExit::'))==22 and sum(1 for n in PAIRS if n.startswith('CRunningScript::'))==22
OUT_OF_BENCH=['CCarEnterExit::GetNearestCarDoor','CCarEnterExit::IsVehicleStealable','CCarEnterExit::MakeUndraggedDriverPedLeaveCar','CCarEnterExit::SetPedInCarDirect','CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle','CRunningScript::SetCharCoordinates','CRunningScript::GivePedScriptedTask-OG']
assert not any(n in PAIRS for n in OUT_OF_BENCH)
# Mapping: class + name against docs/hooks.csv (must be unique and still pending); virtual functions have address 0.
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in PAIRS:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
    assert (TYPE[name]=='V')==(ADDR[name]=='0x00000000'),name
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith('ff89df5b') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
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
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('begin')==1 and events.get('function_summary')==NFUN and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True and begin['functions']==NFUN
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-miscwave2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-miscwave2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), 19264 isolated CPU pairs for 44 functions of CCarEnterExit (22) and CRunningScript (22), zero differences, natural completion event with state_restored, saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake vehicle with a fake traced vtable [slots 31/33/35/37 IsDoorReady/FullyOpen/Closed/Missing U32, 47 GetRideAnimData], fake peds, matrix, fake model info in slot 400 with ColModel/CCollisionData/spheres, fake scripts and bytecode; the real script is never touched) and compares: return value, the whole memory of the tested objects, guards, the trace of virtual and collaborator calls with arguments, the x87 control word (24-bit precision) and the restored globals (ScriptParams, ScriptSpace[0..0xFFF], mission locals, CommandsExecuted, switch data, OnAMissionFlag, FailCurrentMission, clock, CHud::m_BigMessage, CWorld::Players, model info slot 400). '
 'Traced stubs, which get NO credit: CWorld::ProcessLineOfSight, CAnimManager::BlendAnimation, RpAnimBlendClumpGetFirstAssociation, RpAnimBlendGetNextAssociation, RpAnimBlendClumpGetAssociation(uint32), CPed::StopNonPartialAnims, CPed::RestartNonPartialAnims, CMessages::ClearSmallMessagesOnly and CCutsceneMgr::IsCutsceneSkipButtonBeingPressed. '
 'Limits: OUT of the bench and with NO credit: CCarEnterExit::GetNearestCarDoor, IsVehicleStealable, MakeUndraggedDriverPedLeaveCar, SetPedInCarDirect and SetAnimOffsetForEnterOrExitVehicle (real collaborators, pools and streaming) and CRunningScript::SetCharCoordinates and GivePedScriptedTask-OG (real world and task/event pools); the source of some of them was corrected but they are not benched. CRunningScript::ProcessOneCommand and Process run only NOP and WAIT (ScriptParams excluded from the comparison in those two); array indices only in variables 0..3; CollectParameters/StoreParameters with count >= 0; DoDeathArrestCheck only with stack depth >= 1; door ids include out-of-domain values; IsCarQuickJackPossible compares result and memory only (no virtual trace). Only the own part of each function is validated. '
 'Source corrected to what the EXE does before the approved run (uncommitted at the time; see reversing/MiscWave2/README.md for addresses): see source_corrections_before_run. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPEC_TXT={
 'CCarEnterExit::ComputeDoorFlag':'out-of-domain door ids return 0/-1/false as the EXE (no NOTSA_UNREACHABLE).',
 'CCarEnterExit::ComputeOppositeDoorFlag':'out-of-domain door ids return 0/-1/false as the EXE (no NOTSA_UNREACHABLE).',
 'CCarEnterExit::IsCarQuickJackPossible':'the EXE always returns 0 (xor al,al at 0x64EF5F) after virtual queries and CanPedOpenLocks; only result and memory compared.',
 'CCarEnterExit::IsClearToDriveAway':'source corrected to the EXE.',
 'CCarEnterExit::IsPathToDoorBlockedByVehicleCollisionModel':'source corrected: the result was inverted.',
 'CCarEnterExit::AddInCarAnim':'source corrected: SitInBoat case and TWO calls to virtual slot 47 (GetRideAnimData) when non-null (0x64F733/0x64F743 and 0x64F7D5/0x64F7E3); the ride trace appears duplicated.',
 'CCarEnterExit::QuitEnteringCar':'source corrected to the EXE.',
 'CRunningScript::Init':'source corrected: memcpy of 7 bytes.',
 'CRunningScript::ReadTextLabelFromScript':'source corrected: CRT strncpy; the whole 0x300-byte destination with guards is compared, including the zero fill.',
 'CRunningScript::DoDeathArrestCheck':'source corrected: IP = IPStack[0]; only stack depth >= 1 (with 0 the EXE reads m_IPStack[0xFFFF]).',
 'CRunningScript::Process':'source corrected: IP = IPStack[0]; only NOP and WAIT sequences (each ends in WAIT, also scene-skip targets and jump code); ScriptParams excluded.',
 'CRunningScript::ProcessOneCommand':'only NOP and WAIT; ScriptParams excluded; the bytes after the WAIT are NOP (the DLL has a NOTSA VERIFY on the highest vanilla command id that the EXE lacks).',
 'CRunningScript::GetPointerToScriptVariable':'source corrected: returns nullptr as the EXE.',
 'CRunningScript::GetPointerToLocalVariable':'source corrected: 32-bit handling.',
 'CRunningScript::GetPointerToLocalArrayElement':'source corrected: 32-bit handling; array indices only in variables 0..3.',
 'CRunningScript::ReadArrayInformation':'source corrected: 32-bit handling; array indices only in variables 0..3.',
 'CRunningScript::CollectParameters':'count >= 0 (the EXE treats the count as a 16-bit movzx).',
 'CRunningScript::StoreParameters':'count >= 0 (the EXE treats the count as a 16-bit movzx).'}
SPECIFIC={n:'Corpus %d; '%PAIRS[n]+SPEC_TXT.get(n,'compared against the EXE as is.') for n in PAIRS}
SOURCE_CHANGES={'source/game_sa/CarEnterExit.cpp':'AddInCarAnim (SitInBoat and two calls to virtual slot 47); QuitEnteringCar; IsClearToDriveAway; IsPathToDoorBlockedByVehicleCollisionModel (inverted result); ComputeDoorFlag/ComputeOppositeDoorFlag (no NOTSA_UNREACHABLE); GetNearestCarDoor, IsVehicleStealable, SetAnimOffsetForEnterOrExitVehicle (out of bench)',
 'source/game_sa/Scripts/RunningScript.cpp':'ReadTextLabelFromScript (CRT strncpy); Init (memcpy 7); DoDeathArrestCheck and Process (IP = IPStack[0]); GetPointerToScriptVariable (nullptr); GetPointerToLocalVariable/GetPointerToLocalArrayElement/ReadArrayInformation (32 bits)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=PAIRS,covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,begin_events=1,function_summaries=NFUN,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 out_of_bench_not_credited=OUT_OF_BENCH,
 readme_discrepancies='reversing/MiscWave2/README.md said "Sin compilar ni ejecutar todavia" (pre-run state at preparation). The earlier failed run (previous.jsonl) is not preserved. README approval section added at registration.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-misc-wave2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled MiscWave2 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 19264-row log reaudited offline and preserved gzipped with the original hash. CarEnterExit.cpp and RunningScript.cpp were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/MiscWave2/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.misc-wave2.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
