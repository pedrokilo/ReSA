"""Documentary cut365 (54 funciones registradas via AudioWave2); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=311;NEW_APPR=365;OLD_VALID=7047;OLD_PEND=1399;OLD_GROUPS=38
assert VALID==OLD_VALID+54 and PEND==OLD_PEND-54,(VALID,PEND)
assert 'quedan %d en %d grupos'%(OLD_PEND,OLD_GROUPS) in s
assert True
assert GROUPS==OLD_GROUPS-4,GROUPS
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'quedan {OLD_PEND} en {OLD_GROUPS} grupos',f'quedan {PEND} en {GROUPS} grupos'),('Trescientas once aprobaciones nuevas','Trescientas sesenta y cinco aprobaciones nuevas'),(f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),
 ('(MiscWave2), hora','(MiscWave2) y cincuenta y cuatro de CAEScriptAudioEntity, CAEPoliceScannerAudioEntity, CAEPedAudioEntity y CAEStreamingChannel (AudioWave2), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/AudioWave2/validation-summary.json']
assert len(new)==54 and sum(x['evidence']['pairs'] for x in new)==18496
LIM={'CAEScriptAudioEntity::Constructor':'vptr y base CAEAudioEntity enmascarados; fuente corregida antes; ','CAEScriptAudioEntity::Reset':'fuente corregida antes (m_Physical); ','CAEScriptAudioEntity::GetMissionAudioEvent':'fuente corregida antes (índice sin comprobar); ','CAEScriptAudioEntity::PlayResidentSoundEvent':'fuente corregida antes (m_tempSound); ',
'CAEPoliceScannerAudioEntity::Constructor':'vptr y base enmascarados; ','CAEPoliceScannerAudioEntity::Destructor':'vptr y base enmascarados; ','CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue':'fuente corregida antes; ','CAEPoliceScannerAudioEntity::PrepSlots':'fuente corregida antes; ','CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue':'fuente corregida antes; ','CAEPoliceScannerAudioEntity::UpdateParameters':'fuente corregida antes; ',
'CAEPedAudioEntity::HandleFootstepEvent':'fuente corregida antes; ','CAEPedAudioEntity::HandleSkateEvent':'fuente corregida antes; ','CAEPedAudioEntity::HandleLandingEvent':'fuente corregida antes; ','CAEPedAudioEntity::HandleSwimSplash':'fuente corregida antes; ','CAEPedAudioEntity::HandleSwimWake':'fuente corregida antes; ','CAEPedAudioEntity::AddAudioEvent':'fuente corregida antes (crunch y SpeedVariance literal), x87 PC24; ','CAEPedAudioEntity::TurnOnJetPack':'fuente corregida antes; ','CAEPedAudioEntity::UpdateJetPack':'fuente corregida antes; ',
'CAEStreamingChannel::FillBuffer':'fuente corregida antes; ','CAEStreamingChannel::UpdatePlayTime':'fuente corregida antes; ','CAEStreamingChannel::SetReady':'fuente corregida antes; ','CAEStreamingChannel::Pause':'fuente corregida antes; ','CAEStreamingChannel::SetFrequencyScalingFactor':'fuente corregida antes; ','CAEStreamingChannel::AddFX':'fuente corregida antes (resultados no nulos, fxParamEq único, eqFail solo en la 2.ª iteración); ','CAEStreamingChannel::Initialise':'fuente corregida antes (devuelve bool), CoInitialize solo durante la función; ','CAEStreamingChannel::InitialiseSilence':'CreateSoundBuffer fallido no probado; ','CAEStreamingChannel::Destructor':'m_pDirectSoundBuffer siempre no nulo, vptr y base enmascarados; '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con entidades, canal, DirectSound/COM, peatón, físico y sonidos falsos con dobles trazados; {LIM.get(name,'')}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='AudioWave2 añade la aprobación de 54 funciones: 8 de CAEScriptAudioEntity, 13 de CAEPoliceScannerAudioEntity, 15 de CAEPedAudioEntity y 18 de CAEStreamingChannel, 18496 pares con cero diferencias, evento de cierre con state_restored, saves y configuraciones intactos, con el banco simple (hooks de la clase apagados) frente a la candidata dentro del juego completo con partida cargada, todo sobre COM de DirectSound falso y dobles trazados sin crédito (IsSoundBankLoaded, LoadSoundBank, CAESoundManager, CAESound, GetRandomNumberInRange, FindPlayerPed, CAESmoothFadeThread, CAEAudioChannel, CAETwinLoopSoundEntity y otros). Según hooks.csv las cuatro clases quedan completas (sin pendientes). Límites: constructores y destructores con vptr y tramo base de CAEAudioEntity enmascarados; SoundDigest enmascara relleno y basura de pila (+0x55, +0x40..0x4F, +0x5A..0x5B, +0x6A..0x6B, +0x72..0x73); InitialiseSilence con CreateSoundBuffer fallido no probado; CoInitialize solo durante Streaming::Initialise; Destructor de Streaming con m_pDirectSoundBuffer siempre no nulo; fixture eqFail solo en la 2.ª iteración de AddFX. Antes de la corrida aprobada se corrigieron las fuentes AEScriptAudioEntity.cpp, AEPoliceScannerAudioEntity.cpp, AEPedAudioEntity.cpp y AEStreamingChannel.cpp/.h; sin confirmar en git al registrar. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/AudioWave2/validation-summary.json).'+chr(10)+chr(10)
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
(HERE/'documentation-refresh-cut365.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
