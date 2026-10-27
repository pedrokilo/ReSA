"""Registra CVehicle::AddReplacementUpgrade (EXE 0x006D3830; hook SIMPLE, no virtual) con la corrida APROBADA de ReplUpgrade72.

Uso (SOLO tras una corrida aprobada, con el informe en el clon de pruebas):  python register_hook.py <carpeta-de-evidencia>
  p. ej. python register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-replupgrade72/<AAAAMMDD-HHMMSS-micros>"

Archiva la corrida en reversing/CppTwelveLevel6Final/runs/, escribe reversing/ReplUpgrade72/validation-summary.json y anade la entrada a
reversing/validation-registry.json y reversing/CppOnlyMigration/approvals.json. Idempotente. NO toca docs/ (la fila de docs/hooks.csv, si falta, la anade el coordinador:
"CVehicle,AddReplacementUpgrade,0x006D3830,1,0,1,S").
"""
import gzip, hashlib, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BANK = 'ReplUpgrade72'
FUNCS = (
    # (id del banco, direccion del EXE, clase, funcion, fuente, tipo de hook)
    ('AddReplacementUpgrade', '0x006D3830', 'CVehicle', 'AddReplacementUpgrade', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'S'),
)
SCOPE_NOTE = ('Validado en el worktree D:/Original/wy-3 (HEAD 95f7d73eb + Vehicle.cpp, source.patch). CVehicle::AddReplacementUpgrade 0x6D3830 (thiscall, ret 8, sin retorno; ningun llamador del EXE, todos en AddVehicleUpgrade 0x6E3290, lee EAX/ECX/EDX): '
              'vacia el marco del componente (RemoveObjectsCB), crea el atomo del model info de la mejora (vtable +0x2C), lo cuelga del marco y configura su render (SetupUpgradeAtomicRendering); con nodeId 0x13 y la bandera 0x2000 de la manejabilidad crea el atomo de rueda y le da x = -2 * x del marco; con 0xC o 0xD vuelve a aplicar la matricula; si el model info es de atomo danado crea tambien la version danada y aplica la visibilidad. '
              'Sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): GetFrameFromId 0x4C53C0, RwFrameForAllObjects 0x7F1200, RwFrameForAllChildren 0x7F0DC0, CBaseModelInfo::AddRef 0x4C4BA0, RpClumpAddAtomic 0x74A490, '
              'RpAtomicSetFrame 0x74BF20, RwFrameDestroy 0x7F05A0, SetAtomicId 0x732290, SetAtomicFlag 0x7322B0, RpGeometryForAllMaterials 0x74C790, SetAtomicRenderCallback 0x7328A0, SetRenderPipelinesCB 0x4C88F0, CreateReplacementAtomic 0x6D3700, RwMatrixUpdate 0x7F18A0 y SetupClumpAfterVehicleUpgrade 0x6FDFE0; '
              'CreateInstance y AsDamageAtomicModelInfoPtr son virtuales de una vtable falsa. Se compara contra un oraculo independiente en Python (desde el desensamblado). Fuera de cobertura: indices de modelo fuera de ms_modelInfoPtrs[15..19], denormales (MXCSR FTZ), dos NaN, y el cuerpo real de los colaboradores sustituidos.')
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
    for ID, ADDR, CLS, FN, SOURCE, HTYPE in FUNCS:
        if not any(r['hook']['class'] == CLS and r['hook']['fn_name'] == FN for r in data['functions']):
            data['functions'].append(dict(
                hook={'class': CLS, 'fn_name': FN, 'address': '0x00000000' if HTYPE == 'V' else ADDR, 'type': HTYPE},
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
                       'scope': ('Reconstruida en C++ (CVehicle, mejoras de reemplazo): ' + FN + ' sin llamadas al EXE para esta función. '
                                 'El banco sustituye por falsos idénticos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL) los colaboradores y compara la secuencia de llamadas con sus argumentos, '
                                 'los cambios en la arena del caso y ms_bCreateDamagedVersion, contra un oráculo independiente. Banco ReplUpgrade72 con ' + str(int(s['pairs'])) + ' pares (' + str(int(ppf[ID])) + ' propios) y cero diferencias; '
                                 'EAX/ECX/EDX de salida: ver README del banco.' + (' ' + SCOPE_NOTE if SCOPE_NOTE else ''))}
    rp.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    ap.write_bytes(json.dumps(a, indent=2, ensure_ascii=False).encode('utf-8'))
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    main(sys.argv[1])
