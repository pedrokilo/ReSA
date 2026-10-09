"""Documentary cut145 (CRope y CMonsterTruck registrados via WorldFinal1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
assert sum(x['validated']=='1' for x in rows)==6881 and sum(pending.values())==1565 and pending['CPad']==1 and pending['CRope']==0 and pending['CMonsterTruck']==0
GROUPS=len(pending);assert GROUPS==56
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
DONE='145 aprobaciones nuevas' in s  # reintento tras un fallo de escritura: CurrentStatus ya actualizado
if not DONE:
 for a,b in (('135 aprobaciones nuevas','145 aprobaciones nuevas'),('135 identidades nuevas','145 identidades nuevas'),('6871 entradas','6881 entradas'),('1575 pendientes','1565 pendientes'),('distribuidas en **58 grupos**','distribuidas en **56 grupos**'),
  ('quedan 1576 en 58 grupos','quedan 1565 en 56 grupos'),('Ciento treinta y cinco aprobaciones nuevas','Ciento cuarenta y cinco aprobaciones nuevas'),
  ('Distance/Train/NOP y UpdateMouse de CPad, hora','Distance/Train/NOP y UpdateMouse de CPad y cinco funciones de CRope y cinco de CMonsterTruck (WorldFinal1), hora')):
  assert a in s,a;s=s.replace(a,b)
 s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
 reg=json.loads((R/'reversing/validation-registry.json').read_text());new=reg['functions'][-10:]
 assert [x['hook']['class']+'::'+x['hook']['fn_name'] for x in new]==['CRope::CreateHookObjectForRope','CRope::PickUpObject','CRope::ReleasePickedUpObject','CRope::Remove','CRope::Render','CMonsterTruck::Constructor','CMonsterTruck::BurstTyre','CMonsterTruck::PreRender','CMonsterTruck::ProcessEntityCollision','CMonsterTruck::SetUpWheelColModel']
 LIM={
  'CRope::CreateHookObjectForRope':'tipos de cuerda 0..9 y 255, entidad enganchada nula (y no nula solo como salida anticipada), objeto nuevo con posición en placement o matriz; la fuente se corrigió antes: los tipos 0 y ≥8 salen sin crear.',
  'CRope::PickUpObject':'tipos vehículo/objeto/peatón/otros, estados y flags, objeto previo nulo/otro/mismo, entidad enganchada con y sin matriz; la fuente se corrigió antes: RegisterReference sobre &m_pRopeAttachObject y posición con operator* (0x59C890).',
  'CRope::ReleasePickedUpObject':'tipos y estados aleatorios, objeto enganchado presente (70 %) o nulo; entidad enganchada nunca nula.',
  'CRope::Remove':'objeto enganchado presente o ausente; entidad enganchada nula solo sin objeto enganchado.',
  'CRope::Render':'tipos de cuerda, distancias incluida una NaN silenciosa, guion de visibilidad (primera esfera, solo espejo, ninguna), m_bMirrorActive 0/1; la fuente se corrigió antes: NaN en la distancia y tabla de índices del EXE 0x8CD818; RwIm3D*/RwRenderStateSet/IsSphereVisible son dobles.',
  'CMonsterTruck::Constructor':'7 ids de modelo × 5 valores de createdBy; el constructor de CAutomobile y SetupSuspensionLines son dobles; la vtable se comprueba (EXE 0x8717D8 o DLL) y se normaliza.',
  'CMonsterTruck::BurstTyre':'hueco virtual 44; componente y flag físico aleatorios.',
  'CMonsterTruck::PreRender':'hueco virtual 17; camión/dumper, nodos y ángulo aleatorios; CAutomobile::PreRender, GetWheelPosn, SetTransmissionRotation y SetComponentRotation son dobles.',
  'CMonsterTruck::ProcessEntityCollision':'hueco virtual 22; tipos de entidad, flags, datos de colisión con 0/1/2/4/255 líneas, ProcessColModels doble con distancias NaN y posiciones NaN/Inf; la fuente se corrigió antes: salto con NaN y SetEntityReference sin limpiar la referencia previa.',
  'CMonsterTruck::SetUpWheelColModel':'hueco virtual 43; puntero de modelo de colisión aleatorio.'}
 newrows=''
 for x in new:
  name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
  newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU privado, objetos falsos con guardas; {LIM[name]} |\n"
 pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
 marker='## Pendientes por clase';note='WorldFinal1 añade la aprobación de 10 funciones, 5 de CRope y 5 de CMonsterTruck (10368 pares, 0 diferencias, evento de cierre con state_restored y layout_ok, saves y configuraciones intactos), con el banco simple original (hook apagado) frente a la candidata; las dos clases quedan completas en hooks.csv (CRope 7/7, CMonsterTruck 11/11). Límites: objetos falsos con guardas de 32 bytes, entidades siempre con matriz no nula, sin NaN señalizantes (los NaN de los fixtures son silenciosos), sin pool ni ficheros; son dobles trazados, sin crédito de esos colaboradores, CEntity::RegisterReference, CPhysical::AddToMovingList, CWorld::Add/Remove, CObject::operator new y constructor, DefinedState, CCamera::IsSphereVisible, RwRenderStateSet, RwIm3DTransform/RenderIndexedPrimitive/End, el constructor de CAutomobile, SetupSuspensionLines, CAutomobile::PreRender, GetWheelPosn, SetTransmissionRotation, SetComponentRotation, CCollision::ProcessColModels y CPhysical::AddCollisionRecord. Fuera del dominio: pool de CObject lleno (operator new devuelve nulo y ambos desreferencian), Remove y ReleasePickedUpObject con entidad enganchada nula y objeto enganchado, y CreateHookObjectForRope con entidad enganchada no nula salvo como salida anticipada. Antes de la corrida se corrigió la fuente a lo que hace el EXE: CRope::PickUpObject (RegisterReference sobre &m_pRopeAttachObject y posición con operator* 0x59C890), CRope::CreateHookObjectForRope (tipos 0 y ≥8 salen sin crear), CRope::Render (NaN en la distancia y tabla 0x8CD818) y CMonsterTruck::ProcessEntityCollision (salto con NaN y SetEntityReference sin limpiar la anterior); esos cambios de Rope.cpp y MonsterTruck.cpp seguían sin confirmar en git al registrar. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/WorldFinal1/validation-summary.json).\n\n'
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6881 con evidencia histórica, 0 con vigencia acreditada y 1565 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6881,'pending':1565,'pending_groups':GROUPS,'current':0,'new_approvals':145}
(HERE/'documentation-refresh-cut145.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
