"""Deriva una copia nueva del banco histórico; no modifica el original."""
from pathlib import Path
HERE=Path(__file__).resolve().parent
source=(HERE.parent/'GameIdle/LiveComparison.hpp').read_text(encoding='utf-8')
source=source.replace('namespace GameIdleLive {','namespace HookUnlockGamePairs {')
source=source.replace('gameidle-runtime','hookunlockgame-runtime').replace('gameidle-only','hookunlockgame-only')
source=source.replace('InstallFailureCapture("gameidle")','InstallFailureCapture("hookunlockgame")')
source=source.replace('inline void Update() {','inline void Run() {')
source=source.replace('passed ? "complete":"error"','passed ? "idle_pairs_complete":"error"')
source=source.replace('{"original",original["trace"].size()},{"candidate",candidate["trace"].size()}','{"original",original},{"candidate",candidate}')
source=source.replace('{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}','{"original",original},{"candidate",candidate}')
(HERE/'IdlePairs.hpp').write_text(source,encoding='utf-8')
