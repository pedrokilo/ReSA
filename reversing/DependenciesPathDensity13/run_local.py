"""Metodo readonly de caminos: oracle x87PC24 y fixtures crudas independientes."""
import json,math,struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CPathFind::CalcRoadDensity':2048}
GATES=('stack_restored','nonvolatile_preserved','route_ok','arguments_unchanged','fixture_readonly','actual_path_unchanged','globals_restored','floating_state_restored')
CANARIES=(0xa5,0x5a,0xc3,0xd4,0xe5,0xb6,0xb7)
def f32(v):return struct.unpack('<f',struct.pack('<f',v))[0]
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def floating(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def i16(v):return (v+32768)%65536-32768

def fixture(case):
    p,v=case%64,case//64;mode=p%16;area=p;target=64+p%8 if mode==12 else (p+2)%64;nav=(p+3)%64
    count=0 if mode==1 else 0xffffffff if mode==2 else 3 if mode>=9 and mode!=14 else 1
    cx,cy=(32767,-32768) if mode==12 else ((v%5-2)*128,(v//5%5-2)*128)
    sources=[]
    for n in range(3):
        number=0 if mode==3 else 15 if mode in (8,10) else n+1 if mode==9 else (1,15,2)[n] if mode==15 else 1
        sources.append(dict(x=i16(cx+n*8),y=i16(cy+n*8),links=number,base=n*15,flags=number|((p%16)<<4)))
    targets=[dict(x=cx if mode==11 else i16(cx+(n%5-2)*8),y=cy if mode==11 else i16(cy+(n//5-1)*8)) for n in range(16)]
    navi=[]
    for n in range(15):
        opposite,same=(n+v//16)%8,(2*n+p//16)%8
        if mode==6:opposite=same=0
        if mode==7:opposite=same=7
        if mode==15:opposite,same=0,7 if n%2 else 0
        navi.append(opposite|(same<<3)|((p%4)<<6))
    edges=[dict(area=area if mode==14 else (p+4)%64 if mode==9 and e%2 else target,node=0 if mode==14 else (e+3*n)%15,
                navi_area=(p+5)%64 if mode==10 and e%2 else nav,navi=(e+n)%15) for n in range(3) for e in range(15)]
    x,y=f32(cx*.125),f32(cy*.125);q=v%16;delta=(floating(0x429fffff),80.,floating(0x42a00001))
    if 1<=q<=3:x=f32(x+delta[q-1])
    if 4<=q<=6:y=f32(y+delta[q-4])
    if 7<=q<=9:x=f32(x-delta[q-7])
    if 10<=q<=12:y=f32(y-delta[q-10])
    if q==13:x=floating(0x7fc01234)
    if q==14:y=math.inf
    if q==15:x,y=f32(x+79.),f32(y-79.)
    return dict(profile=p,value=v,mode=mode,source_area=area,target_area=target,navi_area=nav,source_count=count,source_loaded=mode!=0,
                target_loaded=mode!=4,navi_loaded=mode!=5,x_bits=bits(x),y_bits=bits(y),sources=sources,targets=targets,navi=navi,edges=edges)

def raw_fixture(i,layout):
    arrays=[bytearray(bytes([canary])*d['bytes']) for canary,d in zip(CANARIES,layout)]
    for a in arrays:a[16:-16]=bytes(len(a)-32)
    path,source,target,marker,navi,links,nlinks=arrays
    ptr=[d['address']+16 for d in layout]
    put=lambda data,offset,value:struct.pack_into('<I',data,16+offset,value)
    word=lambda data,offset,value:struct.pack_into('<h',data,16+offset,i16(value))
    a,t,n=i['source_area'],i['target_area'],i['navi_area']
    put(path,0x804+4*a,ptr[1] if i['source_loaded'] else 0);put(path,0x10c4+4*a,i['source_count'])
    put(path,0x804+4*t,ptr[2] if i['target_loaded'] else 0);put(path,0x10c4+4*t,16)
    put(path,0x804+4*n,ptr[3] if i['navi_loaded'] else 0);put(path,0x924+4*n,ptr[4]);put(path,0xa44+4*a,ptr[5]);put(path,0xda4+4*a,ptr[6])
    for k,node in enumerate(i['sources']):
        word(source,k*0x1c+8,node['x']);word(source,k*0x1c+0xa,node['y']);word(source,k*0x1c+0x10,node['base']);source[16+k*0x1c+0x18]=node['flags']
    for k,node in enumerate(i['targets']):word(target,k*0x1c+8,node['x']);word(target,k*0x1c+0xa,node['y'])
    for k,flag in enumerate(i['navi']):navi[16+k*0xe+0xb]=flag
    for k,edge in enumerate(i['edges']):put(links,k*4,edge['area']|(edge['node']<<16));word(nlinks,k*2,(edge['navi_area']<<10)|edge['navi'])
    return [a.hex() for a in arrays]

def extended_float(v):
    sign=v>>31;exponent=(v>>23)&255;fraction=v&0x7fffff
    if exponent==0 and fraction==0:return struct.pack('<QH',0,sign<<15).hex()
    if not 0<exponent<255:raise RuntimeError('Retorno fuera del dominio finito del banco')
    return struct.pack('<QH',((1<<23)|fraction)<<40,(sign<<15)|(exponent+16256)).hex()

def effects(i):
    paths={'mode_'+str(i['mode']),'area_'+str(i['source_area'])};acc=0.;nonzero=0
    if not i['source_loaded']:paths.add('null_source')
    elif not 0<i['source_count']<0x80000000:paths.add('nonpositive_count')
    else:
        qx,qy=floating(i['x_bits']),floating(i['y_bits'])
        for node in i['sources'][:i['source_count']]:
            sx,sy=f32(node['x']*.125),f32(node['y']*.125);dxq,dyq=f32(sx-qx),f32(sy-qy)
            for label,diff in (('x',dxq),('y',dyq)):
                paths.add(label+'_unordered' if math.isnan(diff) else label+'_equal' if abs(diff)==80. else label+'_below' if abs(diff)<80. else label+'_above')
            if not abs(dxq)<80.:paths.add('x_excluded');continue
            if not abs(dyq)<80.:paths.add('y_excluded');continue
            if not node['links']:paths.add('no_links');continue
            paths.add('links_'+str(node['links']))
            for edge in i['edges'][node['base']:node['base']+node['links']]:
                if edge['area']==i['source_area']:target=i['sources'][edge['node']];paths.add('same_area_target')
                elif edge['area']==i['target_area'] and i['target_loaded']:target=i['targets'][edge['node']];paths.add('target_loaded')
                else:paths.add('target_missing');continue
                tx,ty=f32(target['x']*.125),f32(target['y']*.125);dx,dy=f32(sx-tx),f32(sy-ty)
                length=f32(math.sqrt(f32(f32(dy*dy)+f32(dx*dx))))
                paths.add('zero_length' if length==0. else 'positive_length')
                if edge['navi_area']!=i['navi_area'] or not i['navi_loaded']:paths.add('navi_missing');continue
                paths.add('navi_loaded');flag=i['navi'][edge['navi']];opposite,same=flag&7,(flag>>3)&7
                paths.add('lanes_zero' if opposite+same==0 else 'lanes_nonzero');paths.add('opposite_'+str(opposite));paths.add('same_'+str(same))
                acc=f32(acc+f32(float(opposite)*length));acc=f32(acc+f32(float(same)*length));nonzero+=int(length>0. and opposite+same>0)
    result=bits(f32(acc*floating(bits(.0004))));paths.add('positive_result' if result else 'zero_result')
    if nonzero>1:paths.add('multiple_contributions')
    return result,paths
REQUIRED={'null_source','nonpositive_count','no_links','target_missing','target_loaded','navi_missing','navi_loaded','same_area_target','zero_length','positive_length','lanes_zero','lanes_nonzero','multiple_contributions','zero_result','positive_result','links_1','links_2','links_3','links_15','x_below','x_equal','x_above','x_unordered','y_below','y_equal','y_above'}|{'mode_'+str(n) for n in range(16)}|{'area_'+str(n) for n in range(64)}|{'opposite_'+str(n) for n in range(8)}|{'same_'+str(n) for n in range(8)}

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begins=[r for r in rows if r['event']=='begin']
    if len(begins)!=1:raise RuntimeError('Begin no unico')
    start=begins[0];layout=start['snapshot_layout']
    if start.get('registered_hook') is not True or start.get('floating_state_restored') is not True or start.get('collaborators_stubbed') is not False or start['collaborators']!=[] or start['constant_bits']!=[bits(.125),bits(80.),bits(.0004)]:raise RuntimeError('Rutas/constantes incorrectas')
    lengths=(0x3ca0,3*0x1c+32,16*0x1c+32,0x1c+32,15*0xe+32,45*4+32,45*2+32)
    if len(layout)!=7 or tuple(d['bytes'] for d in layout)!=lengths:raise RuntimeError('Layout incompleto')
    cases=set();pairs=set();coverage=set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        if row['event']!='comparison':continue
        case,pair=row['case'],row['pair']
        if row['function'] not in EXPECTED or case in cases or pair in pairs:raise RuntimeError('Identificador/funcion duplicado')
        cases.add(case);pairs.add(pair)
        if row['original'] is None or row['candidate'] is None or row['original']!=row['candidate'] or row.get('equal') is not True or row.get('accepted') is not True:raise RuntimeError('Diferencia/NULL')
        i=fixture(case);raw=raw_fixture(i,layout);value,paths=effects(i);coverage|=paths
        for side in ('original','candidate'):
            out=row[side]
            if out['input']!=i or out['state']!=raw or not all(out.get(k) is True for k in GATES):raise RuntimeError('Fixture/memoria/canary/gate incorrecto')
            if out['argument0_after']!=i['x_bits'] or out['argument1_after']!=i['y_bits'] or out['float_bits']!=value or out['extended_return']!=extended_float(value):raise RuntimeError('ABI/oracle ST0 incorrecto')
    if cases!=set(range(2048)) or pairs!=set(range(2048)) or not REQUIRED<=coverage:raise RuntimeError('Cobertura incompleta '+str(REQUIRED-coverage))
    initial=[r for r in rows if r['event']=='actual_path_initial'];final_data=[r for r in rows if r['event']=='actual_path_final']
    if len(initial)!=1 or len(final_data)!=1 or len(initial[0]['bytes'])!=0x3c80*2 or initial[0]['bytes']!=final_data[0]['bytes']:raise RuntimeError('ThePaths real alterado/no observado')
    final=[r for r in rows if r['event']=='complete']
    if len(final)!=1 or final[0]['pairs']!=2048 or not all(final[0].get(k) is True for k in ('state_restored','floating_state_restored','actual_path_unchanged')):raise RuntimeError('Final no restaurado')
    summary['complete_paths']=sorted(coverage);summary['coverage_method']='serialized_inputs_full_raw_fixture_and_PC24_oracle_no_instruction_tracing'
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciespathdensity13',manifest['binaries']['asi'],analyze,'DependenciesPathDensity13',timeout=1500))
