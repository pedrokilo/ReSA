"""Documentary cut440 (+9 funciones: WeaponFinal2); preserves byte-exact frozen Markdown and leaves documentation-frozen-exceptions.json untouched."""
import csv,collections,hashlib,json,posixpath,re,subprocess
from pathlib import Path
R=Path(__file__).resolve().parents[2];D=R/'reversing/RequestedValidation20261008/Diagnostics';HERE=Path(__file__).resolve().parent
CFG={'old_appr': 751,
 'old_valid': 7487,
 'old_pend': 959,
 'old_groups': 14,
 'n': 9,
 'groups_drop': 1,
 'old_word': 'Setecientas cincuenta y una',
 'new_word': 'Setecientas sesenta',
 'intro': 'nueve de CWeapon (WeaponFinal2)',
 'complete': ['CWeapon'],
 'pending_counts': {},
 'summary': 'reversing/WeaponFinal2/validation-summary.json',
 'pairs': {'CWeapon::ProcessLineOfSight': 256,
           'CWeapon::DoWeaponEffect': 512,
           'CWeapon::FireOneInstantHitRound': 768,
           'CWeapon::GenerateDamageEvent': 1024,
           'CWeapon::DoBulletImpact': 2048,
           'CWeapon::TakePhotograph': 640,
           'CWeapon::FireInstantHitFromCar2': 640,
           'CWeapon::FireSniper': 640,
           'CWeapon::FireProjectile': 1280},
 'kind': 'CPU en el juego completo con partida cargada, entidades, pools y lista de referencias falsos y eventos, animaciones y colisión doblados con traza '
         '(banco simple, passed=true global)',
 'lim': {'CWeapon::DoBulletImpact': 'eventos, animaciones y colision doblados; tColLighting::GetCurrentLighting doblada y FUERA de alcance; entidades falsas e '
                                    'intercambio temporal de pools de peds y objetos; fuente corregida sin FIX_BUGS (0x73B6C3), estadisticas 0x80/0x7E, '
                                    'AreInSameGroup una sola vez, asin x87 (0x821E70); ',
         'CWeapon::DoWeaponEffect': 'colaboradores de efectos doblados con traza; tipos de arma limitados a 0..46 y 49..58 (47/48 hacen assert en '
                                    'GetWeaponInfoIndex); ',
         'CWeapon::FireInstantHitFromCar2': 'vehiculo falso con BurstTyre en la ranura 0x2C de la vtable; colision, eventos y animaciones doblados con traza; ',
         'CWeapon::FireOneInstantHitRound': 'colision, eventos y animaciones doblados con traza; asin x87 replicado (0x73B11A); tipos de arma 0..46 y 49..58; ',
         'CWeapon::FireProjectile': 'proyectiles, eventos, animaciones y efectos doblados con traza; tipos de arma 0..46 y 49..58; ',
         'CWeapon::FireSniper': 'colaboradores de colision y efectos doblados con traza; fuente corregida: dir*16 (0x73ABFC..0x73AC27); ',
         'CWeapon::GenerateDamageEvent': 'eventos construidos de verdad sobre entidades falsas con CReferences::pEmptyList intercambiada por una lista falsa; '
                                         'CEventGroup::Add doblada (resumen campo a campo); tColLighting::GetCurrentLighting DOBLADA y FUERA de alcance; '
                                         'animaciones (AddAnimation/BlendAnimation/SetCurrentTime) dobladas; fuente corregida sin FIX_BUGS (743C60 en '
                                         '0x73A6E5, SetCurrentTime(0) en 0x73A7A4/0x73A657); ',
         'CWeapon::ProcessLineOfSight': 'prueba de linea de colision del mundo y colaboradores doblados con traza; el CColPoint solo se mira si hubo impacto; '
                                        'tipos de arma 0..46 y 49..58; ',
         'CWeapon::TakePhotograph': 'pools de peds y objetos intercambiados temporalmente por cabeceras falsas (6 peds de 0x7C4, 6 objetos de 0x19C) y '
                                    'restaurados; colaboradores de camara y visibilidad doblados con traza; '},
 'note': 'WeaponFinal2 añade la aprobación de las nueve funciones de CWeapon, 7808 pares con cero diferencias. La corrida 20261009-181216-969478 tiene '
         'passed=true global y evento complete con state_restored=true y layout_ok=true, evento layout correcto, saves y configuraciones intactos, con el '
         'banco simple (hook apagado) frente a la candidata dentro del juego completo con partida cargada. Límites: tColLighting::GetCurrentLighting doblada y '
         'fuera de alcance; eventos (CEventGroup::Add), animaciones y colisión doblados con traza; entidades falsas, pools de peds y objetos y lista de '
         'referencias intercambiados temporalmente y restaurados; tipos de arma 0..46 y 49..58; colaboradores doblados con traza (sin crédito). Correcciones '
         'de fuente: GenerateDamageEvent (0x73A6E5, 0x73A7A4/0x73A657), DoBulletImpact (0x73B6C3, 0x821E70), FireOneInstantHitRound (0x73B11A) y FireSniper '
         '(0x73ABFC..0x73AC27). No quedan pendientes en CWeapon. Aprobación histórica con dependency_review=null; la vigencia sigue en 0. '
         '[Resumen](../reversing/WeaponFinal2/validation-summary.json).'}
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
(HERE/'documentation-refresh-cut440.json').write_text(json.dumps({'date':'2026-10-09','markdown_files':updated,'count':len(updated),'catalog_sha256':sha(R/'docs/hooks.csv'),'snapshot':snapshot},indent=2)+'\n',encoding='utf8');print(json.dumps({'updated':len(updated),'frozen':len(frozen),'snapshot':snapshot}))
