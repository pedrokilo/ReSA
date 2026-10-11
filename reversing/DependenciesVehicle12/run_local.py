"""Equivalencia del trampolin de juego y prueba completa del asignador real compartido."""
import json,struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CVehicle::operator new-anon':2048}
GATES=('stack_restored','nonvolatile_preserved','route_ok','allocator_route_ok','pool_redirect_ok','allocated_pointer_ok','storage_unchanged','release_balanced','globals_restored','floating_state_restored')

def fixture(case):
    profile,value=case%32,case//32;mask=profile%16;generations=(0,1,126,127);ref=(value//5)%4
    return dict(profile=profile,value=value,empty_mask=mask,cursor=value%5-1,size=(0x5a0,0xa18,0x100,0,0xffffffff)[(value//20+profile//16)%5],
                flags=[generations[(ref+n)%4]|(0x80 if mask&(1<<n) else 0) for n in range(4)],storage_pattern=0x30+value%32)

def effects(i,start):
    flags=i['flags'].copy();cursor=i['cursor'];wrapped=False;paths=set();index=-1
    while True:
        cursor+=1
        if cursor==4:
            cursor=0
            if wrapped:break
            wrapped=True
        if flags[cursor]&0x80:
            index=cursor;flags[index]=((flags[index]&0x7f)+1)&0x7f;break
    paths.add('full' if index<0 else 'allocated');paths.add('wrapped' if wrapped else 'not_wrapped')
    if index>=0:paths.add('index_'+str(index));paths.add('ref_'+str(i['flags'][index]&0x7f))
    paths.add('mask_'+str(i['empty_mask']));paths.add('cursor_'+str(i['cursor']));paths.add('size_'+str(i['size']))
    body=bytearray.fromhex(start['initial_pool_bytes']);struct.pack_into('<I',body,12,cursor)
    raw=[struct.pack('<I',start['pool_address']).hex(),body.hex(),bytes(flags).hex(),bytes([i['storage_pattern']])*(4*0xa18)]
    raw[3]=raw[3].hex();cleanup=raw.copy()
    if index>=0:
        flags[index]|=0x80;cleanup[2]=bytes(flags).hex()
    return dict(returned_index=index,eax=0 if index<0 else start['storage_address']+index*0xa18,cursor_after=cursor,state=raw,cleanup_state=cleanup),paths
REQUIRED={'full','allocated','wrapped','not_wrapped'}|{'index_'+str(n) for n in range(4)}|{'ref_'+str(n) for n in (0,1,126,127)}|{'mask_'+str(n) for n in range(16)}|{'cursor_'+str(n) for n in (-1,0,1,2,3)}|{'size_'+str(n) for n in (0x5a0,0xa18,0x100,0,0xffffffff)}

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1:raise RuntimeError('Inicio no unico')
    start=begin[0]
    if not all(start.get(k) is True for k in ('registered_hook','allocator_route_ok','pool_layout_ok','floating_state_restored','real_owned_pool')) or start['capacity']!=4 or start['slot_stride']!=0xa18 or start['allocator_equivalence_cases']!=2048:raise RuntimeError('Rutas/pool incompletos')
    helper=start['allocator_route']
    if helper['entry']!=0x6e2a50 or not all(helper.get(k) is True for k in ('registered','enabled','reconstructed','route_ok')):raise RuntimeError('Asignador DLL real no demostrado')
    game=start['game_pool_snapshot'];capacity=start['game_pool_capacity']
    if len(game)!=3 or len(game[0])!=40 or len(game[1])!=capacity*2 or len(game[2])!=capacity*0xa18*2:raise RuntimeError('Pool real no observado completo')
    body=bytes.fromhex(start['initial_pool_bytes'])
    if len(body)!=20 or struct.unpack_from('<III',body)!=(start['storage_address'],start['flags_address'],4) or body[16:18]!=b'\x01\x00':raise RuntimeError('Layout de pool incorrecto')
    cases={'comparison':set(),'allocator_equivalence':set()};pairs=set();coverage={k:set() for k in cases}
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        event=row['event']
        if event not in cases:continue
        case=row['case']
        if case in cases[event] or not 0<=case<2048:raise RuntimeError('Caso repetido/fuera de dominio')
        cases[event].add(case)
        if event=='comparison':
            if row['function'] not in EXPECTED or row['pair'] in pairs:raise RuntimeError('Funcion/par incorrecto')
            pairs.add(row['pair'])
        if row['original'] is None or row['candidate'] is None or row['original']!=row['candidate'] or row.get('equal') is not True or row.get('accepted') is not True:raise RuntimeError('Diferencia/NULL')
        for side in ('original','candidate'):
            out=row[side];i=fixture(case)
            if out['input']!=i or not all(out.get(k) is True for k in GATES):raise RuntimeError('Fixture/gate incorrecto')
            expected,paths=effects(i,start);coverage[event]|=paths
            if any(out[k]!=v for k,v in expected.items()):raise RuntimeError('Pool/ABI/padding/almacenamiento difiere del oraculo')
            if event=='comparison' and (out.get('argument_unchanged') is not True or out['argument_after']!=i['size']):raise RuntimeError('Argumento fisico modificado')
    if pairs!=set(range(2048)) or any(v!=set(range(2048)) for v in cases.values()):raise RuntimeError('Casos/par incompletos')
    if any(not REQUIRED<=v for v in coverage.values()):raise RuntimeError('Ramas/generaciones/saturacion incompletas')
    allocator=[r for r in rows if r['event']=='allocator_summary']
    if len(allocator)!=1 or allocator[0]['cases']!=2048 or not all(allocator[0].get(k) is True for k in ('state_restored','floating_state_restored')):raise RuntimeError('Asignador no restaurado')
    final=[r for r in rows if r['event']=='complete']
    if len(final)!=1 or final[0]['pairs']!=2048 or final[0]['allocator_equivalence_cases']!=2048 or not all(final[0].get(k) is True for k in ('state_restored','game_pool_untouched','owned_pool_freed','floating_state_restored','allocator_route_ok')):raise RuntimeError('Estado no restaurado/pool sin liberar')
    pool_final=[r for r in rows if r['event']=='game_pool_final']
    if len(pool_final)!=1 or pool_final[0]['bytes']!=start['game_pool_snapshot']:raise RuntimeError('Pool real modificado')
    summary['allocator_equivalence_cases']=2048;summary['complete_paths']={k:sorted(v) for k,v in coverage.items()};summary['coverage_method']='serialized_inputs_and_full_raw_pool_oracle_no_instruction_tracing'
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciesvehicle12',manifest['binaries']['asi'],analyze,'DependenciesVehicle12',timeout=1500))
