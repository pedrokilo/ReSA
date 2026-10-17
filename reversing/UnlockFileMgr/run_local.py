import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='d3ffb3325017a44047c047df082c453b2137f2e5a864e6404a0889fa21cf8f8e'
REQUIRE_ENABLED=False
COUNTS={i:128 for i in range(13)}
def analyze(path):
    if not terminal_event(path):return None
    cases=set();complete=[];states=[];interop=[];load=[];ext=[];seek=[];disk=[]
    with path.open() as f:
        for line in f:
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro de GTA; consultar comparisons.jsonl')
            if r['event']=='complete':complete.append(r)
            if r['event']=='hook_state':states.append(r)
            if r['event']=='interop':interop.append(r)
            if r['event']=='extensions':ext.append(r)
            if r['event']=='disk_interop':disk.append(r)
            if r['event']!='comparison':continue
            key=(r['function'],r['case'])
            if key in cases:raise RuntimeError('Caso duplicado')
            cases.add(key)
            if r['function']==1:
                # LoadFile: ambos lados deben cumplir el modelo independiente del fichero (no basta con que coincidan entre si)
                if not (r['original'].get('expect_ok') is True and r['candidate'].get('expect_ok') is True):raise RuntimeError('LoadFile no cumple el modelo del fichero')
                load.append(r['original'])
            if r['function']==7:seek.append(r)
            if not r['equal'] or r['original']!=r['candidate'] or r['original_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia o contador incorrecto')
    if cases!={(fn,i) for fn,n in COUNTS.items() for i in range(n)} or len(complete)!=1 or not complete[0]['state_restored'] or complete[0]['pairs']!=sum(COUNTS.values()):raise RuntimeError('Cobertura incompleta')
    existing=[o for o in load if not o['absent']];absent=[o for o in load if o['absent']]
    if len(load)!=128 or len(absent)!=16 or {o['file_size'] for o in existing}!={0,1,256,0x3FFF,0x4000,0x4001,0x8000,0xC001} or {o['mode'] for o in existing}!={'rb','r'}:raise RuntimeError('Cobertura de LoadFile insuficiente')
    if any(o['return']==0 for o in existing if o['file_size']>0) or any(o['return']!=0xFFFFFFFF or o['missing_exists_after'] for o in absent):raise RuntimeError('LoadFile devuelve 0 con fichero existente o no devuelve -1 con fichero ausente')
    if len(ext)!=1 or ext[0]['unicode'] is not True or ext[0]['bounded_load'] is not True or not all(ext[0][k]['passed'] is True and len(ext[0][k]['cases'])==6 and ext[0][k]['absent_ok'] is True for k in ('bounded_dll','bounded_exe')):raise RuntimeError('Extensiones incorrectas (unicode / bounded_load)')
    if len(states)!=13 or (REQUIRE_ENABLED and any(not r['enabled'] or r['locked'] for r in states)):raise RuntimeError('Estado de hooks incorrecto')
    # Seek: origenes validos 0..2 e invalidos (3, -1, 4, INT_MIN). El EXE devuelve -1 (true) con errno = EINVAL y no toca el flujo; la DLL debe coincidir sin fail-fast.
    bad=[r for r in seek if r['original']['origin'] not in (0,1,2)]
    if len(seek)!=128 or {r['original']['origin'] for r in seek}!={0,1,2,3,-1,4,-2147483648} or len(bad)!=32:raise RuntimeError('Cobertura de origenes de Seek insuficiente')
    for r in seek:
        for side in ('original','candidate'):
            o=r[side]
            if o['origin'] in (0,1,2):
                if 'errno' in o:raise RuntimeError('Seek con origen valido registra errno')
            elif o['return'] is not True or o['errno']!=22 or o['next_read']!=1:raise RuntimeError('Seek con origen invalido no devuelve true/EINVAL con el flujo intacto')
    # Los unicos estados coherentes de ganchos son todo-EXE (0) y todo-DLL (63): un FILE* no cruza de CRT (README). Cada lado usa de verdad su CRT (owner).
    if len(interop)!=2 or {r['mask'] for r in interop}!={0,63} or not all(r['passed'] for r in interop) or {r['mask']:r['owner'] for r in interop}!={0:'exe',63:'dll'}:raise RuntimeError('Interoperabilidad incompleta')
    if len(disk)!=4 or {(r['writer'],r['reader']) for r in disk}!={(w,x) for w in ('exe','dll') for x in ('exe','dll')} or not all(r['passed'] and r['owner_out']==r['writer'] and r['owner_in']==r['reader'] for r in disk):raise RuntimeError('Interoperabilidad por disco incompleta')
    return dict(load_file_cases=len(load),bounded_load=True,interop_modes=2,disk_interop_modes=4,seek_invalid_origin_cases=len(bad),pairs=sum(COUNTS.values()),pairs_per_function=COUNTS,all_equal=True,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('unlock-file-mgr',PROBE_HASH,analyze,'Desbloqueos: archivos del motor',timeout=300,stop_before_analysis=True,unhook="Audio,Global/SurfaceInfos_c"))
