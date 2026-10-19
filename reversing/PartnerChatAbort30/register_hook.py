"""Registra CTaskComplexPartnerChat::MakeAbortable (0x00682C60, hook VIRTUAL) con la corrida APROBADA de PartnerChatAbort30.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:\\JUEGOS\\GTA San Andreas - CVehicle Test\\resultados-partnerchatabort30\\<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/PartnerChatAbort30/validation-summary.json y anade las entradas a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (las filas de docs/hooks.csv, si faltan, las anade el coordinador).
Antes de ejecutarlo hay que rellenar SCOPE_NOTE con el desajuste de fuentes probadas que haya (si lo hay), como hizo DecisionMakeDecision27 (hooks Virtual: la fila de docs/hooks.csv es CTaskComplexPartnerChat,MakeAbortable,0x00000000,1,0,1,V).
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'PartnerChatAbort30'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente)
    ('CTaskComplexPartnerChat::MakeAbortable', '0x00682C60', 'CTaskComplexPartnerChat', 'MakeAbortable', 'source/game_sa/Tasks/TaskTypes/TaskComplexPartnerChat.cpp'),
)
# Los hooks virtuales se registran con direccion 0x00000000 y tipo V (docs/hooks.csv: "CTaskComplexPartnerChat,MakeAbortable,0x00000000,1,0,1,V"); la direccion real va en evidence.original_address.
SCOPE_NOTE = 'Fuentes probadas compiladas desde el worktree D:/Original/wc-b (HEAD c5318699a + parches del lote 30; ver artifacts/build-manifest.json), de sólo lectura y eliminado tras el registro. El árbol principal tiene además trabajo sin validar del lote 27 en archivos vecinos, fusionado en tres vías, por lo que difieren de verdad, y sólo fuera del cuerpo probado, TaskComplexPartner.h: el árbol principal añade la declaración virtual pura GetPartnerSequence() en la base, ausente en la sonda; el resto coincide (módulo CRLF). Hook virtual (RH_ScopedVMTInstall) registrado como tipo V con original_address 0x00682C60. Ningún colaborador está sustituido: CTask::operator new, los constructores, AddTask y RegisterReference son los ya reconstruidos en C++ y enganchados, por lo que el lado EXE también los ejecuta (no se validan sus cuerpos aquí). EAX/ECX/EDX sólo registrados (0x683B46 los sobrescribe; sólo AL es retorno).'
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
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000', 'type': 'V'},
                evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', ID],
                          'pairs': int(ppf[ID]), 'original_exe_sha256': s['original_exe_sha256'], 'original_address': ADDR},
                tested_sources=tested, dependency_review=None,
                source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
                scope=scope, review_reference=f'reversing/{BANK}/README.md'))
        if ADDR not in a:
            a[ADDR] = {'identity': ID, 'converted_source_functions': 1, 'source': SOURCE, 'bank': BANK,
                       'run': SRC.name, 'pairs': int(s['pairs']), 'own_pairs': int(ppf[ID]),
                       'audit': RUN.relative_to(ROOT).as_posix() + '/summary.json', 'differences': 0, 'null_comparison_records': 0,
                       'state_restored': True,
                       'scope': ('Reconstruida en C++ sin llamadas al EXE: reserva secuencia, giro hacia el compañero y animación (0xA4 / 0x8E según m_leadSpeaker) con los colaboradores ya reconstruidos '
                                 '(operator new, constructores, AddTask, RegisterReference), que el banco exige enganchados en ambos lados. Banco PartnerChatAbort30 con ' + str(int(s['pairs'])) + ' pares y cero diferencias; '
                                 'ECX/EDX sólo registrados porque ningún llamador los lee (0x683B46 los sobrescribe). Sin cobertura del agotamiento total del pool (el EXE se detiene en ese caso).' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
