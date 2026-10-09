"""Registro historico POR FUNCION de TRES funciones del banco simple RoadScriptWave3 (CRoadBlocks::Init, CRoadBlocks::GenerateRoadBlockPedsForCar y CRunningScript::GivePedScriptedTask-OG), corrida 20261009-104926-137526 con passed=false global SOLO por CRunningScript::SetCharCoordinates (109/1024), que se marca RECHAZADA y NO se registra. Re-audita en streaming."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-104926-137526'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-roadscriptwave3')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT={'CRoadBlocks::Init':128,'CRoadBlocks::GenerateRoadBlockPedsForCar':1024,'CRunningScript::GivePedScriptedTask-OG':1024,'CRunningScript::SetCharCoordinates':1024};TOTAL=3200
NEW=['CRoadBlocks::Init','CRoadBlocks::GenerateRoadBlockPedsForCar','CRunningScript::GivePedScriptedTask-OG']
REJECTED=['CRunningScript::SetCharCoordinates']
assert sum(EXPECT.values())==TOTAL
assert summary['passed'] is False and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['error']=='Funciones con diferencias: CRunningScript::SetCharCoordinates: 109/1024'
assert summary['executed_probe_sha256'].startswith('b66d7da2') and summary['original_exe_sha256'].startswith('72ae59e4')
events={};per={};bad={};fsum={};cases=set();complete=None;layout=None;err=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='layout':layout=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
assert complete is None and err is not None and set(err)=={'event','layout_ok','pairs','state_restored'} and err['pairs']==TOTAL and err['state_restored'] is True and err['layout_ok'] is True
assert layout['ok'] is True
assert events.get('comparison')==TOTAL and events.get('layout')==1 and events.get('error')==1 and events.get('function_summary')==4 and events.get('case_start')==TOTAL and len(cases)==TOTAL
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad=={'CRunningScript::SetCharCoordinates':109},bad
PAIRS={f:per[f] for f in NEW}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR=={'CRoadBlocks::Init':'0x00461100','CRoadBlocks::GenerateRoadBlockPedsForCar':'0x00461170','CRunningScript::GivePedScriptedTask-OG':'0x00465C20','CRunningScript::SetCharCoordinates':'0x00464DC0'}
log_sha=sha(SRC/'comparisons.jsonl')
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
SOURCE_CHANGED_AFTER_RUN={}
for p,h in manifest['sources'].items():
    if sha(ROOT/p)!=h:SOURCE_CHANGED_AFTER_RUN[p]=dict(tested_sha256=h,current_sha256=sha(ROOT/p))
assert set(SOURCE_CHANGED_AFTER_RUN)<={'source/game_sa/Scripts/RunningScript.cpp'},SOURCE_CHANGED_AFTER_RUN
# RunningScript.cpp was edited after the run by another agent (SetCharCoordinates work, rejected function). Its diff against HEAD must touch ONLY SetCharCoordinates (GivePedScriptedTask-OG text identical to HEAD).
if SOURCE_CHANGED_AFTER_RUN:
    d=subprocess.check_output(['git','diff','HEAD','-U0','--','source/game_sa/Scripts/RunningScript.cpp'],cwd=ROOT,text=True,encoding='utf-8')
    hunks=[l for l in d.splitlines() if l.startswith('@@')];print('RunningScript.cpp hunks vs HEAD:',hunks)
    import re
    lines=[int(re.match(r'@@ -(\d+)',h).group(1)) for h in hunks];assert lines and all(790<=x<=830 for x in lines),lines
    assert all(('SetCharCoordinates' in l or 'GivePedScriptedTask' not in l) for l in d.splitlines() if l.startswith(('+','-')))
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,('bank changed after run',name)
assert sha(HERE/'artifacts/gta_reversed-roadscriptwave3-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-roadscriptwave3-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes();assert hashlib.sha256(raw).hexdigest()==log_sha
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
COMMON=('PARTIAL, per-function approval. Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 3200 isolated CPU pairs for 4 functions of CRoadBlocks (2) and CRunningScript (2). '
 'Only CRoadBlocks::Init (128), CRoadBlocks::GenerateRoadBlockPedsForCar (1024) and CRunningScript::GivePedScriptedTask-OG (1024) are registered, with ALL pairs equal and the pair count equal to the bank table. The run as a whole has passed=false because CRunningScript::SetCharCoordinates (109/1024) differs: it is REJECTED, not registered and gets no credit. '
 'The run has no natural "complete" event: the bench writes "complete" only when passed, otherwise a terminal "error" event (pairs=3200, layout_ok=true, state_restored=true, no reason, i.e. the run finished normally); saves unchanged, configs restored; the "layout" event (bits and offsets of the DLL vs the EXE) is ok. '
 'Each case rebuilds the same deterministic fixture in both legs (fake buffers) and compares the hash of the fake buffers, the x87 control word (24 bits) and the full trace of collaborators with raw arguments; foreign threads ignored. '
 'Traced stubs and out-of-bench pieces get NO credit. Init: CFileMgr::OpenFile/Read/CloseFile are doubled with an in-memory file (counts 0, 1, 2, 17, 100, 300, 324, 325 and random); the open-failure branch is NOT tested (DLL aborts, EXE dereferences NULL). '
 'GenerateRoadBlockPedsForCar: only pedType COP or GANG1..10 (the other types read the gang table out of range), positions 0..2, 0..2 peds; doubled: CPed::operator new, CCopPed and CCivilianPed constructors, CTask::operator new, CTaskManager::SetTask, CEventGroup::Add, CEntity::GetColModel, GetDefaultCopCarModel, ChooseGangPedModel, CMatrix::SetRotate, CWorld::ProcessVerticalLine/Add, SetClumpAlpha, RegisterReference, GiveDelayedWeapon, SetCurrentWeapon (type/slot), FindPlayerPed, rand and CGeneral::GetRandomNumber; CTaskComplexWanderCop/StandStill/KillPedOnFoot constructors, CEventScriptCommand, CPedPlacement::FindZCoorForPed and GetWeaponInfo run for real; tasks compared without their vptr. CMatrix::SetRotate is doubled. '
 'GivePedScriptedTask-OG: fake script of 0x400 bytes, fake ped pool of 6 slots, fake tasks with traced vtable, fake group; doubled: GetPedsGroup, IsPlayer, CScriptedBrainTaskStore::SetTask, GetVacantSlot, CPedScriptedTaskRecordData::Set/SetAsGroupTask/SetAsAttractorScriptTask, CPedGroupIntelligence::SetScriptCommandTask/GetTaskScriptCommand, CTaskComplexSequence::AddTask, CEventGroup::Add; CEventScriptCommand real; handle -1, ped with script brain, external types 0/2/3/5/other, group/no group, player. '
 'The EXE calls FindPlayerVehicle twice (0x4612E2/0x4612F0) and discards the result; it is pure and is not reproduced. Only the own part of each function is validated. '
 'Source tested == current working tree at registration for RoadBlocks.cpp/.h and RunningScript.h; RunningScript.cpp was edited after the run only inside SetCharCoordinates (rejected function), see source_changed_after_run. Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 NEW[0]:'Corpus 128; MATCH, no source change (rep stos 0x51 dwords + 1 byte, OpenFile "rb", Read 4 and 0x514 bytes, CloseFile, IsActive loop 0xA43AD0..0xA43C90); only the NOTSA open-failure branch differs and is out of the domain; in-memory file doubles.',
 NEW[1]:'Corpus 1024; source corrected (EXE): 0x461592 SetCurrentWeapon(PISTOL) also for civilians and gangs (the DLL did it only for CITYCOP); 0x461637 CCopPed::m_bDontPursuit = true before allocating the cop task; 0x46168D..0x4616B4 no timer assignments (the EXE writes only the WanderCop fields that the constructor already zeroes); 0x46180B gang weapon with SetCurrentWeapon(slot) (GetWeaponInfo(weapon,STD)->m_nSlot) instead of the by-type overload; 0x4613F8 order GetColModel of the vehicle then GetDefaultCopCarModel; 0x4617C1 CGangs::Gang.data()[...] unchecked. Domain: pedType COP or GANG1..10 only.',
 NEW[2]:'Corpus 1024; MATCH, no source change (EXE 0x465C20: branches -1 / script brain / event / group with the same calls and order).'}
SRC_FILES={'CRoadBlocks':['source/game_sa/RoadBlocks.cpp','source/game_sa/RoadBlocks.h'],'CRunningScript':['source/game_sa/Scripts/RunningScript.cpp','source/game_sa/Scripts/RunningScript.h']}
SOURCE_CHANGES={'source/game_sa/RoadBlocks.cpp':'GenerateRoadBlockPedsForCar: SetCurrentWeapon(PISTOL) also for civilians and gangs (0x461592); CCopPed::m_bDontPursuit = true (0x461637); no timer assignments (0x46168D..0x4616B4); gang weapon with SetCurrentWeapon(slot) (0x46180B); GetColModel then GetDefaultCopCarModel order (0x4613F8); CGangs::Gang.data() unchecked (0x4617C1)',
 'source/game_sa/Scripts/RunningScript.cpp':'SetCharCoordinates only (NaN z calls FindGroundZForCoord, 0x464DE2; distance added only if offset, 0x464E80): belongs to the REJECTED function; GivePedScriptedTask-OG has no change'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the run; not registered, no credit (another agent is correcting the source)') for f in REJECTED}
out=dict(source_changed_after_run=SOURCE_CHANGED_AFTER_RUN,passed=False,partial_per_function_approval=True,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=3,functions=PAIRS,covered_functions={k:True for k in NEW},rejected_functions=rejected,scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=False,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=0,terminal_error_events=1,terminal_error_event=err,layout_events=1,layout_ok=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=4,functions_with_zero_failures=3,unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per),
 source_corrections=SOURCE_CHANGES,
 notes='CRunningScript::SetCharCoordinates (109/1024 differences) is REJECTED and gets no registry entry. The RunningScript.cpp hash registered for GivePedScriptedTask-OG is the TESTED one (the file was edited after the run by another agent; see source_changed_after_run; the diff against HEAD touches only the SetCharCoordinates hunks, so the GivePedScriptedTask-OG text is identical to HEAD, but the tested file itself was not preserved, so this is inferred, not byte-proven); later edits of SetCharCoordinates by other agents are not validated here. With the registration of Init and GenerateRoadBlockPedsForCar, CRoadBlocks has no pending entries in docs/hooks.csv; CRunningScript keeps SetCharCoordinates pending.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-roadscript-wave3.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name in NEW:
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources={p:manifest['sources'][p] for p in SRC_FILES[cls]},
     source_provenance='Exact compiled RoadScriptWave3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at registration; full 3200-row log reaudited offline in streaming and preserved gzipped with the original hash. PARTIAL approval: per function, only functions with all pairs equal.',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/RoadScriptWave3/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.roadscript-wave3.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
