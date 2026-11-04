"""Registra las nueve funciones del lote 95A con la corrida APROBADA de Fn95A.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn95a/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/Fn95A/validation-summary.json y anade las entradas a
reversing/validation-registry/ (fragmentos, via contrib/registry_store.py) y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv
las anade el coordinador al registrar).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store  # noqa: E402
BANK = 'Fn95A'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('D3DTextureBuffer::Setup', '0x0072FE80', 'D3DTextureBuffer', 'Setup', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DTextureBuffer::Pop', '0x0072FF60', 'D3DTextureBuffer', 'Pop-Match', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DTextureBuffer::Push', '0x0072FFF0', 'D3DTextureBuffer', 'Push', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DTextureBuffer::Resize', '0x00730020', 'D3DTextureBuffer', 'Resize', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DTextureBuffer::GetTotalDataSize', '0x007300A0', 'D3DTextureBuffer', 'GetTotalDataSize', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DTextureBuffer::PushWithoutIncreasingCounter', '0x00730AD0', 'D3DTextureBuffer', 'PushWithoutIncreasingCounter', 'source/game_sa/RenderWare/D3DTextureBuffer.cpp'),
    ('D3DIndexDataBuffer::Pop', '0x00730270', 'D3DIndexDataBuffer', 'Pop-Size', 'source/game_sa/RenderWare/D3DIndexDataBuffer.cpp'),
    ('D3DIndexDataBuffer::Resize', '0x00730330', 'D3DIndexDataBuffer', 'Resize', 'source/game_sa/RenderWare/D3DIndexDataBuffer.cpp'),
    ('D3DIndexDataBuffer::GetTotalDataSize', '0x007303B0', 'D3DIndexDataBuffer', 'GetTotalDataSize', 'source/game_sa/RenderWare/D3DIndexDataBuffer.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas en el worktree D:/Original/ws-1 sobre main 329ab605c con los cambios del lote (ver artifacts/build-manifest.json). Los recursos D3D9 son objetos COM FALSOS con vtable propia '
              '(Release ranura 2, GetLevelCount / GetDesc ranura 13, GetLevelDesc ranura 17) que registran sus llamadas en orden y escriben la descripcion entera del caso: no se prueba el dispositivo D3D9 real ni las caches reales '
              'de D3DResourceSystem (los buffers son arenas del banco con guardas; la pila de punteros tambien). Colaboradores sustituidos por falsos identicos en la direccion del EXE (sin hook RH en la DLL: la DLL los llama por la '
              'misma direccion): operator new 0x82119A (devuelve siempre la pila nueva de la arena y registra el tamano) y operator delete 0x8214BD (solo registra el puntero; el EXE lo llama tambien con nulo), armados solo durante la '
              'llamada probada y en el hilo del banco. GetTotalDataSize de D3DTextureBuffer se compila con /O2 /Oi /arch:IA32 /fp:fast (x87 de 24 bits: fild, fadd 2^32, fmul 4/3, fiadd y __ftol/_ftol2) y se compara con un oraculo que '
              'redondea cada operacion a 24 bits. Cada lado coincide ademas con el oraculo independiente fixture_oracle.py (eventos en orden, palabras del buffer, cambios de la pila, de las guardas y de la pila nueva). '
              'Fuera de los fixtures: cantidades negativas o mayores que la pila en Pop/Resize/GetTotalDataSize generico (el EXE indexa fuera del arreglo), capacidades de bit alto en Resize (indice negativo), el dispositivo D3D9 real, '
              'el valor de EAX/ECX/EDX de las funciones void y los bits altos de AL.')
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
                       'scope': ('Reconstruida en C++ (los colaboradores de la CRT, operator new y delete, y los objetos COM de D3D9 son falsos identicos en ambos lados en el banco). Banco Fn95A con ' + str(int(s['pairs'])) + ' pares y cero diferencias; Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else '')) }
    registry_store.save_registry(ROOT, data)
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
