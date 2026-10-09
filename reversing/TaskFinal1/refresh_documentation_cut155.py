"""Documentary cut155 (diez constructores/destructores de tareas registrados via TaskFinal1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
CLASSES=('CTaskUtilityLineUpPedWithCar','CTaskGoToVehicleAndLean','CTaskGangHassleVehicle','CTaskGangHasslePed','CTaskComplexPlayHandSignalAnim')
assert sum(x['validated']=='1' for x in rows)==6891 and sum(pending.values())==1555 and all(pending[c]==0 for c in CLASSES)
GROUPS=len(pending);assert GROUPS==51
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('145 aprobaciones nuevas','155 aprobaciones nuevas'),('145 identidades nuevas','155 identidades nuevas'),('6881 entradas','6891 entradas'),('1565 pendientes','1555 pendientes'),('distribuidas en **56 grupos**','distribuidas en **51 grupos**'),
 ('quedan 1565 en 56 grupos','quedan 1555 en 51 grupos'),('Ciento cuarenta y cinco aprobaciones nuevas','Ciento cincuenta y cinco aprobaciones nuevas'),
 ('cinco de CMonsterTruck (WorldFinal1), hora','cinco de CMonsterTruck (WorldFinal1) y diez constructores y destructores de cinco tareas (TaskFinal1), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=reg['functions'][-10:]
NAMES=[c+'::'+f for c in CLASSES for f in ('Constructor','Destructor')]
assert [x['hook']['class']+'::'+x['hook']['fn_name'] for x in new]==NAMES
LIM={
 'CTaskUtilityLineUpPedWithCar::Constructor':'vector con valores finitos y especiales (0, -0, ±Inf, NaN silenciosa, desnormal), tiempo y reloj de valores límite o aleatorios; la fuente se corrigió antes: m_fTime = reloj actual + tiempo (0x64FBCD). No es CTask: se compara el objeto completo.',
 'CTaskUtilityLineUpPedWithCar::Destructor':'bytes aleatorios en todo el objeto de 0x1C; sin colaboradores.',
 'CTaskGoToVehicleAndLean::Constructor':'vehículo falso (80 %) o nulo y entero acotado o aleatorio.',
 'CTaskGoToVehicleAndLean::Destructor':'campos aleatorios, referencia al vehículo registrada o no, contador del bloque de animación «gangs» 0, 0xFFFF o 1..4 y subtarea falsa (75 %) o nula.',
 'CTaskGangHassleVehicle::Constructor':'vehículo falso (80 %) o nulo, entero, byte crudo y dos flotantes especiales o finitos; byte 0x51B del vehículo aleatorio.',
 'CTaskGangHassleVehicle::Destructor':'referencia al vehículo, id de posición de acoso en {-1,-2,INT_MIN,0..8,15,31}, segunda referencia nula/peatón/vehículo, byte 0x30, contador del bloque «gangs» 1..4 y subtarea falsa.',
 'CTaskGangHasslePed::Constructor':'peatón falso (80 %) o nulo y tres enteros (0..99 en la mitad de los casos, el resto aleatorio).',
 'CTaskGangHasslePed::Destructor':'referencia al peatón (80 %, registrada en el 75 %), byte 0x1C, contador del bloque «gangs» 1..4 y subtarea falsa; sin cambio de fuente.',
 'CTaskComplexPlayHandSignalAnim::Constructor':'ids de animación límite o 0..999 y flotante especial o finito; la fuente se corrigió antes: el byte 0x14 se limpia con and 0xF8.',
 'CTaskComplexPlayHandSignalAnim::Destructor':'byte 0x14 aleatorio (los bits 0 y 1 eligen la mano), ms_animBlock -1 o válido, contadores del bloque y de los cuatro modelos de manos límite; la fuente se corrigió antes: la referencia de cada mano solo se quita si su bit está activo y los índices izquierda/derecha estaban invertidos.'}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo, objeto en buffer propio con guardas; {LIM[name]} |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='TaskFinal1 añade la aprobación de 10 funciones, el constructor y el destructor de CTaskUtilityLineUpPedWithCar, CTaskGoToVehicleAndLean, CTaskGangHassleVehicle, CTaskGangHasslePed y CTaskComplexPlayHandSignalAnim (3200 pares, 0 diferencias, evento de cierre con state_restored, saves y configuraciones intactos), con el banco simple original (hook apagado) frente a la candidata dentro del juego completo; las cinco clases quedan completas en hooks.csv. Límites: objeto en un buffer propio de 0x80 bytes con guardas, sin usar el pool de tareas (su ocupación se compara y se restaura); se compara el objeto completo salvo la palabra 0 (vptr), que se sustituye por GetTaskType en los constructores y no se compara en los destructores, no se compara el retorno del destructor y no se ejerce el camino del destructor con borrado ni operator delete; peatón y automóvil son buffers falsos a cero; los flotantes incluyen 0, -0, ±Inf, NaN silenciosa y desnormales pero nunca NaN señalizante. Son dobles trazados, sin crédito de esos colaboradores, CBaseModelInfo::RemoveRef (no toca el contador del TXD) y CStreaming::RemoveModel; RemoveAnimBlockRef, GetAnimationBlockIndex, SetHasslePosId, las referencias y las clases base se ejecutan de verdad pero no se validan aquí. Antes de la corrida se corrigió la fuente a lo que hace el EXE: CTaskUtilityLineUpPedWithCar guarda m_fTime = tiempo actual + tiempo (0x64FBCD) y CTaskComplexPlayHandSignalAnim quita la referencia de cada mano solo si su bit está activo (bit 0 izquierda, bit 1 derecha, byte 0x14), con índices izquierda/derecha corregidos y and byte [esi+14h],0F8h en el constructor; se quitaron inicializadores redundantes en CTaskGoToVehicleAndLean y CTaskGangHassleVehicle sin cambio de comportamiento; esos cambios seguían sin confirmar en git al registrar. El README del banco anunciaba 3072 pares y un cambio de fuente en CTaskGangHasslePed que git no muestra; el banco ejecutado tiene 3200 pares. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/TaskFinal1/validation-summary.json).\n\n'
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6891 con evidencia histórica, 0 con vigencia acreditada y 1555 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6891,'pending':1555,'pending_groups':GROUPS,'current':0,'new_approvals':155}
(HERE/'documentation-refresh-cut155.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
