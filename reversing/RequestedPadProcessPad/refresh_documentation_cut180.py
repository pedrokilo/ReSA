"""Documentary cut180 (CPad::ProcessPad registrada via RequestedPadProcessPad); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
CLASSES=('CPad',)
assert sum(x['validated']=='1' for x in rows)==6916 and sum(pending.values())==1530 and all(pending[c]==0 for c in CLASSES)
GROUPS=len(pending);assert GROUPS==41
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('179 aprobaciones nuevas','180 aprobaciones nuevas'),('179 identidades nuevas','180 identidades nuevas'),('6915 entradas','6916 entradas'),('1531 pendientes','1530 pendientes'),('distribuidas en **42 grupos**','distribuidas en **41 grupos**'),
 ('quedan 1531 en 42 grupos','quedan 1530 en 41 grupos'),('Ciento setenta y nueve aprobaciones nuevas','Ciento ochenta aprobaciones nuevas'),
 ('(TaskFinal2), hora','(TaskFinal2) y CPad::ProcessPad (RequestedPadProcessPad), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/RequestedPadProcessPad/validation-summary.json']
assert [x['hook']['class']+'::'+x['hook']['fn_name'] for x in new]==['CPad::ProcessPad']
newrows=''
for x in new:
 name='CPad::ProcessPad';e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU privado, 11 rutas de dispositivo DirectInput guionizado (sin COM real), 4 variantes de efectos laterales, x87 PC24 y PC53, 64 combinaciones de menú, 4 máscaras de joystick, 41 valores POV y 1536 vectores pseudoaleatorios; RsPadEventHandler trazado, GetPad y FTOL nativos; flotantes finitos, sin NaN; padID solo 0/1; retorno EAX/HRESULT del EXE frente a void de la fuente solo informativo; la fuente de Pad.cpp se corrigió antes (POV con fild con signo y fadd 2^32 como el EXE, y la puerta if (*pDiDevice) solo sobre el bloque de sticks). |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='RequestedPadProcessPad añade la aprobación de CPad::ProcessPad (0x746A10), 17256 pares con cero diferencias y cero discrepancias con el modelo independiente, salida natural 0, saves y configuraciones intactos y restauración del harness (17692 rangos), con el banco de pares frente a la candidata; según hooks.csv CPad queda completa (75/75). Límites: dispositivo DirectInput privado guionizado (Poll, Acquire con reintento por INPUTLOST y GetDeviceState de 272 bytes), sin COM real; RsPadEventHandler trazado (eventos 0x20, 0x20 y 0x1F); GetPad y FTOL nativos en la pierna del EXE (en la candidata compilada GetPad es una llamada a la DLL y la conversión es un cvttss2si en línea), sin crédito para ellos, DirectInput/COM, RsPadEventHandler, integración ni jugabilidad; flotantes finitos y sin NaN (el comiss compilado procede con NaN mientras el EXE salta, inalcanzable); x87 PC24 y PC53; padID distinto de 0/1 fuera del corpus (el EXE desreferencia NULL en 0x746A3A) y bJoyJustInitialised solo 0/1. El EXE devuelve 0 o el HRESULT y la fuente es void: las diferencias de EAX (16392 pares) y de la palabra de estado x87 (15816 pares) son informativas y no cuentan. Antes de esta corrida se corrigió Pad.cpp: (1) POV con fild con signo, cmp/jge y fadd del flotante 2^32 antes de los dos fmul como el EXE (0x746B32..0x746B44), verificado en el ASI compilado; (2) if (*pDiDevice) solo envuelve el bloque de sticks (POV y Z/RZ), de modo que los eventos 0x20 y 0x1F, GetPad y el procesado de ejes son incondicionales como en el EXE (0x746B00 je 0x746BA6). Una primera corrida (20261009-010223-123803) halló 622 diferencias con el dispositivo anulado y queda rechazada. La fuente actual de Pad.cpp (8e61e428...) difiere de la probada en las corridas anteriores de CPad (StartShake_Distance, StartShake_Train, ProcessPCSpecificStuff y UpdateMouse, 89badf36...) por estas correcciones de ProcessPad; el comportamiento de esas funciones no cambia y sus registros no se tocaron. La etiqueta expected_pairs 17000 del manifiesto es obsoleta; el corpus real es 17256. Esos cambios de Pad.cpp seguían sin confirmar en git al registrar. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/RequestedPadProcessPad/validation-summary.json).\n\n'
assert marker in s;s=s.replace(marker,note+marker,1);a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
plan=json.loads((D/'documentation-plan-107.json').read_text());eligible=set(plan['eligible_current_markdown'])
tracked=set(subprocess.check_output(['git','ls-files','--cached'],cwd=R,text=True).splitlines())
for name_ in subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard'],cwd=R,text=True).splitlines():
 if name_.endswith('.md') and not any(part in ('artifacts','build','thirdparty','third-party','runs','preparation-archive','baseline-v1','archive','build-history') or 'archive' in part or part.startswith('baseline-before') for part in Path(name_).parts):
  if name_ in tracked or name_ in eligible:eligible.add(name_)
updated=[]
for name_ in sorted(eligible):
 if name_ in frozen or name_=='docs/CurrentStatus.md':continue
 p=R/name_
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S);link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name_) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6916,'pending':1530,'pending_groups':GROUPS,'current':0,'new_approvals':180}
(HERE/'documentation-refresh-cut180.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
