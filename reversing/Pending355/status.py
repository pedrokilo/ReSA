"""Inventario de solo lectura: separa banco preparado, aprobado y fuente coincidente."""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent))
from runtime_runner import GAME, digest

def inventory():
    scope = json.loads((HERE / 'scope.json').read_text(encoding='utf-8'))
    expected = {(r['class'], r['fn_name']) for r in scope['functions']}
    banks = []
    covered = set()
    approved = set()
    matching = set()
    for folder in sorted(HERE.parent.iterdir()):
        if not (folder.name.endswith('355') or folder.name == 'PedDamagePending'):
            continue
        header, builder = folder / 'LiveComparison.hpp', folder / 'build_probe.py'
        if not header.is_file() or not builder.is_file():
            continue
        content = header.read_text(encoding='utf-8-sig')
        keys = set(re.findall(r'\{\s*"(\w+)"\s*,\s*"([^"\n]+)"\s*,\s*(?:0x[\da-fA-F]+|0)\s*,', content)) & expected
        if not keys:
            continue
        prefix_match = re.search(r'build\([^,]+,\s*[\'"]([^\'"]+)', builder.read_text(encoding='utf-8-sig'))
        if not prefix_match:
            continue
        prefix = prefix_match[1]
        reports = sorted((GAME / f'resultados-{prefix}').glob('*/summary.json'))
        report = json.loads(reports[-1].read_text(encoding='utf-8')) if reports else None
        state = 'prepared'
        covered |= keys
        if report and report.get('passed'):
            state = 'approved_historical'
            approved |= keys
            manifest = folder / 'artifacts/build-manifest.json'
            if manifest.is_file():
                data = json.loads(manifest.read_text())
                same = all((ROOT / p).is_file() and digest(ROOT / p) == h for p, h in data['sources'].items())
                same &= reports[-1].stat().st_mtime >= manifest.stat().st_mtime
                same &= all((folder / p).is_file() and digest(folder / p) == h for p, h in data['instrumentation'].items())
                if same:
                    state = 'approved_source_matches'
                    matching |= keys
        elif report:
            state = 'failed_latest'
        banks.append(dict(bank=folder.name, prefix=prefix, functions=len(keys), status=state,
                          report=str(reports[-1].parent) if reports else None))
    return dict(scope=len(expected), prepared=len(covered), approved_historical=len(approved),
                approved_source_matches=len(matching), missing=sorted(expected - covered), banks=banks)

if __name__ == '__main__':
    result = inventory()
    (HERE / 'progress.json').write_text(json.dumps(result, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
    print(json.dumps({k: v for k, v in result.items() if k not in ('missing', 'banks')}))
