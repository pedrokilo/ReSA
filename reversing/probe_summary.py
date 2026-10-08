"""Genera validation-summary.json de un banco (ultimo informe aprobado) y sus entradas en el registro.

Uso: python reversing/probe_summary.py <carpeta del banco> <prefijo> "<limitacion 1>" ["<limitacion 2>" ...]
Las funciones salen de pairs_per_function ("Clase::funcion") y su direccion/tipo de docs/hooks.csv.
Las fuentes registradas son las del manifiesto de la sonda que produjo el informe (lo que se probo).
"""
import csv
import json
import sys
import time
from pathlib import Path
from runtime_runner import GAME, EXE_HASH, digest

ROOT = Path(__file__).resolve().parents[1]

def main(here, prefix, limitations):
    here = Path(here).resolve()
    runs = sorted((GAME/f'resultados-{prefix}').glob('*/summary.json'))
    if not runs:
        raise RuntimeError('No hay ejecuciones para ' + prefix)
    run = runs[-1]
    result = json.loads(run.read_text(encoding='utf-8'))
    manifest_path = here/'artifacts/build-manifest.json'
    manifest = json.loads(manifest_path.read_text())
    if not result['passed']:
        raise RuntimeError('La última ejecución no está aprobada: ' + str(run))
    if run.stat().st_mtime < manifest_path.stat().st_mtime:
        raise RuntimeError('La sonda se recompiló después del resultado; ejecutar de nuevo: ' + prefix)
    for ext, expected in manifest['binaries'].items():
        if digest(here/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}') != expected:
            raise RuntimeError('Binario archivado no coincide: ' + prefix + '.' + ext)
    functions = result['pairs_per_function']
    summary = dict(result, evidence_directory=str(run.parent), original_exe_sha256=EXE_HASH,
                   covered_functions={k: True for k in functions}, probe_build_manifest=manifest, limitations=limitations)
    out = here/'validation-summary.json'
    out.write_text(json.dumps(summary, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    rows = {(r['class'], r['fn_name']): r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig'))}
    registry_path = ROOT/'reversing/validation-registry.json'
    registry = json.loads(registry_path.read_text(encoding='utf-8'))
    keys = {tuple(k.split('::')) for k in functions}
    registry['functions'] = [r for r in registry['functions'] if (r['hook']['class'].removesuffix('<T>'), r['hook']['fn_name']) not in keys]
    rel = str(out.relative_to(ROOT)).replace('\\', '/')
    for label, pairs in functions.items():
        cls, fn = label.split('::')
        if (cls, fn) not in rows and (cls + '<T>', fn) in rows:
            cls += '<T>'  # plantillas: el banco usa el nombre sin <T> (las etiquetas solo admiten \w+)
        if (cls, fn) not in rows:
            print('sin fila en hooks.csv (no se registra):', label)  # p. ej. CMatrix/CColBox: clases sin hooks instalados
            continue
        row = rows[(cls, fn)]
        registry['functions'].append({
            'hook': {'class': cls, 'fn_name': fn, 'address': row['address'], 'type': row['type']},
            'evidence': {'path': rel, 'sha256': digest(out), 'passed_path': ['covered_functions', label], 'pairs': pairs, 'original_exe_sha256': EXE_HASH},
            'tested_sources': manifest['sources'],
            'source_provenance': f'Independent in-engine fixtures; source hashes from the archived probe build manifest ({run.parent.name}).',
            'dependency_review': None,
            'scope': f"{pairs} pares original/candidato con hook conmutado dentro del motor; banco de {result['pairs']} pares. " + ' '.join(limitations),
            'review_reference': str((here/'README.md').relative_to(ROOT)).replace('\\', '/'),
        })
    temporary = registry_path.with_suffix('.json.tmp')
    temporary.write_text(json.dumps(registry, indent=2, ensure_ascii=False), encoding='utf-8')
    for attempt in range(50):
        try:
            temporary.replace(registry_path)
            break
        except OSError:
            if attempt == 49:
                raise
            time.sleep(.2)
    print(run.parent.name, len(functions), result['pairs'])

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], sys.argv[3:])
