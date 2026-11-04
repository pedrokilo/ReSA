"""Registra la funcion del lote 89A (CCamera::IsItTimeForNewCamera) con la corrida APROBADA de Fn89A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn89a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn89A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv las anade el coordinador al registrar).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn89A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CCamera::IsItTimeForNewCamera', '0x0051D770', 'CCamera', 'IsItTimeForNewCamera', 'source/game_sa/Camera.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en un worktree (wf-2) sobre el arbol principal mas la reconstruccion del lote (ver artifacts/build-manifest.json). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL '
              '(los de hook RH, apagado durante el banco): CWorld::GetIsLineOfSightClear 0x56A490, FindPlayerVehicle 0x56E0D0, FindPlayerCoors 0x56E010, FindPlayerSpeed 0x56E090, CEntity::GetIsOnScreen 0x534540 y los siete CCam::Process_DW_* '
              '(0x51A740, 0x51B120, 0x51B850, 0x51C250, 0x51C760, 0x51CC30, 0x51D100); RwCameraSetNearClipPlane 0x7EE1D0 no tiene hook y la DLL lo llama por su direccion. Registran sus argumentos en orden. La aritmetica (transcurrido en 24 bits, palo derecho, '
              'vector jugador - origen fijo, modulo con CVector::Magnitude y operator- del EXE frente a los ayudantes x87 de la DLL, productos escalares con la velocidad) corre de verdad en ambos lados. Globales reales (reloj 0xB7CB84, aviso 0x8CCDF4, paso 0x8CC471, 0xB6F059 y el eje X del palo derecho 0xB7345C) '
              'guardados y restaurados. Con mutaciones de una escritura dentro de un falso. El banco compara los eventos de los falsos con todos sus argumentos, el valor devuelto (AL), los globales y los bytes cambiados de todas las arenas; cada lado coincide ademas con el oraculo independiente '
              'fixture_oracle.py (aritmetica racional exacta a 24 bits). Hallazgo corregido por el banco: las pruebas de transcurrido > 15000 de los casos 1, 2, 3, 5, 8 y 15..19 estaban invertidas en la reconstruccion (3141 de 16384 pares con diferencias antes de corregirlas). '
              'Fuera de los fixtures: NaN con cargas distintas de 0xFFC00000, NaN senalizantes, bytes altos de los booleanos de los falsos, indices de camara activa mayores de 3 y un objetivo de camara nulo.')
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
                       'scope': ('Reconstruida en C++ (los colaboradores del EXE son falsos identicos en ambos lados en el banco). Banco Fn89A con ' + str(int(s['pairs'])) + ' pares y cero diferencias;  Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
