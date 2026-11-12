"""Registra las 23 funciones del lote 178A con la corrida APROBADA de Fn178A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn178a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn178A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn178A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CEventVehicleThreat::Constructor', '0x004AFB50', 'CEventVehicleThreat', 'Constructor', 'source/game_sa/Events/EventVehicleThreat.cpp'),
    ('CEventVehicleThreat::DestructorBody', '0x004AFBF0', 'CEventVehicleThreat', 'DestructorBody', 'source/game_sa/Events/EventVehicleThreat.cpp'),
    ('InteriorManager_c::Constructor', '0x005984C0', 'InteriorManager_c', 'Constructor', 'source/game_sa/Interior/InteriorManager_c.cpp'),
    ('InteriorManager_c::Destructor', '0x00598580', 'InteriorManager_c', 'Destructor', 'source/game_sa/Interior/InteriorManager_c.cpp'),
    ('CReplay::ApplyPanelDamageToCar', '0x0059CA10', 'CReplay', 'ApplyPanelDamageToCar', 'source/game_sa/Replay.cpp'),
    ('CWinsockLink::BaseDestructorBody', '0x0059CBA0', 'CWinsockLink', 'BaseDestructorBody', 'source/game_sa/WinsockLink.cpp'),
    ('CWinsockLink::ScalarDeletingDestructor', '0x0059CD50', 'CWinsockLink', 'ScalarDeletingDestructor', 'source/game_sa/WinsockLink.cpp'),
    ('CCopPed::ScalarDeletingDestructor', '0x005DE2C0', 'CCopPed', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Ped/CopPed.cpp'),
    ('CDummyPed::Constructor', '0x005DE2E0', 'CDummyPed', 'Constructor', 'source/game_sa/Entity/Dummy/DummyPed.cpp'),
    ('CDummyPed::DestructorBody', '0x005DE300', 'CDummyPed', 'DestructorBody', 'source/game_sa/Entity/Dummy/DummyPed.cpp'),
    ('CDummyPed::ScalarDeletingDestructor', '0x005DE310', 'CDummyPed', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Dummy/DummyPed.cpp'),
    ('CEmergencyPed::Constructor', '0x005DE340', 'CEmergencyPed', 'Constructor', 'source/game_sa/Entity/Ped/EmergencyPed.cpp'),
    ('CEmergencyPed::DestructorBody', '0x005DE3B0', 'CEmergencyPed', 'DestructorBody', 'source/game_sa/Entity/Ped/EmergencyPed.cpp'),
    ('CEmergencyPed::ScalarDeletingDestructor', '0x005DE3E0', 'CEmergencyPed', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Ped/EmergencyPed.cpp'),
    ('CEmergencyPed::ProcessControl', '0x005DE400', 'CEmergencyPed', 'ProcessControl', 'source/game_sa/Entity/Ped/EmergencyPed.cpp'),
    ('CEventAcquaintancePedHateBadlyLit::DestructorBody', '0x005FF2C0', 'CEventAcquaintancePedHateBadlyLit', 'DestructorBody', 'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp'),
    ('CEventAcquaintancePedHateBadlyLit::GetEventType', '0x005FF2D0', 'CEventAcquaintancePedHateBadlyLit', 'GetEventType', 'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp'),
    ('CEventAcquaintancePedHateBadlyLit::CloneEditable', '0x005FF2E0', 'CEventAcquaintancePedHateBadlyLit', 'CloneEditable', 'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp'),
    ('CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent', '0x005FF350', 'CEventAcquaintancePedHateBadlyLit', 'CanBeInterruptedBySameEvent', 'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp'),
    ('CEventAcquaintancePedHateBadlyLit::ScalarDeletingDestructor', '0x005FF360', 'CEventAcquaintancePedHateBadlyLit', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp'),
    ('CPlane::DestructorBody', '0x006C9160', 'CPlane', 'DestructorBody', 'source/game_sa/Entity/Vehicle/Plane.cpp'),
    ('CPlane::ScalarDeletingDestructor', '0x006CADB0', 'CPlane', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Vehicle/Plane.cpp'),
    ('CQuadBike::DestructorBody', '0x006CDC30', 'CQuadBike', 'DestructorBody', 'source/game_sa/Entity/Vehicle/QuadBike.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CEmergencyPed::ProcessControl', 'CEventAcquaintancePedHateBadlyLit::GetEventType', 'CEventAcquaintancePedHateBadlyLit::CloneEditable', 'CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent'}


SCOPE_NOTE = open(Path(__file__).resolve().parent / 'scope_note.txt', encoding='utf-8').read().strip() if (Path(__file__).resolve().parent / 'scope_note.txt').exists() else ''
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn178A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
