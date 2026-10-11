"""Metodo readonly de caminos: oracle x87PC24 y fixtures crudas independientes."""
import json,math,struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CStats::LoadStatUpdateConditions':2048}
GATES=('stack_restored','nonvolatile_preserved','route_ok','helpers_route_ok','mode_bytes_ok','file_written','file_readonly','filehandles_balanced','globals_restored','floating_state_restored')
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]

def fixture(case):
 p,v=case%32,case//32;mode=p%8;counts=(0,1,2,7,8,16,31,64,127,128);ids=('-32768','-1','0','32767','65535','2147483647','-2147483648');floats=('0','-0','1.25','-2.5','1e-10','1e10','3.4028234e38');end='\r\n' if mode&2 else '\n';delimiter=',\t' if mode&4 else ' ';file='# Header'+end if v%2 else '';rows=[]
 for n in range(counts[p%10]):
  id=ids[(n+v)%7];number=floats[(2*n+v)%7];condition=('lessthan','morethan','equals')[(n+p)%3];key=f'K{(v*131+n)%1000000:06d}'
  if mode&1 and n%3==0:file+='# Comment'+end
  file+=delimiter.join((id,'STAT',condition,number,key))+end
  if mode&4:file+='  \t'+end
  rows.append(dict(id=id,number=number,condition=condition,key=key))
 if mode&1:file+='# Tail'+end
 return dict(profile=p,value=v,mode=mode,count=counts[p%10],initial_pattern=0xa0+v%16,line_pattern=0xb0+v%16,errno_seed=(0,2,22,34)[v%4],file=file,rows=rows)
def expected_state(i,start):
 state=[bytearray.fromhex(s) for s in start['initial_state']];msg,count,line,directory,root=state
 msg[16:-16]=bytes([i['initial_pattern']])*2048
 for n,row in enumerate(i['rows']):
  offset=16+16*n;struct.pack_into('<H',msg,offset,int(row['id'])&65535);msg[offset+2]=0
  if row['condition'] in ('lessthan','morethan'):msg[offset+3]=int(row['condition']=='morethan')
  struct.pack_into('<I',msg,offset+4,bits(float(row['number'])));key=row['key'].encode()+b'\0';msg[offset+8:offset+8+len(key)]=key
 struct.pack_into('<I',count,0,i['count']);line[:]=bytes([i['line_pattern']])*512
 for physical in i['file'].encode('ascii').splitlines(keepends=True):
  line[:len(physical)+1]=physical+b'\0'
  for n,b in enumerate(line):
   if b==0:break
   if b<32 or b==44:line[n]=32
 path=start['fixture_root'].encode('ascii')+b'\0';root[:]=bytes([0xdd])*128;root[:len(path)]=path;directory[:]=bytes([0xcc])*128;directory[:len(path)]=path
 return [s.hex() for s in state]
REQUIRED={'empty','full','single','multiple','comments','no_comments','LF','CRLF','comma_tab','space','blank','less','more','unknown','zero','negative_zero','fraction','negative','small','large','maximum','id_negative','id_zero','id_positive','id_truncated','text7'}
def domains(i):
 paths={'empty' if not i['count'] else 'single' if i['count']==1 else 'full' if i['count']==128 else 'multiple','comments' if '#' in i['file'] else 'no_comments','CRLF' if i['mode']&2 else 'LF','comma_tab' if i['mode']&4 else 'space'}
 if '  \t' in i['file']:paths.add('blank')
 numeric={'0':'zero','-0':'negative_zero','1.25':'fraction','-2.5':'negative','1e-10':'small','1e10':'large','3.4028234e38':'maximum'}
 for r in i['rows']:
  paths.add({'lessthan':'less','morethan':'more','equals':'unknown'}[r['condition']]);paths.add(numeric[r['number']]);id=int(r['id']);paths.add('id_negative' if id<0 else 'id_zero' if not id else 'id_positive');paths.add('text7')
  if not -32768<=id<=32767:paths.add('id_truncated')
 return paths
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()];starts=[r for r in rows if r['event']=='begin']
 if len(starts)!=1:raise RuntimeError('Begin no unico')
 start=starts[0]
 if not all(start.get(k) is True for k in ('registered_hook','file_scope_ok','floating_state_restored')) or start.get('collaborators_stubbed') is not False or start['crt_sscanf_entry']!=0x8220ad or start['open_mode_bytes']!='726200':raise RuntimeError('Ruta/archivo/CRT incorrecto')
 expected_helpers={0x5387d0,0x538900,0x536f80,0x5389d0,0x5389b0}
 if {h['entry'] for h in start['helpers']}!=expected_helpers or len(start['helpers'])!=5 or not all(all(h.get(k) is True for k in ('registered','enabled','reconstructed','route_ok')) and h['body']>=h['asi_module'] and h['asi_module'] for h in start['helpers']):raise RuntimeError('Colaborador real ON no demostrado')
 layout=start['snapshot_layout']
 if [x['address'] for x in layout]!=[0xb781f0,0xb794d0,0xb71848,0xb71a60,0xb71ae0] or [x['bytes'] for x in layout]!=[2080,4,512,128,128] or [len(x)//2 for x in start['initial_state']]!=[2080,4,512,128,128]:raise RuntimeError('Snapshot incompleta')
 root=start['fixture_root']
 if not root.startswith('E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\dependenciesstatsfile15-fixture-') or not root.endswith('\\') or len(root.encode())>=128:raise RuntimeError('Ruta fuera del clon o sin aislamiento')
 cases=set();pairs=set();coverage=set()
 for row in rows:
  if row['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
  if row['event']!='comparison':continue
  case,pair=row['case'],row['pair']
  if row['function'] not in EXPECTED or case in cases or pair in pairs or not 0<=case<2048:raise RuntimeError('Caso/par/funcion invalido')
  cases.add(case);pairs.add(pair);i=fixture(case);expected=expected_state(i,start);coverage|=domains(i)
  if row['original'] is None or row['candidate'] is None or row['original']!=row['candidate'] or row.get('equal') is not True or row.get('accepted') is not True:raise RuntimeError('Diferencia/NULL')
  for side in ('original','candidate'):
   out=row[side]
   if out['input']!=i or out['state']!=expected or out['open_mode_before']!='726200' or out['open_mode_after']!='726200' or not all(out.get(k) is True for k in GATES):raise RuntimeError('Fixture/parser/memoria/restauracion/ruta incorrecto')
   if len(out['post_floating_state'])!=216:raise RuntimeError('Estado flotante postcall incompleto')
   if out['cwd_crt']!=root[:-1] or out['cwd_win']!=root[:-1]:raise RuntimeError('Directorio efectivo incorrecto')
 if cases!=set(range(2048)) or pairs!=set(range(2048)) or not REQUIRED<=coverage:raise RuntimeError('Cobertura incompleta '+str(REQUIRED-coverage))
 fs=[r for r in rows if r['event']=='global_state_final']
 if len(fs)!=1 or fs[0]['state']!=start['initial_state'] or fs[0]['cwd_crt']!=start['initial_cwd_crt'] or fs[0]['cwd_win']!=start['initial_cwd_win'] or fs[0]['errno']!=start['initial_errno'] or fs[0]['winerror']!=start['initial_winerror'] or fs[0]['floating_state']!=start['initial_fp'] or fs[0]['mxcsr']!=start['initial_mxcsr']:raise RuntimeError('Snapshot final no identica al original')
 final=[r for r in rows if r['event']=='complete']
 if len(final)!=1 or final[0]['pairs']!=2048 or final[0]['open_mode_bytes']!='726200' or not all(final[0].get(k) is True for k in ('state_restored','mode_bytes_ok','file_scope_ok','fixture_files_removed','floating_state_restored','helpers_route_ok')):raise RuntimeError('Estado/archivos no restaurados')
 summary['complete_paths']=sorted(coverage);summary['coverage_method']='real_files_full_raw_state_parser_oracle_no_instruction_tracing';return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'));sys.exit(runtime_runner.run_case('dependenciesstatsfile15',manifest['binaries']['asi'],analyze,'DependenciesStatsFile15',timeout=1500))
