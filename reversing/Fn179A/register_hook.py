"""Registra las 25 funciones del lote 179A con la corrida APROBADA de Fn179A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn179a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn179A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn179A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CPool<ColDef>::Flush', '0x00410610', 'CColStore', 'PoolFlush', 'source/game_sa/Collision/ColStore.cpp'),
    ('CPool<ColDef>::Constructor', '0x00410F80', 'CColStore', 'PoolConstructor', 'source/game_sa/Collision/ColStore.cpp'),
    ('CPool<ColDef>::FlushSecondCopy', '0x00410FE0', 'CColStore', 'PoolFlushSecondCopy', 'source/game_sa/Collision/ColStore.cpp'),
    ('CPool<ColDef>::New', '0x00411030', 'CColStore', 'PoolNew', 'source/game_sa/Collision/ColStore.cpp'),
    ('CPool<ColDef>::ScalarDeletingDestructor', '0x004110E0', 'CColStore', 'PoolScalarDeletingDestructor', 'source/game_sa/Collision/ColStore.cpp'),
    ('CEventPlayerCommandToGroupGather::GetEventType', '0x00609270', 'CEventPlayerCommandToGroupGather', 'GetEventType', 'source/game_sa/Events/EventPlayerCommandToGroupGather.cpp'),
    ('CEventPlayerCommandToGroupGather::CloneEditable', '0x00609280', 'CEventPlayerCommandToGroupGather', 'CloneEditable', 'source/game_sa/Events/EventPlayerCommandToGroupGather.cpp'),
    ('CEventPlayerCommandToGroupGather::ScalarDeletingDestructor', '0x006092E0', 'CEventPlayerCommandToGroupGather', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventPlayerCommandToGroupGather.cpp'),
    ('CEventPlayerCommandToGroupGather::DestructorBody', '0x00609300', 'CEventPlayerCommandToGroupGather', 'DestructorBody', 'source/game_sa/Events/EventPlayerCommandToGroupGather.cpp'),
    ('CEventNewGangMember::GetEventPriority', '0x00609310', 'CEventNewGangMember', 'GetEventPriority', 'source/game_sa/Events/EventNewGangMember.cpp'),
    ('CEventDontJoinPlayerGroup::GetEventPriority', '0x00609320', 'CEventDontJoinPlayerGroup', 'GetEventPriority', 'source/game_sa/Events/EventDontJoinPlayerGroup.cpp'),
    ('CEventPlayerCommandToGroupGather::GetEventPriority', '0x00609330', 'CEventPlayerCommandToGroupGather', 'GetEventPriority', 'source/game_sa/Events/EventPlayerCommandToGroupGather.cpp'),
    ('CPlayerPed::DestructorBody', '0x006093B0', 'CPlayerPed', 'DestructorBody', 'source/game_sa/Entity/Ped/PlayerPed.cpp'),
    ('CTaskComplexGangFollower::GetTaskType', '0x0065EBA0', 'CTaskComplexGangFollower', 'GetTaskType', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp'),
    ('CTaskComplexGangFollower::CalculateOffsetPosition', '0x0065ED40', 'CTaskComplexGangFollower', 'CalculateOffsetPosition', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp'),
    ('CTaskSimpleGoToPointFine::ScalarDeletingDestructor', '0x006634E0', 'CTaskSimpleGoToPointFine', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp'),
    ('CTaskAllocatorKillOnFoot::ScalarDeletingDestructor', '0x0069D9C0', 'CTaskAllocatorKillOnFoot', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp'),
    ('CTaskAllocatorAttack::ScalarDeletingDestructor', '0x0069DA00', 'CTaskAllocatorAttack', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp'),
    ('CTaskAllocatorKillThreatsBasic::ScalarDeletingDestructor', '0x0069DA40', 'CTaskAllocatorKillThreatsBasic', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp'),
    ('CTaskAllocatorKillThreatsDriveby::ScalarDeletingDestructor', '0x0069DA80', 'CTaskAllocatorKillThreatsDriveby', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp'),
    ('CTaskAllocator::ScalarDeletingDestructor', '0x0069DAC0', 'CTaskAllocator', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/Allocators/TaskAllocator.cpp'),
    ('CTaskAllocatorPlayerCommandAttack::ScalarDeletingDestructor', '0x0069DB00', 'CTaskAllocatorPlayerCommandAttack', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp'),
    ('CTaskAllocatorPlayerCommandAttack::DestructorBody', '0x0069DB40', 'CTaskAllocatorPlayerCommandAttack', 'DestructorBody', 'source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp'),
    ('CAEWeatherAudioEntity::Constructor', '0x0072A620', 'CAEWeatherAudioEntity', 'Constructor', 'source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp'),
    ('CWeather::UpdateWeatherRegion', '0x0072A640', 'CWeather', 'UpdateWeatherRegion', 'source/game_sa/Weather.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CEventPlayerCommandToGroupGather::GetEventType', 'CEventPlayerCommandToGroupGather::CloneEditable', 'CEventNewGangMember::GetEventPriority', 'CEventDontJoinPlayerGroup::GetEventPriority', 'CEventPlayerCommandToGroupGather::GetEventPriority', 'CTaskComplexGangFollower::GetTaskType'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn179A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
