"""Registra CHeli::ProcessControlInputs (0x6C4830, lote 83E, hook V) con la corrida APROBADA de Heli83E.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-heli83e/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Heli83E/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv
`CHeli,ProcessControlInputs,0x00000000,1,0,1,V` la anade el coordinador al registrar).
Antes de ejecutarlo hay que revisar SCOPE_NOTE (colaboradores y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'Heli83E'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CHeli::ProcessControlInputs', '0x006C4830', 'CHeli', 'ProcessControlInputs', 'source/game_sa/Entity/Vehicle/Heli.cpp'),
)
HOOK_TYPE = 'V'   # RH_ScopedVMTInstall: el registro anota los hooks virtuales con direccion 0x00000000 y tipo V (approvals.json usa la direccion del EXE)
SCOPE_NOTE = ('Fuentes probadas compiladas en el arbol principal (HEAD 2c71b35d2 + cambios del lote 83E; ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos '
              'en la direccion del EXE y en el cuerpo de la DLL (con sus hooks RH apagados; no se validan sus cuerpos): CPad::GetAccelerate 0x5403F0, GetBrake 0x540080, GetSteeringLeftRight 0x53FB80, GetSteeringUpDown 0x53FBD0, '
              'GetLookRight 0x53FE10, GetLookLeft 0x53FDD0, GetHorn 0x53FEE0, FindPlayerPed 0x56E210, CPlayerPed::KeepAreaAroundPlayerClear 0x60C1E0, CVector::Normalise 0x59C910 y CrossProduct 0x59C730; los de salida '
              'escriben los valores que da cada caso (EAX de los del mando con basura en los bits altos). Corren de verdad CPad::GetPad (0x53FB70), las estaticas 0xB6EC2E, 0xC1CC03, 0xC1CC04, el raton 0xB73424/0xB73428, '
              'CTimer::ms_fTimeStep, la matriz del helicoptero, los datos de manejo en vuelo y las palabras +0x2C y +0x10E del mando (regiones restauradas); el pow del CRT del EXE (0x822130) corre de verdad en el original y '
              'en el candidato lo emula PowScaleFloatCpp. El banco compara el orden y todos los argumentos de los falsos, el modo de control, los campos escritos y las arenas; cada lado coincide ademas con el oraculo '
              'independiente fixture_oracle.py (el pow solo con tolerancia). Fuera de los fixtures: NaN con cargas distintas (las fixtures usan un unico patron), numero de mando distinto de 0 y 1, estaticas booleanas '
              'distintas de 0/1, puntero de matriz nulo y salidas de los colaboradores incoherentes con la entrada (Normalise y CrossProduct devuelven lo que dice el caso, no el calculo real).')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (los once colaboradores son falsos identicos en ambos lados). Banco Heli83E con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
