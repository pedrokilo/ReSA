"""Documentary cut434 (+36 funciones: CamHud1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 457,
 'old_valid': 7193,
 'old_pend': 1253,
 'old_groups': 23,
 'n': 36,
 'groups_drop': 0,
 'old_word': 'Cuatrocientas cincuenta y siete',
 'new_word': 'Cuatrocientas noventa y tres',
 'intro': 'treinta y seis de CHud y CCam (CamHud1)',
 'complete': [],
 'pending_counts': {'CCam': 2, 'CHud': 6},
 'summary': 'reversing/CamHud1/validation-summary.json',
 'pairs': {'CHud::HelpMessageDisplayed': 64,
           'CHud::GetYPosBasedOnHealth': 512,
           'CHud::SetMessage': 320,
           'CHud::SetBigMessage': 1024,
           'CHud::SetVehicleName': 64,
           'CHud::SetZoneName': 256,
           'CHud::ResetWastedText': 64,
           'CHud::GetRidOfAllHudMessages': 256,
           'CHud::ReInitialise': 128,
           'CHud::SetHelpMessage': 768,
           'CHud::SetHelpMessageStatUpdate': 512,
           'CHud::SetHelpMessageWithNumber': 512,
           'CHud::DrawFadeState': 1024,
           'CHud::RenderArmorBar': 512,
           'CHud::RenderBreathBar': 512,
           'CHud::RenderHealthBar': 512,
           'CHud::DrawAmmo': 1024,
           'CHud::DrawBustedWastedMessage': 768,
           'CHud::DrawAreaName': 1024,
           'CHud::DrawVehicleName': 1024,
           'CHud::DrawMissionTitle': 768,
           'CHud::DrawOddJobMessage': 768,
           'CHud::DrawScriptText': 512,
           'CHud::DrawTripSkip': 128,
           'CCam::WellBufferMe': 1024,
           'CCam::DoCamBump': 128,
           'CCam::GetVectorsReadyForRW': 128,
           'CCam::Constructor': 64,
           'CCam::Init': 64,
           'CCam::CacheLastSettingsDWCineyCam': 128,
           'CCam::Finalise_DW_CineyCams': 512,
           'CCam::GetCoreDataForDWCineyCamMode': 256,
           'CCam::Get_TwoPlayer_AimVector': 768,
           'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown': 768,
           'CCam::Process_Editor': 1024,
           'CCam::Process_Fixed': 768},
 'kind': 'CPU en el juego completo con partida cargada, peds/vehículos/cámara RW falsos y colaboradores doblados con traza (banco simple, passed=true global)',
 'lim': {'CCam::CacheLastSettingsDWCineyCam': '',
         'CCam::Constructor': '',
         'CCam::DoCamBump': '',
         'CCam::Finalise_DW_CineyCams': '',
         'CCam::GetCoreDataForDWCineyCamMode': '',
         'CCam::GetVectorsReadyForRW': '',
         'CCam::Get_TwoPlayer_AimVector': '',
         'CCam::Init': '',
         'CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown': '',
         'CCam::Process_Editor': '',
         'CCam::Process_Fixed': '',
         'CCam::WellBufferMe': '',
         'CHud::DrawAmmo': '',
         'CHud::DrawAreaName': '',
         'CHud::DrawBustedWastedMessage': '',
         'CHud::DrawFadeState': '',
         'CHud::DrawMissionTitle': '',
         'CHud::DrawOddJobMessage': '',
         'CHud::DrawScriptText': '',
         'CHud::DrawTripSkip': '',
         'CHud::DrawVehicleName': 'dominio con estados de HUD 0..4 (el EXE deja el alfa sin inicializar fuera de 1..4); ',
         'CHud::GetRidOfAllHudMessages': '',
         'CHud::GetYPosBasedOnHealth': '',
         'CHud::HelpMessageDisplayed': '',
         'CHud::ReInitialise': '',
         'CHud::RenderArmorBar': '',
         'CHud::RenderBreathBar': '',
         'CHud::RenderHealthBar': '',
         'CHud::ResetWastedText': '',
         'CHud::SetBigMessage': '',
         'CHud::SetHelpMessage': '',
         'CHud::SetHelpMessageStatUpdate': '',
         'CHud::SetHelpMessageWithNumber': '',
         'CHud::SetMessage': '',
         'CHud::SetVehicleName': '',
         'CHud::SetZoneName': ''},
 'note': 'CamHud1 añade la aprobación de las treinta y seis funciones de CHud (24) y CCam (12) con casos en el banco, 18688 pares con cero diferencias. La corrida '
         '20261009-155715-312758 tiene passed=true global y evento complete con state_restored=true, begin con known=true y los 31 sitios de intercepción correctos, '
         'saves y configuraciones intactos, con el banco simple (hook de la función apagado) frente a la candidata dentro del juego completo con partida cargada. '
         'Límites: textos GXT con caracteres 0x20..0x7D (nunca ~), floats moderados sin Inf ni NaN de señalización, peds, vehículos, cámara RW y entidad falsos, '
         'colaboradores de CFont/CSprite2d/CMessages/CWaterLevel/CShadows/rand doblados con traza (sin crédito), DrawVehicleName con estados 0..4. Siguen PENDIENTES '
         '(fuera del banco): CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise, Shutdown (tabla RenderState del motor, texturas/TXD reales) y '
         'CCam::Process_Rocket y Process_1stPerson (jerarquía esquelética y mundo reales), de modo que CHud conserva 6 pendientes y CCam 2. Aprobación histórica con '
         'dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/CamHud1/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut434.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
