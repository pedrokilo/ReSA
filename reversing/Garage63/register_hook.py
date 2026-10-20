"""Registra las nueve funciones de CGarage del banco Garage63 con la corrida APROBADA.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-garage63/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de Misc61 adaptada a nueve hooks Simple. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Garage63/validation-summary.json y anade las
entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador):
`CGarage,<Funcion>,<direccion>,1,0,1,S` para cada una de las nueve.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Garage63'
SRC_FILE = 'source/game_sa/Garage.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('CGarage::InitDoorsAtStart', '0x00447600', 'CGarage', 'InitDoorsAtStart', SRC_FILE, 'S'),
    ('CGarage::PlayerArrestedOrDied', '0x004486C0', 'CGarage', 'PlayerArrestedOrDied', SRC_FILE, 'S'),
    ('CGarage::IsEntityEntirelyInside3D', '0x00448BE0', 'CGarage', 'IsEntityEntirelyInside3D', SRC_FILE, 'S'),
    ('CGarage::IsEntityEntirelyOutside', '0x00448D30', 'CGarage', 'IsEntityEntirelyOutside', SRC_FILE, 'S'),
    ('CGarage::IsStaticPlayerCarEntirelyInside', '0x0044A830', 'CGarage', 'IsStaticPlayerCarEntirelyInside', SRC_FILE, 'S'),
    ('CGarage::IsGarageEmpty', '0x0044A9C0', 'CGarage', 'IsGarageEmpty', SRC_FILE, 'S'),
    ('CGarage::RemoveCarsBlockingDoorNotInside', '0x00449690', 'CGarage', 'RemoveCarsBlockingDoorNotInside', SRC_FILE, 'S'),
    ('CGarage::TidyUpGarage', '0x00449C50', 'CGarage', 'TidyUpGarage', SRC_FILE, 'S'),
    ('CGarage::TidyUpGarageClose', '0x00449D10', 'CGarage', 'TidyUpGarageClose', SRC_FILE, 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wf-1 (HEAD 35e7b9d4b + fuente del lote 63). Todos los colaboradores se sustituyen por falsos identicos en ambos lados (no se validan sus cuerpos): GetColModel (0x535300), '
              'GetMatrix (0x411990), AllocateMatrix (0x54F560), CSimpleTransform::UpdateMatrix (0x54F1B0), la transformacion 0x59C890, CGarage::IsPointInsideGarage punto (0x448740) y radio (0x4487D0; ECX/EDX intactos como el EXE), '
              'IsEntityTouching3D (0x448EE0), CWorld::FindObjectsIntersectingCube (0x568DD0), FindPlayerVehicle (0x56E0D0), FindPlayerPed (0x56E210), CTaskManager::FindActiveTaskByType (0x681740), '
              'CVehicle::CanBeDeleted (0x6D1180), CWorld::Remove (0x563280), IsEntityEntirelyInside3D (0x448BE0, solo para IsStaticPlayerCarEntirelyInside) y el destructor virtual (tabla de metodos falsa). '
              'Entidades, matrices, CColModel/CCollisionData, esferas, pool de vehiculos (ds:[0xB74494] sustituido por un pool privado durante cada llamada), jugador y TaskManager son objetos falsos del banco; '
              'ningun vehiculo, garaje ni pool reales se tocan. Update (0x44AA50) NO esta reconstruida. PlayerArrestedOrDied conserva ECX y EDX (el bucle de 0x449E60 los lee); en el resto solo AL en las funciones bool y '
              'ECX/EDX/EAX de salida solo se registran. No se prueban los vehiculos de TidyUpGarage con matriz nula y sin estado WRECKED, ni los de TidyUpGarageClose sin CCollisionData (el EXE desreferencia sin comprobar).')
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
                                 'en ambos lados). Banco Garage63 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (EAX de los HRESULT si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
