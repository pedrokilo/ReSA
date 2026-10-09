"""Documentary cut135 (CPad::UpdateMouse registered via PadMouse1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
assert sum(x['validated']=='1' for x in rows)==6871 and sum(pending.values())==1575 and pending['CPad']==1
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('134 aprobaciones nuevas','135 aprobaciones nuevas'),('134 identidades nuevas','135 identidades nuevas'),('6870 entradas','6871 entradas'),('1576 pendientes','1575 pendientes'),('Ciento treinta y cuatro aprobaciones nuevas','Ciento treinta y cinco aprobaciones nuevas'),
 ('Distance/Train/NOP de CPad, hora','Distance/Train/NOP y UpdateMouse de CPad, hora'),
 ('# Estado comprobado de ReSA · 8 de octubre de 2026','# Estado comprobado de ReSA · 9 de octubre de 2026'),('**Corte documental: 8 de octubre de 2026,','**Corte documental: 9 de octubre de 2026,'),
 ('UpdateMouse V2 quedó compilado con revisión estática de ABI y calls, sin replay ni aprobación.','UpdateMouse V2 quedó compilado con revisión estática de ABI y calls, sin replay ni aprobación; la aprobación de CPad::UpdateMouse proviene del banco simple PadMouse1 (ver más abajo).')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());x=reg['functions'][-1];assert x['hook']['fn_name']=='UpdateMouse'
name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
row=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU privado, 6 rutas (sin foreground, init con dispositivo nulo, E_FAIL, 0, HRESULT 1, E_ACCESSDENIED) × 12 estados DIMOUSESTATE2 × 8 combinaciones menú/invertir X/Y × 4 semillas × dispositivo previo sí/no × 2 relojes; sin NaN/Inf/INT_MIN y sin COM real (dispositivo = token opaco); diMouseInit, GetMouseState, IsForegroundApp e IsVideoModeExclusive stubeados y trazados (sin crédito de esos helpers); bytes de relleno 7/27/47 de CMouseControllerState no comparados; x87 a 24 bits. |"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+row+s[pos:]
marker='## Pendientes por clase';note='PadMouse1 añade la aprobación de CPad::UpdateMouse (9216 pares, 0 diferencias, evento de cierre con state_restored, saves y configuraciones intactos), con el banco simple original (hook apagado) frente a la candidata. Límites: 6 rutas × 12 estados DIMOUSESTATE2 × 8 combinaciones menú/invertir X/Y × 4 semillas × dispositivo previo sí/no × 2 relojes; sin NaN/Inf/INT_MIN; sin COM real (el dispositivo es un token opaco); diMouseInit, GetMouseState, IsForegroundApp e IsVideoModeExclusive se sustituyen por stubs trazados y no reciben crédito; los bytes de relleno 7, 27 y 47 de CMouseControllerState no se comparan (MouseBytes() los pone a 0; el EXE propaga el byte 7 con rep movs); los stubs ignoran hilos ajenos; x87 forzado a 24 bits. Un intento previo (20261009-000614-797633) dio 579 diferencias en 1752 pares y una violación de acceso; todo era ruido del banco (relleno y trazas del hilo de audio), corregido en LiveComparison.hpp, sin diferencia real fuente/EXE. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. CPad queda con 1 pendiente (ProcessPad). [Resumen](../reversing/PadMouse1/validation-summary.json).\n\n'
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6871 con evidencia histórica, 0 con vigencia acreditada y 1575 pendientes en {len(pending)} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6871,'pending':1575,'pending_groups':len(pending),'current':0,'new_approvals':135}
(HERE/'documentation-refresh-cut135.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
