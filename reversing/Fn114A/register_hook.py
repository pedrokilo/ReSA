"""Registra las funciones del lote 114A (Quit, SayGangAttack, ComputeRouteRoundObject, ComputeTargetPoints) con la corrida APROBADA de Fn114A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn114a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn114A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn114A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CPlaceable::GetRightVector', '0x0041CC70', 'CPlaceable', 'GetRightVector', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPlaceable::GetUpVector', '0x0050E420', 'CPlaceable', 'GetUpVector', 'source/game_sa/Entity/Placeable.cpp'),
    ('CPhysical::ApplyMoveForce', '0x004ABBA0', 'CPhysical', 'ApplyMoveForce-xyz', 'source/game_sa/Entity/Physical.cpp'),
    ('CPedIntelligence::GetPedFOVRange', '0x004ABEA0', 'CPedIntelligence', 'GetPedFOVRange', 'source/game_sa/PedIntelligence.cpp'),
    ('CPedGroups::RegisterKillByPlayer', '0x005F7E30', 'CPedGroups', 'RegisterKillByPlayer', 'source/game_sa/PedGroups.cpp'),
    ('CMissionCleanup::AddEntityToList', '0x004637E0', 'CMissionCleanup', 'AddEntityToList-handle', 'source/game_sa/MissionCleanup.cpp'),
    ('CEventDamage::IsSameEventForAI', '0x004ADBF0', 'CEventDamage', 'IsSameEventForAI', 'source/game_sa/Events/EventDamage.cpp'),
    ('CEntryExitManager::WeAreInInteriorTransition', '0x0043E400', 'CEntryExitManager', 'WeAreInInteriorTransition', 'source/game_sa/EntryExitManager.cpp'),
    ('CPointRoute::AddPoint', '0x0048E180', 'CPointRoute', 'AddPoint', 'source/game_sa/PedGeometryAnalyser.cpp'),
    ('CPointRoute::RemovePoint', '0x005F13A0', 'CPointRoute', 'RemovePoint', 'source/game_sa/PedGeometryAnalyser.cpp'),
    ('CPedGroupIntelligence::GetTask', '0x005F7660', 'CPedGroupIntelligence', 'GetTask', 'source/game_sa/PedGroupIntelligence.cpp'),
    ('CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent', '0x004AE100', 'CEventGroupEvent', 'BaseEventTakesPriorityOverBaseEvent', 'source/game_sa/Events/EventGroupEvent.cpp'),
    ('CEvent::TakesPriorityOverBase', '0x004ABC30', 'CEvent', 'TakesPriorityOverBase', 'source/game_sa/Events/Event.cpp'),
    ('IsPedPointerValid', '0x005E8A30', 'CPed', 'IsPedPointerValid', 'source/game_sa/Entity/Ped/Ped.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree sobre main 7f321c843 con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en su cuerpo del ASI (todos tienen hook RH, apagado durante el banco): '
              'CPhysical::ApplyMoveForce(CVector) 0x5429F0 (ApplyMoveForce xyz); IsEntityPointerValid 0x533310 y CWorld::FindPlayerPed 0x56E210 (IsPedPointerValid). '
              'Las ranuras 1, 2 y 11 de la vtabla de un evento (GetEventType, GetEventPriority, TakesPriorityOver) son falsos identicos de una vtabla propia (BaseEventTakesPriorityOverBaseEvent y TakesPriorityOverBase). '
              'NO se sustituyen y corren de verdad en ambos lados: fsin/fcos del x87 (GetRightVector), toda la logica de las demas funciones y el pool de peds (arena con la estructura del pool). '
              'Cada lado coincide ademas con el oraculo independiente fixture_oracle.py. '
              'Fuera de los fixtures: punteros de objeto nulos, rumbos con |h| > 8 sin matriz (fsin de valores grandes), contadores de ruta negativos por debajo de -5 o indices de ruta negativos con pasadas, '
              'peds apuntando fuera del almacenamiento con indice en rango, y la exactitud de los colaboradores falseados.')
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

    data = registry_store.load_registry(ROOT)
    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    for ID, ADDR, CLS, FN, SOURCE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn114A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
