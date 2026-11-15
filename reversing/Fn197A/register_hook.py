"""Registra las 32 funciones del lote 197A con la corrida APROBADA de Fn197A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn197a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn197A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay). 
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn197A'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as _o  # noqa: E402


# (identidad del oraculo, direccion, clase y nombre de hooks.csv, fuente)
FUNCS = (
    ('CEventHealthLow::CloneEditable', '0x005FF490', 'CEventHealthLow', 'CloneEditable', 'source/game_sa/Events/EventHealthLow.cpp'),
    ('CEventHealthLow::ScalarDeletingDestructor', '0x005FF4E0', 'CEventHealthLow', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventHealthLow.cpp'),
    ('CEventHealthReallyLow::Constructor', '0x005FF500', 'CEventHealthReallyLow', 'Constructor', 'source/game_sa/Events/EventHealthReallyLow.cpp'),
    ('CEventHealthReallyLow::DestructorBody', '0x005FF520', 'CEventHealthReallyLow', 'DestructorBody', 'source/game_sa/Events/EventHealthReallyLow.cpp'),
    ('CEventHealthReallyLow::CloneEditable', '0x005FF550', 'CEventHealthReallyLow', 'CloneEditable', 'source/game_sa/Events/EventHealthReallyLow.cpp'),
    ('CEventHealthReallyLow::ScalarDeletingDestructor', '0x005FF5A0', 'CEventHealthReallyLow', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventHealthReallyLow.cpp'),
    ('CEventLowAngerAtPlayer::Constructor', '0x005FF5C0', 'CEventLowAngerAtPlayer', 'Constructor', 'source/game_sa/Events/EventLowAngerAtPlayer.cpp'),
    ('CEventLowAngerAtPlayer::DestructorBody', '0x005FF5E0', 'CEventLowAngerAtPlayer', 'DestructorBody', 'source/game_sa/Events/EventLowAngerAtPlayer.cpp'),
    ('CEventLowAngerAtPlayer::CloneEditable', '0x005FF610', 'CEventLowAngerAtPlayer', 'CloneEditable', 'source/game_sa/Events/EventLowAngerAtPlayer.cpp'),
    ('CEventLowAngerAtPlayer::ScalarDeletingDestructor', '0x005FF660', 'CEventLowAngerAtPlayer', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventLowAngerAtPlayer.cpp'),
    ('CEventHighAngerAtPlayer::Constructor', '0x005FF680', 'CEventHighAngerAtPlayer', 'Constructor', 'source/game_sa/Events/EventHighAngerAtPlayer.cpp'),
    ('CEventHighAngerAtPlayer::DestructorBody', '0x005FF6A0', 'CEventHighAngerAtPlayer', 'DestructorBody', 'source/game_sa/Events/EventHighAngerAtPlayer.cpp'),
    ('CEventHighAngerAtPlayer::CloneEditable', '0x005FF6D0', 'CEventHighAngerAtPlayer', 'CloneEditable', 'source/game_sa/Events/EventHighAngerAtPlayer.cpp'),
    ('CEventHighAngerAtPlayer::ScalarDeletingDestructor', '0x005FF720', 'CEventHighAngerAtPlayer', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventHighAngerAtPlayer.cpp'),
    ('CEventOnFire::Constructor', '0x005FF740', 'CEventOnFire', 'Constructor', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventOnFire::DestructorBody', '0x005FF760', 'CEventOnFire', 'DestructorBody', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventOnFire::GetEventType', '0x005FF770', 'CEventOnFire', 'GetEventType', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventOnFire::GetLifeTime', '0x005FF780', 'CEventOnFire', 'GetLifeTime', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventOnFire::Clone', '0x005FF790', 'CEventOnFire', 'Clone', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventOnFire::GetLocalSoundLevel', '0x005FF7E0', 'CEventOnFire', 'GetLocalSoundLevel', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventOnFire::ScalarDeletingDestructor', '0x005FF7F0', 'CEventOnFire', 'ScalarDeletingDestructor', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventEscalator::Constructor', '0x005FF820', 'CEventEscalator', 'Constructor', 'source/game_sa/Events/EventEscalator.cpp'),
    ('CEventEscalator::GetEventType', '0x005FF840', 'CEventEscalator', 'GetEventType', 'source/game_sa/Events/EventEscalator.cpp'),
    ('CEventEscalator::GetLifeTime', '0x005FF850', 'CEventEscalator', 'GetLifeTime', 'source/game_sa/Events/EventEscalator.cpp'),
    ('CEventEscalator::DestructorBody', '0x005FF860', 'CEventEscalator', 'DestructorBody', 'source/game_sa/Events/EventEscalator.cpp'),
    ('CEventEscalator::GetEventPriority', '0x005FF870', 'CEventEscalator', 'GetEventPriority', 'source/game_sa/Events/EventEscalator.cpp'),
    ('CEventSeenCop::GetEventPriority', '0x005FF880', 'CEventSeenCop', 'GetEventPriority', 'source/game_sa/Events/EventSeenCop.cpp'),
    ('CEventOnFire::GetEventPriority', '0x005FF890', 'CEventOnFire', 'GetEventPriority', 'source/game_sa/Events/EventOnFire.cpp'),
    ('CEventAcquaintancePedHateBadlyLit::GetEventPriority', '0x005FF8A0', 'CEventAcquaintancePedHateBadlyLit', 'GetEventPriority', 'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp'),
    ('ShouldLoadSunbatheAnims', '0x00632190', 'CTaskComplexSunbathe', 'ShouldLoadSunbatheAnims', 'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp'),
    ('CTaskComplexSunbathe::CreateSubTask', '0x00638290', 'CTaskComplexSunbathe', 'CreateSubTask', 'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp'),
    ('CTaskComplexUseEffect::ScalarDeletingDestructor', '0x00638570', 'CTaskComplexUseEffect', 'ScalarDeletingDestructor', 'source/game_sa/Tasks/TaskTypes/TaskComplexUseEffect.cpp'),
)
assert [f[0] for f in FUNCS] == list(_o.NAMES) and [int(f[1], 16) for f in FUNCS] == list(_o.ENTRIES)
# V = RH_ScopedVMTInstall (direccion 0x00000000 en hooks.csv); el resto S
VIRTUAL = {'CEventHealthLow::CloneEditable', 'CEventHealthReallyLow::CloneEditable', 'CEventLowAngerAtPlayer::CloneEditable', 'CEventHighAngerAtPlayer::CloneEditable', 'CEventOnFire::GetEventType', 'CEventOnFire::GetLifeTime', 'CEventOnFire::Clone', 'CEventOnFire::GetLocalSoundLevel', 'CEventEscalator::GetEventType', 'CEventEscalator::GetLifeTime', 'CEventEscalator::GetEventPriority', 'CEventSeenCop::GetEventPriority', 'CEventOnFire::GetEventPriority', 'CEventAcquaintancePedHateBadlyLit::GetEventPriority'}


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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn197A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
