"""Registro historico de UNA funcion mas del banco simple ShadowsWave3 (CShadows::RenderStoredShadows, 0x70A960), corrida 20261009-110523-344796 con passed=true global (1536 pares, 2 funciones con 0 fallos, evento complete). No duplica CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> (ya registrada por validation-summary.json, corrida 20261009-105057-302560). Re-audita en streaming. CShadows queda completa."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-110523-344796'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-shadowswave3')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
CAST='CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>'
RSS='CShadows::RenderStoredShadows'
EXPECT={RSS:512,CAST:1024};TOTAL=1536
NEW=[RSS];ALREADY=[CAST]
assert sum(EXPECT.values())==TOTAL
assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True and summary['pairs']==TOTAL and summary['pairs_per_function']==EXPECT
assert summary['executed_probe_sha256'].startswith('4d944973') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};bad={};fsum={};cases=set();complete=None;begin=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='begin':begin=row
        else:assert ev=='waiting_for_player',ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('begin')==1 and events.get('function_summary')==2 and len(cases)==TOTAL and not bad and events.get('error',0)==0
assert complete['pairs']==TOTAL and complete['state_restored'] is True
assert begin['known'] is True and all(x['ok'] is True for x in begin['sites']) and len(begin['sites'])==9
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==0,f
PAIRS={f:per[f] for f in NEW};assert PAIRS=={RSS:512}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR[RSS]=='0x0070A960'
for name in ALREADY:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn];assert len(m)==1 and m[0]['validated']=='1',name
assert [r['fn_name'] for r in rows if r['class']=='CShadows' and r['validated']=='0']==['RenderStoredShadows']
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,('source changed after run',p)
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
BANK=('LiveComparison.hpp','SwCommon.hpp','build_probe.py','run_local.py')
for name in BANK:assert sha(HERE/name)==manifest['instrumentation'][name],('bank changed after run',name)
assert sha(HERE/'artifacts/gta_reversed-shadowswave3-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-shadowswave3-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('Second increment of ShadowsWave3 (CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> is registered by validation-summary.json, run 20261009-105057-302560, and is not duplicated). Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 1536 isolated CPU pairs for 2 functions of CShadows; this run has passed=true globally, both functions have 0 failures, natural "complete" event with state_restored, saves unchanged, configs restored. '
 'Only CShadows::RenderStoredShadows (0x70A960, 512 pairs, all equal, pair count equal to the bank table) is registered here. '
 'Each case rebuilds the same deterministic fixture in both legs and compares the collaborator trace with raw arguments, the hash of the CShadows state block, the x87 control word (24 bits) and that the hook switches; foreign threads ignored. '
 'Limits: RwIm3D* drawing functions and the sector lists are doubled with traces (WITHOUT credit); there are no real sector lists. Only the own part of the function is validated. Source tested == current working tree at registration (Shadows.cpp/.h match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={RSS:'Corpus 512; source corrected (EXE): 0x70AA16 test al,2 / jne 0x70AE5B (skips the texture setup when already drawn in the batch); water UV 0x70AFD5 and 0x70B185-0x70B214 with precomputed 1/|v|^2, (-pos.z)*0.0f and V=0.5-dot*inv*0.5; truncations with floor/ceil + _ftol2 and ShadowSectorIndex (0x70AEE5, 0x70AF03, 0x70AF23, 0x70AF4B, 0x70AAFD-0x70ABF3); ProcessOneVertex advances vtxIt (the 4 vertices of each cell); CastRealTimeShadowSectorList receives a non-null pointer to a stack byte as pDayNight (0x70ACB9, 0x70AD0D); GetWaterLevelNoWaves receives the position of the last shadow of the batch (batchPos, 0x70AA89-0x70AAB4 / 0x70AE6A-0x70AE7C). The previous run (20261009-105057-302560) rejected this function with 318/512 differences; it passes with the corrected source.'}
SOURCE_CHANGES={'source/game_sa/Shadows.cpp':'RenderStoredShadows: batch render-mode gate test al,2 / jne (0x70AA16); water UV with precomputed 1/|v|^2 (0x70AFD5, 0x70B185-0x70B214); floor/ceil+_ftol2 truncations and ShadowSectorIndex (0x70AEE5, 0x70AF03, 0x70AF23, 0x70AF4B, 0x70AAFD-0x70ABF3); ProcessOneVertex advances vtxIt; non-null stack byte as pDayNight (0x70ACB9, 0x70AD0D); GetWaterLevelNoWaves with batchPos (0x70AA89-0x70AAB4, 0x70AE6A-0x70AE7C)'}
SRCS=['source/game_sa/Shadows.cpp','source/game_sa/Shadows.h']
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources={k:manifest['instrumentation'][k] for k in BANK},uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,increment=2,previous_summary='reversing/ShadowsWave3/validation-summary.json',pairs=TOTAL,differences=0,registered_pairs=sum(PAIRS.values()),registered_functions=1,functions=PAIRS,covered_functions={k:True for k in NEW},already_registered_by_previous_summary=ALREADY,scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,begin_events=1,waiting_for_player_events=events.get('waiting_for_player',0),function_summaries=2,functions_with_zero_failures=2,unequal_pairs=0,distinct_cases=len(cases),per_function=per),
 source_corrections=SOURCE_CHANGES,
 notes='With this function CShadows has no pending entries in docs/hooks.csv. CastPlayerShadowSectorList (a template instance row) is already registered and is not duplicated.')
sp=HERE/'validation-summary-b.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-shadows-wave3-b.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name in NEW:
    cls,fn=name.split('::',1);hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources={p:manifest['sources'][p] for p in SRCS},
     source_provenance='Exact compiled ShadowsWave3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at registration; full 1536-row log reaudited offline in streaming and preserved gzipped with the original hash. Second increment: only the function not registered by the first summary.',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/ShadowsWave3/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.shadows-wave3-b.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
