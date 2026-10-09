"""Writes prepared-freeze.json (closed input list + hashes). Run after the last edit of any input; the builder re-checks every hash."""
import hashlib, json
from pathlib import Path
HERE = Path(__file__).resolve().parent; ROOT = HERE.parents[1]
digest = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
bank = ['LiveComparison.hpp', 'Preflight.hpp', 'VTableRanges.hpp', 'Fixtures.hpp', 'fixtures.json', 'generate_fixtures.py', 'model.py', 'build_probe.py', 'run_local.py', 'inspect_compiled.py', 'make_freeze.py',
        'corpus-plan.json', 'scope.json', 'README.md', 'evidence/ProcessPad-fresh-combo.txt', 'evidence/ProcessPad.dumpbin.asm.txt', 'evidence/ProcessPad.bytes.bin', 'evidence/ProcessPad-constants.json']
shared = ['reversing/RequestedParallelSmall/HookSnapshots.hpp', 'reversing/ProbeSupport.hpp', 'reversing/ProbeRuntime.hpp', 'reversing/runtime_runner.py']
inputs = {f'reversing/{HERE.name}/{name}': digest(HERE / name) for name in bank}
inputs.update({name: digest(ROOT / name) for name in shared})
freeze = {'status': 'prepared_frozen_not_compiled_not_executed', 'expected_pairs': 17256,
          'scope': '17256 finite isolated ProcessPad cases; private scripted DirectInput device, traced RsPadEventHandler, native GetPad/FTOL; real global CPad/ControlsManager/stick/joy-table/menu bytes compared with guards; PC24 and PC53; padID 0/1 only. No DirectInput/COM/RsPadEventHandler/GetPad/FTOL/integration credit. Matching PE/PDB and call paths must be reviewed after the coordinator build.',
          'inputs': inputs, 'source_before_sha256': {'source/game_sa/Pad.cpp': digest(ROOT / 'source/game_sa/Pad.cpp'), 'source/game_sa/Pad.h': digest(ROOT / 'source/game_sa/Pad.h')}, 'revision': 1}
(HERE / 'prepared-freeze.json').write_text(json.dumps(freeze, indent=2) + '\n', encoding='utf8', newline='\n'); print('freeze written', len(inputs))
