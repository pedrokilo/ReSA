"""Registra CHeli::ProcessControl (0x6C7050, lote 83F, hook V) con la corrida APROBADA de Heli83F.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-heli83f/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Heli83F/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv
`CHeli,ProcessControl,0x00000000,1,0,1,V` la anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Heli83F'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CHeli::ProcessControl', '0x006C7050', 'CHeli', 'ProcessControl', 'source/game_sa/Entity/Vehicle/Heli.cpp'),
)
HOOK_TYPE = 'V'   # RH_ScopedVMTInstall: el registro anota los hooks virtuales con direccion 0x00000000 y tipo V (approvals.json usa la direccion del EXE)
SCOPE_NOTE = ('Fuentes probadas compiladas en el arbol principal (HEAD 2c71b35d2 + cambios del lote 83F; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos '
              'en la direccion del EXE y en el cuerpo de la DLL (con sus hooks RH apagados; no se validan sus cuerpos): CAutomobile::ProcessControl 0x6B1880, FxSystem_c::Kill 0x4AA3F0, CPad::HornJustDown 0x53FF30, '
              'CCullZones::PlayerNoRain 0x72DDC0 y NoPolice 0x72DD50, FindPlayerVehicle 0x56E0D0, FindPlayerEntity 0x56E120, FindPlayerPed 0x56E210, FindPlayerWanted 0x56E230, CAudioEngine::SayPedless 0x5073C0 y '
              'ReportWeaponEvent 0x506F40, CHeli::FindSwatPositionRelativeToHeli 0x6C4760, operator*(CMatrix, CVector) 0x59C890, Multiply3x3 0x59C790 (en la DLL AutoPreMultiply3x3Cpp, parche solo del cuerpo), '
              'CWorld::GetIsLineOfSightClear 0x56A490, FireOneInstantHitRound 0x73AF00, CRopes::RegisterRope 0x556B40 y SetSpeedOfTopNode 0x555DF0, CVehicle::UpdateWinch 0x6D3B80 y ProcessWeapons 0x6E3950, '
              'CInterestingEvents::Add 0x602590, CHeli::SendDownSwat 0x6C69C0 y CVector::Normalise 0x59C910; los de salida escriben los valores que da cada caso. Corren de verdad CPad::GetPad (0x53FB70), '
              'el rand() del CRT del EXE (0x821B1E, con 0x41BD90 del EXE: su hook se apaga), GetUp/GetForward de la matriz (0x50E420/0x41CCB0), CTimer, 0xC0B184, la matriz y los campos del helicoptero y de las '
              'entidades (arenas y regiones restauradas). El banco compara el orden y todos los argumentos de los falsos, las palabras escritas en el helicoptero, el estado del rand() y las arenas; cada lado coincide '
              'ademas con el oraculo independiente fixture_oracle.py. Fuera de los fixtures: NaN con cargas distintas (las fixtures usan un unico patron), rumbo distinto de +-0 sin matriz (sin y cos exactos), '
              'punteros de datos del jugador o CWanted nulos, colaboradores con bytes altos distintos de los del caso en sus salidas booleanas y salidas incoherentes con la entrada (Normalise, operator*, Multiply3x3 y '
              'FindSwat devuelven lo que dice el caso, no el calculo real).')
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
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000', 'type': HOOK_TYPE},
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (los colaboradores son falsos identicos en ambos lados). Banco Heli83F con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
