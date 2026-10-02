import re, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
src = (Path(__file__).parent/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'BreakObject_c::{m[0]}': int(m[1]) for m in re.findall(r'\{"(\w+)",0x[0-9A-F]+,(\d+)\}', src)}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'breakobj', 'BreakObject_c (9)', EXPECTED, timeout=600))
