"""Metodo readonly de caminos: oracle x87PC24 y fixtures crudas independientes."""
import json,math,struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CPathFind::FindNodeOrientationForCarPlacement':2048}
GATES=('stack_restored','nonvolatile_preserved','route_ok','fixture_readonly','actual_path_unchanged','globals_restored','floating_state_restored')
CANARIES=(0xa5,0x5a,0xc3,0xd4,0xe5,0xb6,0xb7)
def f32(v):return struct.unpack('<f',struct.pack('<f',v))[0]
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def floating(v):return struct.unpack('<f',struct.pack('<I',v))[0]
def i16(v):return (v+32768)%65536-32768


def fixture(case):
 p,v=case%64,case//64;mode=p%16;a=p;target=64+p%8 if mode==12 else (p+2)%64;nav=(p+3)%64;index=v%3
 cx,cy=(32767,-32768) if mode==12 else ((v%5-2)*128,(v//5%5-2)*128)
 sources=[dict(x=i16(cx+n*8),y=i16(cy+n*8),z=v%9-4,links=mode,base=n*15,flags=mode|((p%16)<<4)) for n in range(3)]
 targets=[dict(x=i16(cx+index*8) if mode==11 else i16(cx+(n%5-2)*8),y=i16(cy+index*8) if mode==11 else i16(cy+(n//5-1)*8),z=n-8) for n in range(16)]
 navi=[]
 for n in range(15):
  op,same=(n+v//16)%8,(2*n+p//16)%8
  if mode==6:op=same=0
  if mode==7:op=same=7
  navi.append(dict(flags=op|(same<<3)|((p%4)<<6),area=a if v%2 else target,node=index if v%3 else (index+1)%3))
 edges=[dict(area=a if mode==14 else target,node=(n+1)%3 if mode==14 else (e+3*n)%15,navi_area=(p+5)%64 if mode==10 and e%2 else nav,navi=(e+n)%15) for n in range(3) for e in range(15)]
 return dict(profile=p,value=v,mode=mode,source_area=a,target_area=target,navi_area=nav,source_loaded=mode!=0 or bool(v%2),target_loaded=mode!=4,navi_loaded=mode!=5,argument=a|(index<<16),sources=sources,targets=targets,navi=navi,edges=edges)
def raw_fixture(i,layout):
 arrays=[bytearray(bytes([canary])*d['bytes']) for canary,d in zip(CANARIES,layout)]
 for a in arrays:a[16:-16]=bytes(len(a)-32)
 path,source,target,marker,navi,links,nlinks=arrays;ptr=[d['address']+16 for d in layout]
 put=lambda data,offset,value:struct.pack_into('<I',data,16+offset,value)
 word=lambda data,offset,value:struct.pack_into('<h',data,16+offset,i16(value))
 a,t,n=i['source_area'],i['target_area'],i['navi_area']
 put(path,0x804+4*a,ptr[1] if i['source_loaded'] else 0);put(path,0x804+4*t,ptr[2] if i['target_loaded'] else 0);put(path,0x804+4*n,ptr[3] if i['navi_loaded'] else 0);put(path,0x924+4*n,ptr[4]);put(path,0xa44+4*a,ptr[5]);put(path,0xda4+4*a,ptr[6])
 for k,node in enumerate(i['sources']):
  for off,key in ((8,'x'),(10,'y'),(12,'z'),(16,'base')):word(source,k*0x1c+off,node[key])
  source[16+k*0x1c+24]=node['flags']
 for k,node in enumerate(i['targets']):
  for off,key in ((8,'x'),(10,'y'),(12,'z')):word(target,k*0x1c+off,node[key])
 for k,node in enumerate(i['navi']):word(navi,k*14+4,node['area']);word(navi,k*14+6,node['node']);navi[16+k*14+11]=node['flags']
 for k,edge in enumerate(i['edges']):put(links,k*4,edge['area']|(edge['node']<<16));word(nlinks,k*2,(edge['navi_area']<<10)|edge['navi'])
 return [a.hex() for a in arrays]
def selection(i):
 paths={'mode_'+str(i['mode']),'area_'+str(i['source_area'])};index=i['argument']>>16;node=i['sources'][index];arg=i['argument']
 if not i['source_loaded']:return arg,paths|{'source_missing','early_zero'}
 if not node['links']:return arg,paths|{'no_links','early_zero'}
 paths.add('links_'+str(node['links']));chosen=0
 while chosen<node['links']-1:
  e=i['edges'][node['base']+chosen]
  if e['navi_area']!=i['navi_area'] or not i['navi_loaded']:paths.add('navi_missing')
  else:
   n=i['navi'][e['navi']];same=n['area']==i['source_area'] and n['node']==index;lanes=(n['flags']>>3)&7 if same else n['flags']&7
   paths.add('same_owner' if same else 'other_owner');paths.add('lanes_'+str(lanes))
   if lanes:paths.add('selected_early');break
   paths.add('zero_lanes_skipped')
  chosen+=1
 if chosen==node['links']-1:paths.add('selected_last')
 e=i['edges'][node['base']+chosen];arg=e['area']|(e['node']<<16);paths.add('chosen_'+str(chosen))
 if e['area']==i['source_area']:target=i['sources'][e['node']];paths.add('same_area_target')
 elif e['area']==i['target_area'] and i['target_loaded']:target=i['targets'][e['node']];paths.add('target_loaded')
 else:return arg,paths|{'target_missing','early_zero'}
 dx,dy=(target[k]-node[k] for k in ('x','y'));paths.add('zero_xy' if dx==dy==0 else 'nonzero_xy');paths.add('normalized_x_'+('zero' if dx==0 and dy!=0 else 'negative' if dx<0 else 'positive'))
 return target['z'] & 0xffffffff,paths|{'returned_angle'} # EXE 0x45046D overwrites physical argument with sign-extended target Z.
REQUIRED={'source_missing','no_links','navi_missing','same_owner','other_owner','zero_lanes_skipped','selected_early','selected_last','same_area_target','target_loaded','target_missing','early_zero','zero_xy','nonzero_xy','returned_angle','normalized_x_zero','normalized_x_negative','normalized_x_positive'}|{'area_'+str(n) for n in range(64)}|{'mode_'+str(n) for n in range(16)}|{'links_'+str(n) for n in range(1,16)}|{'lanes_'+str(n) for n in range(8)}
def helper_fixture(case):
 p,v=case%32,case//32;components=(0.,1.,-1.,3.,4.,.125,-.125,1e10,1e-10)
 return [bits(0. if p==0 else components[(p+v)%9]),bits(0. if p==0 else components[(2*p+v//3)%9]),bits(0. if p==0 else components[(3*p+v//7)%9])]
def helper_expected(i):
 x,y,z=map(floating,i);square=f32(f32(f32(x*x)+f32(y*y))+f32(z*z));out=[bits(x),bits(y),bits(z)]
 if square==0.:out[0]=bits(1.)
 else:
  inverse=f32(1./f32(math.sqrt(square)));out=[bits(f32(v*inverse)) for v in (x,y,z)]
 return (bytes([0xab])*16+b''.join(struct.pack('<I',v) for v in out)+bytes([0xab])*16).hex()
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()];begins=[r for r in rows if r['event']=='begin']
 if len(begins)!=1:raise RuntimeError('Begin no unico')
 start=begins[0];layout=start['snapshot_layout']
 if not all(start.get(k) is True for k in ('registered_hook','helper_route_ok','floating_state_restored')) or start['helper_entry']!=0x59c910 or start['helper_equivalence_cases']!=2048 or start.get('collaborators_stubbed') is not False:raise RuntimeError('Dependencia/rutas incompletas')
 lengths=(0x3ca0,116,480,60,242,212,122)
 if len(layout)!=7 or tuple(d['bytes'] for d in layout)!=lengths:raise RuntimeError('Layout incompleto')
 cases=set();helper_cases=set();pairs=set();coverage=set();helper_paths=set()
 for row in rows:
  e=row['event']
  if e.startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
  if e not in ('comparison','helper_equivalence'):continue
  case=row['case'];active=cases if e=='comparison' else helper_cases
  if case in active or not 0<=case<2048:raise RuntimeError('Caso duplicado/invalido')
  active.add(case)
  if row['original'] is None or row['candidate'] is None or row['original']!=row['candidate'] or row.get('equal') is not True or row.get('accepted') is not True:raise RuntimeError('Diferencia/NULL')
  if e=='helper_equivalence':
   i=helper_fixture(case);expected=helper_expected(i);
   if not 0x59c910<=row['original_fp_ip_raw']<=0x59c960 or not start['normalise_body']<=row['candidate_fp_ip_raw']<start['normalise_body']+512:raise RuntimeError('Cuerpo del helper no ejecutado')
   helper_paths.add('zero' if not any(i) else 'nonzero')
   for side in ('original','candidate'):
    o=row[side]
    if o['input']!=i or o['state']!=expected or not all(o.get(k) is True for k in ('stack_restored','nonvolatile_preserved','route_ok','globals_restored','floating_state_restored')):raise RuntimeError('Helper/ABI/estado difiere del oracle')
    if len(o['floating_observation']['numeric_env'])!=216:raise RuntimeError('Snapshot x87 incompleta')
  else:
   if row['function'] not in EXPECTED or row['pair'] in pairs:raise RuntimeError('Funcion/par invalido')
   pairs.add(row['pair']);i=fixture(case);arg,paths=selection(i);coverage|=paths;raw=raw_fixture(i,layout)
   for side in ('original','candidate'):
    o=row[side]
    if o['input']!=i or o['state']!=raw or o['argument_after']!=arg or not all(o.get(k) is True for k in GATES):raise RuntimeError('Fixture/seleccion/ABI/estado incorrecto')
    if not math.isfinite(floating(o['float_bits'])) or len(o['extended_return'])!=20:raise RuntimeError('Retorno ST0 invalido')
    if 'early_zero' in paths and o['float_bits']!=0:raise RuntimeError('Retorno temprano no cero')
 if cases!=set(range(2048)) or helper_cases!=set(range(2048)) or pairs!=set(range(2048)) or not REQUIRED<=coverage or helper_paths!={'zero','nonzero'}:raise RuntimeError('Cobertura incompleta '+str(REQUIRED-coverage))
 initial=[r for r in rows if r['event']=='actual_path_initial'];last=[r for r in rows if r['event']=='actual_path_final'];hs=[r for r in rows if r['event']=='helper_summary'];final=[r for r in rows if r['event']=='complete']
 if len(initial)!=1 or len(last)!=1 or len(initial[0]['bytes'])!=0x3c80*2 or initial[0]['bytes']!=last[0]['bytes']:raise RuntimeError('ThePaths real modificado/no completo')
 if len(hs)!=1 or hs[0]['cases']!=2048 or hs[0]['failures']!=0 or not all(hs[0].get(k) is True for k in ('state_restored','floating_state_restored')):raise RuntimeError('Helper no probado/restaurado')
 if len(final)!=1 or final[0]['pairs']!=2048 or final[0]['helper_equivalence_cases']!=2048 or not all(final[0].get(k) is True for k in ('state_restored','floating_state_restored','actual_path_unchanged','helper_route_ok')):raise RuntimeError('Final no restaurado')
 summary['helper_equivalence_cases']=2048;summary['complete_paths']=sorted(coverage);summary['coverage_method']='full_raw_fixture_and_selection_oracle_no_instruction_tracing';return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'));sys.exit(runtime_runner.run_case('dependenciespathorientation14',manifest['binaries']['asi'],analyze,'DependenciesPathOrientation14',timeout=1500))
