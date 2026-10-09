"""Documentary cut424 (+1 funcion por aprobacion parcial: CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> via ShadowsWave3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=423;NEW_APPR=424;OLD_VALID=7159;OLD_PEND=1287;OLD_GROUPS=32
assert VALID==OLD_VALID+1 and PEND==OLD_PEND-1,(VALID,PEND)
assert 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS,GROUPS
assert pending['CShadows']==1 and pending['CRunningScript']==1
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),('Cuatrocientas veintitrés aprobaciones nuevas','Cuatrocientas veinticuatro aprobaciones nuevas'),
 ('(RoadScriptWave3, aprobación parcial por función), hora','(RoadScriptWave3, aprobación parcial por función) y una de CShadows (ShadowsWave3, aprobación parcial por función), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/ShadowsWave3/validation-summary.json']
assert len(new)==1 and new[0]['evidence']['pairs']==1024
LIM={'CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>>':'fuente corregida antes (instancia de plantilla CPtrListSingleLink<CPhysical*> con reinterpret_cast; 0x70A57F índice de modelo con signo movsx); rama con matriz nula (0x70A4F2 AllocateMatrix) y scan code 0xFFFF fuera; entidades falsas con vtable falsa y pool de nodos local sustituyendo POOLS[0]; CastShadowEntityXY doblada; '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con entidades, modelos, matrices y nodos falsos y colaborador doblado con traza (aprobación parcial por función; la corrida global tiene passed=false por CShadows::RenderStoredShadows, rechazada); {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=('ShadowsWave3 añade la aprobación PARCIAL, por función, de CShadows::CastPlayerShadowSectorList<CPtrListSingleLink<CPhysical*>> (0x70A470), 1024 pares con cero diferencias, banco simple (hook de la función apagado) frente a la candidata dentro del juego completo con partida cargada. La corrida 20261009-105057-302560 (1536 pares) tiene passed=false por CShadows::RenderStoredShadows (318/512), RECHAZADA: no se registra ni recibe crédito. El banco no escribe el evento complete cuando hay diferencias; cerró con un evento error (1536 pares, state_restored=true, sin motivo), es decir, la corrida terminó con normalidad; saves y configuraciones intactos. '
 'Según hooks.csv CShadows conserva 1 pendiente (RenderStoredShadows). '
 'Correcciones de fuente previas a la corrida (Shadows.cpp): instancia de plantilla (CSector::Buildings es CPtrListSingleLink<CBuilding*>; reinterpret_cast a <CPhysical*>) y 0x70A57F movsx edx,word [esi+22h] (índice de modelo con signo); el resto del bucle coincide línea a línea con el EXE sin cambios. '
 'Límites: rama con matriz nula (0x70A4F2 AllocateMatrix) y scan code 0xFFFF fuera del banco; entidades falsas con vtable falsa y pool de nodos local sustituyendo POOLS[0]; CastShadowEntityXY doblada sin crédito. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/ShadowsWave3/validation-summary.json).')+chr(10)+chr(10)
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; {VALID} con evidencia histórica, 0 con vigencia acreditada y {PEND} pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p_,h in frozen.items():assert sha(R/p_)==h,p_
snapshot={'historical':VALID,'pending':PEND,'pending_groups':GROUPS,'current':0,'new_approvals':NEW_APPR}
(HERE/'documentation-refresh-cut424.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
