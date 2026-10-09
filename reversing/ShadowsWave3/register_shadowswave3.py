"""Registro historico POR FUNCION de UNA funcion del banco simple ShadowsWave3 (CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>, 0x70A470), corrida 20261009-105057-302560 con passed=false global SOLO por CShadows::RenderStoredShadows (318/512), que se marca RECHAZADA y NO se registra. Re-audita en streaming."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-105057-302560'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-shadowswave3')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
CAST='CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>'
EXPECT={'CShadows::RenderStoredShadows':512,CAST:1024};TOTAL=1536
NEW=[CAST];REJECTED=['CShadows::RenderStoredShadows']
assert sum(EXPECT.values())==TOTAL
assert summary['passed'] is False and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['error']=='Funciones con diferencias: CShadows::RenderStoredShadows: 318/512'
assert summary['executed_probe_sha256'].startswith('c2225e9c') and summary['original_exe_sha256'].startswith('72ae59e4')
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
assert complete is None and err is not None and set(err)=={'event','pairs','state_restored'} and err['pairs']==TOTAL and err['state_restored'] is True
assert events.get('comparison')==TOTAL and events.get('begin')==1 and events.get('error')==1 and events.get('function_summary')==2 and len(cases)==TOTAL
assert begin['known'] is True and all(x['ok'] is True for x in begin['sites']) and len(begin['sites'])==9
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad=={'CShadows::RenderStoredShadows':318},bad
PAIRS={f:per[f] for f in NEW};assert PAIRS=={CAST:1024}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR[CAST]=='0x0070A470' and ADDR[REJECTED[0]]=='0x0070A960'
log_sha=sha(SRC/'comparisons.jsonl')
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,('source changed after run',p)
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,('bank changed after run',name)
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
COMMON=('PARTIAL, per-function approval. Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 1536 isolated CPU pairs for 2 functions of CShadows. '
 'Only CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> (0x70A470, 1024 pairs) is registered, with ALL pairs equal and the pair count equal to the bank table. The run as a whole has passed=false because CShadows::RenderStoredShadows (318/512) differs: it is REJECTED, not registered and gets no credit. '
 'The run has no natural "complete" event: the bench writes "complete" only when passed, otherwise a terminal "error" event (pairs=1536, state_restored=true, no reason, i.e. the run finished normally); saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs and compares the collaborator trace with raw arguments (nineteen arguments of CastShadowEntityXY), the hash of the CShadows state block (0xC403A8-0xC4B6B0), the RenderBuffer temporary, the scan code, the x87 control word (24 bits) and that the hook switches (JumpsToDll); foreign threads ignored. '
 'Fixtures: 1-6 fake entities with a fake vtable (slot 9 = GetBoundRect, the rest trap), fake models 19900..19907 in ms_modelInfoPtrs (only m_pColModel and its min/max box), own matrix per entity (at.z in {1, 0.97f, 0.97f+-ulp, 0.98, 0.5, -1, 0, NaN}), bUsesCollision/bDontCastShadowsOn flags, scan code equal or different from the current one, area equal/13/other (CGame::currArea), rectangles on an integer grid with exact equalities against the caller rectangle (and NaN), z of the box and zDistance that provoke equalities; list of 0-8 nodes (with repeated items) built in a LOCAL NODE POOL that temporarily replaces POOLS[0] (0xB74484). '
 'Limits: the branch with a null matrix (0x70A4F2, CPlaceable::GetMatrix -> AllocateMatrix, touches the real matrix pool) and scan code 0xFFFF (ResetScanCodes walks the real world) are OUT of the bench; fake entities with a fake vtable and a local node pool replace POOLS[0]; CastShadowEntityXY (0x7086B0) is doubled with a trace and gets NO credit; notsa::PoolOf<CPhysical> does not exist so IsItemValid consults no entity pool. '
 'Only the own part of the function is validated. Source tested == current working tree at registration (both CShadows sources match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation.')
SPECIFIC={CAST:'Corpus 1024; source corrected (EXE 0x70A470): template instance (CSector::Buildings is CPtrListSingleLink<CBuilding*>; the EXE calls the CPtrListSingleLink<CPhysical*> instance, so the callers use reinterpret_cast to <CPhysical*>); 0x70A57F movsx edx,word [esi+22h] (model index with sign, int32(int16(...)) instead of uint16). MATCH verified line by line without change: loop flags and order (0x70A4A0-0x70A4E9: scan code, +1Ch bit 0, +1Eh bit 0, area 0x0D), GetMatrix (0x411990), fcomp [0x872724] + test ah,41h (up.z > 0.97), rectangle comparisons on CRect +8/+0/+4/+0xC, collision box (+8 min z, +14h max z, fcompp), 19 arguments to 0x7086B0.'}
SRCS=['source/game_sa/Shadows.cpp','source/game_sa/Shadows.h']
SOURCE_CHANGES={'source/game_sa/Shadows.cpp (+ .h)':'CastPlayerShadowSectorList: template instance (reinterpret_cast to CPtrListSingleLink<CPhysical*> in the callers) and signed model index (0x70A57F movsx). The same file also holds the RenderStoredShadows corrections (floor/ceil with _ftol2 truncation, water UV, batch render-mode gate), which belong to the REJECTED function and get no credit.'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the run; not registered, no credit (another agent is correcting the source)') for f in REJECTED}
out=dict(passed=False,partial_per_function_approval=True,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=1,functions=PAIRS,covered_functions={k:True for k in NEW},rejected_functions=rejected,scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=False,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=0,terminal_error_events=1,terminal_error_event=err,begin_events=1,waiting_for_player_events=events.get('waiting_for_player',0),function_summaries=2,functions_with_zero_failures=1,unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per),
 source_corrections=SOURCE_CHANGES,
 notes='CShadows::RenderStoredShadows (318/512 differences) is REJECTED and gets no registry entry. Shadows.cpp tested hash is registered; later edits by other agents on RenderStoredShadows are not validated here. CShadows keeps RenderStoredShadows pending in docs/hooks.csv.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-shadows-wave3.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name in NEW:
    cls,fn=name.split('::',1);hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources={p:manifest['sources'][p] for p in SRCS},
     source_provenance='Exact compiled ShadowsWave3 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at registration; full 1536-row log reaudited offline in streaming and preserved gzipped with the original hash. PARTIAL approval: per function, only functions with all pairs equal.',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/ShadowsWave3/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.shadows-wave3.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp)),indent=2))
