"""Anexa a cada banco el resultado final del lote, sin alterar sus límites."""
import json
from pathlib import Path

HERE = Path(__file__).resolve().parent
summary = json.loads((HERE/'summary.json').read_text())
assert summary['passed'] and summary['functions'] == 78
for result in summary['results']:
    path = HERE.parent/result['bank']/'README.md'
    text = path.read_text(encoding='utf-8-sig')
    marker = '\n<!-- BEGIN FINAL PENDING BATCH RESULT -->'
    if marker in text:
        text = text[:text.index(marker)].rstrip()+'\n'
    text += (marker+'\n'
             f"\nResultado final: **APROBADO**, {result['pairs']:,} pares exactos. ".replace(',', '.')+
             f"[Resumen y hashes](validation-summary.json); [informe completo](<{result['evidence_directory']}/summary.json>). "
             'Esta aprobación cubre los fixtures y límites descritos arriba.\n'
             '<!-- END FINAL PENDING BATCH RESULT -->\n')
    path.write_text(text, encoding='utf-8')
print('README de 30 bancos sincronizados con resultados finales')
