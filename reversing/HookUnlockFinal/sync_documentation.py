"""Sincroniza seis locks aprobados; sin --apply sólo verifica y presenta el alcance."""
import argparse
import csv
import gzip
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
TARGETS = {
    ('CAEVehicleAudioEntity', 'ProcessPlayerVehicleEngine'): ('source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp', '0x4FBB10'),
    ('CAEVehicleAudioEntity', 'ProcessVehicle'): ('source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp', '0x501E10'),
    ('CAEVehicleAudioEntity', 'Service'): ('source/game_sa/Audio/Entities/AEVehicleAudioEntity.cpp', '0x502280'),
    ('Game', 'FrontendIdle'): ('source/app/app_game.cpp', '0x53E770'),
    ('Game', 'Idle'): ('source/app/app_game.cpp', '0x53E920'),
    ('Game', 'Render2dStuff'): ('source/app/app_game.cpp', '0x53E230'),
}
RUNS = {
    'audio': ('HookUnlockAudio', '20261010-170746-451158', 19840, 'active_audio'),
    'audio_original': ('HookUnlockAudio', '20261010-171052-497326', 19840, 'original_audio'),
    'game': ('HookUnlockGame', '20261010-170235-722875', 2560, None),
    'full': ('HookUnlockFinal', 'vehaudio-20261010-170444-407103', 20024, None),
}
MARKER = re.compile(r'<!-- RESA-HOOK-UNLOCK-FINAL:BEGIN -->.*?<!-- RESA-HOOK-UNLOCK-FINAL:END -->\s*', re.S)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def verify_run(bank, run, expected_pairs, expected_profile=None):
    directory = ROOT/'reversing'/bank/'runs'/run
    summary = json.loads((directory/'summary.json').read_text(encoding='utf-8'))
    manifest = json.loads((directory/'build-manifest.json').read_text(encoding='utf-8'))
    if summary['executed_probe_sha256'] != manifest['binaries']['asi']:
        raise RuntimeError(f'ASI no coincide con el manifiesto: {bank}/{run}')
    if expected_profile is not None and summary.get('audio_profile') != expected_profile:
        raise RuntimeError(f'Perfil de Audio incorrecto: {bank}/{run}')
    for key in ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored'):
        if summary.get(key) is not True:
            raise RuntimeError(f'{bank}/{run}: {key} no aprobado')
    raw = gzip.decompress((directory/'comparisons.jsonl.gz').read_bytes())
    if sha(raw) != summary['log_sha256']:
        raise RuntimeError(f'Log alterado: {bank}/{run}')
    rows = [json.loads(line) for line in raw.splitlines()]
    pairs = [r for r in rows if r.get('event') == 'comparison']
    if len(pairs) != expected_pairs or {r['pair'] for r in pairs} != set(range(expected_pairs)):
        raise RuntimeError(f'Cobertura incompleta: {bank}/{run}')
    if any(r.get('original') is None or r.get('candidate') is None or not r.get('equal') or
           not r.get('accepted') or r['original'] != r['candidate'] for r in pairs):
        raise RuntimeError(f'Diferencia o par nulo: {bank}/{run}')
    if any(r.get('event', '').startswith('first_chance_av') or r.get('event') == 'error' for r in rows):
        raise RuntimeError(f'Error runtime: {bank}/{run}')
    if bank == 'HookUnlockAudio':
        if (summary.get('live_frames'), summary.get('live_explicit_calls'), summary.get('live_mixed_contexts')) != (960, 2240, 32):
            raise RuntimeError('Cobertura real de Audio incompleta')
    elif bank == 'HookUnlockGame':
        if summary.get('game_frames') != 320 or summary.get('real_render_frames') != 160:
            raise RuntimeError('Cobertura real de Game incompleta')
        if summary.get('idle_background_dispatch_guard_proven') is not True:
            raise RuntimeError('Guarda background de Idle no acreditada')
    elif len({r['function'] for r in pairs}) != 102:
        raise RuntimeError('Regresión completa de 102 funciones incompleta')
    return summary


def source_unlocked():
    for (owner, name), (filename, address) in TARGETS.items():
        lines = (ROOT/filename).read_text(encoding='utf-8').splitlines()
        candidates = [line for line in lines if 'RH_Scoped' in line and name in line and address.lower() in line.lower()]
        if len(candidates) != 1:
            raise RuntimeError(f'Registro ambiguo: {owner}::{name}')
        code = candidates[0].split('//', 1)[0]
        if re.search(r'\.locked\s*=\s*true|\.reversed\s*=\s*false', code):
            raise RuntimeError(f'La fuente sigue bloqueada: {owner}::{name}')


def protected_hashes():
    frozen_file = ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json'
    frozen = json.loads(frozen_file.read_text(encoding='utf-8'))['frozen_markdown_exceptions']
    result = {}
    for filename, entry in frozen.items():
        actual = sha((ROOT/filename).read_bytes())
        if actual != entry['sha256']:
            raise RuntimeError(f'Documento congelado alterado: {filename}')
        result[filename] = actual
    result['reversing/runtime_runner.py'] = sha((ROOT/'reversing/runtime_runner.py').read_bytes())
    # Protege snapshots y documentos archivados de los dos runs aprobados.
    for spec in RUNS.values():
        bank, run = spec[:2]
        directory = ROOT/'reversing'/bank/'runs'/run
        for path in directory.rglob('*'):
            if path.is_file():
                result[path.relative_to(ROOT).as_posix()] = sha(path.read_bytes())
    return result


def evidence_block(relative_prefix, rows):
    locked = [r for r in rows if r['locked'] == '1']
    lines = [
        '<!-- RESA-HOOK-UNLOCK-FINAL:BEGIN -->',
        '**Bloqueos reales pendientes: '+str(len(locked))+'.** Los seis locks restantes del corte anterior se retiraron después de pruebas dentro del juego y reparación del ABI. Los dos desbloqueos anteriores de CEntryExit/CPostEffects siguen acreditados en su banco. Los 413 indicadores históricos obsoletos no se presentan como desbloqueos nuevos.',
        '',
        '| Clase | Hook | Bloqueado | Evidencia de conmutación |',
        '| --- | --- | --- | --- |',
    ]
    for owner, name in TARGETS:
        bank, run = RUNS['audio' if owner == 'CAEVehicleAudioEntity' else 'game'][:2]
        link = relative_prefix+f'reversing/{bank}/runs/{run}/summary.json'
        lines.append(f'| {owner} | {name} | 0 | [Resumen]({link}) |')
    lines += [
        '',
        'Audio: 19.840 pares aislados, cero diferencias, 960 frames, 2.240 llamadas reales y 32 contextos. Se reprodujo un crash real por ECX: el caller EXE 0x4FC2A9 reutiliza ECX tras 0x4FC29B; el hook de IsAccInhibitedForLowSpeed conserva ahora ese registro mediante stackArguments=1. Game: 2.560 pares, 320 frames y 160 frames de render real; se conserva y prueba la guarda de background de Idle y se elimina el segundo parche no conmutable de Render2dStuff.',
        'Se exige también la repetición completa de las 102 funciones (20.024 pares) y la misma campaña de audio con el perfil original del subsistema; ambos perfiles se ejecutan con los flags finales retirados. [Archivo y alcance de los cuatro bancos]('+relative_prefix+'reversing/HookUnlockFinal/README.md).',
        '',
        'Se comprobaron restauración de hooks, configuración y partidas. La sesión de audio se descarta sin guardar; no se afirma restauración exacta de tareas, estadísticas o streaming ni estabilidad universal, todos los modelos o todos los controladores. Desbloqueo no significa independencia del EXE. El corte 453 sigue en 8.445/8.446 funciones validadas; Win::NOTSA_WinMain permanece pendiente.',
        '<!-- RESA-HOOK-UNLOCK-FINAL:END -->', '',
    ]
    return '\n'.join(lines)


def update_current_documents(rows):
    for filename, prefix in [('README.md', ''), ('docs/CurrentStatus.md', '../')]:
        path = ROOT/filename
        text = MARKER.sub('', path.read_text(encoding='utf-8'))
        # Esta es una descripción actual, no un párrafo histórico fechado del cuerpo.
        if filename.endswith('CurrentStatus.md'):
            text = re.sub(r'^\*\*Auditoría posterior de bloqueos:\*\*[^\n]*\n\s*', '', text, flags=re.M)
            first, rest = text.split('\n', 1)
            text = first+'\n\n'+evidence_block(prefix, rows)+rest.lstrip('\n')
        else:
            # Mantiene el aviso global primero y sitúa el resultado actual antes del cuerpo.
            match = re.search(r'<!-- RESA-CURRENT-STATUS:END -->\s*', text)
            position = match.end() if match else 0
            text = text[:position]+evidence_block(prefix, rows)+'\n'+text[position:]
        path.write_text(text, encoding='utf-8')
    roadmap = ROOT/'docs/ProjectRoadmap.md'
    text = roadmap.read_text(encoding='utf-8')
    replacement = ('2. **Conmutación de los seis locks acreditada en el alcance probado.** '
                   'Se retiraron los bloqueos de los tres hooks de audio y de los tres del bucle/render tras pruebas de mezclas, '
                   'transiciones y render real; se corrigió el contrato ECX del helper de motor y el parche '
                   'duplicado de Render2dStuff. La [evidencia final](../reversing/HookUnlockFinal/README.md) '
                   'separa los desbloqueos efectivos de los indicadores históricos obsoletos. '
                   'La campaña completa de Alt-Tab/minimizar/reactivar con mensajes nativos y todos los '
                   'controladores sigue fuera de lo acreditado; no se presenta como lock pendiente.')
    text = re.sub(r'^2\. \*\*Revisar los seis locks restantes\.\*\*[^\n]*', replacement, text, flags=re.M)
    roadmap.write_text(text, encoding='utf-8')
    audit = ROOT/'reversing/HookUnlockAudit/README.md'
    marker = '<!-- RESA-HOOK-UNLOCK-SUPERSEDED -->'
    text = audit.read_text(encoding='utf-8')
    if marker not in text:
        text += '\n'+marker+'\n\n**Actualización posterior:** los seis locks conservados en este informe histórico se probaron y retiraron posteriormente. Consultar [estado actual](../../docs/CurrentStatus.md), [Audio](../HookUnlockAudio/README.md) y [Game](../HookUnlockGame/README.md). Las cifras y resultados del cuerpo mantienen su fecha y alcance.\n'
        audit.write_text(text, encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true', help='Aplicar el plan después de todas las verificaciones')
    parser.add_argument('--date', default='2026-10-10')
    args = parser.parse_args()
    protected = protected_hashes()
    summaries = {name: verify_run(*spec) for name, spec in RUNS.items()}
    if summaries['audio']['executed_probe_sha256'] != summaries['audio_original']['executed_probe_sha256']:
        raise RuntimeError('Los dos perfiles no probaron el mismo ASI final')
    source_unlocked()
    path = ROOT/'docs/hooks.csv'
    with path.open(encoding='utf-8-sig', newline='') as stream:
        reader = csv.DictReader(stream); fields = reader.fieldnames; rows = list(reader)
    identities = [(r['class'], r['fn_name']) for r in rows]
    for target in TARGETS:
        if identities.count(target) != 1:
            raise RuntimeError(f'Identidad CSV ausente o ambigua: {target}')
    if any(r['locked'] == '1' and (r['class'], r['fn_name']) not in TARGETS for r in rows):
        raise RuntimeError('Hay otros bloqueos; el alcance de seis filas debe revisarse')
    old = [r.copy() for r in rows]
    for row in rows:
        if (row['class'], row['fn_name']) in TARGETS:
            row['locked'] = '0'
    for before, after in zip(old, rows):
        if any(before[key] != after[key] for key in fields if key != 'locked'):
            raise RuntimeError('Cambio CSV fuera de locked')
    changed = [(a['class'], a['fn_name']) for a,b in zip(old, rows) if a != b]
    commands = [
        ['contrib/sync_docs.py', '--update-source-status'],
        ['contrib/sync_readme_counts.py'],
        ['contrib/build-memory-map.py'],
        ['contrib/build-binary-map.py'],
        ['contrib/sync_current_markdown.py', '--date', args.date],
    ]
    plan = {'csv_locked_rows': changed, 'final_locked': 0, 'approved_runs': RUNS, 'commands': commands,
            'protected_files': len(protected), 'apply': args.apply}
    print(json.dumps(plan, indent=2, ensure_ascii=False))
    if not args.apply:
        return
    with path.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, lineterminator='\n'); writer.writeheader(); writer.writerows(rows)
    for command in commands:
        subprocess.run([sys.executable, '-B', *command], cwd=ROOT, check=True)
    update_current_documents(rows)
    subprocess.run([sys.executable, '-B', 'contrib/audit_hook_catalog.py', '--strict'], cwd=ROOT, check=True)
    for filename, expected in protected.items():
        if sha((ROOT/filename).read_bytes()) != expected:
            raise RuntimeError(f'Archivo protegido modificado: {filename}')
    print(json.dumps({'updated': True, 'locked': 0, 'csv_rows_changed': len(changed), 'protected_intact': len(protected)}))


if __name__ == '__main__':
    main()
