"""Documentary cut441 (+54 funciones: PathFinal3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 760,
 'old_valid': 7496,
 'old_pend': 950,
 'old_groups': 13,
 'n': 54,
 'groups_drop': 1,
 'old_word': 'Setecientas sesenta',
 'new_word': 'Ochocientas catorce',
 'intro': 'cincuenta y cuatro de CPathFind (PathFinal3)',
 'complete': ['CPathFind'],
 'pending_counts': {},
 'summary': 'reversing/PathFinal3/validation-summary.json',
 'pairs': {'CPathFind::AddDynamicLinkBetween2Nodes_For1Node': 1024,
           'CPathFind::AddInteriorLink': 512,
           'CPathFind::AddInteriorLinkToExternalNode': 256,
           'CPathFind::AddNodeToList': 512,
           'CPathFind::AddNodeToNewInterior': 512,
           'CPathFind::AreNodesLoadedForArea': 1024,
           'CPathFind::CountNeighboursToBeSwitchedOff': 512,
           'CPathFind::FindLinkBetweenNodes': 512,
           'CPathFind::FindNearestExteriorNodeToInteriorNode': 256,
           'CPathFind::FindNodeClosestToCoorsFavourDirection': 1024,
           'CPathFind::FindNodeCoorsForScript-LinkedNode': 1024,
           'CPathFind::FindNodeCoorsForScript-TwoNodes': 1024,
           'CPathFind::FindRegionForCoors': 1024,
           'CPathFind::FindStartPointOfRegion-': 256,
           'CPathFind::FindXCoorsForRegion': 128,
           'CPathFind::FindXRegionForCoors': 1024,
           'CPathFind::FindYCoorsForRegion': 128,
           'CPathFind::FindYRegionForCoors': 1024,
           'CPathFind::GetPathNode': 256,
           'CPathFind::HaveRequestedNodesBeenLoaded': 256,
           'CPathFind::Init': 128,
           'CPathFind::IsAreaNodesAvailable': 256,
           'CPathFind::IsWaterNodeNearby': 768,
           'CPathFind::Load': 256,
           'CPathFind::LoadPathFindData-Area': 512,
           'CPathFind::LoadPathFindData-FromStream': 512,
           'CPathFind::LoadSceneForPathNodes': 256,
           'CPathFind::MakeRequestForNodesToBeLoaded': 512,
           'CPathFind::MarkRegionsForCoors': 1024,
           'CPathFind::MarkRoadNodeAsDontWander': 256,
           'CPathFind::ReInit': 64,
           'CPathFind::ReleaseRequestedNodes': 64,
           'CPathFind::RemoveInterior': 768,
           'CPathFind::RemoveNodeFromList': 256,
           'CPathFind::ReturnInteriorNodeIndex': 256,
           'CPathFind::Save': 256,
           'CPathFind::SetLinksBridgeLights': 512,
           'CPathFind::SetPathsNeededAtPosition': 128,
           'CPathFind::Shutdown': 128,
           'CPathFind::StartNewInterior': 256,
           'CPathFind::SwitchOffNodeAndNeighbours': 1024,
           'CPathFind::SwitchPedRoadsOffInArea': 512,
           'CPathFind::SwitchRoadsOffInArea': 768,
           'CPathFind::SwitchRoadsOffInAreaForOneRegion': 768,
           'CPathFind::TakeWidthIntoAccountForWandering': 512,
           'CPathFind::TestCrossesRoad': 512,
           'CPathFind::TestForPedTrafficLight': 512,
           'CPathFind::These2NodesAreAdjacent': 512,
           'CPathFind::ThisNodeHasToBeSwitchedOff': 128,
           'CPathFind::ThisNodeWillLeadIntoADeadEnd': 512,
           'CPathFind::TidyUpNodeSwitchesAfterMission': 128,
           'CPathFind::UnLoadPathFindData': 256,
           'CPathFind::UnMarkAllRoadNodesAsDontWander': 256,
           'CPathFind::UpdateStreaming': 1024},
 'kind': 'CPU en el juego completo con partida cargada, grafo de nodos en arena propio, pool de vehiculos falso y RwStream/Malloc simulados (banco '
         'simple, passed=true global)',
 'lim': {'CPathFind::AddDynamicLinkBetween2Nodes_For1Node': 'fuente corregida: primer bloque libre (area -1) y comparaciones con signo '
                                                            '(0x44E000/0x44E043); ',
         'CPathFind::AddInteriorLink': 'filas llenas incluidas (el EXE escribe en row[6], 0x44DEE6..0x44DEF7); fuente corregida: filas de 6 bytes y '
                                       'primer byte con signo (0x44DED0); ',
         'CPathFind::AddInteriorLinkToExternalNode': '',
         'CPathFind::AddNodeToList': 'fuente corregida: cubo dist & 511 truncado a 16 bits (0x44D1E0 and eax,1FFh; movsx); ',
         'CPathFind::AddNodeToNewInterior': 'fuente corregida: bDontWander en el array 0x96EC40 y filas de 6 bytes de ConnectsToGiven (0x450E90); ',
         'CPathFind::AreNodesLoadedForArea': 'fuente corregida: sin CRect, regiones sueltas (0x44DD10); ',
         'CPathFind::CountNeighboursToBeSwitchedOff': '',
         'CPathFind::FindLinkBetweenNodes': 'solo enlaces existentes o areas sin cargar (el EXE busca sin limite de m_nNumLinks, '
                                            '0x4513B8..0x4513C7); fuente corregida: array de enlaces del area de la direccion (0x451350); ',
         'CPathFind::FindNearestExteriorNodeToInteriorNode': 'fuente corregida: busqueda sobre ThePaths (0x450F30); ',
         'CPathFind::FindNodeClosestToCoorsFavourDirection': 'mitad alta de la salida normalizada cuando el area es 0xFFFF (basura de pila del EXE); '
                                                             'fuente reescrita con x87 en orden, mejor puntuacion 10000.0 (0x461C4000) (0x44FCE0); ',
         'CPathFind::FindNodeCoorsForScript-LinkedNode': 'fuente corregida: primer enlace en m_pNodeLinks[area][base], normalizacion 2D (0x44E480), '
                                                         'escala ancho*0.0625+2.7 (0x4505E0); ',
         'CPathFind::FindNodeCoorsForScript-TwoNodes': 'fuente corregida: rumbo fpatan(-dx,dy)*57.29578, CVector::Normalise de 3 componentes '
                                                       '(0x59C910), perpendicular derecho (0x450780); ',
         'CPathFind::FindRegionForCoors': 'fuente corregida: (p+3000)*0.0013333333f truncado y acotado a [0,7] (0x44D830); ',
         'CPathFind::FindStartPointOfRegion-': '',
         'CPathFind::FindXCoorsForRegion': '',
         'CPathFind::FindXRegionForCoors': 'fuente corregida: (p+3000)*0.0013333333f truncado y acotado a [0,7] (0x44D890); ',
         'CPathFind::FindYCoorsForRegion': '',
         'CPathFind::FindYRegionForCoors': 'fuente corregida: (p+3000)*0.0013333333f truncado y acotado a [0,7] (0x44D8C0); ',
         'CPathFind::GetPathNode': '',
         'CPathFind::HaveRequestedNodesBeenLoaded': '',
         'CPathFind::Init': 'fuente corregida: NumTempExternalNodes es la global 0x96F02C (0x44D080); ',
         'CPathFind::IsAreaNodesAvailable': '',
         'CPathFind::IsWaterNodeNearby': 'fuente corregida: distancia 2D estricta (0x450E1E..0x450E58); ',
         'CPathFind::Load': '',
         'CPathFind::LoadPathFindData-Area': 'RwStream, CTimer::Suspend/Resume y Malloc simulados (sprintf real, nombre de fichero y gString '
                                             'comparados); fuente corregida: sprintf del CRT (0x452F40) y RwStreamClose(stream,0) (0x452F7D); ',
         'CPathFind::LoadPathFindData-FromStream': 'grafos de los fixtures con <=1 enlace o bidireccionales (*outNext2 nulo, 0x452250); fichero '
                                                   'servido por RwStream simulado; fuente corregida: Malloc/Free del EXE (0x72F420/0x72F430) y '
                                                   'reinicio de 16 enlaces dinamicos del area (0x452C4B); ',
         'CPathFind::LoadSceneForPathNodes': 'fuente corregida: bucle x exterior, y interior, modelo 0x63A7+x+y*8 (0x44DE48..0x44DE5D); ',
         'CPathFind::MakeRequestForNodesToBeLoaded': 'CStreaming::RequestModel/RemoveModel y FindPlayerPed doblados con traza; sin cambios de '
                                                     'fuente; ',
         'CPathFind::MarkRegionsForCoors': 'fuente corregida: ToBeStreamed por columnas [x*8+y] con cuatro regiones sueltas (0x44DB60); ',
         'CPathFind::MarkRoadNodeAsDontWander': 'fuente corregida: solo comprueba el area (0x450560 cmp ax,0FFFFh); ',
         'CPathFind::ReInit': '',
         'CPathFind::ReleaseRequestedNodes': '',
         'CPathFind::RemoveInterior': 'reproduce el desplazamiento erroneo de R* (0x44E272..0x44E27F) con base+k acotado a 4..56; fuente reescrita '
                                      '(0x44E1A0, 0x44E1F1) con CMemoryMgr::Free; ',
         'CPathFind::RemoveNodeFromList': '',
         'CPathFind::ReturnInteriorNodeIndex': 'fuente corregida: sin el parametro unkn (ret 0Ch), 0x451300/0x451321; ',
         'CPathFind::Save': '',
         'CPathFind::SetLinksBridgeLights': 'fuente corregida: limites estrictos x>xMin && x<xMax sin CRect (0x44D960); ',
         'CPathFind::SetPathsNeededAtPosition': '',
         'CPathFind::Shutdown': '',
         'CPathFind::StartNewInterior': '',
         'CPathFind::SwitchOffNodeAndNeighbours': 'grafos bidireccionales o con <=1 enlace por nodo porque el EXE escribe en *outNext2 nulo '
                                                  '(0x452248/0x452250); hook de CountNeighboursToBeSwitchedOff apagado durante esta prueba porque el '
                                                  'EXE conserva ECX (0x4521D0); fuente corregida: vecino con bBackToOriginal (0x4521F4..0x452239) y '
                                                  'CountNeighbours sobre el vecino; ',
         'CPathFind::SwitchPedRoadsOffInArea': 'grafos bidireccionales o con <=1 enlace por nodo (*outNext2 nulo, 0x452248/0x452250); sin cambios de '
                                               'fuente; ',
         'CPathFind::SwitchRoadsOffInArea': 'grafos bidireccionales o con <=1 enlace por nodo (*outNext2 nulo, 0x452248/0x452250); fuente corregida: '
                                            'comparaciones ordenadas como el asm; ',
         'CPathFind::SwitchRoadsOffInAreaForOneRegion': 'grafos bidireccionales o con <=1 enlace por nodo (*outNext2 nulo, 0x452248/0x452250); '
                                                        'fuente corregida: comparaciones ordenadas como el asm y copia de 0x1C bytes; ',
         'CPathFind::TakeWidthIntoAccountForWandering': 'fuente corregida: & 0xF y >> 4 con signo, solo comprueba el area (0x4509A0); ',
         'CPathFind::TestCrossesRoad': '',
         'CPathFind::TestForPedTrafficLight': '',
         'CPathFind::These2NodesAreAdjacent': 'fuente corregida: array de enlaces del area de la direccion (0x44D230); ',
         'CPathFind::ThisNodeHasToBeSwitchedOff': '',
         'CPathFind::ThisNodeWillLeadIntoADeadEnd': 'fuente corregida: comportamiento 0 y >10 (0x44D36B..0x44D378); ',
         'CPathFind::TidyUpNodeSwitchesAfterMission': 'fuente corregida: minimo con signo (0x44D3B0 cmp eax,36h; jl); ',
         'CPathFind::UnLoadPathFindData': 'fuente corregida: CMemoryMgr::Free (0x44D0F0); ',
         'CPathFind::UnMarkAllRoadNodesAsDontWander': '',
         'CPathFind::UpdateStreaming': 'FindPlayerPed solo a traves del doble; fuente corregida: ToBeStreamedForScript global 0x96EF90, pool de '
                                       'vehiculos de la ultima ranura a la primera (0x450B40), rep stos por columnas (0x450CA5/0x450CBF), bucle '
                                       'final x+y*8 (0x450D00..0x450D4D); '},
 'note': 'PathFinal3 añade la aprobación de las cincuenta y cuatro funciones de CPathFind, 26880 pares con cero diferencias. La corrida '
         '20261009-195539-016084 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y configuraciones '
         'intactos, con el banco (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: ThePaths y las '
         'globales vecinas se sobrescriben con datos deterministas en un arena propio y se restauran; areas falsas; pool de vehiculos falso; '
         'RwStreamOpen/Read/Close y CMemoryMgr::Malloc/Free simulados; colaboradores doblados con traza (sin credito). Correcciones de fuente por '
         'función en el README del banco y en el alcance de cada función del registro. No quedan pendientes en CPathFind. Aprobación histórica con '
         'dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/PathFinal3/validation-summary.json).'}
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=CFG['old_appr'];NEW_APPR=OLD_APPR+CFG['n'];OLD_VALID=CFG['old_valid'];OLD_PEND=CFG['old_pend'];OLD_GROUPS=CFG['old_groups']
assert VALID==OLD_VALID+CFG['n'] and PEND==OLD_PEND-CFG['n'],(VALID,PEND)
assert 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS-CFG['groups_drop'],GROUPS
for c in CFG['complete']:assert c not in pending,c
for c,k in CFG['pending_counts'].items():assert pending[c]==k,(c,pending[c])
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
INTRO=', hora de Ciudad de México.**'
assert INTRO in s and s.index(INTRO)<4000
s=s.replace(INTRO,' y '+CFG['intro']+INTRO,1)
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),(CFG['old_word']+' aprobaciones nuevas',CFG['new_word']+' aprobaciones nuevas')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']==CFG['summary']]
assert len(new)==CFG['n'] and {x['hook']['class']+'::'+x['hook']['fn_name']:x['evidence']['pairs'] for x in new}==CFG['pairs']
LIM=CFG['lim']
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | {CFG['kind']}; {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=CFG['note']+chr(10)+chr(10)
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
(HERE/'documentation-refresh-cut441.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
