"""Documentary cut425 (+1 funcion: CTrain::ProcessControl via TrainWave3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=424;NEW_APPR=425;OLD_VALID=7160;OLD_PEND=1286;OLD_GROUPS=32
assert VALID==OLD_VALID+1 and PEND==OLD_PEND-1,(VALID,PEND)
assert 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS-1,GROUPS
assert 'CTrain' not in pending and pending['CShadows']==1 and pending['CRunningScript']==1
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),('Cuatrocientas veinticuatro aprobaciones nuevas','Cuatrocientas veinticinco aprobaciones nuevas'),
 ('(ShadowsWave3, aprobación parcial por función), hora','(ShadowsWave3, aprobación parcial por función) y una de CTrain (TrainWave3), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/TrainWave3/validation-summary.json']
assert len(new)==1 and new[0]['evidence']['pairs']==1024
LIM={'CTrain::ProcessControl':'fuente corregida antes (banderas y bNotOnARailRoad por vagón, parada *0.02f, pow del EXE, producto escalar z,x,y, rumbo fpatan con pi del EXE, velocidad sqrt, flotabilidad con la fuerza devuelta, FindPlayerPed solo en estados 0 y 2, distancia de nodo *0.33333334f); unos 25 colaboradores doblados; fuera: vagón con m_matrix nulo en la rama sobre vía, m_pRwObject real, GetLightingFromCollision sin iluminación, valores fuera de dominio y NaN; '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con tren de 1 a 4 vagones falsos, vía falsa de 6 a 72 nodos, modelo y handling falsos y colaboradores doblados con traza; virtual (hueco 10 de la vtable 0x872370, cuerpo 0x6F86A0; dirección 0 en hooks.csv); {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=('TrainWave3 añade la aprobación de CTrain::ProcessControl (virtual, hueco 10 de la vtable 0x872370 = 0x6F86A0; hooks.csv la lista con dirección 0), 1024 pares con cero diferencias, evento complete con state_restored=true y layout_ok=true, evento layout correcto, saves y configuraciones intactos, con el banco simple (hook de la función apagado) frente a la candidata dentro del juego completo con partida cargada. Con esta función CTrain queda COMPLETA en hooks.csv (era su única pendiente). '
 'Correcciones de fuente previas a la corrida (Train.cpp): banderas por vagón or/and [eax+5B8h] (0x6F88D0, 0x6F8910, 0x6F8940); bNotOnARailRoad por vagón (0x6F8DF0); velocidad de parada fmul 0.02f (0x6F8991); pow del EXE 0x822130 con base double sin redondear (0x6F8BF2, 0x6F8E49, 0x6F9D1B); producto escalar de nodos en orden z,x,y (0x6F8D93); rumbo fpatan menos rumbo viejo con las constantes pi del EXE (0x6F9626); m_fMovingSpeed sqrt((dz2+dy2)+dx2) (0x6F9910); flotabilidad con la fuerza devuelta por ProcessBuoyancy (0x6F9CA6..0x6F9DA2); FindPlayerPed(-1) solo en los estados 0 y 2 (0x6F8EAD, 0x6F8F13); distancia de nodo uint16*0.33333334f (0x859040), también en CTrainNode::GetDistanceFromStart (TrainNode.cpp, 0x6F54B0; ese fichero no figura en el manifiesto de la sonda, su equivalencia con lo probado se infiere por fechas). '
 'Límites: unos 25 colaboradores doblados con trazas y sin crédito; FUERA del banco el vagón con m_matrix nulo en la rama sobre vía, m_pRwObject real (nulo), GetLightingFromCollision sin iluminación encontrada, valores fuera de dominio y NaN; tren falso de 1 a 4 vagones y vía falsa de 6 a 72 nodos; el doble de GetLightingFromCollision tiene una versión distinta para la DLL (retorno por puntero oculto). Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/TrainWave3/validation-summary.json).')+chr(10)+chr(10)
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
(HERE/'documentation-refresh-cut425.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
