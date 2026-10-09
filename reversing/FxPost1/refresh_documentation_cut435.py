"""Documentary cut435 (+66 funciones: FxPost1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 493,
 'old_valid': 7229,
 'old_pend': 1217,
 'old_groups': 23,
 'n': 66,
 'groups_drop': 2,
 'old_word': 'Cuatrocientas noventa y tres',
 'new_word': 'Quinientas cincuenta y nueve',
 'intro': 'sesenta y seis de FxSystem_c y CPostEffects (FxPost1)',
 'complete': ['CPostEffects', 'FxSystem_c'],
 'pending_counts': {},
 'summary': 'reversing/FxPost1/validation-summary.json',
 'pairs': {'FxSystem_c::AddOffsetPos': 256,
           'FxSystem_c::AttachToBone': 256,
           'FxSystem_c::Constructor': 128,
           'FxSystem_c::CopyParentMatrix': 128,
           'FxSystem_c::Destructor': 256,
           'FxSystem_c::DoFxAudio': 512,
           'FxSystem_c::EnablePrim': 128,
           'FxSystem_c::Exit': 256,
           'FxSystem_c::ForAllParticles': 512,
           'FxSystem_c::GetBoundingSphereLcl': 128,
           'FxSystem_c::GetBoundingSphereWld': 512,
           'FxSystem_c::GetCompositeMatrix': 256,
           'FxSystem_c::GetPlayStatus': 64,
           'FxSystem_c::Init': 512,
           'FxSystem_c::IsVisible': 512,
           'FxSystem_c::Kill': 256,
           'FxSystem_c::Pause': 128,
           'FxSystem_c::Play': 512,
           'FxSystem_c::PlayAndKill': 256,
           'FxSystem_c::ResetBoundingSphere': 256,
           'FxSystem_c::SetBoundingSphere': 256,
           'FxSystem_c::SetConstTime': 256,
           'FxSystem_c::SetLocalParticles': 128,
           'FxSystem_c::SetMatrix': 128,
           'FxSystem_c::SetMustCreatePrts': 128,
           'FxSystem_c::SetOffsetPos': 256,
           'FxSystem_c::SetRateMult': 256,
           'FxSystem_c::SetTimeMult': 256,
           'FxSystem_c::SetVelAdd': 128,
           'FxSystem_c::SetZTestEnable': 128,
           'FxSystem_c::Stop': 256,
           'CPostEffects::CCTV': 256,
           'CPostEffects::Close': 64,
           'CPostEffects::ColourFilter': 256,
           'CPostEffects::DarknessFilter': 256,
           'CPostEffects::DoScreenModeDependentInitializations': 128,
           'CPostEffects::DrawQuad': 512,
           'CPostEffects::FilterFX_RestoreDayNightBalance': 128,
           'CPostEffects::FilterFX_StoreAndSetDayNightBalance': 128,
           'CPostEffects::Fog': 512,
           'CPostEffects::Grain': 192,
           'CPostEffects::HeatHazeFXInit': 256,
           'CPostEffects::ImmediateModeFilterStuffInitialize': 256,
           'CPostEffects::ImmediateModeRenderStatesReStore': 128,
           'CPostEffects::ImmediateModeRenderStatesSet': 64,
           'CPostEffects::ImmediateModeRenderStatesStore': 128,
           'CPostEffects::InfraredVision': 256,
           'CPostEffects::InfraredVisionRestoreLightsForHeatObjects': 64,
           'CPostEffects::InfraredVisionSetLightsForDefaultObjects': 64,
           'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects': 512,
           'CPostEffects::Initialise': 64,
           'CPostEffects::IsVisionFXActive': 64,
           'CPostEffects::NightVision': 256,
           'CPostEffects::NightVisionSetLights': 64,
           'CPostEffects::Render': 1024,
           'CPostEffects::ScriptCCTVSwitch': 64,
           'CPostEffects::ScriptDarknessFilterSwitch': 128,
           'CPostEffects::ScriptHeatHazeFXSwitch': 64,
           'CPostEffects::ScriptInfraredVisionSwitch': 64,
           'CPostEffects::ScriptNightVisionSwitch': 64,
           'CPostEffects::ScriptResetForEffects': 64,
           'CPostEffects::SetFilterMainColour': 256,
           'CPostEffects::SetSpeedFXManualSpeedCurrentFrame': 256,
           'CPostEffects::SetupBackBufferVertex': 256,
           'CPostEffects::UnderWaterRipple': 512,
           'CPostEffects::Update': 128},
 'kind': 'CPU en el juego completo con partida cargada, dispositivo RenderWare falso (RwEngineInstance) y colaboradores doblados con traza (banco simple, passed=true '
         'global)',
 'lim': {'CPostEffects::CCTV': 'dominio sin altura >= 448 (bucle sin fin en el EXE); ',
         'CPostEffects::Close': '',
         'CPostEffects::ColourFilter': '',
         'CPostEffects::DarknessFilter': '',
         'CPostEffects::DoScreenModeDependentInitializations': '',
         'CPostEffects::DrawQuad': '',
         'CPostEffects::FilterFX_RestoreDayNightBalance': '',
         'CPostEffects::FilterFX_StoreAndSetDayNightBalance': '',
         'CPostEffects::Fog': '',
         'CPostEffects::Grain': 'RNG resumido en hash y solo el byte bajo de la fuerza; ',
         'CPostEffects::HeatHazeFXInit': 'RNG resumido en hash y srand sin traza; ',
         'CPostEffects::ImmediateModeFilterStuffInitialize': '',
         'CPostEffects::ImmediateModeRenderStatesReStore': '',
         'CPostEffects::ImmediateModeRenderStatesSet': '',
         'CPostEffects::ImmediateModeRenderStatesStore': '',
         'CPostEffects::InfraredVision': '',
         'CPostEffects::InfraredVisionRestoreLightsForHeatObjects': '',
         'CPostEffects::InfraredVisionSetLightsForDefaultObjects': '',
         'CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects': '',
         'CPostEffects::Initialise': 'RNG resumido en hash y srand sin traza; ',
         'CPostEffects::IsVisionFXActive': '',
         'CPostEffects::NightVision': '',
         'CPostEffects::NightVisionSetLights': '',
         'CPostEffects::Render': 'ALCANCE LIMITADO: la rama de guardado de foto (0x705301..0x7053E3, E/S real de fichero) queda fuera y los fixtures la evitan; sí se '
                                 'comparan las ramas sin E/S (borrado por m_bIsSaveDone, armado de s_WaitForOneFrame y s_SavePhotoToGallery) y los subefectos van '
                                 'doblados; el color de UnderWaterRipple sin su byte alto; ',
         'CPostEffects::ScriptCCTVSwitch': '',
         'CPostEffects::ScriptDarknessFilterSwitch': '',
         'CPostEffects::ScriptHeatHazeFXSwitch': '',
         'CPostEffects::ScriptInfraredVisionSwitch': '',
         'CPostEffects::ScriptNightVisionSwitch': '',
         'CPostEffects::ScriptResetForEffects': '',
         'CPostEffects::SetFilterMainColour': '',
         'CPostEffects::SetSpeedFXManualSpeedCurrentFrame': '',
         'CPostEffects::SetupBackBufferVertex': 'sin la rama de fallo de RwRasterCreate (0x7044B5..0x7044CF); ',
         'CPostEffects::UnderWaterRipple': 'dominio sin yoffset > 0 (locales sin inicializar en el EXE); ',
         'CPostEffects::Update': '',
         'FxSystem_c::AddOffsetPos': '',
         'FxSystem_c::AttachToBone': '',
         'FxSystem_c::Constructor': 'se comparan los bytes [0,0x7C) y los dos dwords de FireAE que escribe el EXE; vptr y resto de m_FireAE enmascarados; ',
         'FxSystem_c::CopyParentMatrix': '',
         'FxSystem_c::Destructor': 'se comparan los bytes [0,0x7C); vptr enmascarado; ',
         'FxSystem_c::DoFxAudio': '',
         'FxSystem_c::EnablePrim': '',
         'FxSystem_c::Exit': '',
         'FxSystem_c::ForAllParticles': '',
         'FxSystem_c::GetBoundingSphereLcl': '',
         'FxSystem_c::GetBoundingSphereWld': '',
         'FxSystem_c::GetCompositeMatrix': '',
         'FxSystem_c::GetPlayStatus': '',
         'FxSystem_c::Init': '',
         'FxSystem_c::IsVisible': '',
         'FxSystem_c::Kill': '',
         'FxSystem_c::Pause': '',
         'FxSystem_c::Play': '',
         'FxSystem_c::PlayAndKill': '',
         'FxSystem_c::ResetBoundingSphere': '',
         'FxSystem_c::SetBoundingSphere': '',
         'FxSystem_c::SetConstTime': '',
         'FxSystem_c::SetLocalParticles': '',
         'FxSystem_c::SetMatrix': '',
         'FxSystem_c::SetMustCreatePrts': '',
         'FxSystem_c::SetOffsetPos': '',
         'FxSystem_c::SetRateMult': '',
         'FxSystem_c::SetTimeMult': '',
         'FxSystem_c::SetVelAdd': '',
         'FxSystem_c::SetZTestEnable': '',
         'FxSystem_c::Stop': ''},
 'note': 'FxPost1 añade la aprobación de las sesenta y seis funciones pendientes de FxSystem_c (31) y CPostEffects (35), 15488 pares con cero diferencias. La corrida '
         '20261009-161301-487563 tiene passed=true global y evento complete con state_restored=true, begin con known=true, layout y los 44 sitios correctos, saves y '
         'configuraciones intactos, con el banco simple (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Con ellas FxSystem_c y '
         'CPostEffects quedan COMPLETAS en hooks.csv (sin pendientes). Alcance limitado de CPostEffects::Render: la rama de guardado de foto (0x705301..0x7053E3, E/S '
         'real de fichero) queda fuera del banco y los fixtures la evitan, aunque sí se comparan las ramas sin E/S. Límites: dispositivo RenderWare falso, objetos '
         'FxSystem_c falsos con arena fija, colaboradores de RW, RNG y audio doblados con traza (sin crédito); sin NaN de señalización ni entradas que hagan al EXE leer '
         'locales sin inicializar o entrar en bucles sin fin; Constructor/Destructor de FxSystem_c comparan solo los bytes que escribe el EXE; '
         'Grain/Initialise/HeatHazeFXInit resumen el RNG en un hash. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. '
         '[Resumen](../reversing/FxPost1/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut435.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
