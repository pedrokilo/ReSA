"""Registra las 102 funciones solo después de reauditar una corrida completa aprobada.
Uso: python -B reversing/VehAudioFinal4/register_vehaudiofinal4.py ID_CORRIDA
"""
import csv,gzip,hashlib,json,os,shutil,subprocess,time,sys,re
from pathlib import Path
from audit_run import audit
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
assert len(sys.argv)==2, 'Indique explícitamente el ID de la corrida aprobada'
run_id=sys.argv[1]
assert re.fullmatch(r'\d{8}-\d{6}-\d{6}',run_id),run_id
run=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-vehaudiofinal4')/run_id
expect, audited_summary=audit(run)
assert len(expect)==102
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
selected={r['class']+'::'+r['fn_name']:r for r in rows if r['class']=='CAEVehicleAudioEntity' and r['validated']=='0'}
assert set(selected)==set(expect) and all(r['validated']=='0' for r in selected.values())
limits='mundo falso de vehículos, entidades de audio, matrices, transmisiones, peatones y sonidos; gestor/hardware de audio, azar, pad, entorno, referencias y radio doblados con traza sin crédito; dominio real del EXE; escritorio m_tempSound y campos no escritos de CAESound excluidos conforme README; '
common=('Banco por pares dentro del juego completo con partida cargada: original con hooks apagados frente a candidata con hooks activos. '+str(sum(expect.values()))+' pares para las 102 funciones, cero diferencias, complete natural, estado/globales/x87 restaurados, saves y configuraciones intactos. Cada caso compara retorno, trazas con argumentos crudos, solicitudes de sonido y campos reales, mundo falso y globales. x87 PC24. Hilos ajenos no trazados. '+limits+'Solo la parte propia de cada función recibe crédito. Fuentes probadas iguales al árbol al registrar; dependency_review=null, aprobación histórica sin vigencia acreditada.')
specific={name:'Corpus '+str(pairs)+'; '+limits for name,pairs in expect.items()}
for fn,extra in {
 'PlayAircraftSound':'Tipos 0, 6 o >7 fuera: EXE 0x4F9602 usa CAESound sin construir. ',
 'RequestNewPlayerCarEngineSound':'Tipo fuera de rango excluido: 0x4F7AF5 lee pila indeterminada. ',
 'CancelVehicleEngineSound':'Tipo fuera de rango excluido: 0x4F55E3. ',
 'GetVolForPlayerEngineSound':'Tipo fuera de rango excluido: 0x4F5D09. ',
 'GetFreqForPlayerEngineSound':'Tipo fuera de rango excluido: 0x4F81A5. ',
 'ProcessPlayerTrainBrakes':'Solo tren del jugador: 0x500B87 lee local indeterminado en otro dominio. ',
 'UpdateParameters':'Helicóptero/sonido cercano fuera: colaborador real GetComponentWorldPosition. ',
}.items():specific['CAEVehicleAudioEntity::'+fn]+=extra
CFG=dict(bank='VehAudioFinal4',run_id=run_id,expect=expect,total=sum(expect.values()),new=sorted(expect),rejected=[],extra_unregistered=[],passed=True,error=None,bad={},
 addr={name:r['address'] for name,r in selected.items()},types={name:r['type'] for name,r in selected.items()},pending_after=[],probe_prefix=audited_summary['executed_probe_sha256'],
 allow_src_change=[],allow_bank_change=[],registry_before=8343,tag='vehaudiofinal4',source_changes={
 'AudioLog10ScaledExe':'0x4D9E67 fyl2x; 0x4D9E69 ret conserva ST0 hasta fmul del consumidor, sin spill float intermedio. Getters y Process*: evidence/float-review.md y evidence/process-review.md.',
 'ProcessVehicleRoadNoise':'0x4F8C24 push eax; 0x4F8C30 fstp [esp+14h] escribe frecuencia tras desplazar ESP; se repone distancia a cámara y orden x87.',
 'GetVolForPlayerEngineSound':'0x4FC9C1 call volumen; 0x4FC9CD call frecuencia sin recargar ECX; wrapper conserva ECX y ret 8.',
 'ProcessPlayerVehicleEngine':'0x4FBBC6..0x4FBBCD acceso a marcha sin comprobación de std::array; se usa data()[RealGear]. 0x4FBBCD fsub, 0x4FBBD2 fsub, 0x4FBBD5 fdivp conservan subnormales de transmisión; se reproduce x87 para evitar FTZ/DAZ de SSE.',
 'ProcessDummyRCCar':'0x500EED push 0x8CC0A4; 0x500EFE push 0x8CBF8C: lectura de tablas originales mutables, sin copias constexpr.',
 },common=common,specific=specific,
 provenance='Probe VehAudioFinal4 exacto (ASI/PDB y fuentes comprobados contra build-manifest); corrida completa reauditada por audit_run.audit, log íntegro conservado comprimido sin pérdida; correcciones asm en README e informes evidence del banco.',
 notes='VehAudioFinal4 registra las 102 funciones de CAEVehicleAudioEntity; cero diferencias. Límites por función en function_scope y README. Solo Win::NOTSA_WinMain queda pendiente en el catálogo.',
 readme_section='## Aprobación (10/10/2026)\n\nCorrida `'+run_id+'`: 102 funciones, '+str(sum(expect.values()))+' pares, cero diferencias; passed/all_equal/state_restored=true, saves/configuraciones intactos. Reauditoría obligatoria sin registros pasantes nulos.\n')
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
            assert row.get('original') is not None and row.get('candidate') is not None, (f,row['case'],'salida nula')
            if row.get('equal') is not True or row.get('accepted') is not True:bad[f]=bad.get(f,0)+1
        elif ev=='function_summary':assert row['function'] not in fsum;fsum[row['function']]=row
        elif ev=='complete':complete=row
        elif ev=='error':err=row
        elif ev=='layout':layout=row
        elif ev=='begin':begin=row
        else:assert ev in('waiting_for_player','case_start','probe_stage'),ev
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
rejected={f:dict(pairs=per[f],failures=bad[f],equal=per[f]-bad[f],reason='diferencias frente al EXE en la corrida; no registrada, sin crédito') for f in REJECTED}
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
     dependency_review=None,scope=COMMON+' Específico de la función: '+SPECIFIC[name],review_reference='reversing/%s/README.md'%BANK))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.%s.tmp'%CFG['tag']);temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before
for _try in range(30):
    try:os.replace(temp,registry);break
    except PermissionError:time.sleep(2)
else:raise SystemExit('registry locked')
# README: only append the approval section at the end (append mode, bank file otherwise untouched)
readme=HERE/'README.md';assert readme.exists(),'falta README.md del banco';raw_r=readme.read_bytes();nl='\r\n' if b'\r\n' in raw_r else '\n'
section=CFG['readme_section'].replace('\n',nl)
assert '## Aprobación' not in raw_r.decode('utf-8')
with open(readme,'ab') as fh:fh.write((nl+section+nl).encode('utf-8'))
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),summary_sha256=sha(sp),changed_src=list(CHANGED_SRC),changed_bank=list(CHANGED_BANK)),indent=2))
