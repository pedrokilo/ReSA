"""Documentary cut419 (+2 funciones GfxWave2: CWaterLevel::RenderFlatWaterRectangle y SplitWaterTriangleAlongXLine); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
rows=list(csv.DictReader((R/'docs/hooks.csv').open(encoding='utf-8-sig')));pending=collections.Counter(x['class'] for x in rows if x['validated']=='0')
VALID=sum(x['validated']=='1' for x in rows);PEND=sum(pending.values());GROUPS=len(pending)
p=R/'docs/CurrentStatus.md';s=p.read_text(encoding='utf8')
OLD_APPR=417;NEW_APPR=419;OLD_VALID=7153;OLD_PEND=1293;OLD_GROUPS=34
assert VALID==OLD_VALID+2 and PEND==OLD_PEND-2,(VALID,PEND)
assert 'quedan %d en %d grupos'%(OLD_PEND,OLD_GROUPS) in s
assert GROUPS==OLD_GROUPS,GROUPS
old=json.loads((R/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text());frozen={p_:x['sha256'] for p_,x in old['frozen_markdown_exceptions'].items()}
for p_,h in frozen.items():assert sha(R/p_)==h,p_
for a,b in ((f'{OLD_APPR} aprobaciones nuevas',f'{NEW_APPR} aprobaciones nuevas'),(f'{OLD_APPR} identidades nuevas',f'{NEW_APPR} identidades nuevas'),(f'{OLD_VALID} entradas',f'{VALID} entradas'),(f'{OLD_PEND} pendientes',f'{PEND} pendientes'),
 (f'quedan {OLD_PEND} en {OLD_GROUPS} grupos',f'quedan {PEND} en {GROUPS} grupos'),('Cuatrocientas diecisiete aprobaciones nuevas','Cuatrocientas diecinueve aprobaciones nuevas'),
 ('cincuenta y dos de CClothesBuilder, CShadows y CWaterLevel (GfxWave2, aprobación parcial por función)','cincuenta y cuatro de CClothesBuilder, CShadows y CWaterLevel (GfxWave2, aprobación parcial por función, en dos corridas)')):
 assert a in s,a;s=s.replace(a,b)
s=re.sub(r'Lista de \d+ excepciones',f'Lista de {len(frozen)} excepciones',s)
reg=json.loads((R/'reversing/validation-registry.json').read_text(encoding='utf-8'));new=[x for x in reg['functions'] if x['evidence']['path']=='reversing/GfxWave2/validation-summary-b.json']
assert len(new)==2 and sum(x['evidence']['pairs'] for x in new)==1536
LIM={'CWaterLevel::RenderFlatWaterRectangle':'fuente corregida antes (P2 como tercer CRenPar en la mitad inferior de Y, como el EXE; lerp = (1-t)*a + t*b); hermanas de dibujo aisladas sin crédito; ','CWaterLevel::SplitWaterTriangleAlongXLine':'fuente corregida antes (ramas X1==X3 y X2==X3 como el EXE); hermanas de dibujo aisladas sin crédito; '}
newrows=''
for x in new:
 name=x['hook']['class']+'::'+x['hook']['fn_name'];e=x['evidence'];assert '| '+name+' |' not in s
 newrows+=f"| {name} | {e['pairs']} | [Resumen](../{e['path']}) | CPU en el juego completo con objetos falsos y dibujo sustituido por trazas (aprobación parcial por función, segunda corrida 20261009-100807-276064; la corrida global tiene passed=false por CWaterLevel::AddWaterLevelQuad); {LIM[name]}x87 PC24, traza de llamadas con argumentos. |\n"
pos=s.index('\n\n',s.index('| Función | Pares aprobados |'));s=s[:pos]+'\n'+newrows.rstrip('\n')+s[pos:]
wl=pending['CWaterLevel']
anchor='[Resumen](../reversing/GfxWave2/validation-summary.json).'+chr(10)+chr(10)
note=('GfxWave2, segundo incremento: la corrida 20261009-100807-276064 (27616 pares, passed=false) deja a CWaterLevel::AddWaterLevelQuad como única función con diferencias (254/1024) y las otras 54 con todos sus pares iguales; se registran SOLO CWaterLevel::RenderFlatWaterRectangle (1024 pares) y CWaterLevel::SplitWaterTriangleAlongXLine (512 pares), rechazadas en la corrida anterior (472/1024 y 258/512) y aprobadas tras corregir la fuente al comportamiento del EXE; las 52 anteriores no se duplican y AddWaterLevelQuad sigue rechazada, sin registro ni crédito. Saves y configuraciones intactos, state_restored=true. '
 f'CWaterLevel conserva {wl} pendientes en hooks.csv. Aviso: WaterPart.hpp del banco se modificó en el árbol de trabajo después de esta corrida (trabajo sobre AddWaterLevelQuad); el registro conserva el hash probado. Las 19 entradas anteriores de CWaterLevel conservan el hash de WaterLevel.cpp probado en la primera corrida. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/GfxWave2/validation-summary-b.json).')+chr(10)+chr(10)
assert anchor in s
old_wl='CWaterLevel 6 (las tres rechazadas,';assert old_wl in s
s=s.replace(old_wl,'CWaterLevel 4 (AddWaterLevelQuad rechazada; RenderFlatWaterRectangle y SplitWaterTriangleAlongXLine, rechazadas en esa corrida, se registraron después, ver abajo;',1)
s=s.replace(anchor,anchor+note,1);a=s.index('| Clase/grupo | Funciones sin validar |');s=s[:a]+'| Clase/grupo | Funciones sin validar |\n| --- | ---: |\n'+'\n'.join(f'| {c} | {n} |' for c,n in sorted(pending.items()))+'\n';p.write_text(s,encoding='utf8')
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
(HERE/'documentation-refresh-cut419.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
