"""Registra CVehicle::GetPlaneWeaponFiringStatus (0x6E3440, lote 74) con la corrida APROBADA de PlaneFiring74.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-planefiring74/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PlaneFiring74/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv
`CVehicle,GetPlaneWeaponFiringStatus,0x006E3440,1,0,1,S` la anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PlaneFiring74'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CVehicle::GetPlaneWeaponFiringStatus', '0x006E3440', 'CVehicle', 'GetPlaneWeaponFiringStatus', 'source/game_sa/Entity/Vehicle/Vehicle.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/wy-1 (HEAD f13a750cf + source.patch del lote 74; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos '
              'en la direccion del EXE y en el cuerpo de la DLL (con sus hooks RH apagados; no se validan sus cuerpos): CWorld::FindPlayerSlotWithVehiclePointer 0x564000 (WldC, 256 pares), '
              'CPad::GetCarGunFired 0x53FF90, CPad::CarGunJustDown 0x53FFE0, CPad::GetEnterTargeting 0x5406B0 y CPad::GetTarget 0x540670 (RequestedPadSelectors, 3876 pares), CWeaponEffects::IsLockedOn 0x742BD0 (Ped9, 128 pares), '
              'CVehicle::ScanAndMarkTargetForHeatSeekingMissile 0x6E0400 (HeatSeek72) y CProjectileInfo::AddProjectile 0x737C80 (ProjectileAdd355); CPad::GetPad 0x53FB70 (inline sin hook) se ejecuta de verdad. '
              'El banco compara el orden y todos los argumentos de los falsos, el bool (un solo byte) y el tipo de ordenanza (dword) escritos solo en los caminos del EXE (vecinos intactos), EAX (objetivo), la hora de la ultima bengala '
              '(+0x4D4), el inicio y el objetivo de la fijacion (+0x9DC/+0x9E0), la mira global (gCrossHair[0]: color, rotacion y tiempo), los bytes globales 0xC1CC00/0xB6EC2E, la posicion de la bengala (x87 a 24 bits, incluidos NaN no canonicos, '
              'infinitos, desnormales y ceros con signo; reglas de NaN validadas ademas contra el hardware con flare_nan_hw) y 5 arenas (vehiculo CPlane completo, objetivo, otra entidad, matriz y salidas). Cada lado coincide ademas con el oraculo '
              'independiente fixture_oracle.py. El EXE lee matriz + 0x10/+0x20 sin comprobar nulo en la bengala del Hydra: la matriz nula solo se prueba cuando no hay bengala (la rama de posicion compacta this+4 con matriz nula no es ejercitable: '
              'se cae el propio EXE). ECX/EDX solo se registran: el unico llamador, CVehicle::ProcessWeapons 0x6E39F4, los sobrescribe antes de leerlos; EAX si se compara. El colaborador GetPlaneWeaponFiringStatus de ProcessWeapons lo valida '
              'este banco (VehicleWeapons67 lo sustituye por un falso en la direccion del EXE y en el cuerpo de la DLL).')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (los cuatro colaboradores son falsos identicos en ambos lados). Banco PlaneFiring74 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados. Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
