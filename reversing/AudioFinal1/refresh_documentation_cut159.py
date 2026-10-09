"""Documentary cut159 (CAEAudioHardware::Initialise y tres funciones de CAEFrontendAudioEntity registradas via AudioFinal1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
CLASSES=('CAEAudioHardware','CAEFrontendAudioEntity')
assert sum(x['validated']=='1' for x in rows)==6895 and sum(pending.values())==1551 and all(pending[c]==0 for c in CLASSES)
GROUPS=len(pending);assert GROUPS==49
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('155 aprobaciones nuevas','159 aprobaciones nuevas'),('155 identidades nuevas','159 identidades nuevas'),('6891 entradas','6895 entradas'),('1555 pendientes','1551 pendientes'),('distribuidas en **51 grupos**','distribuidas en **49 grupos**'),
 ('quedan 1555 en 51 grupos','quedan 1551 en 49 grupos'),('Ciento cincuenta y cinco aprobaciones nuevas','Ciento cincuenta y nueve aprobaciones nuevas'),
 ('de cinco tareas (TaskFinal1), hora','de cinco tareas (TaskFinal1) y CAEAudioHardware::Initialise y tres funciones de CAEFrontendAudioEntity (AudioFinal1), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=reg['functions'][-4:]
NAMES=['CAEAudioHardware::Initialise','CAEFrontendAudioEntity::Initialise','CAEFrontendAudioEntity::IsLoadingTuneActive','CAEFrontendAudioEntity::Reset']
assert [x['hook']['class']+'::'+x['hook']['fn_name'] for x in new]==NAMES
LIM={
 'CAEAudioHardware::Initialise':'objeto CAEAudioHardware falso de 0x1100 bytes; DirectSoundCreate/DirectSoundCreate8 sustituidas por un dispositivo COM falso con HRESULT y capacidades guionizados (dwFreeHw3DAllBuffers alrededor de 0x18/0x40), operator new sobre una arena de 1 MiB con 0xCD para comparar bloques de los constructores reales de cargadores y canales; 777 casos llegan hasta CAEStreamThread::Start y 759 salen antes, 573 con mezcla por hardware y 204 sin ella; la fuente se corrigió antes: operator new(0x6E4) más Constructor() para CAEMP3BankLoader. Tras el fallo de un cargador MP3 doble la DLL escribe un mensaje NOTSA_LOG_ERR que el EXE no tiene y desde ahí se congelan traza, bloques y arena; el camino con m_nUserTracksScanState == IN_PROGRESS no se prueba.',
 'CAEFrontendAudioEntity::Initialise':'entidad falsa de 0x1EC bytes con guarda; LoadSoundBank, LoadSound y GetRandomNumberInRange<int> son dobles trazados (valores 0..3, 125..300 y -1; 182 de 512 con byte ≥ 128); retorno void no comparado; la fuente se corrigió antes: escribe los campos que faltaban y LoadSound usa id con signo.',
 'CAEFrontendAudioEntity::IsLoadingTuneActive':'entidad falsa; AreSoundsOfThisEventPlayingForThisEntity doble que devuelve 0 o distinto de 0 (67 de 128); se compara el retorno y el objeto.',
 'CAEFrontendAudioEntity::Reset':'entidad falsa; CancelSoundsOfThisEventPlayingForThisEntity es un doble trazado; se comparan el objeto y la traza.'}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo, objeto falso; {LIM[name]} |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='AudioFinal1 añade la aprobación de 4 funciones, CAEAudioHardware::Initialise y CAEFrontendAudioEntity::{Initialise, IsLoadingTuneActive, Reset} (2304 pares: 1536, 512, 128 y 128; 0 diferencias, evento de cierre con state_restored, saves y configuraciones intactos, umbrales de cobertura cumplidos), con el banco simple original (hook apagado) frente a la candidata dentro del juego completo; las clases CAEAudioHardware y CAEFrontendAudioEntity quedan completas en hooks.csv. Límites: el objeto de hardware es un doble de 0x1100 bytes y nunca se toca AEAudioHardware ni se abre el dispositivo real, porque DirectSoundCreate (importación del EXE) y DirectSoundCreate8 (importación de la DLL) se sustituyen por un dispositivo COM falso solo durante la llamada; operator new se redirige a una arena rellena con 0xCD (los hilos ajenos reenvían al original) para comparar bloque a bloque los constructores reales de cargadores y canales, con el puntero a vtable de los canales enmascarado. Son dobles trazados, sin crédito de esos colaboradores, CAEAudioHardware::InitDirectSoundListener, CAEMP3BankLoader::Initialise, CAEMP3TrackLoader::Initialise, CAEStreamingChannel::Initialise, CAEAudioChannel::SetVolume, CAEUserRadioTrackManager::ScanUserTracks e Initialise, CAESmoothFadeThread::Initialise y Start, CAEStreamThread::Initialise y Start, LoadSoundBank, LoadSound, GetRandomNumberInRange<int>, AreSoundsOfThisEventPlayingForThisEntity y CancelSoundsOfThisEventPlayingForThisEntity; IsForegroundApp aparca el hilo real de streaming y no se crea ningún hilo de audio. Límite declarado: tras el fallo de un cargador MP3 doble, la DLL escribe un mensaje NOTSA_LOG_ERR (spdlog reserva memoria) que el EXE no tiene; desde ese punto la traza, la lista de bloques y la arena se congelan, de modo que reservas o llamadas dobles posteriores de la DLL solo se detectan por el hash del objeto, el retorno y los globales comparados. El camino con m_nUserTracksScanState == IN_PROGRESS (bucle Sleep(100)) no se prueba; los estados usados son OFF, COMPLETE, ERROR, 4, -1 e INT_MAX. Antes de la corrida se corrigió la fuente a lo que hace el EXE: CAEAudioHardware::Initialise usa operator new(0x6E4) más Constructor() para CAEMP3BankLoader, y CAEFrontendAudioEntity::Initialise escribe los campos que faltaban y LoadSound usa id con signo; esos cambios seguían sin confirmar en git al registrar. La primera corrida (20261009-005347-353159) falló con 341 diferencias de 1536 en CAEAudioHardware::Initialise, todas solo en arena_used, blocks y recs (reservas del log NOTSA), y se conserva rechazada solo como hashes. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/AudioFinal1/validation-summary.json).\n\n'
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6895 con evidencia histórica, 0 con vigencia acreditada y 1551 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6895,'pending':1551,'pending_groups':GROUPS,'current':0,'new_approvals':159}
(HERE/'documentation-refresh-cut159.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
