"""Registra exclusivamente el lote de 78 pendientes tras comprobar sus 30 bancos."""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent))
import probe_summary
from runtime_runner import GAME, digest

BANKS = '''AudioFrontendPending AudioPedPending AudioScannerPending
AudioStreamingPending AudioVehiclePending DecisionPending CriminalPending
AttractorTaskPending GangHasslePending ProstitutePending ProstituteControlPending
DriveByPending ArrestPending ArmedPending ArmedNextPending LineUpPending
LineUpProcessPending PathFindPending PickupsPending PopulationPending
GroupEventPending GroupPlacerPending GangWarsPending RopePending PostEffectsPending
RendererPending ShadowManagerPending FontPending FxSystemPending PoolsPending'''.split()

def main():
    expected = {(r['class'], r['fn_name']) for r in json.loads((HERE/'scope.json').read_text())['functions']}
    covered = set()
    plans = []
    for name in BANKS:
        bank = HERE.parent/name
        script = (bank/'run_local.py').read_text(encoding='utf-8-sig')
        prefix = re.search(r"main\(Path\(__file__\)\.parent,\s*['\"]([^'\"]+)", script).group(1)
        reports = sorted((GAME/f'resultados-{prefix}').glob('*/summary.json'))
        if not reports:
            raise RuntimeError('Sin ejecución: ' + name)
        report = reports[-1]
        result = json.loads(report.read_text(encoding='utf-8'))
        manifest_path = bank/'artifacts/build-manifest.json'
        manifest = json.loads(manifest_path.read_text())
        if not result['passed'] or report.stat().st_mtime < manifest_path.stat().st_mtime:
            raise RuntimeError('Repetir banco antes de registrar: ' + name)
        for ext, value in manifest['binaries'].items():
            assert digest(bank/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}') == value
        keys = {tuple(label.split('::')) for label in result['pairs_per_function']}
        if covered & keys:
            raise RuntimeError('Función cubierta por más de un banco: ' + str(covered & keys))
        covered |= keys
        plans.append((name, bank, prefix, result, report))
    if covered != expected:
        raise RuntimeError(f'Alcance distinto: faltan {expected-covered}; sobran {covered-expected}')
    # Ninguna mutación hasta comprobar el lote completo.
    cases_path = HERE.parent/'cases.json'
    cases = json.loads(cases_path.read_text())
    for name, bank, prefix, result, report in plans:
        probe_summary.main(bank, prefix, [
            'Equivalencia limitada a los fixtures del banco; dependencias aisladas y límites descritos en su README. No acredita independencia del EXE ni una campaña integral de juego.'
        ])
        cases = [c for c in cases if c['id'] != prefix]
        owners = sorted({label.split('::')[0] for label in result['pairs_per_function']})
        cases.append(dict(id=prefix, name=', '.join(owners)+': pendientes', runner=f'{name}/run_local.py'))
    cases_path.write_text(json.dumps(cases, indent=2, ensure_ascii=False)+'\n', encoding='utf-8')
    aggregate = dict(classes=27, functions=len(covered), banks=len(plans),
                     pairs=sum(p[3]['pairs'] for p in plans), passed=True,
                     results=[dict(bank=p[0], prefix=p[2], pairs=p[3]['pairs'],
                                   evidence_directory=str(p[4].parent)) for p in plans])
    (HERE/'summary.json').write_text(json.dumps(aggregate, indent=2, ensure_ascii=False)+'\n', encoding='utf-8')
    print(json.dumps({k:v for k,v in aggregate.items() if k!='results'}))

if __name__ == '__main__':
    main()
