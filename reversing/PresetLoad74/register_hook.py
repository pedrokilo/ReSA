"""Registra RsLoadPresetViews (0x619D60, lote 74) con la corrida APROBADA de PresetLoad74.

Uso (SOLO tras una corrida aprobada): python register_hook.py <carpeta-de-evidencia>. NO toca docs/ (la fila de docs/hooks.csv `Rs,RsLoadPresetViews,0x00619D60,1,0,1,S` la anade el coordinador).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PresetLoad74'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('RsLoadPresetViews', '0x00619D60', 'Rs', 'RsLoadPresetViews', 'source/app/platform/preset_view.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/wy-4 (HEAD f13a750cf + cambios integrados del lote 74; ver artifacts/build-manifest.json). RsLoadPresetViews es cdecl sin argumentos. Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL: psPathnameCreate 0x745470, psPathnameDestroy 0x7454E0, RsErrorMessage 0x619B00 y CMemoryMgr::Malloc 0x72F420 (con hook RH apagado), RsDestroyPresetViews 0x6198C0 y CMemoryMgr::Free 0x72F430 (reconstruidas sin hook) e isprint del CRT 0x8228B5 (la DLL lo llama por la direccion del EXE); y, durante la llamada, las entradas fopen, fclose, fgets, sprintf, strcpy, strlen y sscanf de la tabla de RenderWare (RwEngineInstance + 0xC4 / + 0xF0). El banco alimenta el contenido de views.txt desde un buffer controlado. NO prueba: la E/S real de ficheros (fopen/fgets/fclose reales), el formato real de sscanf (el falso devuelve los valores configurados), el isprint real del CRT ni la liberacion/asignacion real de memoria; tampoco el desbordamiento del buffer de nombre de 0x40 bytes del EXE (nombres de mas de 63 caracteres) ni RwOsGetFileInterface nulo. Valida el filtrado por isprint (hash de las llamadas), el orden de las llamadas, la lista enlazada (nodos y descripciones byte a byte, floats con NaN no canonicos), los caminos de memoria insuficiente (sin cerrar el fichero) y el valor de retorno. EAX completo solo se registra: el unico llamador, RsSavePresetView, lo lee como bool.')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE (los colaboradores son falsos identicos en ambos lados). Banco PresetLoad74 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados. Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
