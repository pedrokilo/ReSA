"""Ejecuta el banco de CAEWeaponAudioEntity con el runner compartido.

Audio permanece ACTIVO durante el banco (solo se excluye Global/SurfaceInfos_c):
las dependencias de audio que no se controlan (CAESound::Initialise, etc.) son las
reconstrucciones habilitadas, iguales en ambos lados del par.

run_local.py NO reconstruye la sonda: tras cambiar Probe.hpp/LiveComparison.hpp,
ejecutar build_probe.py, copiar artifacts/gta_reversed-weaponaudio-probe.{asi,pdb}
a <juego>/_baseline/ y actualizar PROBE_HASH.
"""
import json
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case, digest
PROBE_HASH = 'f7065c02727426ebf2556cff951a6fbb664d68b6508b1b3c22a2520616db0657'
FUNCTIONS = [
    'Initialise', 'Terminate', 'Reset', 'WeaponFire', 'WeaponReload',
    'PlayGunSounds', 'PlayGoggleSound', 'ReportStealthKill', 'ReportChainsawEvent',
    'PlayChainsawStopSound', 'PlayWeaponLoopSound', 'PlayMiniGunFireSounds', 'PlayMiniGunStopSound',
    'PlayFlameThrowerSounds', 'PlayFlameThrowerIdleGasLoop', 'StopFlameThrowerIdleGasLoop',
    'PlayCameraSound', 'UpdateParameters',
]
LOCKED = ['Constructor', 'Destructor']


def rows_of(path):
    return [json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]


def analyze(path, strict=True):
    if not path.exists(): return None
    rows = rows_of(path)
    if any(r['event'] in ('assertion', 'error') for r in rows):
        if strict: raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed = [r for r in rows if r['event'] in ('complete', 'error') and 'pairs' in r]
    if not completed: return None
    ready = [r for r in rows if r['event'] == 'player_ready']
    if len(ready) != 1 or not ready[0]['independent_inputs']: raise RuntimeError('Entradas no independientes.')
    pairs = [r for r in rows if r['event'] == 'comparison']
    locked = [r for r in rows if r['event'] == 'locked_comparison']
    c = completed[-1]
    if [r['pair'] for r in pairs] != list(range(len(pairs))) or c['pairs'] != len(pairs):
        raise RuntimeError('Cobertura incompleta o desordenada.')
    failed = [r for r in pairs if not r['passed']]
    per_fn = Counter(r['fn'] for r in pairs)
    summary = dict(
        pairs=len(pairs), passed_pairs=len(pairs) - len(failed), failed_pairs=len(failed),
        per_function=dict(sorted(per_fn.items())),
        failed_per_function=dict(sorted(Counter(r['fn'] for r in failed).items())),
        inexact_pairs=c['inexact_pairs'], max_float_diff=c['max_float_diff'],
        foreign_calls=c['foreign_calls'], state_restored=c['state_restored'],
        hooks_before=ready[0]['hooks_before'], camera=ready[0]['camera'],
        requested_sounds_original=sum(sum(1 for e in r['original']['events'] if e['t'] == 'sound') for r in pairs if r.get('original')),
        locked=[dict(fn=r['fn'], address=hex(r['address']), equal=r['equal'],
                     original_candidate_calls=r['original_candidate_calls'], candidate_calls=r['candidate_calls']) for r in locked],
        first_failures=[dict(pair=r['pair'], fn=r['fn'], case=r['case']) for r in failed[:20]],
        log_sha256=digest(path),
    )
    if strict:
        missing = [f for f in FUNCTIONS if per_fn.get(f, 0) == 0]
        if missing: raise RuntimeError('Funciones sin pares: ' + ', '.join(missing))
        if failed: raise RuntimeError(f'{len(failed)} pares divergentes; primero: {failed[0]["pair"]} {failed[0]["fn"]}')
        if [r['fn'] for r in locked] != LOCKED: raise RuntimeError('Faltan comparaciones de entradas bloqueadas.')
        if not c['state_restored'] or c['event'] != 'complete': raise RuntimeError('Estado no restaurado.')
        if any(r['original_candidate_calls'] != 0 or r['candidate_calls'] < 1 for r in pairs):
            raise RuntimeError('Contadores de ejecución inesperados.')
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        result = analyze(Path(sys.argv[2]), strict=False)
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result, indent=2))
    else:
        sys.exit(run_case('weaponaudio', PROBE_HASH, analyze,
                          'CAEWeaponAudioEntity (20 entradas, Audio activo)',
                          timeout=300, stop_before_analysis=True, unhook='Global/SurfaceInfos_c'))
