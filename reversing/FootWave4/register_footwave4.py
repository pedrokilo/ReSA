"""Registro historico de CTaskSimplePlayerOnFoot (5 funciones) del banco simple FootWave4 (corrida 20261009-123357-157735). Re-audita en streaming la corrida, conserva la evidencia en runs/ y anade las entradas aprobadas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
CFG={'bank': 'FootWave4',
 'run_id': '20261009-123357-157735',
 'probe_prefix': 'a9334da8',
 'tag': 'foot-wave4',
 'registry_before': 7163,
 'error': None,
 'passed': True,
 'bad': {},
 'expect': {'CTaskSimplePlayerOnFoot::PlayIdleAnimations': 1024,
            'CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon': 1024,
            'CTaskSimplePlayerOnFoot::PlayerControlDucked': 1024,
            'CTaskSimplePlayerOnFoot::PlayerControlZelda': 1024,
            'CTaskSimplePlayerOnFoot::ProcessPlayerWeapon': 1024},
 'total': 5120,
 'new': ['CTaskSimplePlayerOnFoot::PlayIdleAnimations',
         'CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon',
         'CTaskSimplePlayerOnFoot::PlayerControlDucked',
         'CTaskSimplePlayerOnFoot::PlayerControlZelda',
         'CTaskSimplePlayerOnFoot::ProcessPlayerWeapon'],
 'rejected': [],
 'addr': {'CTaskSimplePlayerOnFoot::PlayIdleAnimations': '0x006872C0',
          'CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon': '0x00687C20',
          'CTaskSimplePlayerOnFoot::PlayerControlDucked': '0x00687F30',
          'CTaskSimplePlayerOnFoot::PlayerControlZelda': '0x006883D0',
          'CTaskSimplePlayerOnFoot::ProcessPlayerWeapon': '0x006859A0'},
 'types': {'CTaskSimplePlayerOnFoot::PlayIdleAnimations': 'S',
           'CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon': 'S',
           'CTaskSimplePlayerOnFoot::PlayerControlDucked': 'S',
           'CTaskSimplePlayerOnFoot::PlayerControlZelda': 'S',
           'CTaskSimplePlayerOnFoot::ProcessPlayerWeapon': 'S'},
 'pending_after': [],
 'allow_src_change': [],
 'allow_bank_change': [],
 'provenance': 'Exact compiled FootWave4 probe (ASI/PDB hashes in build-manifest.json); class sources hashed from the probe build manifest and equal to the working tree at registration; full '
               '5120-row log reaudited offline in streaming and preserved gzipped with the original hash.',
 'common': 'Simple pair bench inside the full game with a loaded save (original with the function hook off vs candidate with the hook on), 5120 isolated CPU pairs for the 5 pending functions of '
           'CTaskSimplePlayerOnFoot, ALL pairs equal, pair counts equal to the bank table, passed=true globally, natural "complete" event with state_restored, layout event ok, saves unchanged, '
           'configs restored. Each case rebuilds the same deterministic fixture in both legs (full CPlayerPed buffer with raw EXE offsets, target ped, other ped, vehicle, fake intelligences, '
           'CPlayerPedData, full CPad, fake group, fake clump, fake animation associations/groups/blocks, camera state) and compares return value, memory of the tested object and its padding, new '
           'pool tasks (type + fields set by the constructor), fake task memory, fingerprint of all buffers, the whole trace of doubled collaborators with raw arguments (including random reads), '
           'fingerprint of touched globals, the x87 control word (24 bits) and an empty x87 stack; foreign threads ignored. NaN cases use quiet NaN only. Limits: about 75 collaborators are doubled '
           'with traces WITHOUT credit (CPad queries, CCamera, CPed/CPlayerPed members, animation, CGameLogic, intelligence and task functions, groups, events, crimes, audio, IK, rand and '
           'GetRandomNumber). Constructors/destructors of stack objects (CPedDamageResponseCalculator 0x4AD3F0/0x4AD420, CEventDamage 0x4AD830/0x4AD960, CEventGunAimedAt 0x4B0700/0x4B07B0, '
           'CEventGroupEvent 0x4ADFD0/0x4AE070) and constructors of really new tasks (CTaskSimpleUseGun, CTaskSimpleFight, CTaskSimpleStealthKill, CTaskSimpleThrowProjectile, CTaskComplexJump, '
           'CTaskComplexUseGoggles) run for real and are only summarised. The two static functions (PlayerControlZeldaWeapon, PlayerControlDucked) are `ret 4` in the EXE and cdecl in the DLL (not '
           'changed): the original is entered by EXE address with stdcall and the candidate calls the DLL function directly with cdecl; the hook is only safe because its only EXE caller is the '
           'hooked ProcessPed. Real globals touched for microseconds and restored after EACH run: CWorld::Players[1].m_pPed, gLastTouchTimeDelta, gLastRandomNumberForIdleAnimationID, '
           'gbUnknown_8D2FE8, gDuckAnimBlendData, TheCamera fields and four ms_aAnimBlocks entries. Only the own part of each function is validated. Source tested == current working tree at '
           'registration (TaskSimplePlayerOnFoot.cpp/.h match the probe build manifest). Historical evidence approved with explicit limits; no current dependency accreditation.',
 'specific': {'CTaskSimplePlayerOnFoot::PlayIdleAnimations': 'Corpus 1024; source corrected (EXE): 0x6872CE GetPadFromPlayer before the cooperative-mode check (0x6872D5..0x6872E5); 0x687300 test '
                                                             'byte [ebp+478h],2 is bUsingMobilePhone (not bCrouchWhenShooting); 0x687323/0x687393/0x687424 signed comparisons (jg/jle); 0x68740D test '
                                                             'byte [ebp+46Ch],0Ch / jne: it is enough that ONE of bIsLooking/bIsRestoringLook is set; 0x6873BD..0x687406 animation index tested first '
                                                             '<= idx < first + num (exclusive upper end) via RpAnimBlendClumpGetFirstAssociation/GetNextAssociation; 0x687430 '
                                                             'CGeneral::GetRandomIntInRange(0,4).',
              'CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon': 'Corpus 1024; source corrected (EXE): 0x687D73 fsin, 0x687D99 fcos and 0x687EA9 fpatan with full x87 precision (std::sin/cos/atan2 '
                                                                   'differed in the last bit); rest MATCH. Static function: ret 4 in the EXE, cdecl in the DLL (not changed).',
              'CTaskSimplePlayerOnFoot::PlayerControlDucked': 'Corpus 1024; source corrected (EXE): 0x687F5F/0x687F75 UD then LR pad reads and 0x687FE2/0x687FED BEFORE checking finished/aborting; '
                                                              '0x6880C8..0x6880E9 and 0x68821D..0x68823A direction (-sin, cos, 0) passed to IsPlayerAllowedToGoInThisDirection; 0x6882C9/0x688343 '
                                                              'fcomp 0.5 / test ah,41h / jne (NaN); two GetTaskUseGun calls (0x68829B/0x6882A8); fsin/fcos/fpatan x87. Static function: ret 4 in the '
                                                              'EXE, cdecl in the DLL (not changed).',
              'CTaskSimplePlayerOnFoot::PlayerControlZelda': 'Corpus 1024; source corrected (EXE): 0x688491 !(ratio > 0) with NaN; 0x68849C..0x6884C4 angle computed only in the update branch; '
                                                             '0x68852D/0x68854B NaN-aware clamps; 0x6885A7..0x6885B0 standing-on entity checked with the PHYSICAL flags (+0x40) bit 2 set and bit 3 '
                                                             'clear; 0x688687..0x6886CB DuckJustDown and CanPedDuck always queried (bAvoidJumpingAndDucking only skips SetTaskDuckSecondary(0) and the '
                                                             'jump block); 0x6884D1..0x688508 (-sin, cos, 0) with x87 fsin/fcos.',
              'CTaskSimplePlayerOnFoot::ProcessPlayerWeapon': 'Corpus 1024; source corrected (EXE): 0x6862F4..0x68632D in weapon state 2 the command is AIM only if GetTarget or a target exists or '
                                                              'm_bFreeAiming, otherwise the whole weapon control is skipped (je 0x6860AD); 0x686AF9..0x686B0D IsInSeeingRange/GetActivePrimaryTask '
                                                              'asked to the TARGET intelligence; 0x686BE4..0x686C08 CEventGunAimedAt added to the TARGET event group; 0x6871F0 !(dot > 0) with NaN; '
                                                              '0x686F90 fightMovement.y < -0.5 -> 16 else 15 (NaN included); 0x686DB5 unrounded fsin + z and 0x6870A9 fpatan; 0x68674C CMatrix '
                                                              'MultiplyMatrixWithVector 0x59C890 summation order; weapon slot indexed with movsx and no range check.'},
 'source_changes': {'source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp': 'PlayIdleAnimations (0x6872CE, 0x687300, 0x687323, 0x687393, 0x687424, 0x68740D, 0x6873BD..0x687406, 0x687430), '
                                                                                 'PlayerControlZeldaWeapon (0x687D73, 0x687D99, 0x687EA9 x87), PlayerControlDucked (0x687F5F..0x687FED, '
                                                                                 '0x6880C8..0x6880E9, 0x68821D..0x68823A, 0x6882C9, 0x688343, 0x68829B/0x6882A8), PlayerControlZelda '
                                                                                 '(0x688491..0x68854B, 0x6885A7..0x6885B0, 0x688687..0x6886CB), ProcessPlayerWeapon (0x6862F4..0x68632D, '
                                                                                 '0x686AF9..0x686C08, 0x6871F0, 0x686F90, 0x686DB5, 0x6870A9, 0x68674C)'},
 'notes': 'With these five functions CTaskSimplePlayerOnFoot has no pending entries in docs/hooks.csv (the others, including PlayerControlFighter, were registered by earlier banks). The two static '
          'functions keep the cdecl hook against the ret 4 of the EXE (not changed).',
 'readme_section': '## Aprobación (09/10/2026)\n'
                   '\n'
                   'Corrida `20261009-123357-157735`: passed=true, 5120 pares, 5 funciones con 0 fallos (1024 cada una), evento complete (state_restored), evento layout correcto, saves y '
                   'configuraciones intactos, `executed_probe_sha256` a9334da8... Registrada con `register_footwave4.py` (evidencia en `runs/`, `validation-summary.json`). Con esto '
                   'CTaskSimplePlayerOnFoot queda COMPLETA. Límites: ~75 colaboradores doblados con traza y sin crédito; ctor/dtor de objetos de pila y ctors de tareas nuevas reales; las dos '
                   'estáticas (ZeldaWeapon, Ducked) son `ret 4` en el EXE y cdecl en la DLL (no cambiado). Las notas anteriores de "no compilado ni ejecutado" quedan superadas.'}
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
