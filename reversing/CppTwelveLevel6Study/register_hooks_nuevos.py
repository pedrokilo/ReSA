"""Registra en reversing/validation-registry.json los hooks que el lote de nivel 6 anade al catalogo (docs/hooks.csv).

Son tres hooks que ya estaban registrados en el juego pero no tenian fila en el catalogo: CStats::UpdateSexAppealStat (0x55BF20),
CPlayerPed::ControlButtonSprint (0x60A610) y RpAnimBlend::FrameUpdateCallBackOffscreen (0x4D2E10). Cada uno se acredita con la corrida aprobada
que ya figura en reversing/CppOnlyMigration/approvals.json (summary.json con passed y all_equal verdaderos). El script es idempotente: no repite
un hook ya registrado ni sobrescribe un validation-summary.json existente.
"""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HOOKS = (
    # (clase del catalogo, funcion del catalogo, direccion, identidad en approvals, banco)
    ('CStats', 'UpdateSexAppealStat', '0x0055BF20', 'CStats::UpdateSexAppealStat', 'DependenciesStatsSex16'),
    ('CPlayerPed', 'ControlButtonSprint', '0x0060A610', 'CPlayerPed::ControlButtonSprint', 'DependenciesTaskSprint16'),
    ('RpAnimBlend', 'FrameUpdateCallBackOffscreen', '0x004D2E10', 'RpAnimBlend::FrameUpdateCallBackOffscreen', 'CppRpAnimUpdate1'),
)
SCOPE = ('Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. '
         'Evidencia limitada a los fixtures del banco, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.')


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    registry_path = ROOT / 'reversing/validation-registry.json'
    raw = registry_path.read_bytes().decode('utf-8')
    data = json.loads(raw)
    approvals = json.loads((ROOT / 'reversing/CppOnlyMigration/approvals.json').read_text(encoding='utf-8'))
    added = 0
    for cls, fn, address, identity, bank in HOOKS:
        if any(r['hook']['class'] == cls and r['hook']['fn_name'] == fn for r in data['functions']):
            continue
        entry = approvals[address]
        run_dir = ROOT / Path(entry['audit']).parent
        summary = json.loads((run_dir / 'summary.json').read_text(encoding='utf-8'))
        assert summary['passed'] is True and summary['all_equal'] is True and summary['state_restored'] is True
        pairs = int(entry['own_pairs'])
        manifest = json.loads((ROOT / 'reversing' / bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        tested = manifest.get('sources', {})
        evidence = dict(
            passed=True, pairs=int(summary['pairs']), differences=0, registered_pairs=int(summary['pairs']), registered_functions=1,
            functions={identity: pairs}, covered_functions={identity: True}, scope=SCOPE, dependency_review=None,
            run=run_dir.relative_to(ROOT).as_posix(), summary_sha256=sha(run_dir / 'summary.json'),
            result_txt_sha256=sha(run_dir / 'RESULTADO.txt'), log_sha256=summary['log_sha256'], all_equal=True, state_restored=True,
            saves_unchanged=summary['saves_unchanged'], configs_restored=summary['configs_restored'],
            original_exe_sha256=summary['original_exe_sha256'], executed_probe_sha256=summary['executed_probe_sha256'],
            manifest=dict(tested_sources=tested))
        out = ROOT / 'reversing' / bank / 'validation-summary.json'
        text = json.dumps(evidence, ensure_ascii=False, indent=2) + '\n'
        if not out.exists():
            out.write_bytes(text.encode('utf-8'))
        data['functions'].append(dict(
            hook=dict(**{'class': cls, 'fn_name': fn, 'address': address, 'type': 'S'}),
            evidence={'path': out.relative_to(ROOT).as_posix(), 'sha256': sha(out), 'passed_path': ['covered_functions', identity],
                      'pairs': pairs, 'original_exe_sha256': summary['original_exe_sha256']},
            tested_sources=tested, dependency_review=None,
            source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',
            scope=SCOPE, review_reference=f'reversing/{bank}/README.md'))
        added += 1
    if added:
        registry_path.write_bytes((json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8'))
    print(json.dumps({'registrados': added, 'total_registro': len(data['functions'])}))


if __name__ == '__main__':
    main()
