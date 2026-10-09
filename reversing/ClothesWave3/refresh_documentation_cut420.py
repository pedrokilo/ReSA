"""Documentary cut420 (+1 funcion: CClothesBuilder::CreateSkinnedClump via ClothesWave3); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=419;NEW_APPR=420;OLD_VALID=7155;OLD_PEND=1291;OLD_GROUPS=34
assert VALID==OLD_VALID+1 and PEND==OLD_PEND-1,(VALID,PEND)
assert 'quedan %d en %d grupos'%(OLD_PEND,OLD_GROUPS) in s or 'distribuidas en **%d grupos**'%OLD_GROUPS in s
assert GROUPS==OLD_GROUPS-1,GROUPS
assert 'CClothesBuilder' not in pending
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'distribuidas en **{OLD_GROUPS} grupos**',f'distribuidas en **{GROUPS} grupos**'),('Cuatrocientas diecinueve aprobaciones nuevas','Cuatrocientas veinte aprobaciones nuevas'),
 ('(GfxWave2, aprobación parcial por función, en dos corridas), hora','(GfxWave2, aprobación parcial por función, en dos corridas) y una de CClothesBuilder (ClothesWave3), hora')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/ClothesWave3/validation-summary.json']
assert len(new)==1 and new[0]['evidence']['pairs']==256
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con descriptores, texturas, geometrías, pieles, marcos y clump falsos; RenderWare, streaming y pools de ropa sustituidos por 34 dobles trazados sin crédito; fuente corregida antes (color del material en línea antes de RpMaterialSetTexture; RpSkinGetNumBones y RpSkinGetSkinToBoneMatrices antes de RpSkinCreate); ConstructTextures, ConstructGeometryArray y ConstructGeometryAndSkinArrays no se ejecutan (dobladas); SetModel y GetUppercaseKey reales; x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
marker='## Pendientes por clase'
note=('ClothesWave3 añade la aprobación de CClothesBuilder::CreateSkinnedClump (0x5A69D0), 256 pares con cero diferencias, evento complete con state_restored=true, saves y configuraciones intactos, con el banco simple (hook de la clase apagado) frente a la candidata dentro del juego completo con partida cargada. Con esta función CClothesBuilder queda COMPLETA en hooks.csv (sin pendientes). '
 'Correcciones de fuente previas a la corrida: 0x5A6B65-0x5A6B77 el color del material se escribe en línea ANTES de RpMaterialSetTexture; 0x5A6CAD-0x5A6CD0 RpSkinGetNumBones y RpSkinGetSkinToBoneMatrices se llaman antes de RpSkinCreate (locales explícitos); ClothesBuilder.cpp sin confirmar en git al registrar. '
 'Límites: 34 colaboradores doblados con trazas y sin crédito (RenderWare, streaming, pools de ropa); ConstructTextures, ConstructGeometryArray y ConstructGeometryAndSkinArrays NO se ejecutan en este banco (doblados; cubiertos por sus validaciones previas en GfxWave2); CPedClothesDesc::SetModel y CKeyGen::GetUppercaseKey reales. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/ClothesWave3/validation-summary.json).')+chr(10)+chr(10)
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
(HERE/'documentation-refresh-cut420.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
