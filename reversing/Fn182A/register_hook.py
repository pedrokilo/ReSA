"""Registra las 23 funciones del lote 182A con la corrida APROBADA de Fn182A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn182a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn182A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Ademas pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones registradas (NO ejecuta sync_docs ni hooks2md). Argumentos opcionales tras la carpeta: nombres de funciones EXCLUIDAS (no se registran).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn182A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CGame::ActivateMenuOnSetFocus', '0x0053BC60', 'CGame', 'ActivateMenuOnSetFocus', 'source/game_sa/Game.cpp'),
    ('CGame::ReleaseBlockA', '0x0053C860', 'CGame', 'ReleaseBlockA', 'source/game_sa/Game.cpp'),
    ('CGame::ReleaseBlockB', '0x0053C890', 'CGame', 'ReleaseBlockB', 'source/game_sa/Game.cpp'),
    ('CTheZones::GetCurrentZoneLockedOrUnlocked', '0x00572130', 'CTheZones', 'GetCurrentZoneLockedOrUnlocked', 'source/game_sa/TheZones.cpp'),
    ('CPad::IsEscJustPressed', '0x00572DB0', 'CPad', 'IsEscJustPressed', 'source/game_sa/Pad.cpp'),
    ('CPad::IsMouseRButtonPressed', '0x00572E70', 'CPad', 'IsMouseRButtonPressed', 'source/game_sa/Pad.cpp'),
    ('CSaveGarage::CopyGarageIntoSaveGarage', '0x005D3020', 'CSaveGarage', 'CopyGarageIntoSaveGarage', 'source/game_sa/Garage.cpp'),
    ('CSaveGarage::CopyGarageOutOfSaveGarage', '0x005D30C0', 'CSaveGarage', 'CopyGarageOutOfSaveGarage', 'source/game_sa/Garage.cpp'),
    ('CTaskSimpleTriggerLookAt::Constructor', '0x00634440', 'CTaskSimpleTriggerLookAt', 'Constructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp'),
    ('CTaskSimpleTriggerLookAt::GetTaskType', '0x006344F0', 'CTaskSimpleTriggerLookAt', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp'),
    ('CTaskSimpleTriggerLookAt::Clone', '0x00634560', 'CTaskSimpleTriggerLookAt', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp'),
    ('CTaskSimpleTriggerLookAt::MakeAbortable', '0x00634610', 'CTaskSimpleTriggerLookAt', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp'),
    ('CTaskSimpleTriggerLookAt::ProcessPed', '0x00634620', 'CTaskSimpleTriggerLookAt', 'ProcessPed', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTriggerLookAt.cpp'),
    ('CTaskComplexWalkRoundBuildingAttempt::ConstructorEntity', '0x00654840', 'CTaskComplexWalkRoundBuildingAttempt', 'ConstructorEntity', 'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.cpp'),
    ('CTaskComplexWalkRoundObject::Clone', '0x00655C10', 'CTaskComplexWalkRoundObject', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp'),
    ('CTaskComplexWalkRoundObject::ScalarDeletingDestructor', '0x00657200', 'CTaskComplexWalkRoundObject', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexWalkRoundObject.cpp'),
    ('CTaskComplexMoveBackAndJump::ScalarDeletingDestructor', '0x00657750', 'CTaskComplexMoveBackAndJump', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexMoveBackAndJump.cpp'),
    ('D3DTextureBuffer::Clear', '0x0072FEC0', 'D3DTextureBuffer', 'Clear', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DTextureBuffer::Destroy', '0x0072FEF0', 'D3DTextureBuffer', 'Destroy', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DIndexDataBuffer::Clear', '0x007301D0', 'D3DIndexDataBuffer', 'Clear', 'source/game_sa/RenderWare/D3DIndexDataBuffer.cpp'),
    ('D3DIndexDataBuffer::Destroy', '0x00730200', 'D3DIndexDataBuffer', 'Destroy', 'source/game_sa/RenderWare/D3DIndexDataBuffer.cpp'),
    ('D3DResourceSystem::GrowableArrayFree', '0x00730450', 'D3DResourceSystem', 'GrowableArrayFree', 'source/game_sa/RenderWare/D3DResourceSystem.cpp'),
    ('D3DResourceSystem::GrowableArrayResize', '0x007304B0', 'D3DResourceSystem', 'GrowableArrayResize', 'source/game_sa/RenderWare/D3DResourceSystem.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CTaskSimpleTriggerLookAt::GetTaskType', 'CTaskSimpleTriggerLookAt::Clone', 'CTaskSimpleTriggerLookAt::MakeAbortable', 'CTaskSimpleTriggerLookAt::ProcessPed', 'CTaskComplexWalkRoundObject::Clone'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn182A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    update_hooks_csv(FUNCS)
    print('ok', len(FUNCS), 'funciones registradas')


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    main(sys.argv[1], tuple(sys.argv[2:]))
