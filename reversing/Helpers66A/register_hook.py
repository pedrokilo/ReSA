"""Registra CPlayerPed::PlayerWantsToAttack (0x0060CC50), CPlayerPed::EvaluateNeighbouringTarget (0x0060D1C0), CTaskSimpleGangDriveBy::HasClearLineOfSight (0x00621B10),
Fx_c::AddPunchImpact (0x0049F670) y RsSavePresetView (0x00619FA0) con la corrida APROBADA de Helpers66A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-helpers66a/<corrida>"

PLANTILLA: la de Helpers65D adaptada a cinco hooks Simple. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Helpers66A'
FUNCS = (
    # (id del banco, direccion del EXE, clase del hook, nombre del hook, fuente, tipo de hook)
    ('CPlayerPed::PlayerWantsToAttack', '0x0060CC50', 'CPlayerPed', 'PlayerWantsToAttack', 'source/game_sa/Entity/Ped/PlayerPed.cpp', 'S'),
    ('CPlayerPed::EvaluateNeighbouringTarget', '0x0060D1C0', 'CPlayerPed', 'EvaluateNeighbouringTarget', 'source/game_sa/Entity/Ped/PlayerPed.cpp', 'S'),
    ('CTaskSimpleGangDriveBy::HasClearLineOfSight', '0x00621B10', 'CTaskSimpleGangDriveBy', 'HasClearLineOfSight', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp', 'S'),
    ('Fx_c::AddPunchImpact', '0x0049F670', 'Fx_c', 'AddPunchImpact', 'source/game_sa/Fx/Fx.cpp', 'S'),
    ('RsSavePresetView', '0x00619FA0', 'Rs', 'RsSavePresetView', 'source/app/platform/preset_view.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wu-1 (HEAD 5765c0b5f). PlayerWantsToAttack 0x60CC50: CPedGroupMembership::CountMembersExcludingLeader 0x5F6AA0, CPedGroupIntelligence::ReportAllBarScriptTasksFinished 0x5F8780, '
              'CTagManager::IsTag 0x49CCE0, CObject::CanBeTargetted 0x59F320, CPlayerPed::FindPedToAttack 0x60C5F0 y CPedGroup::PlayerGaveCommand_Attack 0x5F7CC0 sustituidos por falsos identicos; el EAX de salida no se compara '
              '(la funcion no lo define y el unico llamador, 0x60D5A0 -> 0x685A7A, no lo lee). EvaluateNeighbouringTarget 0x60D1C0: CPlayerPed::DoesTargetHaveToBeBroken 0x60C0C0 y CGeneral::GetATanOfXY 0x53CC70 sustituidos; '
              'compensacion y rumbo finitos o NaN (con +-inf el bucle de normalizacion del propio EXE no termina). HasClearLineOfSight 0x621B10: CWorld::GetIsLineOfSightClear 0x56A490 y rand() del EXE 0x821B1E sustituidos; '
              'solo AL se compara (unico llamador 0x62780F: test al,al). AddPunchImpact 0x49F670: FxSystem_c::AddParticle 0x4AA440 sustituido; el constructor FxPrtMult_c 0x4AB290 corre de verdad. RsSavePresetView 0x619FA0 (sin llamadores '
              'en el EXE; incluye el ayudante usercall 0x619910): se sustituyen psPathnameCreate 0x745470, psPathnameDestroy 0x7454E0, psErrorMessage 0x7451D0, RwFrameGetLTM 0x7F0990, RwV3dNormalize 0x7ED9B0, RsLoadPresetViews '
              '0x619D60, RsSetPresetView 0x619780 y las entradas fopen/fclose/fputs/sprintf/strlen de la tabla de RenderWare (RwEngineInstance +0xC4/+0xF0/+0x120); RsErrorMessage 0x619B00 y RsKeyboardEventHandler 0x619560 corren de verdad. '
              'La declaracion pasa de void RsSavePresetView() a bool RsSavePresetView(RwCamera*) y RsLoadPresetViews de void a bool.')
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
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': KIND},
                evidence=evidence, tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE para la propia funcion (los colaboradores sin reconstruir se llaman por direccion y el banco los sustituye por falsos identicos '
                                 'en ambos lados). Banco Helpers66A con ' + str(int(s['pairs'])) + ' pares y cero diferencias; ECX/EDX solo registrados y EAX comparado donde un llamador lo lee. '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
