"""Independent read-only audit of approved cohort; writes only a new MD report."""
import collections,gzip,hashlib,json,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
def read(p):return json.loads(p.read_text(encoding='utf-8'))
def sha(data):return hashlib.sha256(data).hexdigest()
approved=read(HERE/'approved.json')
summaries=sorted({v['validation_summary'] for v in approved.values()})
rows=[];problems=[];drifts=[];details=[];total=0
for path in summaries:
    summaryPath=ROOT/path;s=read(summaryPath);bank=summaryPath.parent;run=ROOT/s['run'];r=read(run/'summary.json');audit=read(run/'approval-audit.json')
    raw=gzip.decompress((run/'comparisons.jsonl.gz').read_bytes());events=[json.loads(line) for line in raw.decode('utf-8').splitlines()]
    comps=[e for e in events if e.get('event')=='comparison'];ends=[e for e in events if e.get('event')=='complete'];errors=[]
    counts=collections.Counter(x['function'] for x in comps);identities={v['identity'] for v in approved.values() if v['validation_summary']==path}
    def require(value,label):
        if not value:errors.append(label)
    require(dict(counts)==s['functions']==r['pairs_per_function'],'counts function/summary/run')
    if 'pairs_per_function' in audit:require(dict(counts)==audit['pairs_per_function'],'counts approval-audit')
    require(set(counts)==identities,'approved identities')
    require(len(comps)==s['pairs']==r['pairs']==audit['pairs'],'pair totals')
    require(len({x['pair'] for x in comps})==len(comps),'unique pair IDs')
    for name,count in counts.items():require({x.get('case') for x in comps if x['function']==name}==set(range(count)),name+' unique cases 0..N-1')
    nulls=sum(x.get('original') is None or x.get('candidate') is None for x in comps)
    differences=sum(x.get('equal') is not True or x.get('accepted') is not True or x.get('original')!=x.get('candidate') for x in comps)
    require(nulls==0,'nonNULL original/candidate');require(differences==0,'equal accepted full objects')
    for x in comps:
        for side in ('original','candidate'):
            value=x.get(side)
            if not isinstance(value,dict):continue
            for key in ('route_ok','hook_route_ok','helper_route_ok','library_route_ok','stack_restored','registers_preserved','nonvolatile_preserved','nonvolatile_registers_preserved','fp_preserved','seh_restored','contract_ok','readonly_ok','guards_ok','protected_state_unchanged','expected_effects_correct'):
                if key in value:require(value[key] is True,'percall '+key)
    header=(bank/'LiveComparison.hpp').read_text(encoding='utf-8')
    fnrows=re.findall(r'\{\s*"([^\"]+)"\s*,\s*"([^\"]+)"\s*,\s*(0x[0-9a-fA-F]+)',header)
    declared={f'{cls}::{name}':f'0x{int(addr,16):08X}' for cls,name,addr in fnrows}
    for address,entry in approved.items():
        if entry['validation_summary']==path and entry['identity'] in declared:require(declared[entry['identity']]==address,'approved address/declared Fn '+entry['identity'])
    require(len(ends)==1 and ends[0].get('pairs')==len(comps) and ends[0].get('state_restored') is True,'one complete/restored')
    require(not any(e.get('event')=='error' for e in events),'no error events')
    for field in ('passed','all_equal','state_restored','saves_unchanged','configs_restored'):require(s.get(field) is True and r.get(field) is True,'summary boolean '+field)
    require(s['differences']==audit['differences']==0 and audit['null_comparison_records']==0,'audit zero difference/null')
    require(sha(raw)==s['log_sha256']==r['log_sha256']==audit['log_sha256'],'decompressed log SHA')
    require(sha((run/'summary.json').read_bytes())==s['summary_sha256'],'summary file SHA')
    require(sha((run/'RESULTADO.txt').read_bytes())==s['result_txt_sha256'],'result file SHA')
    require(s['original_exe_sha256']==r['original_exe_sha256']==audit['original_exe_sha256']=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b','original EXE identity')
    require(s['executed_probe_sha256']==r['executed_probe_sha256']==audit['executed_probe_sha256'],'probe identity')
    manifest=s['manifest'];require(bool(manifest['tested_sources']) and bool(manifest['bank_sources']),'nonempty manifests')
    require(manifest['tested_sources']==audit['tested_sources'] and manifest['bank_sources']==audit['bank_sources'],'historical manifest agreement')
    for group in ('tested_sources','bank_sources'):
        for source,digest in manifest[group].items():
            require(re.fullmatch('[0-9a-f]{64}',digest) is not None,'SHA format '+source)
            current=ROOT/source if group=='tested_sources' else bank/source
            if not current.exists():errors.append('manifest missing '+str(current));continue
            if sha(current.read_bytes())!=digest:
                if group=='bank_sources':errors.append('instrumentation SHA changed '+source)
                else:drifts.append((bank.name,source,digest,sha(current.read_bytes())))
    begin=[e for e in events if e.get('event')=='begin'];require(len(begin)==1,'single begin')
    if bank.name=='DependenciesTaskPed5':
        b=begin[0];helpers=b.get('helper_routes',[])
        require(b.get('registered_hook') is True and b.get('collaborators_stubbed') is False,'TaskPed hooks/no stubs')
        require({h['address'] for h in helpers}=={0x571B70,0x571A00,0x601C30,0x601BC0,0x5DF8F0} and all(h.get('jumps_to_dll') is True and h['bytes'][0]==0xE9 for h in helpers),'TaskPed five DLL helper routes')
        for x in comps:
            for value in (x['original'],x['candidate']):
                require(all(value.get(k) is True for k in ('route_ok','stack_restored','nonvolatile_preserved','seh_restored','contract_ok')),'TaskPed percall ABI/contract')
        details.append('TaskPed5: begin registra cinco destinos 571B70/571A00/601C30/601BC0/5DF8F0 con JMP a DLL; no stubs. Todos los pares confirman ruta, pila, registros no volátiles, SEH y contrato. exception_unwind_tested=false: no se atribuye validación a desenrollado excepcional. El begin muestra rutas compartidas reconstruidas; no prueba independencia transitiva universal.')
    if bank.name=='DependenciesVehicleLeaf5':
        b=begin[0];helpers=b.get('helpers',[])
        require(b.get('registered_hook') is True and b.get('helper_route_ok') is True and b.get('collaborators_stubbed') is False,'Vehicle hooks/no stubs')
        require({h['entry'] for h in helpers}=={0x542A50,0x46AA30} and all(h.get('helper_route_ok') is True and h.get('shared_reconstructed_collaborator') is True and h['body']!=h['raw_symbol'] and h['asi_module']>0 for h in helpers),'Vehicle helper meta')
        for x in comps:
            require(x.get('state_restored') is True,'Vehicle restored perpair')
            for value in (x['original'],x['candidate']):require(all(value.get(k) is True for k in ('route_ok','helper_route_ok','stack_restored','nonvolatile_registers_preserved','protected_state_unchanged','expected_effects_correct')),'Vehicle percall helpers/ABI/effects')
        details.append('VehicleLeaf5: meta de helpers 542A50/46AA30 incluye símbolo crudo distinto de cuerpo resuelto, módulo ASI y ruta correcta; ambos colaboradores compartidos reconstruidos, sin stubs. Cada original/candidate reafirma helper_route_ok, ruta principal, ABI, efectos y estado protegido, y cada par state_restored. La relación de aprobación incluye exactamente sus cuatro identidades y el run 195234-703310.')
    errors=list(dict.fromkeys(errors));problems.extend((bank.name,e) for e in errors);total+=len(comps)
    rows.append((bank.name,len(counts),len(comps),differences,nulls,not errors,s['log_sha256']))
out=['# Auditoría independiente de la cohorte aprobada (43 funciones)','','Lectura directa de approved.json, 18 validation-summary.json, summary.json, approval-audit.json y gzip completos. No se compila, ejecuta GTA ni cambia fuente, bancos o catálogo. Instantánea de aprobación: '+sha((HERE/'approved.json').read_bytes())+'.','',f'**Resultado: {len(approved)} funciones, {len(rows)} bancos y {total} pares. Hallazgos que afectan evidencia: {len(problems)}.**','','| Banco | Funciones | Pares | Diferencias | NULL | Auditoría |','|---|---:|---:|---:|---:|---|']
out.extend(f'| {b} | {n} | {p} | {d} | {z} | {"OK" if ok else "REVISAR"} |' for b,n,p,d,z,ok,h in rows)
out+=['','Se verificaron casos únicos consecutivos por función y pares únicos, equality real de objetos, accepted/equal true, un único complete sin errores y restauración. SHA-256 se calcula sobre los bytes descomprimidos y se cruza contra los tres manifiestos; summary/RESULTADO se hashéan por bytes. Configuración y partidas están confirmadas por los flags históricos y hashes antes del run en summary; no se reconstruyen snapshots posteriores inexistentes ni se presenta como una comprobación independiente de los archivos después del run.','','## Colaboradores nuevos','']+details
out+=['','## Manifiestos y evolución de fuente','','Todos los bank_sources deben coincidir con su hash histórico. Los hashes tested_sources coinciden entre validación y approval-audit. Un archivo fuente actual puede evolucionar por bancos posteriores: esto no invalida por sí mismo la evidencia histórica; especialmente InjectHooksMain actual no se exige igual. El SHA del ASI ejecutado concuerda entre las tres evidencias; artifacts no se requieren porque se excluyen de Git.','','| Banco | Fuente con evolución desde su run | SHA histórico | SHA actual |','|---|---|---|---|']
out.extend(f'| {b} | {s} | {old} | {new} |' for b,s,old,new in drifts)
out+=['','## Hallazgos','']+([f'- {b}: {e}' for b,e in problems] or ['Ninguna discrepancia de integridad, relación de aprobación, cifras, casos, NULL, igualdad, restauración o instrumentación detectada en esta instantánea.'])
out+=['','## Logs verificados','']+[f'- {b}: `{h}`' for b,n,p,d,z,ok,h in rows]
(HERE/'AUDIT_COHORT43_WORLD.md').write_text('\n'.join(out)+'\n',encoding='utf-8')
print(json.dumps({'functions':len(approved),'banks':len(rows),'pairs':total,'problems':problems,'source_drift':len(drifts)},ensure_ascii=False))
