"""Registro historico de CBike (4 funciones) del banco simple BikeWave4 (corrida 20261009-124045-743237). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'BikeWave4',
 'run_id': '20261009-124045-743237',
 'probe_prefix': '0a37a16e',
 'tag': 'bike-wave4',
 'registry_before': 7179,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CBike::Constructor': 512, 'CBike::Destructor': 256, 'CBike::DamageKnockOffRider': 1024, 'CBike::ProcessEntityCollision': 1024},
 'total': 2816,
 'new': ['CBike::Constructor', 'CBike::Destructor', 'CBike::DamageKnockOffRider', 'CBike::ProcessEntityCollision'],
 'rejected': [],
 'addr': {'CBike::Constructor': '0x006BF430', 'CBike::Destructor': '0x006B57A0', 'CBike::DamageKnockOffRider': '0x006B5A10', 'CBike::ProcessEntityCollision': '0x00000000'},
 'types': {'CBike::Constructor': 'S', 'CBike::Destructor': 'S', 'CBike::DamageKnockOffRider': 'S', 'CBike::ProcessEntityCollision': 'V'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled BikeWave4 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at '
               'registration; full 2816-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 2816 isolated CPU pairs for the 4 '
           'pending functions of CBike, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event with state_restored and layout_ok, '
           'layout event ok, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture in both legs and compares the resulting state, the x87 control '
           'word (24 bits) and the whole trace of doubled collaborators with raw arguments; foreign threads ignored. Limits: no NaN in matrices or velocity, handlingId 0..209, '
           'entities with non-null matrix and collision; CEventKnockOffBike and CEventGroup::Add are fakes (traced). The bank EvId lookup was fixed to search from the last to the '
           'first match (the DLL reuses the stack slot for the driver/passenger events) and it keeps up to 8 failures with raw trace. Only the own part of each function is '
           'validated. Source tested == current working tree at registration (Bike.cpp and Bike.h match the probe build manifest). Historical evidence approved with explicit '
           'limits; no current dependency accreditation.',
 'specific': {'CBike::Constructor': 'Corpus 512; source corrected (EXE): handling index uses only the low byte (0x6BF520 movzx eax,dl); m_fSteerAngleTan via x87 fptan on the '
                                    'float product (0x6BF59E..0x6BF5BD); wheel loops write only the fields the EXE writes (suspension heights/orig heights, col points, suspension '
                                    'and line lengths are not initialised; ratio history = 1.0, 0x6BF6E0..0x6BF714); lines[1].m_vecStart.z = 99999.99 (0x6BF75D, not .x); and '
                                    'al,0F0h also clears bIsBus (0x6BF79A); two dword stores at +0x60C/+0x610 (0x6BF468/0x6BF46E).',
              'CBike::Destructor': 'Corpus 256; MATCH with no source change; the comparison masks m_pAttachMatrix (the DLL CMatrix::~CMatrix nulls it, the EXE one at 0x59ACD0 '
                                   'does not) (EXE 0x6B57A0).',
              'CBike::DamageKnockOffRider': 'Corpus 1024; source corrected (EXE): task type is TASK_SIMPLE_GANG_DRIVEBY (cmp eax,3FEh at 0x6B5B03; '
                                            'CTaskSimpleGangDriveBy::GetTaskType 3FEh at 0x6218B0, TASK_SIMPLE_BE_HIT is 3F0h at 0x620800), then pedType == cop.',
              'CBike::ProcessEntityCollision': 'Corpus 1024 (virtual, vtable 0x871360 slot 22 = 0x6BDEA0); source corrected (EXE): driver matrix = vehicle matrix copy + driver '
                                               'seat dummy rotated with Multiply3x3 (0x59C790, no translation; dummy 0 for boats else 4) (0x6BDFF8..0x6BE08C); static 32-element '
                                               'ped colpoint array; only with result > 0 (0x6BE0D9); loop exits at numColPts >= 31, skips piece type 0, copies with the EXE '
                                               'CColPoint operator= (0x40FC80, 0x6BE110..0x6BE144); wheel ratio test with NaN semantics (0x6BE190..0x6BE1B6); SetEntityReference '
                                               'without clearing the old reference (0x6BE1DC..0x6BE1E1).'},
 'source_changes': {'source/game_sa/Entity/Vehicle/Bike.cpp': 'Constructor: handling low byte, fptan tangent, wheel array init as the EXE, lines[1].m_vecStart.z, bIsBus cleared, '
                                                              'dword stores at +0x60C/+0x610; DamageKnockOffRider: TASK_SIMPLE_GANG_DRIVEBY (0x6B5B03); ProcessEntityCollision: '
                                                              'Multiply3x3 seat, static colpoint array, >0, 31 limit, EXE CColPoint copy, NaN ratio test, SetEntityReference'},
 'notes': 'With these four functions CBike has no pending entries in docs/hooks.csv.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-124045-743237`: passed=true, 2816 pares, 4 funciones con 0 fallos (Constructor 512, Destructor 256, DamageKnockOffRider 1024 y '
                   'ProcessEntityCollision 1024), evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_bikewave4.py` (evidencia '
                   'en `runs/`, `validation-summary.json`). Con esto CBike queda COMPLETA. Correcciones de fuente: Constructor (byte bajo del handling, fptan, inicialización de '
                   'ruedas como el EXE, lines[1].z, bIsBus, stores dword +0x60C/+0x610), DamageKnockOffRider (TASK_SIMPLE_GANG_DRIVEBY, cmp eax,3FEh 0x6B5B03; GetTaskType 3FEh en '
                   '0x6218B0 y BE_HIT 3F0h en 0x620800) y ProcessEntityCollision (asiento por Multiply3x3, array estático de colpoints, límite 31, CColPoint::operator= 0x40FC80, '
                   'test de ratio con NaN, SetEntityReference). Límites: sin NaN en matriz ni velocidad, handlingId 0..209, entidades con matriz y colisión no nulas; eventos '
                   'CEventKnockOffBike y CEventGroup::Add falsos; el banco corrigió su EvId (búsqueda del último al primero) y guarda hasta 8 fallos con traza cruda.'}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)==set(EXPECT) and not set(NEW)&set(REJECTED)
assert summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith(CFG['probe_prefix']) and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl')
if PASSED:
    assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True and summary['pairs']==TOTAL and summary['pairs_per_function']==EXPECT and log_sha==summary['log_sha256']
else:
    assert summary['passed'] is False and summary['error']==CFG['error'] and 'pairs' not in summary
events={};per={};bad={};fsum={};cases=set();complete=None;layout=None;err=None;begin=None
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
        elif ev=='begin':begin=row
        else:assert ev in('waiting_for_player','case_start'),ev
print('events',events)
term=complete if PASSED else err
if PASSED:
    assert err is None and complete is not None and complete['pairs']==TOTAL and complete['state_restored'] is True and complete.get('layout_ok',True) is True
else:
    assert complete is None and err is not None and set(err)<={'event','layout_ok','pairs','state_restored'} and err['pairs']==TOTAL and err['state_restored'] is True and err.get('layout_ok') is True
assert layout is not None and layout.get('ok') is True and events.get('layout')==1
if begin is not None and isinstance(begin.get('sites'),list):assert all(x.get('ok') is True for x in begin['sites'])
assert events.get('comparison')==TOTAL and events.get('function_summary')==len(EXPECT) and len(cases)==TOTAL
assert events.get('case_start',TOTAL)==TOTAL and events.get('complete',0)+events.get('error',0)==1
assert per==EXPECT and set(fsum)==set(EXPECT)
for f,r in fsum.items():assert r['cases']==EXPECT[f] and r['failures']==bad.get(f,0),f
assert bad==CFG['bad'],bad
for f in NEW:assert f not in bad
PAIRS={f:per[f] for f in NEW}
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in NEW+REJECTED:
    cls,fn=name.split('::',1);m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
assert ADDR==CFG['addr'],ADDR
for name,t in CFG['types'].items():assert TYPE[name]==t,(name,TYPE[name])
CLS=sorted({n.split('::',1)[0] for n in NEW})
pend_after=[r['class']+'::'+r['fn_name'] for r in rows if r['class'] in CLS and r['validated']=='0' and (r['class']+'::'+r['fn_name']) not in NEW]
assert sorted(pend_after)==sorted(CFG['pending_after']),pend_after
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
CHANGED_SRC={};CHANGED_BANK={}
for p,h in manifest['sources'].items():
    if sha(ROOT/p)!=h:CHANGED_SRC[p]=dict(tested_sha256=h,current_sha256=sha(ROOT/p))
for n,h in manifest['instrumentation'].items():
    if sha(HERE/n)!=h:CHANGED_BANK[n]=dict(tested_sha256=h,current_sha256=sha(HERE/n))
assert set(CHANGED_SRC)<=set(CFG['allow_src_change']),CHANGED_SRC
assert set(CHANGED_BANK)<=set(CFG['allow_bank_change']),CHANGED_BANK
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
assert sha(HERE/('artifacts/gta_reversed-%s-probe.asi'%BANK.lower()))==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/('gta_reversed-%s-probe.pdb'%BANK.lower());assert sha(pdb)==manifest['binaries']['pdb']
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
COMMON=CFG['common'];SPECIFIC=CFG['specific'];SOURCE_CHANGES=CFG['source_changes']
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,registered_pairs=sum(PAIRS.values()),asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='differences against the EXE in the run; not registered, no credit (another agent is diagnosing it)') for f in REJECTED}
reaudit=dict(comparison_events=TOTAL,complete_events=events.get('complete',0),terminal_error_events=events.get('error',0),terminal_event=term,layout_events=1,layout_ok=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=len(EXPECT),functions_with_zero_failures=len(NEW),unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per)
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
if not PASSED:
    out.update(partial_per_function_approval=True,rejected_functions=rejected)
if CHANGED_SRC or CHANGED_BANK:
    out.update(source_changed_after_run=CHANGED_SRC,bank_changed_after_run=CHANGED_BANK)
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/('reversing/validation-registry.backup-before-%s.json'%CFG['tag'])
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
assert n==CFG['registry_before'],n
for name in NEW:
    cls,fn=name.split('::',1);hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=PAIRS[name],original_exe_sha256=summary['original_exe_sha256']),tested_sources=dict(manifest['sources']),
     source_provenance=CFG['provenance'],
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/%s/README.md'%BANK))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.%s.tmp'%CFG['tag']);temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
# README: only append the approval section at the end (append mode, bank file otherwise untouched)
readme=HERE/'README.md';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
