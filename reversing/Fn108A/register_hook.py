"""Registra las funciones del lote 108A (Quit, SayGangAttack, ComputeRouteRoundObject, ComputeTargetPoints) con la corrida APROBADA de Fn108A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn108a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn108A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn108A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CMBlur::MotionBlurRender', '0x0071D700', 'CMBlur', 'MotionBlurRender', 'source/game_sa/MBlur.cpp'),
    ('CMBlur::MotionBlurOpen', '0x0071D710', 'CMBlur', 'MotionBlurOpen', 'source/game_sa/MBlur.cpp'),
    ('CMBlur::MotionBlurClose', '0x0071D720', 'CMBlur', 'MotionBlurClose', 'source/game_sa/MBlur.cpp'),
    ('CMBlur::SetDrunkBlur', '0x0071D730', 'CMBlur', 'SetDrunkBlur', 'source/game_sa/MBlur.cpp'),
    ('CMBlur::ClearDrunkBlur', '0x0071D780', 'CMBlur', 'ClearDrunkBlur', 'source/game_sa/MBlur.cpp'),
    ('CRubbish::SetVisibility', '0x007204C0', 'CRubbish', 'SetVisibility', 'source/game_sa/Rubbish.cpp'),
    ('CHeli::SpecialHeliPreRender', '0x006C4750', 'CHeli', 'SpecialHeliPreRender', 'source/game_sa/Entity/Vehicle/Heli.cpp'),
    ('CPlaneBanners::Init', '0x00717820', 'CPlaneBanners', 'Init', 'source/game_sa/PlaneBanners.cpp'),
    ('CTaskSimpleInAir::ApplyRollAndPitch', '0x00678E80', 'CTaskSimpleInAir', 'ApplyRollAndPitch', 'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp'),
    ('CTaskSimpleGoToPointFine::SetMoveRatio', '0x0065F3A0', 'CTaskSimpleGoToPointFine', 'SetMoveRatio', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp'),
    ('CVehicleModelInfo::GetWheelSize', '0x006A06F0', 'CVehicleModelInfo', 'GetWheelSize', 'source/game_sa/Models/VehicleModelInfo.cpp'),
    ('CTaskManager::GetLastTaskOf-of-task', '0x00681970', 'CTaskManager', 'GetLastTaskOf-of-task', 'source/game_sa/Tasks/TaskManager.cpp'),
    ('CTaskManager::GetLastTaskOf-at-index', '0x00681A00', 'CTaskManager', 'GetLastTaskOf-at-index', 'source/game_sa/Tasks/TaskManager.cpp'),
    ('CTaskSimpleDuck::StopFireGun', '0x0061C420', 'CTaskSimpleDuck', 'StopFireGun', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp'),
    ('CTaskSimpleDuck::AbortBecauseOfOtherDuck', '0x00692340', 'CTaskSimpleDuck', 'AbortBecauseOfOtherDuck', 'source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.cpp'),
    ('CPed::SayJacked', '0x006497A0', 'CPed', 'SayJacked', 'source/game_sa/Entity/Ped/Ped.cpp'),
    ('CPed::SayJacking', '0x006497F0', 'CPed', 'SayJacking', 'source/game_sa/Entity/Ped/Ped.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree wv-3 sobre main 498690aa5 con los cambios del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en su cuerpo del ASI (todos tienen hook RH, apagado durante el banco): '
              'CAnimBlendAssociation::SetFinishCallback 0x4CEBE0 (AbortBecauseOfOtherDuck; el falso normaliza el callback DefaultAnimCB / 0x4D1390), CAEVehicleAudioEntity::GetVehicleTypeForAudio 0x4F4F00 y CPed::Say 0x5EFFE0 (SayJacked y SayJacking). '
              'CTask::GetSubTask (ranura 2) en GetLastTaskOf y CAEPedSpeechAudioEntity::IsPedFemaleForAudio (ranura 7, ped+0x294) en SayJacking van por vtablas propias de las arenas (falsos identicos). '
              'NO se sustituyen y corren de verdad en ambos lados: las comparaciones de float de SetDrunkBlur y StopFireGun (x87 del EXE frente al C++ de la DLL), el retorno en ST0 de GetWheelSize y las escrituras de memoria (CMBlur::Drunkness 0xC73C58 y CTimer::ms_fTimeScale 0xB7CB64 son memoria real restaurada tras cada lado). '
              'Cada lado coincide ademas con el oraculo independiente fixture_oracle.py. Fuera de los fixtures: punteros nulos o invalidos en GetLastTaskOf y StopFireGun/AbortBecauseOfOtherDuck (animaciones distintas de nula), indices fuera de 0..4 en GetLastTaskOf(ePrimaryTasks), tipos de audio cuyos 16 bits bajos no son 0..2 con bits altos distintos de cero en SayJacked/SayJacking, NaN con cargas no incluidas y la exactitud de los colaboradores falseados.')
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn108A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
