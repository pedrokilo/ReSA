"""Registro historico de las nueve funciones de CHud (6), CCam (2) y CPickups (1) del banco simple HudCamFinal2 (corrida 20261009-180840-287896). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'HudCamFinal2',
 'run_id': '20261009-180840-287896',
 'probe_prefix': '6626f872',
 'tag': 'hudcamfinal2',
 'registry_before': 7478,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CHud::Draw': 1024,
            'CHud::DrawAfterFade': 512,
            'CHud::DrawRadar': 1024,
            'CHud::DrawWeaponIcon': 768,
            'CHud::Initialise': 64,
            'CHud::Shutdown': 64,
            'CCam::Process_1stPerson': 1024,
            'CCam::Process_Rocket': 1024,
            'CPickups::Update': 1024},
 'total': 6528,
 'new': ['CCam::Process_1stPerson',
         'CCam::Process_Rocket',
         'CHud::Draw',
         'CHud::DrawAfterFade',
         'CHud::DrawRadar',
         'CHud::DrawWeaponIcon',
         'CHud::Initialise',
         'CHud::Shutdown',
         'CPickups::Update'],
 'rejected': [],
 'addr': {'CCam::Process_1stPerson': '0x00517EA0',
          'CCam::Process_Rocket': '0x00511B50',
          'CHud::Draw': '0x0058FAE0',
          'CHud::DrawAfterFade': '0x0058D490',
          'CHud::DrawRadar': '0x0058A330',
          'CHud::DrawWeaponIcon': '0x0058D7D0',
          'CHud::Initialise': '0x005BA850',
          'CHud::Shutdown': '0x00588850',
          'CPickups::Update': '0x00458DE0'},
 'types': {'CCam::Process_1stPerson': 'S',
           'CCam::Process_Rocket': 'S',
           'CHud::Draw': 'S',
           'CHud::DrawAfterFade': 'S',
           'CHud::DrawRadar': 'S',
           'CHud::DrawWeaponIcon': 'S',
           'CHud::Initialise': 'S',
           'CHud::Shutdown': 'S',
           'CPickups::Update': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled HudCamFinal2 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the '
               'working tree at registration; full 6528-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 6528 isolated CPU '
           'pairs for 9 pending functions of CHud, CCam and CPickups, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural '
           '"complete" event with state_restored, saves unchanged, configs restored. Each case rebuilds the same deterministic fixture (seed = case index) in '
           'both legs and compares the return value, the bytes of the fake objects, the touched global ranges, the x87 control word (24 bits) and the whole '
           'trace of doubled collaborators with raw arguments; foreign threads ignored. Traced doubles get NO credit. Only the own part of each function is '
           'validated. Source tested == current working tree at registration (class sources match the probe build manifest). Historical evidence approved with '
           'explicit limits; no current dependency accreditation. Fixtures: fake CCam/ped/vehicle/ModelInfo objects, real pickup array and HUD statics copied '
           'and restored, fake RW render-state table (0xC97B24) swapped temporarily and restored; padding and uninitialised bytes masked. Limits are declared '
           'per function in the function-specific scope and in the bank README.',
 'specific': {'CCam::Process_1stPerson': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). fake CCam '
                                         'object (0x238 bytes filled with 0xCD, compared whole), fake ped/vehicle/ModelInfo slot; bones, world line of sight, '
                                         'water level and matrix collaborators doubled with trace; times >= 2^31 exercise the fild+2^32 sum; source corrected: '
                                         'single rounding fild + fadd 2^32 (0x517F4E..0x517F5E) and movsx of the vehicle model (0x5180EB).',
              'CCam::Process_Rocket': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). fake CCam '
                                      'object, rocket target, player and RW camera; GetATanOfXY stays real, line of sight, motion blur, matrices and '
                                      'weapon-effect collaborators doubled with trace; source corrected: GetATanOfXY returns in ST0 and fsub halfPi '
                                      '(0x511E99).',
              'CHud::Draw': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). m_bDraw3dMarkers takes 0, '
                            '1, 2 and 0x80 (EXE compares the byte with 1, 0x58FCA6); drawing through a fake RW render-state table (RwEngineInstance 0xC97B24 '
                            'swapped temporarily), collaborators (DrawRadar, DrawWeaponIcon, sprites) doubled with trace; source corrected: cmp byte '
                            'ds:[0BAA45Ch],1 (0x58FCA6).',
              'CHud::DrawAfterFade': 'Corpus 512; source reviewed against the EXE and corrected before the run where needed (see git diff). drawing through a '
                                     'fake RW render-state table, sprites and collaborators doubled with trace.',
              'CHud::DrawRadar': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). drawing through a '
                                 'fake RW render-state table; aircraft-ring vehicles always carry a matrix (EXE dereferences [eax+14h] unchecked, 0x58A3ED); '
                                 'collaborators doubled with trace; state 7 renamed for readability only (same value).',
              'CHud::DrawWeaponIcon': 'Corpus 768; source reviewed against the EXE and corrected before the run where needed (see git diff). a FREE TXD slot '
                                      'is NOT tested (the EXE reads address 0, 0x58D825/0x58D831; the source returns); only in-use slots with a fake or null '
                                      'dictionary and texture; RenderOneXLUSprite and CSprite2d::Draw doubled; fake RW render-state table.',
              'CHud::Initialise': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). TXD/sprite '
                                  'collaborators doubled with trace (HUD.TXD never loaded, no texture created or destroyed, TXD pool untouched); ReInitialise '
                                  'is real.',
              'CHud::Shutdown': 'Corpus 64; source reviewed against the EXE and corrected before the run where needed (see git diff). TXD/sprite collaborators '
                                'doubled with trace (no texture destroyed, TXD pool untouched).',
              'CPickups::Update': 'Corpus 1024; source reviewed against the EXE and corrected before the run where needed (see git diff). CPickup::Update is '
                                  'DOUBLED (it may cancel the type or visibility of the pickup), as are IsVisible, GiveUsAPickUpObject, CWorld::Add/Remove and '
                                  'the CPad/CPedIntelligence collaborators; CollectedPickUpIndex >= 20 is NOT tested (EXE writes out of the table, the DLL '
                                  'aborts in debug); real pickup array copied and restored; source corrected: the player-2 branch is not an else (0x459033 je '
                                  '0x459069).'},
 'source_changes': {'source/game_sa/Hud.cpp': 'CHud::Draw (cmp byte [0BAA45Ch],1, 0x58FCA6) y DrawRadar (estado 7 = SHADEMODE FLAT, solo legibilidad) '
                                              'corregidos contra el EXE; ver git diff',
                    'source/game_sa/Cam.cpp': 'CCam::Process_1stPerson (fild + fadd 2^32 una sola redondeada, 0x517F4E..0x517F5E; movsx del modelo del '
                                              'vehiculo, 0x5180EB) y CCam::Process_Rocket (GetATanOfXY devuelve en ST0 y fsub halfPi, 0x511E99) corregidos '
                                              'contra el EXE; ver git diff',
                    'source/game_sa/Pickups.cpp': 'CPickups::Update: la rama del jugador 2 no es un else (0x459033 test al,al; je 0x459069); ver git diff'},
 'notes': 'HudCamFinal2 registers the nueve funciones de CHud (6), CCam (2) y CPickups (1) (6528 pairs, 0 differences). Limits are declared per function; see '
          'scope and the bank README.',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-180840-287896`: passed=true, 6528 pares, 9 funciones con 0 fallos, evento complete (state_restored, layout_ok), evento '
                   'layout correcto, saves y configuraciones intactos. Registrada con `register_hudcamfinal2.py` (evidencia en `runs/`, '
                   '`validation-summary.json`). Limites: `CHud::DrawWeaponIcon` con ranura TXD libre no se prueba; `CPickups::Update` con '
                   '`CollectedPickUpIndex>=20` no se prueba y con `CPickup::Update` doblada; dibujo RW con tabla falsa de estados; colaboradores doblados sin '
                   'credito.'}
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
assert layout is not None and layout.get('ok') is True and events.get('layout')==1 and begin is not None and events.get('begin')==1 and begin.get('known') is True and isinstance(begin.get('sites'),list) and len(begin['sites'])>0
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
