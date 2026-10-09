"""Registro historico de DOS funciones mas del banco GfxWave2 (CWaterLevel::RenderFlatWaterRectangle y SplitWaterTriangleAlongXLine), corrida 20261009-100807-276064. Re-audita en streaming; AddWaterLevelQuad sigue rechazada. No toca las 52 anteriores."""
import csv,gzip,hashlib,json,os,re,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-100807-276064'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-gfxwave2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
HPP=(HERE/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECT={}
for m in re.finditer(r'^//\s+\{"(\w+)","([\w\-]+)",0x[0-9A-Fa-f]+,\d+,-?\d+,(\d+)\},',HPP,re.M):EXPECT[m.group(1)+'::'+m.group(2)]=int(m.group(3))
assert len(EXPECT)==55 and sum(EXPECT.values())==27616
NEW=['CWaterLevel::RenderFlatWaterRectangle','CWaterLevel::SplitWaterTriangleAlongXLine']
REJECTED=['CWaterLevel::AddWaterLevelQuad']
assert summary['passed'] is False and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['error']=='Funciones con diferencias: CWaterLevel::AddWaterLevelQuad: 254/1024'
assert summary['executed_probe_sha256'].startswith('69537e60') and summary['original_exe_sha256'].startswith('72ae59e4')
events={};per={};bad={};fsum={};cases=set();complete=None;begin=None;err=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            f=row['function'];per[f]=per.get(f,0)+1;cases.add((f,row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='begin':begin=row
        else:assert ev=='waiting_for_player',ev
print('events',events)
assert complete is None and err is not None and set(err)=={'event','pairs','state_restored'} and err['pairs']==27616 and err['state_restored'] is True
assert events.get('comparison')==27616 and events.get('begin')==1 and events.get('error')==1 and events.get('function_summary')==55 and len(cases)==27616
assert begin['known'] is True and all(x['ok'] is True for x in begin['sites'])
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad=={'CWaterLevel::AddWaterLevelQuad':254},bad
assert sum(1 for f in EXPECT if fsum[f]['failures']==0)==54
PAIRS={f:per[f] for f in NEW};assert PAIRS=={NEW[0]:1024,NEW[1]:512}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
log_sha=sha(SRC/'comparisons.jsonl')
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,('source changed after run',p)
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
BANK_CHANGED_AFTER_RUN={}
for name in ('ClothesPart.hpp','GfxCommon.hpp','LiveComparison.hpp','ShadowsPart.hpp','WaterPart.hpp'):
    if sha(HERE/name)!=manifest['instrumentation'][name]:BANK_CHANGED_AFTER_RUN[name]=dict(tested_sha256=manifest['instrumentation'][name],current_sha256=sha(HERE/name))
assert set(BANK_CHANGED_AFTER_RUN)<={'WaterPart.hpp'},BANK_CHANGED_AFTER_RUN
print('bank changed after run',BANK_CHANGED_AFTER_RUN)
assert sha(HERE/'artifacts/gta_reversed-gfxwave2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-gfxwave2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=('PARTIAL, per-function approval (second increment of GfxWave2; the first 52 functions are registered by validation-summary.json, run 20261009-095105-342457). Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), 27616 isolated CPU pairs for 55 functions of CClothesBuilder (14), CShadows (19) and CWaterLevel (22). '
 'Only CWaterLevel::RenderFlatWaterRectangle (1024) and CWaterLevel::SplitWaterTriangleAlongXLine (512) are registered here, with ALL pairs equal and the pair count equal to the bank table; the other 52 functions also have all pairs equal in this run but are registered by the earlier summary and are not duplicated. The run as a whole has passed=false because CWaterLevel::AddWaterLevelQuad (254/1024) differs: it is REJECTED, not registered and gets no credit. '
 'The run has no natural "complete" event: the bench writes "complete" only when passed, otherwise a terminal "error" event (pairs=27616, state_restored=true, no reason, i.e. the run finished normally); saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake objects; drawing replaced by traces of RwIm3D*/RwRaster*/etc. with raw arguments, nothing is drawn) and compares return value, collaborator traces, the bytes of the real fields of the fixtures (padding masked), and the restored globals; x87 at 24 bits, no signalling NaN, foreign threads ignored (Mine()). '
 'Traced stubs and out-of-bench pieces get NO credit (e.g. RwIm3D*, LittleTest, GetWaterLevelNoWaves, FindPlayerCoors, CanSeeWaterFromCurrArea, HandleBeachToysStuff and, per case, sibling CWaterLevel drawing functions). Masked/controlled noise: CWaterVertex padding, CRenPar 2-byte padding, structure padding. '
 'Only the own part of each function is validated. Source tested == current working tree at registration (all six class sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={
 NEW[0]:'Corpus 1024; source corrected (EXE 0x6EBEC0): in the lower half of the Y split the EXE (0x6EC3F1) passes P2 as the third CRenPar, (P13,P24,P2,P4), which looks like an EXE bug and is replicated; CRenPar lerp of the EXE 0x6E5280 = (1-t)*a + t*b; halves are painted recursively by this function with t=0.5f. Sibling drawing functions are traced/isolated and get no credit.',
 NEW[1]:'Corpus 512; source corrected (EXE 0x6ECF00): X1==X3 branch (0x6ECF5D..0x6ED2DB) interpolates from P3 with B=lerp(P3,P1,t), H=lerp(P3,P2,t) and calls Bottom(B,H,P3), rect(P1,P12,B,H), Right(P12,P2,H); X2==X3 branch calls Bottom (0x6ED094), then the rectangle (0x6ED5D9) with (P12,P2,P13,P23) and finally Left (0x6ED6B8). Sibling drawing functions are traced/isolated and get no credit.'}
SRCS=['source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h']
SOURCE_CHANGES={'source/game_sa/WaterLevel.cpp (+ .h)':'RenderFlatWaterRectangle (P2 as third CRenPar in the lower Y half as the EXE; lerp(CRenPar) = (1-t)*a + t*b) and SplitWaterTriangleAlongXLine (X1==X3 branch from P3 with B,H lerps; X2==X3 call order Bottom, rect(P12,P2,P13,P23), Left)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=27616,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the approved run; not registered, no credit') for f in REJECTED}
out=dict(bank_changed_after_run=BANK_CHANGED_AFTER_RUN,passed=False,partial_per_function_approval=True,increment=2,previous_summary='reversing/GfxWave2/validation-summary.json',pairs=27616,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=2,functions=PAIRS,covered_functions={k:True for k in NEW},rejected_functions=rejected,scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=False,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=27616,complete_events=0,terminal_error_events=1,terminal_error_event=err,begin_events=1,waiting_for_player_events=events.get('waiting_for_player',0),function_summaries=55,functions_with_zero_failures=54,unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per),
 source_corrections=SOURCE_CHANGES,
 notes='Previous run 20261009-095105-342457 rejected these two functions (472/1024 and 258/512); both pass in this run after the source corrections above. The bank files in this manifest are the ones that ran; WaterPart.hpp was edited in the working tree after the run by another agent (AddWaterLevelQuad work), see bank_changed_after_run: the registry keeps the TESTED hash (the later edit is not validated here). The 19 earlier CWaterLevel registry entries keep the hash of WaterLevel.cpp tested in the earlier run.')
sp=HERE/'validation-summary-b.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-gfx-wave2-b.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name in NEW:
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources={p:manifest['sources'][p] for p in SRCS},
     source_provenance='Exact compiled GfxWave2 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at registration; full 27616-row log reaudited offline in streaming and preserved gzipped with the original hash. PARTIAL approval, second increment: per function, only functions with all pairs equal.',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/GfxWave2/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.gfx-wave2-b.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
