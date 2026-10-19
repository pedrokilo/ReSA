"""Registra CSprite2d::AddToBuffer (0x00728200) con la corrida aprobada de SpriteAddBuf28. 

Uso: python -B reversing/SpriteAddBuf28/register_hook.py <carpeta-de-corrida>   (p. ej. 20261018-131101-659386, dentro de E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\resultados-spriteaddbuf28)
Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/SpriteAddBuf28/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK, ADDR, ID, CLS, FN = 'SpriteAddBuf28', '0x00728200', 'CSprite2d::AddToBuffer', 'CSprite2d', 'AddToBuffer'
SCOPE_NOTE = ('Fuentes probadas compiladas desde el worktree D:/Original/gta-wt-b28 (HEAD de6ed4a4d + source.patch de cada banco + tres arreglos posteriores; ver artifacts/build-manifest.json); idénticas al árbol principal en el momento del registro. Colaboradores sustituidos por falsos con traza idénticos en ambos lados: CSprite2d::SetVertices (0x727A00) y RenderVertexBuffer (0x7273D0); IsVertexBufferFull (0x7273A0) es real (función pura). No se valida aquí el cuerpo real de SetVertices ni de RenderVertexBuffer. NaN señalizantes cubiertos (la DLL copia con movss).')
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main(run_name):
    src = Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test') / 'resultados-spriteaddbuf28' / run_name
    run = ROOT / 'reversing/CppTwelveLevel6Final/runs' / (BANK + '-' + src.name)
    run.mkdir(exist_ok=True)
    for n in ('summary.json', 'RESULTADO.txt'):
        if not (run / n).exists():
            shutil.copyfile(src / n, run / n)
    if not (run / 'comparisons.jsonl.gz').exists():
        with open(src / 'comparisons.jsonl', 'rb') as f, gzip.GzipFile(run / 'comparisons.jsonl.gz', 'wb', mtime=0) as g:
            shutil.copyfileobj(f, g)
    s = json.loads((run / 'summary.json').read_text(encoding='utf-8'))
    assert s['passed'] is True and s['all_equal'] is True and s['state_restored'] is True
    ppf = dict(s['pairs_per_function'])
    tested = json.loads((ROOT / 'reversing' / BANK / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))['sources']
    ev = dict(passed=True, pairs=int(s['pairs']), differences=0, registered_pairs=int(s['pairs']), registered_functions=len(ppf), functions=ppf,
              covered_functions={k: True for k in ppf}, scope=SCOPE + ' ' + SCOPE_NOTE, dependency_review=None, run=run.relative_to(ROOT).as_posix(),
              summary_sha256=sha(run / 'summary.json'), result_txt_sha256=sha(run / 'RESULTADO.txt'), log_sha256=s['log_sha256'], all_equal=True,
              state_restored=True, saves_unchanged=s['saves_unchanged'], configs_restored=s['configs_restored'],
              original_exe_sha256=s['original_exe_sha256'], executed_probe_sha256=s['executed_probe_sha256'], manifest=dict(tested_sources=tested))
    out = ROOT / 'reversing' / BANK / 'validation-summary.json'
    out.write_bytes((json.dumps(ev, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))

    rp = ROOT / 'reversing/validation-registry.json'
    data = json.loads(rp.read_bytes().decode('utf-8'))
    if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
        data['functions'].append(dict(
            hook={'class': CLS, 'fn_name': FN, 'address': ADDR, 'type': 'S'},
            evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                      'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256']},
            tested_sources=tested, dependency_review=None,
            source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
            scope=SCOPE + ' ' + SCOPE_NOTE, review_reference=f'reversing/{BANK}/README.md'))
        rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))

    ap = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    a = json.loads(ap.read_bytes().decode('utf-8'))
    if ADDR not in a:
        a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': 'source/game_sa/Sprite2d.cpp', 'bank': BANK,
                   'run': src.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                   'audit': run.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                   'state_restored': True,
                   'scope': 'Reconstruida en C++ sin llamadas al EXE (SetVertices 0x727A00 y RenderVertexBuffer 0x7273D0 sustituidos por falsos con traza idénticos en ambos lados; IsVertexBufferFull 0x7273A0 real). Banco SpriteAddBuf28 con 4096 pares y cero diferencias; EAX/ECX/EDX sólo registrados porque ningún llamador los lee. Sin cobertura exhaustiva más allá de los fixtures del banco. ' + SCOPE_NOTE}
        ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    main(sys.argv[1])
