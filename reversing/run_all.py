"""Run archived engine probes serially and audit fresh logs and restoration."""
import csv
import importlib.util
import json
import subprocess
import sys
import time
from collections import Counter
from datetime import datetime
from pathlib import Path
from runtime_runner import GAME, EXE_HASH, digest, saves

PREFIXES = {'entryexit-pool': 'entryexit-pool', 'shooting-resources': 'shooting-resources', 'leave-car-bus': 'leave-car-bus', 'population-factory': 'population-factory', 'scanner-drag-fx': 'scanner-drag-fx', 'sound-copy': 'sound-copy', 'general': 'general', 'clothes-scripted-sprint': 'clothes-scripted-sprint', 'furniture': 'furniture', 'playerinfo-constructor': 'playerinfo-constructor', 'playerinfo': 'playerinfo', 'pedlist': 'pedlist', 'conversations': 'conversations', 'cped-completion': 'cped-completion', 'jetpack': 'jetpack', 'wander': 'wander', 'usegun': 'usegun', 'menusystem': 'menusystem', 'clouds': 'clouds', 'cvehicle': 'cvehicle', 'pcsave': 'pcsave', 'cplayerped': 'sprint',
            'cped-attached': 'attached', 'cped-position': 'updateposition',
            'weaponeffects': 'weaponeffects', 'fxmanager': 'fxmanager',
            'basemodelinfo': 'basemodelinfo', 'rpanimblend': 'rpanimblend'}
PREFIXES.update({name: name for name in ['unlock-data-stream', 'unlock-track-entity', 'unlock-audio', 'unlock-core', 'unlock-driving', 'unlock-input', 'unlock-physical', 'unlock-platform', 'unlock-radio', 'unlock-tasks']})
PREFIXES['namedanim-init'] = 'namedanim-init'
PREFIXES['ik-data'] = 'ik-data'
PREFIXES['conversation-for-ped'] = 'conversation-for-ped'
PREFIXES['audio-completion'] = 'audio-completion'
PREFIXES.update({
    'ped-to-player': 'ped-to-player',
    'gang-leader-type': 'gang-leader',
    'shutdown-lists': 'shutdown-lists',
    'bullet-hit': 'bullet-hit',
    'sitatdesk': 'sitatdesk',
    'interioruseinfo': 'interioruseinfo',
    'weaponaudio': 'weaponaudio',
})


def game_running():
    rows = csv.reader(subprocess.check_output(['tasklist', '/FO', 'CSV', '/NH'], text=True, errors='replace').splitlines())
    return any(r and r[0].lower() in ('gta_sa.exe', 'gta-sa.exe') for r in rows)

def main():
    root = Path(__file__).resolve().parent
    cases = json.loads((root / 'cases.json').read_text(encoding='utf-8'))
    # The isolated filename test has a corresponding full in-engine case above.
    cases = [c for c in cases if c['id'] != 'pcsave-isolated']
    if any(c['id'] not in PREFIXES for c in cases): raise RuntimeError('Caso nuevo sin ruta de informe configurada.')
    if game_running(): raise RuntimeError('GTA ya esta abierto; no se cerrara una sesion ajena.')
    if digest(GAME / 'gta_sa.exe') != EXE_HASH: raise RuntimeError('Ejecutable no reconocido.')
    before_saves = saves()
    before_binaries = {ext: digest(GAME / f'scripts/gta_reversed.{ext}') for ext in ('asi', 'pdb')}
    artifacts = root / 'artifacts'; artifacts.mkdir(exist_ok=True)
    lock = artifacts / 'suite.lock'
    with lock.open('x') as f: f.write(str(datetime.now()))
    folder = artifacts / ('suite-' + datetime.now().strftime('%Y%m%d-%H%M%S-%f'))
    folder.mkdir()
    report = dict(passed=False, scope='archived probes in GTA; not a combined current-build stability test',
                  game=str(GAME), expected_cases=len(cases), normal_binaries_before=before_binaries,
                  saves_before=before_saves, cases=[])
    def save(): (folder / 'summary.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    save()
    try:
        for case in cases:
            name = case['id']; prefix = PREFIXES[name]
            result_root = GAME / f'resultados-{prefix}'
            existing = set(result_root.glob('*/summary.json'))
            print(f'INICIO {name}', flush=True)
            started = time.monotonic()
            with (folder / f'{name}.txt').open('w', encoding='utf-8') as output:
                process = subprocess.run([sys.executable, str(root / case['runner'])], cwd=root.parent, stdout=output, stderr=subprocess.STDOUT)
            result = dict(id=name, returncode=process.returncode, elapsed_seconds=round(time.monotonic()-started, 2), passed=False)
            try:
                fresh = set(result_root.glob('*/summary.json')) - existing
                if len(fresh) != 1: raise RuntimeError('No hay un unico informe nuevo; revisar salida del ejecutor.')
                path = fresh.pop(); summary = json.loads(path.read_text(encoding='utf-8'))
                result.update(report=str(path), summary=summary)
                log = path.parent / 'comparisons.jsonl'
                spec = importlib.util.spec_from_file_location('audit_' + name.replace('-', '_'), root / case['runner'])
                module = importlib.util.module_from_spec(spec); spec.loader.exec_module(module)
                evidence = module.analyze(log)
                if evidence is None: raise RuntimeError('Registro nuevo incompleto.')
                if digest(log) != summary.get('log_sha256'): raise RuntimeError('Hash del registro no coincide con informe.')
                rows = [json.loads(line) for line in log.read_text(encoding='utf-8').splitlines()]
                result['log_events'] = dict(Counter(r.get('event', 'unknown') for r in rows))
                result['comparisons'] = sum(result['log_events'].get(e, 0) for e in ('comparison', 'edge_comparison', 'comparison_update'))
                if name in ('namedanim-init', 'ik-data'):
                    result['comparisons'] = evidence['pairs']
                if process.returncode != 0 or not summary.get('passed'): raise RuntimeError('El ejecutor no aprobo el caso.')
                result['passed'] = True
            except Exception as error: result['error'] = str(error)
            restored = {ext: digest(GAME / f'scripts/gta_reversed.{ext}') for ext in before_binaries} == before_binaries
            unchanged = saves() == before_saves
            closed = not game_running()
            result.update(normal_binaries_restored=restored, saves_unchanged=unchanged, game_closed=closed)
            result['passed'] &= restored and unchanged and closed
            report['cases'].append(result); save()
            print(('APROBADO ' if result['passed'] else 'FALLO ') + name + f' ({result["elapsed_seconds"]} s)', flush=True)
            if not (restored and unchanged and closed):
                raise RuntimeError('Se detiene la serie por falta de restauracion o proceso abierto.')
        report['passed'] = len(report['cases']) == len(cases) and all(c['passed'] for c in report['cases'])
    except Exception as error: report['error'] = str(error)
    finally:
        report['normal_binaries_after'] = {ext: digest(GAME / f'scripts/gta_reversed.{ext}') for ext in before_binaries}
        report['saves_unchanged'] = saves() == before_saves
        report['total_comparisons'] = sum(c.get('comparisons', 0) for c in report['cases'])
        save(); lock.unlink()
        print('Informe conjunto: ' + str(folder / 'summary.json'), flush=True)
    return 0 if report['passed'] else 1

if __name__ == '__main__': sys.exit(main())
