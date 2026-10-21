"""Registra RemoveVehicleUpgrade (0x006DF930), AddUpgrade (0x006DFA20), AddVehicleUpgrade (0x006E3290), PossiblyDropFreeFallBombForPlayer (0x006E07E0) y ProcessSirenAndHorn (0x006E0950)
con la corrida APROBADA de Vehicle66.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-vehicle66/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de SmallHelpers64A adaptada a cinco hooks Simple. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Vehicle66/validation-summary.json y anade las
entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
Filas de hooks.csv (clase CVehicle, tipo S, ninguna virtual): CVehicle,RemoveVehicleUpgrade,0x006DF930 / CVehicle,AddUpgrade,0x006DFA20 / CVehicle,AddVehicleUpgrade,0x006E3290 /
CVehicle,PossiblyDropFreeFallBombForPlayer,0x006E07E0 / CVehicle,ProcessSirenAndHorn,0x006E0950.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Vehicle66'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('RemoveVehicleUpgrade', '0x006DF930', 'CVehicle', 'RemoveVehicleUpgrade', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
    ('AddUpgrade', '0x006DFA20', 'CVehicle', 'AddUpgrade', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
    ('AddVehicleUpgrade', '0x006E3290', 'CVehicle', 'AddVehicleUpgrade', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
    ('PossiblyDropFreeFallBombForPlayer', '0x006E07E0', 'CVehicle', 'PossiblyDropFreeFallBombForPlayer', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
    ('ProcessSirenAndHorn', '0x006E0950', 'CVehicle', 'ProcessSirenAndHorn', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wu-2 (HEAD 5765c0b5f + Vehicle.cpp/Vehicle.h del lote 66). Colaboradores sustituidos por falsos identicos en ambos lados (parche en la entrada del EXE y, '
              'si la DLL tiene hook, en el cuerpo de la DLL; no se validan sus cuerpos): ClearVehicleUpgradeFlags (0x6D3210), FindOtherUpgrade (0x4C74D0), RemoveReplacementUpgrade (0x6D39E0), '
              'RpClumpForAllAtomics (0x749B70), GetFrameFromId (0x4C53C0), CreateUpgradeAtomic (0x6D3510), RwFrameForAllObjects (0x7F1200), RwFrameForAllChildren (0x7F0DC0), '
              'SetupClumpAfterVehicleUpgrade (0x6FDFE0), SetVehicleUpgradeFlags (0x6D30E0), GetReplacementUpgrade (0x6D3A50), AddReplacementUpgrade (0x6D3830), GetUpgrade (0x6D3650), '
              'AddUpgrade (0x6DFA20, para AddVehicleUpgrade), GetPlaneOrdnanceRateOfFire (0x6D4590), GetPlaneOrdnancePosition (0x6D46E0), CProjectileInfo::AddProjectile (0x737C80), '
              'CPad::StartShake (0x53F920), UsesSiren (0x6D8470) y CPad::GetHorn (0x53FEE0). CPed::IsPlayer y CPad::GetPad se ejecutan de verdad. Vehiculo, model infos (instalados en '
              'ms_modelInfoPtrs[15..19]), estructura del vehiculo, manejabilidad, conductor y matriz son FALSOS en una arena; se escriben y restauran MI_OFFROAD_WHEEL (0x8CD758), la region '
              'del historial de bocina del pad 0 (0xB73569, 0x100 bytes) y CTimer (0xB7CB84). Se exige EAX de AddVehicleUpgrade (la mejora sustituida; el llamador 0x4985DA la guarda en 0xA43C78); '
              'de las demas funciones ningun llamador del EXE lee EAX/ECX/EDX. Los float de la ordenanza/posicion del bombardeo no incluyen denormales (MXCSR FTZ del juego) ni dos NaN en una suma.')
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
                                 'en ambos lados). Banco SmallHelpers64A con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (EAX de los HRESULT si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
