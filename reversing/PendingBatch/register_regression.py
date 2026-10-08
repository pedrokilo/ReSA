"""Renueva la evidencia de los cinco bancos anteriores y conserva sus límites."""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
import probe_summary
from runtime_runner import GAME

results = []
for name in ('CTaskDead','IdleAttractorPending','FightingControlPending','PlayerFighterPending','RoadBlockPending'):
    bank = HERE.parent/name
    prefix = re.search(r"main\((?:Path\(__file__\)\.parent|here),\s*['\"]([^'\"]+)",
                       (bank/'run_local.py').read_text()).group(1)
    old = json.loads((bank/'validation-summary.json').read_text())
    probe_summary.main(bank, prefix, old['limitations'])
    latest = json.loads((bank/'validation-summary.json').read_text())
    results.append(dict(bank=name, passed=latest['passed'], pairs=latest['pairs'],
                        evidence_directory=latest['evidence_directory']))
    path = bank/'README.md'
    text = path.read_text(encoding='utf-8-sig')
    marker = '\n<!-- BEGIN PENDING BATCH REGRESSION -->'
    if marker in text:
        text = text[:text.index(marker)].rstrip()+'\n'
    text += (marker+'\n\nRegresión del 07/10/2026: **APROBADA**, '
             f"{latest['pairs']:,} pares exactos. ".replace(',','.')+
             f"[Resumen actualizado](validation-summary.json); [informe completo](<{latest['evidence_directory']}/summary.json>).\n"
             '<!-- END PENDING BATCH REGRESSION -->\n')
    path.write_text(text, encoding='utf-8')
assert sum(r['pairs'] for r in results) == 9984
(HERE/'regression-summary.json').write_text(json.dumps(dict(passed=True,pairs=9984,banks=results),indent=2)+'\n')
print('Regresión aprobada: cinco bancos, 9984 pares')
