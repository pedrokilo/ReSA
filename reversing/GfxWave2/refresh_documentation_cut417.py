"""Documentary cut417 (52 funciones registradas via GfxWave2, aprobacion parcial por funcion); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=365;NEW_APPR=417;OLD_VALID=7101;OLD_PEND=1345;OLD_GROUPS=34
assert VALID==OLD_VALID+52 and PEND==OLD_PEND-52,(VALID,PEND)
assert 'quedan %d en %d grupos'%(OLD_PEND,OLD_GROUPS) in s
assert GROUPS==OLD_GROUPS,GROUPS
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'quedan {OLD_PEND} en {OLD_GROUPS} grupos',f'quedan {PEND} en {GROUPS} grupos'),('Trescientas sesenta y cinco aprobaciones nuevas','Cuatrocientas diecisiete aprobaciones nuevas'),(f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),
 ('(AudioWave2), hora','(AudioWave2) y cincuenta y dos de CClothesBuilder, CShadows y CWaterLevel (GfxWave2, aprobación parcial por función), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/GfxWave2/validation-summary.json']
assert len(new)==52 and sum(x['evidence']['pairs'] for x in new)==25056
LIM={'CClothesBuilder::GetAtomicWithName':'fuente corregida antes (gana la primera coincidencia); ','CClothesBuilder::AddWeightToBoneVertex':'fuente corregida antes; ','CClothesBuilder::StoreBoneArray':'fuente corregida antes; ','CClothesBuilder::BuildBoneIndexConversionTable':'fuente corregida antes; ','CClothesBuilder::CopyTexture':'fuente corregida antes; ','CClothesBuilder::PlaceTextureOnTopOfTexture':'fuente corregida antes; ',
'CClothesBuilder::BlendTextures-Dst-Src':'fuente reescrita antes; ','CClothesBuilder::BlendTextures-Dst-Src1-Src2':'fuente reescrita antes; ','CClothesBuilder::BlendTextures-Dst-Src1-Src2-Tat':'fuente reescrita antes; ',
'CShadows::RemoveOilInArea':'fuente corregida antes; ','CShadows::AffectColourWithLighting':'fuente corregida antes; ','CShadows::StoreShadowForPedObject':'fuente corregida antes; GetFxQuality sin traza; ','CShadows::StoreRealTimeShadow':'fuente corregida antes; GetFxQuality sin traza; ','CShadows::RenderExtraPlayerShadows':'fuente corregida antes (orden de llamadas); ','CShadows::RenderIndicatorShadow':'fuente corregida antes; ','CShadows::StoreShadowForPole':'fuente corregida antes; ','CShadows::StoreCarLightShadow':'fuente corregida antes; GetFxQuality sin traza; ','CShadows::RenderStaticShadows':'fuente corregida antes; ','CShadows::CalcPedShadowValues':'NaN; ',
'CWaterLevel::AddWaterLevelVertex':'fuente corregida antes; ','CWaterLevel::AddWaterLevelTriangle':'fuente corregida antes; ','CWaterLevel::CalculateWavesOnlyForCoordinate':'fuente reescrita antes (__asm); ','CWaterLevel::ScanThroughBlocks':'fuente corregida antes; ','CWaterLevel::PreRenderWater':'fuente corregida antes; ','CWaterLevel::RenderWaterTriangle':'fuente corregida antes; ','CWaterLevel::RenderWaterRectangle':'fuente corregida antes; hermanas de dibujo aisladas sin crédito; ','CWaterLevel::SplitWaterRectangleAlongYLine':'fuente corregida antes; ','CWaterLevel::RenderFlatWaterRectangle_OneLayer':'fuente reescrita antes; ','CWaterLevel::RenderFlatWaterTriangle_OneLayer':'fuente reescrita antes; ','CWaterLevel::RenderWakeSegment':'fuente reescrita antes; ','CWaterLevel::SetUpWaterFog':'fuente corregida antes; '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con objetos falsos y dibujo sustituido por trazas (aprobación parcial por función; la corrida global tiene passed=false por otras tres funciones); {LIM.get(name,'')}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
cl=pending['CClothesBuilder'];sh=pending['CShadows'];wl=pending['CWaterLevel']
marker='## Pendientes por clase';note=('GfxWave2 añade la aprobación PARCIAL, por función, de 52 funciones: 14 de CClothesBuilder (AddWeightToBoneVertex, BlendTextures x3, BuildBoneIndexConversionTable, CopyTexture, DestroySkinArrays, GetAtomicWithName, GetTextureFromTxdAndLoadNextTxd, LoadCdDirectory, PlaceTextureOnTopOfTexture, RequestGeometry, RequestTexture, StoreBoneArray), 19 de CShadows y 19 de CWaterLevel, 25056 pares con cero diferencias en cada una, saves y configuraciones intactos, banco simple (hooks de la clase apagados) frente a la candidata dentro del juego completo con partida cargada. '
 'La corrida 20261009-095105-342457 (27616 pares) tiene passed=false por tres funciones RECHAZADAS que no se registran ni reciben crédito: CWaterLevel::AddWaterLevelQuad (254/1024), RenderFlatWaterRectangle (472/1024) y SplitWaterTriangleAlongXLine (258/512). El banco no escribe el evento complete cuando hay diferencias; cerró con un evento error (27616 pares, state_restored=true, sin motivo), es decir, la corrida terminó con normalidad. '
 f'Según hooks.csv ninguna de las tres clases queda completa: CClothesBuilder conserva {cl} pendiente (CreateSkinnedClump), CShadows {sh} (RenderStoredShadows y CastPlayerShadowSectorList) y CWaterLevel {wl} (las tres rechazadas, RenderBoatWakes, SplitWaterTriangleAlongYLine y WaterLevelInitialise). '
 'Límites: dobles trazados sin crédito (GetFirstTexture, RwRaster*, RwTextureCreate, colaboradores de CStreaming/CDirectory/CTimer, CEntity::GetColModel, CPed::GetBonePosition, CCamera::IsSphereVisible, CRealTimeShadow, RwIm3D*, LittleTest, ScanWorld, GetWaterLevelNoWaves y otros); Fx_c::GetFxQuality es un stub fijo sin traza (la rama por calidad de FX de StoreShadowForPedObject, StoreRealTimeShadow y StoreCarLightShadow no se ejercita); relleno enmascarado (CWaterVertex, CRenPar, estructuras); texturas solo con rásters coherentes (w*h<=36, 32 bits); NumLights 0..6. Fuera del banco y sin crédito: CreateSkinnedClump, RenderStoredShadows, CastPlayerShadowSectorList, WaterLevelInitialise, RenderBoatWakes y SplitWaterTriangleAlongYLine. '
 'Antes de la corrida se corrigieron las fuentes ClothesBuilder.cpp, Shadows.cpp y WaterLevel.cpp/.h; sin confirmar en git al registrar. AVISO: WaterLevel.cpp se modificó en el árbol de trabajo después de la corrida (09:57), por lo que las entradas de CWaterLevel conservan el hash probado y su coincidencia de fuente es falsa hasta volver a validar. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/GfxWave2/validation-summary.json).')+chr(10)+chr(10)
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
(HERE/'documentation-refresh-cut417.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
