"""Authorized cut124 documentary update; preserves byte-exact frozen Markdown."""
import csv,collections,hashlib,json,posixpath,re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3];sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(r['class'] for r in rows if r['validated']=='0');assert sum(r['validated']=='1' for r in rows)==6860 and sum(pending.values())==1586 and len(pending)==58
plan=json.loads((Path(__file__).parent/'documentation-plan-107.json').read_text());old=json.loads((ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text())
frozen={p:r['actual_sha256'] for p,r in plan['exact_frozen_markdown'].items()}
for p,r in old['frozen_markdown_exceptions'].items():
 if (ROOT/p).exists() and sha(ROOT/p)==r['sha256']:frozen[p]=r['sha256']
hexmd='reversing/RequestedPadDistanceTrainNopHex/README.md'
if (ROOT/hexmd).exists():frozen[hexmd]=sha(ROOT/hexmd)
for p,h in frozen.items():assert sha(ROOT/p)==h,p
p=ROOT/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8');s=s.replace('112 aprobaciones nuevas','124 aprobaciones nuevas').replace('112 identidades nuevas','124 identidades nuevas').replace('6848 entradas','6860 entradas').replace('1598 pendientes','1586 pendientes').replace('**60 grupos**','**58 grupos**').replace('Ciento doce aprobaciones nuevas','Ciento veinticuatro aprobaciones nuevas')
s=s.replace('cuatro despachos CPU, hora','cuatro despachos CPU, ArrestPed, FxEmitter, cinco wrappers de geometría y el getter de radio frontend, hora')
registry=json.loads((ROOT/'reversing/validation-registry.json').read_text(encoding='utf8'));added=[]
for r in registry['functions'][-12:]:
 name=r['hook']['class']+'::'+r['hook']['fn_name'];e=r['evidence']
 if '| '+name+' |' in s:continue
 scope=('Estados privados, relojes uint32 y duraciones signed; gates frontend/cutscene. Sin controlador real ni vibración del sistema.' if name=='CPad::StartShake' else 'Despacho CPU privado, orden/trazas y bytes/retorno definidos. Sin implementación de colaboradores, gráficos ni ciclo de vida de recursos.')
 if name.startswith('CTaskComplexArrestPed::'):scope='Arenas privadas, vptr/retorno exactos y trazas de referencias, child, armas o selección. Sin actores ni helpers acreditados.'
 if name.startswith('FxEmitterBP_c::'):scope='Objetos y texturas opacas privados; constructor CPU real o lecturas trazadas; NULL conserva handles. Sin assets, gráficos ni lifecycle de recursos.'
 if name.startswith('CPedGeometryAnalyser::'):scope='Wrappers con helper nativo read-only, fixtures geométricos finitos y traza exacta; sin actors ni acreditación del helper.'
 if name=='CAEFrontendAudioEntity::IsRadioTuneSoundActive':scope='Getter CPU con bytes privados y retorno AL crudo; sin subsistema de radio/sonido acreditado.'
 added.append(f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | {scope} |")
marker='## Pendientes por clase';heading=s.index(marker);table_end=s.rfind('| CPad::Update |');assert table_end>=0;insert=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:insert]+'\n'+'\n'.join(added)+s[insert:]
note='StartShake añadió 89600 casos con cero diferencias, incluyendo relojes envueltos y duraciones negativas como caracterización finita; compara arenas completas y FP sin normalizar bytes. Las cuatro funciones de DispatchShadow añadieron 5120 casos CPU privados: Initialise/Shutdown/PluginAttach de CCustomBuildingRenderer y ReturnRealTimeShadow. Son aprobaciones históricas con dependency_review=null; no acreditan gráficos, helpers ni recursos reales. Los bancos posteriores de Arrest/Fx/Distance/Geometry continúan pendientes hasta sus replays y auditorías.\n\n'
newnote='ArrestPed aprobó sus cuatro identidades en 2816 pares con salida natural 0 y restauración de 17692 rangos: constructor 128, destructor 256, MakeAbortable 384 y CreateFirstSubTask 2048. FxEmitter aprobó constructor 512 y LoadTextures 5120 en replays de 5632; el baseline conserva 512 diferencias de LoadTextures ante NULL, corregidas sin cambiar corpus ni modelo. El runner inicial observaba un nombre de archivo distinto: se conservan esos rechazos y se usó un adaptador externo con ASI/corpus congelados. Las aprobaciones separan vptr reales declarados, colaboradores aislados y límites de lifecycle. Los cinco wrappers de GeometryV4 añadieron 5120 con harness global completo; V3 no se acreditó por omitirlo. El getter frontend añadió 2048 casos CPU privados en su replay V2. La vigencia permanece 0.\n\n'
if newnote not in s:s=s.replace(marker,newnote+marker)
a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
updated=[]
for name in plan['eligible_current_markdown']:
 if name in frozen or name=='docs/CurrentStatus.md':continue
 p=ROOT/name
 if not p.exists():continue
 try:s=p.read_text(encoding='utf-8-sig')
 except UnicodeDecodeError:s=p.read_text(encoding='cp1252')
 s=re.sub(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*','',s,flags=re.S)
 link=posixpath.relpath('docs/CurrentStatus.md',posixpath.dirname(name) or '.')
 prefix=f'<!-- RESA-CURRENT-STATUS:BEGIN -->\n> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6860 con evidencia histórica, 0 con vigencia acreditada y 1586 pendientes en 58 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance]({link}). Las mediciones anteriores conservan su fecha y alcance histórico.\n<!-- RESA-CURRENT-STATUS:END -->\n\n'
 p.write_text(prefix+s,encoding='utf8');updated.append(name)
for name,h in frozen.items():assert sha(ROOT/name)==h,name
exceptions={'snapshot':'6860 documentary checkpoint; exact current frozen Markdown and dated archived evidence','frozen_markdown_exceptions':{p:{'sha256':h,'referenced_by':([x['reference'] for x in plan['exact_frozen_markdown'].get(p,{}).get('exact_references',[])] or old['frozen_markdown_exceptions'].get(p,{}).get('referenced_by',[]) or ['Root coordination: preserve pending DistanceHex bank README byte-exact'])} for p,h in frozen.items()}}
(ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').write_text(json.dumps(exceptions,indent=2)+'\n',encoding='utf8')
(ROOT/'reversing/RequestedValidation20261008/documentation-refresh.json').write_text(json.dumps({'date':'2026-10-08','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(ROOT/'docs/hooks.csv'),'snapshot':{'historical':6860,'pending':1586,'pending_groups':58,'current':0,'new_approvals':124}},indent=2)+'\n',encoding='utf8')
print('Updated',len(updated),'prefixes; frozen',len(frozen),'exact; central124rows.')
