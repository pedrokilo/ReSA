"""Comprueba evidencia del lote contra las fuentes finales y el entorno restaurado."""
import hashlib
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT/'contrib'))
from validation_registry import evaluate, KEY

scope = json.loads((Path(__file__).parent/'scope.json').read_text())
expected = {tuple(r[k] for k in KEY) for r in scope['functions']}
evaluated = evaluate(ROOT, ROOT/'docs/hooks.csv')
found = [r for r in evaluated['functions'] if tuple(r['hook'][k] for k in KEY) in expected]
assert len(found) == 78
assert all(r['evidence_ok'] and r['source_match'] for r in found)
summary = json.loads((Path(__file__).parent/'summary.json').read_text())
assert summary['passed'] and summary['pairs'] == 81664
for result in summary['results']:
    data = json.loads((Path(result['evidence_directory'])/'summary.json').read_text())
    for flag in ('passed', 'saves_unchanged', 'configs_restored', 'state_restored'):
        assert data[flag], (result['bank'],flag)
game = ROOT/'source/game_sa/Game.cpp'
assert hashlib.sha256(game.read_bytes()).hexdigest() == '0639900130cc088be6264f392dbdf674433dd6d60db02bc2450cc14a338d1c87'
print(json.dumps({k:evaluated[k] for k in ('total','documented','source_matches','current')}))
print('78 evidencias aprobadas, fuentes coincidentes, partidas/INI/estado restaurados y Game.cpp limpio')
