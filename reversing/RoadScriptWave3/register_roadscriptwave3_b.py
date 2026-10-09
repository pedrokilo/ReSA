"""Registro historico de UNA funcion mas del banco simple RoadScriptWave3 (CRunningScript::SetCharCoordinates, 0x464DC0), corrida 20261009-110441-788318 con passed=true global (3200 pares, 4 funciones con 0 fallos, evento complete). No duplica las 3 ya registradas por validation-summary.json (corrida 20261009-104926-137526). Re-audita en streaming. CRunningScript queda completa."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-110441-788318'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-roadscriptwave3')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT={'CRoadBlocks::Init':128,'CRoadBlocks::GenerateRoadBlockPedsForCar':1024,'CRunningScript::GivePedScriptedTask-OG':1024,'CRunningScript::SetCharCoordinates':1024};TOTAL=3200
NEW=['CRunningScript::SetCharCoordinates'];ALREADY=['CRoadBlocks::Init','CRoadBlocks::GenerateRoadBlockPedsForCar','CRunningScript::GivePedScriptedTask-OG']
assert sum(EXPECT.values())==TOTAL
assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True and summary['pairs']==TOTAL and summary['pairs_per_function']==EXPECT
assert summary['executed_probe_sha256'].startswith('e80681e2') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};bad={};fsum={};cases=set();complete=None;layout=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='layout':layout=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('layout')==1 and events.get('function_summary')==4 and events.get('case_start')==TOTAL and len(cases)==TOTAL and not bad and events.get('error',0)==0
assert complete['pairs']==TOTAL and complete['state_restored'] is True and complete['layout_ok'] is True and layout['ok'] is True
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==0,f
PAIRS={f:per[f] for f in NEW};assert PAIRS=={NEW[0]:1024}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR[NEW[0]]=='0x00464DC0'
for name in ALREADY:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn];assert len(m)==1 and m[0]['validated']=='1',name
assert [r['fn_name'] for r in rows if r['class']=='CRunningScript' and r['validated']=='0']==['SetCharCoordinates']
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,('source changed after run',p)
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name in ('LiveComparison.hpp','build_probe.py','run_local.py'):assert sha(HERE/name)==manifest['instrumentation'][name],('bank changed after run',name)
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
COMMON=('Second increment of RoadScriptWave3 (the first 3 functions are registered by validation-summary.json, run 20261009-104926-137526, and are not duplicated). Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 3200 isolated CPU pairs for 4 functions of CRoadBlocks (2) and CRunningScript (2); this run has passed=true globally, the 4 functions have 0 failures, natural "complete" event with state_restored and layout_ok, layout event ok, saves unchanged, configs restored. '
 'Only CRunningScript::SetCharCoordinates (0x464DC0, 1024 pairs, all equal, pair count equal to the bank table) is registered here. '
 'Each case rebuilds the same deterministic fixture in both legs (fake script, fake ped/vehicle buffers) and compares the hash of the fake buffers, the x87 control word (24 bits) and the full trace of collaborators with raw arguments; foreign threads ignored. '
 'Traced stubs and out-of-bench pieces get NO credit. CMatrix::SetRotate is doubled. Only the own part of the function is validated. Source tested == current working tree at registration (RunningScript.cpp/.h match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={NEW[0]:'Corpus 1024; source corrected (EXE): 0x464DE2..0x464DED fcomp -100.0 / test ah,41h / jp: with z NaN FindGroundZForCoord is NOT called (condition posn.z <= MAP_Z_LOW_LIMIT; a previous agent had it reversed); 0x464E80 adds the distance only if offset. CMatrix::SetRotate doubled. The previous run (20261009-104926-137526) rejected this function with 109/1024 differences; it passes with the corrected source.'}
SOURCE_CHANGES={'source/game_sa/Scripts/RunningScript.cpp':'SetCharCoordinates: z NaN does not call FindGroundZForCoord (0x464DE2..0x464DED, fcomp -100.0 / test ah,41h / jp, condition posn.z <= MAP_Z_LOW_LIMIT); distance added only if offset (0x464E80)'}
SRCS=['source/game_sa/Scripts/RunningScript.cpp','source/game_sa/Scripts/RunningScript.h']
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources={k:manifest['instrumentation'][k] for k in ('LiveComparison.hpp','build_probe.py','run_local.py')},uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,increment=2,previous_summary='reversing/RoadScriptWave3/validation-summary.json',pairs=TOTAL,differences=0,registered_pairs=sum(PAIRS.values()),registered_functions=1,functions=PAIRS,covered_functions={k:True for k in NEW},already_registered_by_previous_summary=ALREADY,scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,layout_events=1,layout_ok=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=4,functions_with_zero_failures=4,unequal_pairs=0,distinct_cases=len(cases),per_function=per),
 source_corrections=SOURCE_CHANGES,
 notes='With this function CRunningScript has no pending entries in docs/hooks.csv (the other CRunningScript rows were registered earlier, GivePedScriptedTask-OG by validation-summary.json). The 3 functions already registered are not duplicated.')
sp=HERE/'validation-summary-b.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-roadscript-wave3-b.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name in NEW:
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources={p:manifest['sources'][p] for p in SRCS},
     source_provenance='Exact compiled RoadScriptWave3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at registration; full 3200-row log reaudited offline in streaming and preserved gzipped with the original hash. Second increment: only the function not registered by the first summary.',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/RoadScriptWave3/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.roadscript-wave3-b.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
