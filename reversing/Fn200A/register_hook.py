"""Registra las 31 funciones del lote 200A con la corrida APROBADA de Fn200A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn200a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn200A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones registradas (NO ejecuta sync_docs ni hooks2md). Argumentos opcionales tras la carpeta: nombres de funciones EXCLUIDAS (no se registran).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn200A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CBoundingBox::ConstructorAlias5B2AB0', '0x005B2AB0', 'CBoundingBox', 'ConstructorAlias5B2AB0', 'source/game_sa/Collision/BoundingBox.cpp'),
    ('CTaskComplexStuckInAir::DestructorBody', '0x0067BA70', 'CTaskComplexStuckInAir', 'DestructorBody', 'source/game_sa/Tasks/TaskTypes/TaskComplexStuckInAir.cpp'),
    ('CTaskSimpleGetUp::Clone', '0x0067C0C0', 'CTaskSimpleGetUp', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp'),
    ('CTaskComplexGetUpAndStandStill::Clone', '0x0067C130', 'CTaskComplexGetUpAndStandStill', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexGetUpAndStandStill.cpp'),
    ('CTaskSimpleFall::Clone', '0x0067C180', 'CTaskSimpleFall', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp'),
    ('CTaskComplexFallAndGetUp::Clone', '0x0067C200', 'CTaskComplexFallAndGetUp', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp'),
    ('CTaskComplexInAirAndLand::Clone', '0x0067C2E0', 'CTaskComplexInAirAndLand', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp'),
    ('CTaskSimpleInAir::Clone', '0x0067C360', 'CTaskSimpleInAir', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp'),
    ('CTaskSimpleLand::Clone', '0x0067C3F0', 'CTaskSimpleLand', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp'),
    ('CTaskComplexFallToDeath::Clone', '0x0067C480', 'CTaskComplexFallToDeath', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallToDeath.cpp'),
    ('CTaskSimpleGetUp::ScalarDeletingDestructor', '0x0067C750', 'CTaskSimpleGetUp', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGetUp.cpp'),
    ('CTrain::ScalarDeletingDestructor', '0x006F6300', 'CTrain', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Vehicle/Train.cpp'),
    ('CTrain::GetDooorAngleOpenRatioU32', '0x006F5930', 'CTrain', 'GetDooorAngleOpenRatioU32', 'source/game_sa/Entity/Vehicle/Train.cpp'),
    ('CTrain::GetDooorAngleOpenRatio', '0x006F59C0', 'CTrain', 'GetDooorAngleOpenRatio', 'source/game_sa/Entity/Vehicle/Train.cpp'),
    ('CTrain::IsRoomForPedToLeaveCar', '0x006F5EE0', 'CTrain', 'IsRoomForPedToLeaveCar', 'source/game_sa/Entity/Vehicle/Train.cpp'),
    ('CTrain::OpenDoor', '0x006F6370', 'CTrain', 'OpenDoor', 'source/game_sa/Entity/Vehicle/Train.cpp'),
    ('CTrain::Teleport', '0x006F7190', 'CTrain', 'Teleport', 'source/game_sa/Entity/Vehicle/Train.cpp'),
    ('CVector::DivideAssignExe', '0x00411A30', 'CVector', 'DivideAssignExe', 'source/game_sa/Core/Vector.cpp'),
    ('CColPoint::GetNormalAddress', '0x00411BA0', 'CColPoint', 'GetNormalAddress', 'source/game_sa/Collision/ColPoint.cpp'),
    ('CColPoint::SetSurfaceTypeA', '0x00411BF0', 'CColPoint', 'SetSurfaceTypeA', 'source/game_sa/Collision/ColPoint.cpp'),
    ('CPtrNodeSingleLinkPool::FlushJump', '0x0043E1A0', 'CPtrNodeSingleLinkPool', 'FlushJump', 'source/game_sa/Core/PtrNodeSingleLink.cpp'),
    ('CMatrixLinkList::Constructor', '0x0054F440', 'CMatrixLinkList', 'Constructor', 'source/game_sa/Core/MatrixLinkList.cpp'),
    ('CPlaceable::DestructorBody', '0x0054F490', 'CPlaceable', 'DestructorBody', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ScalarDeletingDestructor', '0x0054F5D0', 'CPlaceable', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FA10', '0x0054FA10', 'CPlaceable', 'ArrayPairReset54FA10', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FA60', '0x0054FA60', 'CPlaceable', 'ArrayPairReset54FA60', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FAB0', '0x0054FAB0', 'CPlaceable', 'ArrayPairReset54FAB0', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FB00', '0x0054FB00', 'CPlaceable', 'ArrayPairReset54FB00', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FB50', '0x0054FB50', 'CPlaceable', 'ArrayPairReset54FB50', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FBA0', '0x0054FBA0', 'CPlaceable', 'ArrayPairReset54FBA0', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::ArrayPairReset54FBF0', '0x0054FBF0', 'CPlaceable', 'ArrayPairReset54FBF0', 'source/game_sa/Entity/Placeable.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CTaskSimpleGetUp::Clone', 'CTaskComplexGetUpAndStandStill::Clone', 'CTaskSimpleFall::Clone', 'CTaskComplexFallAndGetUp::Clone', 'CTaskComplexInAirAndLand::Clone', 'CTaskSimpleInAir::Clone', 'CTaskSimpleLand::Clone', 'CTaskComplexFallToDeath::Clone', 'CTrain::GetDooorAngleOpenRatioU32', 'CTrain::GetDooorAngleOpenRatio', 'CTrain::IsRoomForPedToLeaveCar', 'CTrain::OpenDoor', 'CTrain::Teleport'}


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def update_hooks_csv(funcs):
    """validated = 1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas"""
    path = ROOT / 'docs' / 'hooks.csv'
    raw = path.read_bytes().decode('utf-8')
    crlf = '\r\n' in raw
    lines = raw.replace('\r\n', '\n').split('\n')
    done = set()
    for n, line in enumerate(lines):
        f = line.split(',')
        if len(f) == 7:
            for ID, ADDR, CLS, FN, SOURCE in funcs:
                if f[0] == CLS and f[1] == FN and (f[2] == ADDR or f[2] == '0x00000000' and ID in VIRTUAL):
                    f[5] = '1'
                    lines[n] = ','.join(f)
                    done.add(ID)
    missing = [ID for ID, *_ in funcs if ID not in done]
    if missing:
        raise SystemExit('Filas de hooks.csv sin encontrar: %s' % missing)
    text = '\n'.join(lines)
    path.write_bytes((text.replace('\n', '\r\n') if crlf else text).encode('utf-8'))


def main(src, excluded=()):
    global FUNCS
    FUNCS = tuple(f for f in FUNCS if f[0] not in excluded)
    SRC = Path(src)
    RUN = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + SRC.name)
    RUN.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (RUN / n).exists():
            shutil.copyfile(SRC / n, RUN / n)
    if not (RUN / 'comparisons.jsonl.gz').exists() and not (RUN / 'comparisons.jsonl.xz').exists():
        with open(SRC / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(RUN / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
        # GitHub rechaza ficheros de mas de 100 MB: por encima de ~90 MB se guarda como .xz
        if (RUN / 'comparisons.jsonl.gz').stat().st_size > 90 * 1000 * 1000:
            import lzma
            with open(SRC / 'comparisons.jsonl', 'rb') as f, lzma.open(RUN / 'comparisons.jsonl.xz', 'wb', preset=6) as g:
                shutil.copyfileobj(f, g)
            (RUN / 'comparisons.jsonl.gz').unlink()
    s = json.loads((RUN / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    ppf = dict(s['pairs_per_function'])
    assert {f[0] for f in FUNCS} <= set(ppf), ppf
    tested = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['sources']
    scope = SCOPE + (' ' + SCOPE_NOTE if SCOPE_NOTE else '')
    ev = dict(passed=True, pairs=int(s['pairs']), differences=0, registered_pairs=int(s['pairs']), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=scope, dependency_review=None, run=RUN.relative_to(ROOT).as_posix(),
              summary_sha256=sha(RUN / 'summary.json'), result_txt_sha256=sha(RUN / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    data = registry_store.load_registry(ROOT)
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if ID in VIRTUAL else ADDR, 'type': 'V' if ID in VIRTUAL else 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], **({'original_address': ADDR} if ID in VIRTUAL else {})},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn200A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    update_hooks_csv(FUNCS)
    print('ok', len(FUNCS), 'funciones registradas')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    main(sys.argv[1], tuple(sys.argv[2:]))
