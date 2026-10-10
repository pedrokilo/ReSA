"""Registro historico de las 9 funciones de CPopulation del banco PopFinal4 (corrida 20261009-214531-885595). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'PopFinal4',
 'run_id': '20261009-214531-885595',
 'probe_prefix': 'e76cfa3c',
 'tag': 'popfinal4',
 'registry_before': 7809,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CPopulation::AddDeadPedInFrontOfCar': 256,
            'CPopulation::AddPed': 1024,
            'CPopulation::AddPedAtAttractor': 256,
            'CPopulation::AddPedInCar': 512,
            'CPopulation::CreateWaitingCoppers': 512,
            'CPopulation::GeneratePedsAtAttractors': 512,
            'CPopulation::PlaceCouple': 512,
            'CPopulation::PopulateInterior': 256,
            'CPopulation::TestSafeForRealObject': 256},
 'total': 4096,
 'new': ['CPopulation::AddDeadPedInFrontOfCar',
         'CPopulation::AddPed',
         'CPopulation::AddPedAtAttractor',
         'CPopulation::AddPedInCar',
         'CPopulation::CreateWaitingCoppers',
         'CPopulation::GeneratePedsAtAttractors',
         'CPopulation::PlaceCouple',
         'CPopulation::PopulateInterior',
         'CPopulation::TestSafeForRealObject'],
 'rejected': [],
 'addr': {'CPopulation::AddDeadPedInFrontOfCar': '0x00612CD0',
          'CPopulation::AddPed': '0x00612710',
          'CPopulation::AddPedAtAttractor': '0x00614210',
          'CPopulation::AddPedInCar': '0x00613A00',
          'CPopulation::CreateWaitingCoppers': '0x006133F0',
          'CPopulation::GeneratePedsAtAttractors': '0x00615970',
          'CPopulation::PlaceCouple': '0x00613D60',
          'CPopulation::PopulateInterior': '0x00616470',
          'CPopulation::TestSafeForRealObject': '0x006123A0'},
 'types': {'CPopulation::AddDeadPedInFrontOfCar': 'S',
           'CPopulation::AddPed': 'S',
           'CPopulation::AddPedAtAttractor': 'S',
           'CPopulation::AddPedInCar': 'S',
           'CPopulation::CreateWaitingCoppers': 'S',
           'CPopulation::GeneratePedsAtAttractors': 'S',
           'CPopulation::PlaceCouple': 'S',
           'CPopulation::PopulateInterior': 'S',
           'CPopulation::TestSafeForRealObject': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'extra_unregistered': [],
 'provenance': 'Exact compiled PopFinal4 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest '
               'and equal to the working tree at registration; full 4096-row log reaudited offline in streaming and preserved gzipped with '
               'the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), '
           '4096 isolated CPU pairs for 9 functions of CPopulation, ALL pairs equal, pair counts equal to the bank table, passed=true '
           'globally, natural "complete" event with state_restored and layout_ok, saves unchanged, configs restored. Each case rebuilds '
           'the same deterministic fixture (seed = case index) in both legs and compares the return value, the bytes of the fake objects, '
           'the touched global ranges, the x87 control word (24 bits) and the whole trace of doubled collaborators with raw arguments; '
           'foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is validated. Source tested == '
           'current working tree at registration (class sources match the probe build manifest). Historical evidence approved with '
           'explicit limits; no current dependency accreditation. Padding and uninitialised bytes masked. Bank limits (PopFinal4): '
           'CPed::operator new (0x5E4720), CVehicle::operator new (0x6E2D50) and CTask::operator new (0x61A5A0) return holes of own '
           'buffers filled with 0xCD (fake buffers, not real pool objects); the constructors of CCivilianPed (0x5DDB70), CCopPed '
           '(0x5DDC60), CEmergencyPed (0x5DE340) and CAutomobile (0x6B0A90) are doubled in the EXE and in the DLL (they set only the '
           'fields read afterwards: vtable, model, type, intelligence, matrix, clump type 2), so they get no credit; stack tasks and '
           'events are REAL (EXE and DLL constructors/destructors) with CTaskManager::SetTask (0x681AF0) and CEventGroup::Add (0x4AB420) '
           'doubled and traced (type and words written; unwritten 0xCD arena bytes masked; in events only the fields 0x10/0x12/0x14/0x18 '
           'and 0x1C/0x20 of the attractor); world, camera, ground, collisions, paths, weapons, intelligence, streaming and the '
           'CPopulation functions each case does not test are doubled with traces and get no credit; ThePaths and the sector lists '
           '0xB992B8 (3072 bytes), pool headers and every touched global are backed up and restored per case; CMatrix::operator* and '
           'CPed::SetHeading run undoubled (identity matrices / ped without matrix). Fixture domain: model ids 0..319 only '
           '(PedMICanBeCreatedAtThisAttractor reads [mi+0x28] without a null check, 0x61120C); PopulateInterior numPeds <= 6 (the EXE '
           'dereferences the ped returned by AddPed without a null check, 0x6165A2); the 2D effect is of attractor type '
           '(EFFECT_ATTRACTOR=3). Limits are declared per function in the function-specific scope and in the bank README.',
 'specific': {'CPopulation::AddDeadPedInFrontOfCar': 'Corpus 256; fake ped/vehicle buffers (0xCD) and doubled constructors; world, ground '
                                                     'Z and collision collaborators doubled with trace. ',
              'CPopulation::AddPed': 'Corpus 1024; ped/vehicle/task operator new return fake 0xCD buffers and the civilian/cop/emergency '
                                     'ped constructors are doubled (no credit); model ids 0..319; collaborators (world, intelligence, '
                                     'weapons, groups) doubled with trace. ',
              'CPopulation::AddPedAtAttractor': 'Corpus 256; source corrected: the attractor event uses task type 233 = 0xE9 '
                                                '(TASK_COMPLEX_USE_ATTRACTOR, 0x61438F; the DLL enum value is 235) - already in HEAD; '
                                                'stack event real, CEventGroup::Add doubled (only the attractor fields '
                                                '0x10/0x12/0x14/0x18/0x1C/0x20 compared); 2D effect of attractor type '
                                                '(EFFECT_ATTRACTOR=3); model ids 0..319 (0x61120C). ',
              'CPopulation::AddPedInCar': 'Corpus 512; fake ped/vehicle buffers (0xCD) and doubled ped/CAutomobile constructors; '
                                          'CCarEnterExit and intelligence doubled with trace. ',
              'CPopulation::CreateWaitingCoppers': 'Corpus 512; source corrected against the EXE: 0x6D8470 is CVehicle::UsesSiren (not '
                                                   'IsLawEnforcementVehicle 0x6D2370); FindPlayerPed(-1) is called BEFORE reserving the '
                                                   'task (0x61391A..0x61395D); CTaskComplexKillPedOnFoot built with 6 arguments followed '
                                                   'by `or [task+0Ch],30h` (bits 4 and 5 = m_bAimImmediate, m_bWaitForPlayerToBeSafe); '
                                                   'fake vehicle/ped/task buffers (0xCD), doubled cop/emergency ped and CAutomobile '
                                                   'constructors, SetTask and CEventGroup::Add doubled; JoinCarWithRoadSystem (0x42F5A0) '
                                                   'patched only in the EXE. ',
              'CPopulation::GeneratePedsAtAttractors': 'Corpus 512; fake ped buffers (0xCD) and doubled constructors; attractor, path and '
                                                       'position collaborators doubled with trace; 2D effect of attractor type '
                                                       '(EFFECT_ATTRACTOR=3); model ids 0..319 (PedMICanBeCreatedAtThisAttractor reads '
                                                       '[mi+0x28] without a null check, 0x61120C). ',
              'CPopulation::PlaceCouple': 'Corpus 512; fake ped buffers (0xCD) and doubled constructors; path/attractor/position-clear '
                                          'collaborators doubled with trace; model ids 0..319. ',
              'CPopulation::PopulateInterior': 'Corpus 256; numPeds limited to <= 6 because the EXE dereferences the ped returned by '
                                               'AddPed without a null check (0x6165A2); interior path nodes are fake '
                                               '(ThePaths.m_pPathNodes[5..7], restored); fake ped buffers (0xCD) and doubled '
                                               'constructors. ',
              'CPopulation::TestSafeForRealObject': 'Corpus 256; sector lists 0xB992B8 and pool headers backed up and restored; model '
                                                    'infos fake (ids 0..319); FindObjectsInRange/ProcessColModels/collision collaborators '
                                                    'doubled with trace. '},
 'source_changes': {'source/game_sa/Population.cpp': 'CreateWaitingCoppers corregida contra el EXE: UsesSiren (0x6D8470) en vez de '
                                                     'IsLawEnforcementVehicle (0x6D2370); FindPlayerPed(-1) antes de reservar la tarea '
                                                     '(0x61391A..0x61395D); CTaskComplexKillPedOnFoot con 6 argumentos y `or '
                                                     '[task+0Ch],30h` (bits 4 y 5). AddPedAtAttractor usa el tipo de tarea 233=0xE9 '
                                                     '(TASK_COMPLEX_USE_ATTRACTOR, 0x61438F) y las correcciones generales de PopFinal3 ya '
                                                     'estan en el arbol; ver git diff y el README del banco.'},
 'notes': 'PopFinal4 registers the last 9 functions of CPopulation (4096 pairs, 0 differences). Limits are declared per function; see '
          'scope and the bank README. No pending functions remain in CPopulation.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-214531-885595`: passed=true, 4096 pares, 9 funciones registradas con 0 fallos, evento complete '
                   '(state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_popfinal4.py` (evidencia en '
                   '`runs/`, `validation-summary.json`). Con este banco CPopulation queda sin pendientes. Limites declarados en el alcance '
                   'de cada funcion y en las secciones anteriores de este README.'}
BANK=CFG['bank'];RUN_ID=CFG['run_id']
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')/('resultados-'+BANK.lower())/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
EXPECT=CFG['expect'];TOTAL=sum(EXPECT.values());NEW=CFG['new'];REJECTED=CFG['rejected'];PASSED=CFG['passed']
EXTRA=CFG.get('extra_unregistered',[])
assert TOTAL==CFG['total'] and set(NEW)|set(REJECTED)|set(EXTRA)==set(EXPECT) and not set(NEW)&set(REJECTED) and not set(EXTRA)&set(NEW)
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
assert layout is not None and layout.get('ok') is True and events.get('layout')==1 and begin is None
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
reaudit=dict(comparison_events=TOTAL,complete_events=events.get('complete',0),terminal_error_events=events.get('error',0),terminal_event=term,layout_events=events.get('layout',0),begin_events=events.get('begin',0),begin_checked=True,waiting_for_player_events=events.get('waiting_for_player',0),case_start_events=events.get('case_start',0),function_summaries=len(EXPECT),functions_with_zero_failures=len(NEW),unequal_pairs=sum(bad.values()),unequal_by_function=bad,distinct_cases=len(cases),per_function=per)
out=dict(passed=PASSED,pairs=TOTAL,differences=sum(bad.values()),registered_pairs=sum(PAIRS.values()),registered_functions=len(NEW),not_registered_extra=EXTRA,functions=PAIRS,covered_functions={k:True for k in NEW},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=PASSED,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,reaudit=reaudit,source_corrections=SOURCE_CHANGES,notes=CFG['notes'])
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
