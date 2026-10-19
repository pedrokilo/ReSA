"""Registra CQuaternion::Get(RwV3d*, float*) (0x0059C230) con la corrida APROBADA de QuatGetAxis29.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-quatget28/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/QuatGetAxis29/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila `CQuaternion,Get-axis-angle,0x0059C230,1,0,1,S` de docs/hooks.csv,
si falta, la anade el coordinador).
Antes de ejecutarlo hay que rellenar SCOPE_NOTE (colaboradores sustituidos, resultado del sondeo del CRT y desajuste de fuentes probadas, si lo hay).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'QuatGetAxis29'
FUNCS = (
    # (id del banco, direccion, clase, funcion, fuente)
    ('CQuaternion::Get(RwV3d*, float*)', '0x0059C230', 'CQuaternion', 'Get-axis-angle', 'source/game_sa/Core/Quaternion.cpp'),
)
SCOPE_NOTE = ('Fuentes probadas = árbol principal (Quaternion.cpp/.h, X87Math.cpp/.h, InjectHooksMain.cpp, CMakeLists.txt idénticos byte a byte), compiladas desde el worktree D:/Original/gta-wt-b29 (HEAD 29305c78d + parche de QuatGetAxis29; ver artifacts/build-manifest.json). CQuaternion::Get(RwV3d*, float*) sin colaboradores sustituidos; usa el _CIacos del EXE a través de FlcX87 (X87Math.cpp). Caso 2w == -1 (fldpi de 64 bits) con eje desconocido en el oráculo: sólo EXE == DLL. errno comparado EXE/DLL. El oráculo independiente se relajó a 2 ulp en los floats de salida porque acos del CRT del EXE y fsin no se modelan al ulp; la comparación EXE == DLL es estricta bit a bit en los 4096 pares. La primera corrida (20261018-221739-268232) se rechazó sólo por ese oráculo y no es evidencia. NaN siempre silenciosos (sNaN no cubierto); los empates de NaN sólo exigen EXE == DLL. EAX/ECX/EDX sólo registrados; ningún llamador del EXE. La clase CQuaternion conserva otras funciones pendientes (Multiply, Set(x,y,z), Slerp(t), Conjugate y Get(float*,float*,float*) 0x59C160, sin llamadores): este banco no la completa.')
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
                       'scope': ('Reconstruida en C++ sin llamadas al EXE por la función (QuaternionGetAxisAngleCpp de X87Math.cpp sobre la máquina FlcX87 con el _CIacos del EXE; sin colaboradores sustituidos). Banco QuatGetAxis29 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; '
                                 'EAX/ECX/EDX sólo registrados (la función no tiene llamadores en el EXE). Sin cobertura exhaustiva más allá de los fixtures del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
