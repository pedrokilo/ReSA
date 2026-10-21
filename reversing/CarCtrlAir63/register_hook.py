"""Registra las funciones de CCarCtrl (parte A: helicopteros, aviones, lanchas y combate aereo) del banco CarCtrlAir63 con la corrida APROBADA.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-carctrlair63/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de Garage63 adaptada. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/CarCtrlAir63/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador):
`CCarCtrl,<Funcion>,<direccion>,1,0,1,S` para cada una (todas Simple, con la direccion real del EXE).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'CarCtrlAir63'
SRC_FILE = 'source/game_sa/CarCtrl.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CCarCtrl::GetAIHeliToFlyInDirection', '0x0042A730', 'CCarCtrl', 'GetAIHeliToFlyInDirection', SRC_FILE, 'S'),
    ('CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer', '0x0042F370', 'CCarCtrl', 'GetAIPlaneToDoDogFightAgainstPlayer', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliToCrashAndBurn', '0x004238E0', 'CCarCtrl', 'SteerAIHeliToCrashAndBurn', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIPlaneToCrashAndBurn', '0x00423880', 'CCarCtrl', 'SteerAIPlaneToCrashAndBurn', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIPlaneTowardsTargetCoors', '0x00423790', 'CCarCtrl', 'SteerAIPlaneTowardsTargetCoors', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIPlaneToFollowEntity', '0x004237F0', 'CCarCtrl', 'SteerAIPlaneToFollowEntity', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliTowardsTargetCoors', '0x0042A630', 'CCarCtrl', 'SteerAIHeliTowardsTargetCoors', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliFlyingAwayFromPlayer', '0x0042ACB0', 'CCarCtrl', 'SteerAIHeliFlyingAwayFromPlayer', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliToLand', '0x0042AD30', 'CCarCtrl', 'SteerAIHeliToLand', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliAsPoliceHeli', '0x0042AAD0', 'CCarCtrl', 'SteerAIHeliAsPoliceHeli', SRC_FILE, 'S'),
    ('CCarCtrl::TestWhetherToFirePlaneGuns', '0x00429520', 'CCarCtrl', 'TestWhetherToFirePlaneGuns', SRC_FILE, 'S'),
    ('CCarCtrl::TriggerDogFightMoves', '0x00429300', 'CCarCtrl', 'TriggerDogFightMoves', SRC_FILE, 'S'),
    ('CCarCtrl::GetAIPlaneToAttackPlayer', '0x00429780', 'CCarCtrl', 'GetAIPlaneToAttackPlayer', SRC_FILE, 'S'),
    ('CCarCtrl::GetAIPlaneToDoDogFight', '0x00429890', 'CCarCtrl', 'GetAIPlaneToDoDogFight', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliToFollowEntity', '0x0042A750', 'CCarCtrl', 'SteerAIHeliToFollowEntity', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIHeliToKeepEntityInView', '0x0042AEB0', 'CCarCtrl', 'SteerAIHeliToKeepEntityInView', SRC_FILE, 'S'),
    ('CCarCtrl::GetAIHeliToAttackPlayer', '0x0042F3C0', 'CCarCtrl', 'GetAIHeliToAttackPlayer', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget', '0x00428BE0', 'CCarCtrl', 'SteerAIBoatWithPhysicsHeadingForTarget', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer', '0x00428DE0', 'CCarCtrl', 'SteerAIBoatWithPhysicsAttackingPlayer', SRC_FILE, 'S'),
    ('CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer', '0x00429090', 'CCarCtrl', 'SteerAIBoatWithPhysicsCirclingPlayer', SRC_FILE, 'S'),
    ('CCarCtrl::FireHeliRocketsAtTarget', '0x0042B270', 'CCarCtrl', 'FireHeliRocketsAtTarget', SRC_FILE, 'S'),
    ('CCarCtrl::FlyAIHeliToTarget_FixedOrientation', '0x00423940', 'CCarCtrl', 'FlyAIHeliToTarget_FixedOrientation', SRC_FILE, 'S'),
    ('CCarCtrl::FlyAIPlaneInCertainDirection', '0x00423000', 'CCarCtrl', 'FlyAIPlaneInCertainDirection', SRC_FILE, 'S'),
    ('CCarCtrl::FlyAIHeliInCertainDirection', '0x00429A70', 'CCarCtrl', 'FlyAIHeliInCertainDirection', SRC_FILE, 'S'),
)
SCOPE_NOTE = 'Validado en el worktree D:/Original/wh-1 (HEAD df26d6fe7 + fuente del lote 63 CarCtrlAir63, CCarCtrl parte A). Todos los colaboradores se sustituyen por falsos identicos en ambos lados (no se validan sus cuerpos ni la integracion con el mundo real): CGeneral::GetATanOfXY (0x53CC70), CWorld::FindPlayerVehicle (0x56E0D0), FindPlayerPed (0x56E210), FindPlayerCoors (0x56E010), FindPlayerSpeed (0x56E090), FindPlayerEntity (0x56E120) y ProcessLineOfSight (0x56BA00), CVector::Normalise (0x59C910), la rand() del CRT del EXE (0x821B1E), CVehicle::GetVehicleAppearance (0x6D1080), CPlaceable::AllocateMatrix (0x54F560), CSimpleTransform::UpdateMatrix (0x54F1B0), CTaskManager::GetActiveTask (0x681720) y CTask::GetTaskType (virtual), CPlane::SetGearUp (0x6CAC20), CCarCtrl::PossiblyFireHSMissile (0x429600), CCollision::DistToMathematicalLine (0x412970), CProjectileInfo::AddProjectile (0x737C80), la funcion 0x422F80 del EXE (sondeo de rumbo del avion, sin reconstruir), 0x73DF00 (lancha modelo 430, sin reconstruir) y, cuando una funcion del lote llama a otra (p. ej. GetAIHeliToAttackPlayer -> FlyAIHeliInCertainDirection / TestWhetherToFirePlaneGuns / FireHeliRocketsAtTarget), la otra tambien es un falso en esa prueba. Los vehiculos, matrices, tareas, jugador y CColPoint son imagenes falsas del banco; ningun objeto real del juego se toca. NO hay oraculo independiente en Python: la referencia es el EXE, con 4096 casos deterministas por funcion que cubren umbrales exactos (+-1 ulp), cero con signo, NaN silenciosos, infinitos, subnormales y valores enormes; los NaN son siempre silenciosos (la x87 resuelve distinto un SNaN en memoria que un QNaN en ST(0) y ningun calculo del juego produce un SNaN). Los angulos que entran en bucles de normalizacion (+-2pi) se acotan a valores finitos y moderados porque con valores enormes el original no termina. El EXE lee m_matrix de los vehiculos sin comprobar nulo en varias funciones: esos casos usan siempre matriz. Las funciones con aritmetica x87 usan ayudantes de X87Math.cpp (prefijo CcAir) compilados con /O2 /Oi /arch:IA32 /fp:fast; fsqrt/fpatan/fsin/fcos van en ayudantes separados fuera de las regiones precise (dentro saldrian como llamadas a la CRT). EAX/ECX/EDX de salida solo se registran: ningun llamador del EXE los lee (llamadores: el despachador 0x436A90 y las propias funciones del lote, comprobados con callers.py sobre todo el .text). FlyAIHeliToTarget_FixedOrientation: el EXE recibe un cuarto argumento (true) que no lee; el hook mantiene los tres de la declaracion existente.'
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main(src):
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
    RUN.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    if not (RUN / 'comparisons.jsonl.gz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    ppf = dict(s['pairs_per_function'])
    assert set(ppf) == {f[0] for f in FUNCS}, ppf
    tested = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['sources']
    scope = SCOPE + (' ' + SCOPE_NOTE if SCOPE_NOTE else '')
    ev = dict(passed=True, pairs=int(s['pairs']), differences=0, registered_pairs=int(s['pairs']), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=scope, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE, KIND in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            evidence = {'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                        'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']}
            if KIND == 'V':
                evidence['original_address'] = ADDR   # los hooks virtuales se registran con direccion 0x00000000 y tipo V
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR if KIND == 'S' else '0x00000000', 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco Garage63 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (EAX de los HRESULT si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
