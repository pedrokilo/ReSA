"""Documentary cut436 (+61 funciones: RadioFont1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 559,
 'old_valid': 7295,
 'old_pend': 1151,
 'old_groups': 21,
 'n': 61,
 'groups_drop': 2,
 'old_word': 'Quinientas cincuenta y nueve',
 'new_word': 'Seiscientas veinte',
 'intro': 'sesenta y una de CAERadioTrackManager y CFont (RadioFont1)',
 'complete': ['CAERadioTrackManager', 'CFont'],
 'pending_counts': {},
 'summary': 'reversing/RadioFont1/validation-summary.json',
 'pairs': {'CAERadioTrackManager::AddAdvertIndexToHistory': 512,
           'CAERadioTrackManager::AddDJBanterIndexToHistory': 512,
           'CAERadioTrackManager::AddIdentIndexToHistory': 512,
           'CAERadioTrackManager::AddMusicTrackIndexToHistory': 512,
           'CAERadioTrackManager::CheckForMissionStatsChanges': 1024,
           'CAERadioTrackManager::CheckForPause': 512,
           'CAERadioTrackManager::CheckForStationRetuneDuringPause': 512,
           'CAERadioTrackManager::ChooseTracksForStation': 512,
           'CAERadioTrackManager::DisplayRadioStationName': 512,
           'CAERadioTrackManager::GetCurrentRadioStationID': 128,
           'CAERadioTrackManager::GetRadioStationListenTimes': 64,
           'CAERadioTrackManager::GetRadioStationName': 128,
           'CAERadioTrackManager::GetRadioStationNameKey': 128,
           'CAERadioTrackManager::HasRadioRetuneJustStarted': 64,
           'CAERadioTrackManager::Initialise': 256,
           'CAERadioTrackManager::InitialiseRadioStationID': 128,
           'CAERadioTrackManager::IsRadioOn': 256,
           'CAERadioTrackManager::Load': 128,
           'CAERadioTrackManager::PlayRadioAnnouncement': 64,
           'CAERadioTrackManager::Reset': 256,
           'CAERadioTrackManager::ResetStatistics': 128,
           'CAERadioTrackManager::RetuneRadio': 256,
           'CAERadioTrackManager::Save': 128,
           'CAERadioTrackManager::SetBassEnhanceOnOff': 256,
           'CAERadioTrackManager::SetBassSetting': 256,
           'CAERadioTrackManager::SetRadioAutoRetuneOnOff': 64,
           'CAERadioTrackManager::StartRadio-manual': 1024,
           'CAERadioTrackManager::StartRadio-with-settings': 512,
           'CAERadioTrackManager::StartTrackPlayback': 128,
           'CFont::DrawFonts': 64,
           'CFont::GetCharacterSize': 512,
           'CFont::GetLetterIdPropValue': 512,
           'CFont::GetNumberLines': 256,
           'CFont::GetStringWidth': 512,
           'CFont::GetTextRect': 256,
           'CFont::InitPerFrame': 64,
           'CFont::Initialise': 64,
           'CFont::LoadFontValues': 128,
           'CFont::ParseToken': 512,
           'CFont::PrintString': 256,
           'CFont::PrintStringFromBottom': 256,
           'CFont::ProcessStringToDisplay': 256,
           'CFont::SetAlphaFade': 64,
           'CFont::SetBackground': 64,
           'CFont::SetBackgroundColor': 64,
           'CFont::SetCentreSize': 64,
           'CFont::SetColor': 256,
           'CFont::SetDropColor': 256,
           'CFont::SetDropShadowPosition': 64,
           'CFont::SetEdge': 64,
           'CFont::SetFontStyle': 128,
           'CFont::SetJustify': 64,
           'CFont::SetOrientation': 128,
           'CFont::SetProportional': 64,
           'CFont::SetRightJustifyWrap': 64,
           'CFont::SetScale': 128,
           'CFont::SetScaleForCurrentLanguage': 128,
           'CFont::SetSlant': 64,
           'CFont::SetSlantRefPoint': 64,
           'CFont::SetWrapx': 64,
           'CFont::Shutdown': 64},
 'kind': 'CPU en el juego completo con partida cargada, objeto CAERadioTrackManager falso y colaboradores doblados con traza (banco simple, passed=true global)',
 'lim': {'CAERadioTrackManager::AddAdvertIndexToHistory': '',
         'CAERadioTrackManager::AddDJBanterIndexToHistory': '',
         'CAERadioTrackManager::AddIdentIndexToHistory': '',
         'CAERadioTrackManager::AddMusicTrackIndexToHistory': '',
         'CAERadioTrackManager::CheckForMissionStatsChanges': '',
         'CAERadioTrackManager::CheckForPause': '',
         'CAERadioTrackManager::CheckForStationRetuneDuringPause': '',
         'CAERadioTrackManager::ChooseTracksForStation': '',
         'CAERadioTrackManager::DisplayRadioStationName': '',
         'CAERadioTrackManager::GetCurrentRadioStationID': '',
         'CAERadioTrackManager::GetRadioStationListenTimes': '',
         'CAERadioTrackManager::GetRadioStationName': '',
         'CAERadioTrackManager::GetRadioStationNameKey': 'solo identificadores 0..13; ',
         'CAERadioTrackManager::HasRadioRetuneJustStarted': '',
         'CAERadioTrackManager::Initialise': '',
         'CAERadioTrackManager::InitialiseRadioStationID': '',
         'CAERadioTrackManager::IsRadioOn': '',
         'CAERadioTrackManager::Load': '',
         'CAERadioTrackManager::PlayRadioAnnouncement': '',
         'CAERadioTrackManager::Reset': '',
         'CAERadioTrackManager::ResetStatistics': '',
         'CAERadioTrackManager::RetuneRadio': '',
         'CAERadioTrackManager::Save': '',
         'CAERadioTrackManager::SetBassEnhanceOnOff': '',
         'CAERadioTrackManager::SetBassSetting': '',
         'CAERadioTrackManager::SetRadioAutoRetuneOnOff': '',
         'CAERadioTrackManager::StartRadio-manual': 'solo identificadores >= 0; ',
         'CAERadioTrackManager::StartRadio-with-settings': 'solo identificadores >= 0; ',
         'CAERadioTrackManager::StartTrackPlayback': '',
         'CFont::DrawFonts': '',
         'CFont::GetCharacterSize': '',
         'CFont::GetLetterIdPropValue': '',
         'CFont::GetNumberLines': 'textos GXT bien formados; ',
         'CFont::GetStringWidth': 'textos GXT bien formados; ',
         'CFont::GetTextRect': 'textos GXT bien formados; ',
         'CFont::InitPerFrame': '',
         'CFont::Initialise': '',
         'CFont::LoadFontValues': 'identificador de fuente 0 o 1, secciones completas y DATA\\FONTS.DAT sustituido por líneas del fixture; ',
         'CFont::ParseToken': 'textos GXT bien formados; ',
         'CFont::PrintString': '',
         'CFont::PrintStringFromBottom': '',
         'CFont::ProcessStringToDisplay': 'textos GXT bien formados; ',
         'CFont::SetAlphaFade': '',
         'CFont::SetBackground': '',
         'CFont::SetBackgroundColor': '',
         'CFont::SetCentreSize': '',
         'CFont::SetColor': '',
         'CFont::SetDropColor': '',
         'CFont::SetDropShadowPosition': '',
         'CFont::SetEdge': '',
         'CFont::SetFontStyle': '',
         'CFont::SetJustify': '',
         'CFont::SetOrientation': '',
         'CFont::SetProportional': '',
         'CFont::SetRightJustifyWrap': '',
         'CFont::SetScale': '',
         'CFont::SetScaleForCurrentLanguage': '',
         'CFont::SetSlant': '',
         'CFont::SetSlantRefPoint': '',
         'CFont::SetWrapx': '',
         'CFont::Shutdown': 'FindTxdSlot siempre devuelve una ranura válida y la rama de SafeRemoveTxdSlot(-1) queda fuera; '},
 'note': 'RadioFont1 añade la aprobación de las sesenta y una funciones pendientes de CAERadioTrackManager (29) y CFont (32), 14976 pares con cero diferencias. La '
         'corrida 20261009-161801-628784 tiene passed=true global y evento complete con state_restored=true, begin con layout correcto y sitios de intercepción '
         'correctos, saves y configuraciones intactos, con el banco simple (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Con ellas '
         'CAERadioTrackManager y CFont quedan COMPLETAS en hooks.csv (sin pendientes). Límites: textos GXT bien formados (el EXE no comprueba el NUL), '
         'GetRadioStationNameKey solo con identificadores 0..13, StartRadio solo con identificadores >= 0, Shutdown con FindTxdSlot siempre válido (rama de '
         'SafeRemoveTxdSlot(-1) fuera), LoadFontValues con fuente 0 o 1, secciones completas y sin fichero real; objeto CAERadioTrackManager falso (nunca se toca el '
         'real); audio, CTxdStore, CFileMgr, CSprite2d, CStats y RNG doblados con traza (sin crédito). Aprobación histórica con dependency_review=null; la vigencia '
         'sigue en 0. [Resumen](../reversing/RadioFont1/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut436.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
