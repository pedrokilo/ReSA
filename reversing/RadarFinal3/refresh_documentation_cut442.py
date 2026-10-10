"""Documentary cut442 (+63 funciones: RadarFinal3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 814,
 'old_valid': 7550,
 'old_pend': 896,
 'old_groups': 12,
 'n': 63,
 'groups_drop': 1,
 'old_word': 'Ochocientas catorce',
 'new_word': 'Ochocientas setenta y siete',
 'intro': 'sesenta y tres de CRadar (RadarFinal3)',
 'complete': ['CRadar'],
 'pending_counts': {},
 'summary': 'reversing/RadarFinal3/validation-summary.json',
 'pairs': {'CRadar::AddBlipToLegendList': 256,
           'CRadar::CalculateBlipAlpha': 256,
           'CRadar::CalculateCachedSinCos': 512,
           'CRadar::ChangeBlipBrightness': 128,
           'CRadar::ChangeBlipColour': 128,
           'CRadar::ChangeBlipDisplay': 128,
           'CRadar::ChangeBlipScale': 128,
           'CRadar::ClearActualBlip-OG': 128,
           'CRadar::ClearBlip': 128,
           'CRadar::ClearBlipForEntity-OG': 128,
           'CRadar::ClipRadarTileCoords': 128,
           'CRadar::DisplayThisBlip': 512,
           'CRadar::Draw3dMarkers': 512,
           'CRadar::DrawAreaOnRadar': 512,
           'CRadar::DrawBlips': 512,
           'CRadar::DrawCoordBlip': 1024,
           'CRadar::DrawLegend': 512,
           'CRadar::DrawMap': 512,
           'CRadar::DrawRadarGangOverlay': 256,
           'CRadar::DrawRadarMap': 256,
           'CRadar::DrawRadarMask': 128,
           'CRadar::DrawRadarSection': 512,
           'CRadar::DrawRadarSectionMap': 256,
           'CRadar::DrawRadarSprite': 256,
           'CRadar::DrawRotatingRadarSprite': 256,
           'CRadar::DrawYouAreHereSprite': 256,
           'CRadar::GetActualBlipArrayIndex': 128,
           'CRadar::GetNewUniqueBlipIndex': 128,
           'CRadar::GetRadarTraceColour': 128,
           'CRadar::GetTextureCorners': 128,
           'CRadar::InitFrontEndMap': 128,
           'CRadar::Initialise': 64,
           'CRadar::IsPointInsideRadar': 256,
           'CRadar::LimitRadarPoint': 256,
           'CRadar::LimitToMap': 256,
           'CRadar::LineRadarBoxCollision': 512,
           'CRadar::Load': 64,
           'CRadar::LoadTextures': 64,
           'CRadar::RemoveMapSection': 128,
           'CRadar::RemoveRadarSections': 64,
           'CRadar::RequestMapSection': 128,
           'CRadar::Save': 64,
           'CRadar::SetBlipAlwaysDisplayInZoom': 128,
           'CRadar::SetBlipEntryExit': 128,
           'CRadar::SetBlipFade': 128,
           'CRadar::SetBlipFriendly': 128,
           'CRadar::SetBlipSprite': 128,
           'CRadar::SetCoordBlip': 256,
           'CRadar::SetCoordBlipAppearance': 128,
           'CRadar::SetEntityBlip': 256,
           'CRadar::SetMapCentreToPlayerCoords': 128,
           'CRadar::SetShortRangeCoordBlip': 256,
           'CRadar::SetupAirstripBlips': 512,
           'CRadar::SetupRadarRect': 64,
           'CRadar::ShowRadarMarker': 128,
           'CRadar::ShowRadarTrace': 256,
           'CRadar::ShowRadarTraceWithHeight': 512,
           'CRadar::Shutdown': 64,
           'CRadar::StreamRadarSections-vec': 256,
           'CRadar::StreamRadarSections-xy': 128,
           'CRadar::TransformRadarPointToRealWorldSpace': 128,
           'CRadar::TransformRadarPointToScreenSpace': 256,
           'CRadar::TransformRealWorldPointToRadarSpace': 256},
 'kind': 'CPU en el juego completo con partida cargada, motor RW, pools y dibujo doblados con traza (banco simple, passed=true global)',
 'lim': {'CRadar::AddBlipToLegendList': '',
         'CRadar::CalculateBlipAlpha': 'fuente corregida: constante 0x85F0A0; ',
         'CRadar::CalculateCachedSinCos': 'objetivo sin matriz con dobles de AllocateMatrix/UpdateMatrix; fuente corregida: fpatan/fsin sin '
                                          'redondear; ',
         'CRadar::ChangeBlipBrightness': 'fuente corregida: 0x583CAE; ',
         'CRadar::ChangeBlipColour': '',
         'CRadar::ChangeBlipDisplay': '',
         'CRadar::ChangeBlipScale': '',
         'CRadar::ClearActualBlip-OG': '',
         'CRadar::ClearBlip': '',
         'CRadar::ClearBlipForEntity-OG': '',
         'CRadar::ClipRadarTileCoords': '',
         'CRadar::DisplayThisBlip': 'GetBlipName con sprites 0,1,2,4,56 o >63 fuera de dominio (NOTSA_UNREACHABLE en la DLL); ',
         'CRadar::Draw3dMarkers': 'FindPlayerPed siempre valido (el EXE desreferencia [eax+2Fh] sin comprobar nulo, 0x585F37); fuente corregida: '
                                  'Nevada 0.6, orden de suma, enex; ',
         'CRadar::DrawAreaOnRadar': 'solo rectangulos ordenados porque ClipRadarPoly (0x585040) sobrepasa un arreglo local de 8 vertices con '
                                    'rectangulo invertido (0x5851F4, 0x585300) y el EXE no ordena (0x585431..0x5855B9); UV enmascaradas por memoria '
                                    'sin inicializar (0x585662..0x5856B5), solo se traza posicion y color; fuente corregida: rechazo 0x5853E2; ',
         'CRadar::DrawBlips': 'FindPlayerPed siempre valido porque el EXE desreferencia [eax+2Fh] sin comprobar nulo (0x585CA7); fuente corregida: '
                              'orden isSprite, jugador 0, pi, color 50/50/80, ancho=alto; ',
         'CRadar::DrawCoordBlip': 'fuente corregida: apariencia, alfa, marcador de depuracion, alturas; ',
         'CRadar::DrawLegend': 'fuente corregida: 0x582E62; ',
         'CRadar::DrawMap': 'FindPlayerPed siempre valido (el EXE desreferencia [eax+2Fh] sin comprobar nulo, 0x583B50); fuente corregida: sin '
                            'FindPlayerPed inicial, 16.67, coop; ',
         'CRadar::DrawRadarGangOverlay': 'fuente corregida: constante 0x865034; ',
         'CRadar::DrawRadarMap': 'vehiculos con matriz (0x586A76); fuente corregida: pi double, fpatan; ',
         'CRadar::DrawRadarMask': 'fuente corregida: cuadrado previo a zBufferFar y esquinas; ',
         'CRadar::DrawRadarSection': 'las UV si se trazan; fuente corregida contra el EXE; ',
         'CRadar::DrawRadarSectionMap': '',
         'CRadar::DrawRadarSprite': '',
         'CRadar::DrawRotatingRadarSprite': 'fuente corregida: constantes del EXE; ',
         'CRadar::DrawYouAreHereSprite': 'fuente corregida: 500 ms (0x58497B) / 200 ms (0x58499E); ',
         'CRadar::GetActualBlipArrayIndex': '',
         'CRadar::GetNewUniqueBlipIndex': '',
         'CRadar::GetRadarTraceColour': '',
         'CRadar::GetTextureCorners': '',
         'CRadar::InitFrontEndMap': '',
         'CRadar::Initialise': 'fuente corregida: contador = 1; ',
         'CRadar::IsPointInsideRadar': 'fuente corregida: comparacion inclusiva; ',
         'CRadar::LimitRadarPoint': '',
         'CRadar::LimitToMap': 'fuente corregida: ftol + movsx (0x583351); ',
         'CRadar::LineRadarBoxCollision': 'fuente corregida contra el EXE; ',
         'CRadar::Load': '',
         'CRadar::LoadTextures': '',
         'CRadar::RemoveMapSection': 'fuente corregida: orden x,y y RemoveModel; ',
         'CRadar::RemoveRadarSections': 'fuente corregida: orden x,y y RemoveModel; ',
         'CRadar::RequestMapSection': 'fuente corregida: orden x,y; ',
         'CRadar::Save': '',
         'CRadar::SetBlipAlwaysDisplayInZoom': '',
         'CRadar::SetBlipEntryExit': '',
         'CRadar::SetBlipFade': '',
         'CRadar::SetBlipFriendly': '',
         'CRadar::SetBlipSprite': '',
         'CRadar::SetCoordBlip': 'fuente corregida: byte de banderas = 3 (0x5838D7); ',
         'CRadar::SetCoordBlipAppearance': 'fuente corregida: tipo COORD, bits 6-7 del primer byte (0x583E8C); ',
         'CRadar::SetEntityBlip': 'fuente corregida: byte de banderas = 3 (0x583A60); ',
         'CRadar::SetMapCentreToPlayerCoords': '',
         'CRadar::SetShortRangeCoordBlip': '',
         'CRadar::SetupAirstripBlips': 'fuente corregida: contador de fotogramas y empates; ',
         'CRadar::SetupRadarRect': '',
         'CRadar::ShowRadarMarker': '',
         'CRadar::ShowRadarTrace': 'fuente corregida: 7 dwords, rectangulos +-1; ',
         'CRadar::ShowRadarTraceWithHeight': '',
         'CRadar::Shutdown': '',
         'CRadar::StreamRadarSections-vec': 'fuente corregida: orden x,y y RemoveModel; ',
         'CRadar::StreamRadarSections-xy': 'fuente corregida: orden x,y y RemoveModel; ',
         'CRadar::TransformRadarPointToRealWorldSpace': 'fuente corregida: reciproco y orden x87; ',
         'CRadar::TransformRadarPointToScreenSpace': 'funcion naked con retorno por puntero oculto; fuente corregida: reciproco y orden x87; ',
         'CRadar::TransformRealWorldPointToRadarSpace': 'fuente corregida: reciproco y orden x87; '},
 'note': 'RadarFinal3 añade la aprobación de las sesenta y tres funciones de CRadar, 15040 pares con cero diferencias. La corrida '
         '20261009-203202-095801 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, saves y configuraciones '
         'intactos, con el banco (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: motor RW falso '
         '(zBufferFar, estados, Im2D), pools falsos de TXD, EntryExit, vehiculos, peds y objetos; dibujo y TXD doblados con traza (sin credito); '
         'colores y alturas enmascarados a 8 bits; FindPlayerPed/Vehicle no se trazan. Correcciones de fuente por función en el README del banco y '
         'en el alcance de cada función del registro. No quedan pendientes en CRadar. Aprobación histórica con dependency_review=null; la vigencia '
         'sigue en 0. [Resumen](../reversing/RadarFinal3/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut442.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
