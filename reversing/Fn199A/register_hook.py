"""Registra las 30 funciones del lote 199A con la corrida APROBADA de Fn199A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn199a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn199A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. Pone validated=1 (sexto campo) en las filas de docs/hooks.csv de las funciones aprobadas (no regenera docs ni md).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn199A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CTaskComplexReactToAttack::MakeAbortable', '0x00620BF0', 'CTaskComplexReactToAttack', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp'),
    ('CTaskComplexReactToAttack::CreateSubTask', '0x00620C30', 'CTaskComplexReactToAttack', 'CreateSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp'),
    ('CTaskComplexReactToAttack::ControlSubTask', '0x00620E00', 'CTaskComplexReactToAttack', 'ControlSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp'),
    ('CTaskComplexReactToAttack::Clone', '0x00623300', 'CTaskComplexReactToAttack', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexReactToAttack.cpp'),
    ('CTaskComplexKillPedOnFoot::Clone', '0x00623370', 'CTaskComplexKillPedOnFoot', 'Clone', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp'),
    ('CEventVehicleHitAndRun::GetEventType', '0x004AEA00', 'CEventVehicleHitAndRun', 'GetEventType', 'source/game_sa/Events/EventVehicleHitAndRun.cpp'),
    ('CEventVehicleHitAndRun::GetLifeTime', '0x004AEA10', 'CEventVehicleHitAndRun', 'GetLifeTime', 'source/game_sa/Events/EventVehicleHitAndRun.cpp'),
    ('CEventVehicleHitAndRun::AffectsPed', '0x004AEA20', 'CEventVehicleHitAndRun', 'AffectsPed', 'source/game_sa/Events/EventVehicleHitAndRun.cpp'),
    ('CEventVehicleHitAndRun::GetLocalSoundLevel', '0x004AEA30', 'CEventVehicleHitAndRun', 'GetLocalSoundLevel', 'source/game_sa/Events/EventVehicleHitAndRun.cpp'),
    ('CEventVehicleHitAndRun::GetEventPriority', '0x004AEA40', 'CEventVehicleHitAndRun', 'GetEventPriority', 'source/game_sa/Events/EventVehicleHitAndRun.cpp'),
    ('CEventPedEnteredMyVehicle::AffectsPed', '0x004AEC00', 'CEventPedEnteredMyVehicle', 'AffectsPed', 'source/game_sa/Events/EventPedEnteredMyVehicle.cpp'),
    ('CEventPedEnteredMyVehicle::CloneEditable', '0x004B7170', 'CEventPedEnteredMyVehicle', 'CloneEditable', 'source/game_sa/Events/EventPedEnteredMyVehicle.cpp'),
    ('CEventChatPartner::AffectsPed', '0x004AEDC0', 'CEventChatPartner', 'AffectsPed', 'source/game_sa/Events/EventChatPartner.cpp'),
    ('CEventChatPartner::Clone', '0x004B7210', 'CEventChatPartner', 'Clone', 'source/game_sa/Events/EventChatPartner.cpp'),
    ('CEventSexyPed::AffectsPed', '0x004AEF00', 'CEventSexyPed', 'AffectsPed', 'source/game_sa/Events/EventSexyPed.cpp'),
    ('CEventSexyPed::CloneEditable', '0x004B7280', 'CEventSexyPed', 'CloneEditable', 'source/game_sa/Events/EventSexyPed.cpp'),
    ('CEventSexyVehicle::AffectsPed', '0x004AF100', 'CEventSexyVehicle', 'AffectsPed', 'source/game_sa/Events/EventSexyVehicle.cpp'),
    ('CEventSexyVehicle::Clone', '0x004B72F0', 'CEventSexyVehicle', 'Clone', 'source/game_sa/Events/EventSexyVehicle.cpp'),
    ('CEventPedToChase::Clone', '0x004B7360', 'CEventPedToChase', 'Clone', 'source/game_sa/Events/EventPedToChase.cpp'),
    ('CEventPedToFlee::Clone', '0x004B73D0', 'CEventPedToFlee', 'Clone', 'source/game_sa/Events/EventPedToFlee.cpp'),
    ('CPedPool::CreateAtRef', '0x005E0540', 'CPedPool', 'CreateAtRef', 'source/game_sa/Pools/PedPool.h'),
    ('CPed::operator delete-poolRef', '0x005E47A0', 'CPed', 'operator delete-poolRef', 'source/game_sa/Entity/Ped/Ped.cpp'),
    ('CPed::CanWeRunAndFireWithWeapon', '0x005E88E0', 'CPed', 'CanWeRunAndFireWithWeapon', 'source/game_sa/Entity/Ped/Ped.cpp'),
    ('CPed::ScalarDeletingDestructor', '0x005E8C80', 'CPed', 'ScalarDeletingDestructor', 'source/game_sa/Entity/Ped/Ped.cpp'),
    ('CCamera::DestructorBody', '0x0050A870', 'CCamera', 'DestructorBody', 'source/game_sa/Camera.cpp'),
    ('CCamera::ScalarDeletingDestructor', '0x00514010', 'CCamera', 'ScalarDeletingDestructor', 'source/game_sa/Camera.cpp'),
    ('CCam::TwoPlayerCameraVisible', '0x00513220', 'CCam', 'TwoPlayerCameraVisible', 'source/game_sa/Cam.cpp'),
    ('CCam::TwoPlayerCameraGeometry', '0x005132D0', 'CCam', 'TwoPlayerCameraGeometry', 'source/game_sa/Cam.cpp'),
    ('CHandShaker::Constructor', '0x00517740', 'CHandShaker', 'Constructor', 'source/game_sa/HandShaker.cpp'),
    ('CIdleCam::Constructor', '0x00517760', 'CIdleCam', 'Constructor', 'source/game_sa/IdleCam.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CTaskComplexKillPedOnFoot::Clone', 'CEventVehicleHitAndRun::GetEventType', 'CEventVehicleHitAndRun::GetLifeTime', 'CEventVehicleHitAndRun::AffectsPed', 'CEventVehicleHitAndRun::GetLocalSoundLevel', 'CEventVehicleHitAndRun::GetEventPriority', 'CEventPedEnteredMyVehicle::AffectsPed', 'CEventPedEnteredMyVehicle::CloneEditable', 'CEventChatPartner::AffectsPed', 'CEventChatPartner::Clone', 'CEventSexyPed::AffectsPed', 'CEventSexyPed::CloneEditable', 'CEventSexyVehicle::AffectsPed', 'CEventSexyVehicle::Clone', 'CEventPedToChase::Clone', 'CEventPedToFlee::Clone'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn199A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    # docs/hooks.csv: las filas de las funciones aprobadas pasan a validated=1 (sexto campo); las que no estan aprobadas no se tocan
    csvp = ROOT / 'docs/hooks.csv'
    raw = csvp.read_bytes().decode('utf-8')
    eol = chr(13) + chr(10) if chr(13) + chr(10) in raw else chr(10)
    lines = raw.split(eol)
    approved = {(CLS, FN, ADDR) for ID, ADDR, CLS, FN, SOURCE in FUNCS if ID in ppf}
    done = 0
    for k, ln in enumerate(lines):
        f = ln.split(',')
        if len(f) == 7 and (f[0], f[1]) in {(c, n) for c, n, _ in approved}:
            want = [x for x in approved if x[0] == f[0] and x[1] == f[1]][0]
            if f[2] == ('0x00000000' if want[2] in {r[1] for r in FUNCS if r[0] in VIRTUAL} else want[2]):
                if f[5] != '1':
                    f[5] = '1'
                    lines[k] = ','.join(f)
                done += 1
    assert done == len(approved), (done, len(approved))
    csvp.write_bytes(eol.join(lines).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
