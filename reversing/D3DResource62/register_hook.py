"""Registra D3DResourceSystem::CancelBuffering (0x00730900), CreateIndexBuffer (0x007306A0), CreateTexture (0x00730510), DestroyIndexBuffer (0x00730D30), DestroyTexture (0x00730B70),
Init (0x00730830) y Shutdown (0x00730A00) con la corrida APROBADA de D3DResource62.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-d3dresource62/<AAAAMMDD-HHMMSS-micros>"

PLANTILLA: es la de Misc61 adaptada a siete hooks Simple. Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/D3DResource62/validation-summary.json y anade las
entradas a reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador):
`D3DResourceSystem,<Funcion>,<direccion>,1,0,1,S` para cada una de las siete.
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'D3DResource62'
SRC_FILE = 'source/game_sa/RenderWare/D3DResourceSystem.cpp'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('D3DResourceSystem::CancelBuffering', '0x00730900', 'D3DResourceSystem', 'CancelBuffering', SRC_FILE, 'S'),
    ('D3DResourceSystem::CreateIndexBuffer', '0x007306A0', 'D3DResourceSystem', 'CreateIndexBuffer', SRC_FILE, 'S'),
    ('D3DResourceSystem::CreateTexture', '0x00730510', 'D3DResourceSystem', 'CreateTexture', SRC_FILE, 'S'),
    ('D3DResourceSystem::DestroyIndexBuffer', '0x00730D30', 'D3DResourceSystem', 'DestroyIndexBuffer', SRC_FILE, 'S'),
    ('D3DResourceSystem::DestroyTexture', '0x00730B70', 'D3DResourceSystem', 'DestroyTexture', SRC_FILE, 'S'),
    ('D3DResourceSystem::Init', '0x00730830', 'D3DResourceSystem', 'Init', SRC_FILE, 'S'),
    ('D3DResourceSystem::Shutdown', '0x00730A00', 'D3DResourceSystem', 'Shutdown', SRC_FILE, 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/we-5 (HEAD 0ac292a76 + fuente del lote 62). NO se inicializa ni se destruye el dispositivo D3D real: durante cada llamada _RwD3DDevice (0xC97C28) apunta a un '
              'dispositivo falso y todos los recursos (texturas e index buffers) son objetos COM falsos con tablas de metodos propias (Release, GetLevelCount, GetLevelDesc, GetDesc, CreateTexture, '
              'CreateIndexBuffer); el estado real de las caches (0xC87C60..0xC87FE8 y el byte 0x8D6084) se guarda antes de cada lado, se sustituye por el del caso y se restaura justo despues. '
              'Colaboradores del EXE sin reconstruir sustituidos por falsos identicos en ambos lados (no se validan sus cuerpos reales): D3DTextureBuffer::Pop (0x72FF60), Setup (0x72FE80), Push (0x72FFF0), '
              'PushWithoutIncreasingCounter (0x730AD0), Resize (0x730020), D3DIndexDataBuffer::Pop (0x730270), Resize (0x730330) y el new/delete del EXE (0x82119A / 0x8214BD, solo en el hilo del banco y con la '
              'ventana armada; fuera de ella reenvian al codigo real). Los falsos de Resize solo cambian la capacidad segun el modo del caso; los de Pop devuelven el recurso del caso sin sacarlo de la pila. '
              'Init, Shutdown y CancelBuffering se ejercitan con pilas/recursos falsos (nunca con los reales del juego). EAX/ECX/EDX solo registrados, salvo EAX (HRESULT) de CreateTexture y CreateIndexBuffer, '
              'que los llamadores del EXE (0x4CB935, 0x4CD9FE, 0x4C997C) si leen. No se prueba CreateIndexBuffer con numIndices >= 0x80000001 (grupo negativo: el EXE indexa fuera del arreglo).')
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
                                 'en ambos lados). Banco D3DResource62 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; EAX/ECX/EDX solo registrados (EAX de los HRESULT si se compara). '
                                 'Sin cobertura exhaustiva mas alla de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
