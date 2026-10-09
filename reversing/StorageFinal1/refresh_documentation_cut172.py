"""Documentary cut172 (cinco funciones de CGenericGameStorage registrados via StorageFinal1); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
CLASSES=('CGenericGameStorage',)
assert sum(x['validated']=='1' for x in rows)==6908 and sum(pending.values())==1538 and all(pending[c]==0 for c in CLASSES)
GROUPS=len(pending);assert GROUPS==44
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p:x['sha256'] for p,x in old['frozen_markdown_exceptions'].items()}
for p,h in frozen.items():assert sha(R/p)==h,p
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
for a,b in (('167 aprobaciones nuevas','172 aprobaciones nuevas'),('167 identidades nuevas','172 identidades nuevas'),('6903 entradas','6908 entradas'),('1543 pendientes','1538 pendientes'),('distribuidas en **45 grupos**','distribuidas en **44 grupos**'),
 ('quedan 1543 en 45 grupos','quedan 1538 en 44 grupos'),('Ciento sesenta y siete aprobaciones nuevas','Ciento setenta y dos aprobaciones nuevas'),
 ('y ocho funciones de CCollision, CPlayerPed, CPedGeometryAnalyser y CRealTimeShadowManager (GeoFinal1), hora','y ocho funciones de CCollision, CPlayerPed, CPedGeometryAnalyser y CRealTimeShadowManager (GeoFinal1) y cinco de CGenericGameStorage (StorageFinal1), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text());new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/StorageFinal1/validation-summary.json']
NAMES=['CGenericGameStorage::SaveDataToWorkBuffer-org','CGenericGameStorage::LoadDataFromWorkBuffer-org','CGenericGameStorage::ReportError','CGenericGameStorage::GenericSave','CGenericGameStorage::GenericLoad']
assert [x['hook']['class']+'::'+x['hook']['fn_name'] for x in new]==NAMES
LIM={'CGenericGameStorage::SaveDataToWorkBuffer-org': '9 tamaños × 10 posiciones (incluidas las que cruzan 0xC800) × fallo previo × 3 modos de vaciado × 3 ms_WorkBufferSize; la fuente se corrigió antes: umbral fijo 0xC800 y resto 0xC800-pos; SaveWorkBuffer es doble.', 'CGenericGameStorage::LoadDataFromWorkBuffer-org': 'misma rejilla con recarga simulada y ms_WorkBufferSize ∈ {0xC800, 51000, 40000}; la fuente se corrigió antes: umbral ms_WorkBufferSize y resto 0xC800-pos; LoadWorkBuffer es doble.', 'CGenericGameStorage::ReportError': '26 bloques × 8 códigos; bloques 25, 26 y ≥ 28 fuera (el EXE usa un búfer sin inicializar); el EXE formatea con sprintf (sustituido) y la DLL emite el mismo texto por spdlog (capturado); la fuente se corrigió antes: sin mensaje para códigos > 2.', 'CGenericGameStorage::GenericSave': 'sin E/S real: el fichero es un vector y el FILE* un token; totales de 0 a 230000 bytes, cabeceras partidas, fallos de apertura/vaciado/bloque/cierre; todos los guardados de bloque y colaboradores son dobles; la fuente se corrigió antes: bucle de relleno hasta 0x317FC y strncpy de 0x103.', 'CGenericGameStorage::GenericLoad': 'sin E/S real: fichero virtual de 28 bloques con cabecera corrupta opcional, recargas y bloques con fallo, CPools::Load falso/verdadero; ms_WorkBufferSize fijo en 0xC800; la rama NOTSA de versión no se ejercita; la fuente se corrigió antes: llama a ReportError(bloque-1, 2).'}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo, objetos falsos con guardas; {LIM[name]} |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase';note='StorageFinal1 añade la aprobación de las 5 funciones pendientes de CGenericGameStorage, SaveDataToWorkBuffer-org, LoadDataFromWorkBuffer-org, ReportError, GenericSave y GenericLoad (4984 pares: 1620, 1620, 208, 768 y 768; 0 diferencias, evento de cierre con state_restored, saves y configuraciones intactos), con el banco simple original (hook apagado) frente a la candidata dentro del juego completo; según hooks.csv CGenericGameStorage queda completa. Límites: ninguna E/S real, el fichero es un vector y los guardados reales se verifican por hash antes y después; son dobles trazados, sin crédito de esos colaboradores, OpenFileForWriting y OpenFileForReading, SaveWorkBuffer y LoadWorkBuffer, CFileMgr::CloseFile, CPad::UpdatePads, CCheat::ResetCheats, CSimpleVariablesSaveStructure::Construct y Extract, ReportError dentro de GenericLoad, DoGameSpecificStuffAfterSucessLoad, los 24 guardados y 24 cargas de bloque y DoScriptSetupAfterPoolsHaveLoaded (CPools::Load guionizado). Fuera del dominio: bloques 25, 26 y ≥ 28 de ReportError, ms_WorkBufferSize distinto de 0xC800 en GenericLoad (su efecto se cubre en LoadDataFromWorkBuffer-org), tamaños de búfer menores que 0xC800 saneados a pos = kBuf para evitar la recursión infinita del propio EXE, y la rama NOTSA de versión de GenericLoad. Desviaciones NOTSA dejadas: la comprobación de versión en GenericLoad y el nombre de los bloques 25/26 en ReportError. Antes de la corrida se corrigió la fuente (umbral y resto de Save/Load, ReportError(bloque-1, 2) en GenericLoad, bucle de relleno hasta 0x317FC y strncpy 0x103 en GenericSave, sin mensaje para códigos > 2 en ReportError; sin confirmar en git al registrar). Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/StorageFinal1/validation-summary.json).\n\n'
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
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6908 con evidencia histórica, 0 con vigencia acreditada y 1538 pendientes en {GROUPS} grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n';p.write_text(prefix+s,encoding='utf8');updated.append(name_)
for p,h in frozen.items():assert sha(R/p)==h,p
snapshot={'historical':6908,'pending':1538,'pending_groups':GROUPS,'current':0,'new_approvals':172}
(HERE/'documentation-refresh-cut172.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
