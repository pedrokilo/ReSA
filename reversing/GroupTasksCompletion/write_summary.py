"""Genera validation-summary.json y las entradas del registro desde el ultimo informe aprobado."""
import hashlib
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import GAME, EXE_HASH, digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
FUNCTIONS = {
    'Random::AllocateDefaultTasks': ('CPedGroupDefaultTaskAllocatorRandom', 'AllocateDefaultTasks', '0x00000000', 'V',
                                     ['source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h']),
    'SitInLeaderCar::AllocateDefaultTasks': ('CPedGroupDefaultTaskAllocatorSitInLeaderCar', 'AllocateDefaultTasks', '0x00000000', 'V',
                                             ['source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorSitInLeaderCar.h']),
    'GangFollower::SetCanEnterLeadersCar': ('CTaskComplexGangFollower', 'SetCanEnterLeadersCar', '0x0065EE90', 'S',
                                            ['source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.h',
                                             'source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp']),
}
LIMITATIONS = [
    'Fixtures aislados: grupo, peatones y vehiculo en memoria propia (sin IA completa ni streaming); se ejecuta el codigo real de los constructores/destructores de tareas y el pool de tareas real.',
    'CCarEnterExit::ComputeTargetDoorToEnterAsPassenger se controla (EXE y DLL) y se compara la secuencia de consultas.',
    'En CTaskComplexSequence se excluyen los 3 bytes de relleno tras m_bFlushTasks (0x39..0x3B): el original los deja con el patron del pool y la inicializacion por valor de la DLL los pone a cero; no tienen valor definido.',
    'Los constructores de tareas son dependencias compartidas: el original llama a sus direcciones del EXE (algunas ya enganchadas a la DLL); no se acredita su equivalencia aqui.',
    'Sin dependency_review: no acredita independencia del EXE ni estabilidad prolongada.',
]

def main():
    runs = sorted((GAME/'resultados-group-alloc').glob('*/summary.json'))
    run = next(p for p in reversed(runs) if json.loads(p.read_text(encoding='utf-8'))['passed'])
    result = json.loads(run.read_text(encoding='utf-8'))
    manifest = json.loads((HERE/'artifacts/build-manifest.json').read_text())
    sources = {p.replace('\\', '/'): h for p, h in manifest['sources'].items()}
    summary = dict(result, evidence_directory=str(run.parent), original_exe_sha256=EXE_HASH,
                   covered_functions={k: True for k in FUNCTIONS}, probe_build_manifest=manifest, limitations=LIMITATIONS,
                   class_recorded_progress={'CPedGroupDefaultTaskAllocatorRandom': '2/2', 'CPedGroupDefaultTaskAllocatorSitInLeaderCar': '2/2'})
    out = HERE/'validation-summary.json'
    out.write_text(json.dumps(summary, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    registry_path = ROOT/'reversing/validation-registry.json'
    registry = json.loads(registry_path.read_text(encoding='utf-8'))
    keep = [r for r in registry['functions'] if (r['hook']['class'], r['hook']['fn_name']) not in {(v[0], v[1]) for v in FUNCTIONS.values()}]
    for name, (cls, fn, addr, typ, files) in FUNCTIONS.items():
        tested = {}
        for f in files:
            current = hashlib.sha256((ROOT/f).read_bytes()).hexdigest()
            if f in sources and sources[f] != current: raise RuntimeError(f'{f} cambio desde la compilacion de la sonda')
            tested[f] = current
        keep.append({
            'hook': {'class': cls, 'fn_name': fn, 'address': addr, 'type': typ},
            'evidence': {'path': 'reversing/GroupTasksCompletion/validation-summary.json', 'sha256': digest(out),
                         'passed_path': ['covered_functions', name], 'pairs': result['pairs_per_function'][name],
                         'original_exe_sha256': EXE_HASH},
            'tested_sources': tested,
            'source_provenance': f'Independent in-engine fixtures; source hashes from the archived probe build manifest ({run.parent.name}); .cpp hash taken at registration (hook registration only).',
            'dependency_review': None,
            'scope': f"{result['pairs_per_function'][name]} pares original/candidato con hook conmutado dentro del motor; banco completo de {result['pairs']} pares (GroupTasksCompletion). " + ' '.join(LIMITATIONS[:3]),
            'review_reference': 'reversing/GroupTasksCompletion/README.md',
        })
    registry['functions'] = keep
    registry_path.write_text(json.dumps(registry, indent=2, ensure_ascii=False), encoding='utf-8')
    print(run.parent, digest(out))

if __name__ == '__main__': main()
