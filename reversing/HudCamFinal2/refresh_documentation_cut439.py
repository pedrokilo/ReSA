"""Documentary cut439 (+9 funciones: HudCamFinal2); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 742,
 'old_valid': 7478,
 'old_pend': 968,
 'old_groups': 17,
 'n': 9,
 'groups_drop': 3,
 'old_word': 'Setecientas cuarenta y dos',
 'new_word': 'Setecientas cincuenta y una',
 'intro': 'nueve de CHud, CCam y CPickups (HudCamFinal2)',
 'complete': ['CHud', 'CCam', 'CPickups'],
 'pending_counts': {},
 'summary': 'reversing/HudCamFinal2/validation-summary.json',
 'pairs': {'CHud::Draw': 1024,
           'CHud::DrawAfterFade': 512,
           'CHud::DrawRadar': 1024,
           'CHud::DrawWeaponIcon': 768,
           'CHud::Initialise': 64,
           'CHud::Shutdown': 64,
           'CCam::Process_1stPerson': 1024,
           'CCam::Process_Rocket': 1024,
           'CPickups::Update': 1024},
 'kind': 'CPU en el juego completo con partida cargada, objetos falsos de HUD, CCam y pickups y colaboradores doblados con traza (banco simple, passed=true '
         'global)',
 'lim': {'CCam::Process_1stPerson': 'objeto CCam falso (0x238 bytes con 0xCD, comparado entero), ped/vehiculo/ModelInfo falsos; huesos, linea de vision, nivel '
                                    'de agua y matrices doblados con traza; tiempos >= 2^31 ejercitan la suma fild+2^32; fuente corregida: una sola redondeada '
                                    'fild + fadd 2^32 (0x517F4E..0x517F5E) y movsx del modelo del vehiculo (0x5180EB); ',
         'CCam::Process_Rocket': 'objeto CCam falso, objetivo de misil, jugador y camara RW falsos; linea de vision, motion blur, matrices y efectos de arma '
                                 'con colaboradores doblados con traza; fuente corregida: GetATanOfXY devuelve en ST0 y fsub halfPi (0x511E99); ',
         'CHud::Draw': 'dibujo RW con tabla falsa de estados (RwRenderStateSet por el puntero 0xC97B24), colaboradores doblados con traza; fuente corregida: '
                       'cmp byte ds:[0BAA45Ch],1 (0x58FCA6); ',
         'CHud::DrawAfterFade': 'dibujo RW con tabla falsa de estados, sprites y colaboradores doblados con traza; ',
         'CHud::DrawRadar': 'dibujo RW con tabla falsa de estados; los vehiculos del anillo del avion llevan siempre matriz (el EXE desreferencia [eax+14h] '
                            'sin comprobar, 0x58A3ED); colaboradores doblados con traza; el estado 7 solo cambia de nombre (mismo valor); ',
         'CHud::DrawWeaponIcon': 'una ranura de TXD libre NO se prueba (el EXE lee la direccion 0, 0x58D825/0x58D831; la fuente retorna); solo ranuras en uso '
                                 'con diccionario y textura falsos o nulos; RenderOneXLUSprite y CSprite2d::Draw doblados; tabla falsa de estados RW; ',
         'CHud::Initialise': 'colaboradores de TXD y sprites doblados con traza (nunca se carga HUD.TXD, no se crea ni destruye ninguna textura, el pool de '
                             'TXD no se toca); ReInitialise es real; ',
         'CHud::Shutdown': 'colaboradores de TXD y sprites doblados con traza (no se destruye ninguna textura, el pool de TXD no se toca); ',
         'CPickups::Update': 'CPickup::Update esta DOBLADA (puede anular el tipo o la visibilidad del pickup), igual que IsVisible, GiveUsAPickUpObject, '
                             'CWorld::Add/Remove y los colaboradores de CPad/CPedIntelligence; CollectedPickUpIndex >= 20 NO se prueba (el EXE escribe fuera '
                             'de la tabla, la DLL aborta en depuracion); array real de pickups copiado y restaurado; fuente corregida: la rama del jugador 2 '
                             'no es un else (0x459033 je 0x459069); '},
 'note': 'HudCamFinal2 añade la aprobación de las nueve funciones de CHud (6), CCam (2) y CPickups (1), 6528 pares con cero diferencias. La corrida '
         '20261009-180840-287896 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, evento layout correcto, saves y '
         'configuraciones intactos, con el banco simple (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: '
         'CHud::DrawWeaponIcon con ranura de TXD libre no se prueba (el EXE lee la dirección 0, 0x58D831); CPickups::Update con CollectedPickUpIndex>=20 no se '
         'prueba y con CPickup::Update doblada; el dibujo RW se hace con una tabla falsa de estados de render (0xC97B24, restaurada); objetos falsos de CCam, '
         'peds y vehículos; colaboradores doblados con traza (sin crédito). Correcciones de fuente: CHud::Draw (0x58FCA6), CCam::Process_1stPerson (0x517F4E, '
         '0x5180EB), CCam::Process_Rocket (0x511E99) y CPickups::Update (0x459033). No quedan pendientes en CHud, CCam ni CPickups. Aprobación histórica con '
         'dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/HudCamFinal2/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut439.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
