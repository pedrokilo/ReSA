"""Documentary cut430 (+4 funciones: WaterWave4); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 439,
 'old_valid': 7175,
 'old_pend': 1271,
 'old_groups': 27,
 'n': 4,
 'groups_drop': 1,
 'old_word': 'Cuatrocientas treinta y nueve',
 'new_word': 'Cuatrocientas cuarenta y tres',
 'intro': 'cuatro de CWaterLevel (WaterWave4)',
 'complete': ['CWaterLevel'],
 'pending_counts': {},
 'summary': 'reversing/WaterWave4/validation-summary.json',
 'pairs': {'CWaterLevel::AddWaterLevelQuad': 1024, 'CWaterLevel::RenderBoatWakes': 512, 'CWaterLevel::SplitWaterTriangleAlongYLine': 1024, 'CWaterLevel::WaterLevelInitialise': 256},
 'kind': 'CPU en el juego completo con agua, barcos, matrices y archivo water.dat falsos y colaboradores doblados con traza (banco simple, passed=true global)',
 'lim': {'CWaterLevel::AddWaterLevelQuad': 'MATCH sin cambios de fuente (rechazada en GfxWave2 por un error del fixture, que contaba la Y mínima con la Y de entrada en vez de la clamped a -3000); ',
         'CWaterLevel::RenderBoatWakes': 'fuente reescrita (pasada inlineada: raster 0xC228B8, lista 0xC27994, ancho 0.65/0.4, distancia 50/80); ',
         'CWaterLevel::SplitWaterTriangleAlongYLine': 'fuente reescrita con las 4 ramas del EXE (jge 0x6EE5D7, jne 0x6EE5DF/0x6EED74; la rama D conserva un bug del EXE, 0x6EF3E1/0x6EF445: '
                                                      'lerp(P1,P3) dos veces); ',
         'CWaterLevel::WaterLevelInitialise': 'fuente corregida antes (rb, DATA\\water.dat 0x6EAF47, flags=1 por omisión 0x6EB069, líneas que empiezan por ; \\0 * p ignoradas, sscanf 29/28/22 sin '
                                              'comprobar, bug del EXE flowY del vértice 1 = el del vértice 2 en cuadriláteros); water.dat en memoria con sscanf real; '},
 'note': 'WaterWave4 añade la aprobación de las cuatro funciones pendientes de CWaterLevel (AddWaterLevelQuad 0x6E7EF0, RenderBoatWakes 0x6ED9A0, SplitWaterTriangleAlongYLine 0x6EE5A0 y '
         'WaterLevelInitialise 0x6EAE80), 2816 pares con cero diferencias. La corrida 20261009-123543-732613 tiene passed=true global y evento complete con state_restored=true y layout_ok=true, '
         'evento layout correcto, saves y configuraciones intactos, con el banco simple (hook de la función apagado) frente a la candidata dentro del juego completo con partida cargada. '
         'AddWaterLevelQuad figuraba RECHAZADA en GfxWave2 (sin registro allí) y se registra solo desde este banco. Con estas funciones CWaterLevel queda COMPLETA en hooks.csv (sin pendientes). '
         'Correcciones de fuente previas a la corrida (WaterLevel.cpp): SplitWaterTriangleAlongYLine reescrita con las 4 ramas del EXE (la rama D conserva un bug del EXE: lerp(P1,P3) dos veces), '
         'RenderBoatWakes transcribe la pasada inlineada, WaterLevelInitialise (rb, DATA\\water.dat, flags=1 por omisión, líneas ignoradas, sscanf sin comprobar, bug del EXE de flowY en '
         'cuadriláteros). Límites: water.dat en memoria con sscanf real; barcos, ColModel y matrices falsos; dominio excluido por pila sin inicializar (Y mínima 1 o 3, config distinta de 0/1, líneas '
         'mal formadas, barco sin matriz). Aprobación histórica con dependency_review=null; la vigencia sigue en 0. [Resumen](../reversing/WaterWave4/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut430.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
