"""Build temporary instrumentation and restore sources even when compilation fails."""
import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

TASK_CPP = ROOT/'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.cpp'
TASK_H = ROOT/'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h'
GAME_CPP = ROOT/'source/game_sa/Game.cpp'

def main():
    paths = [TASK_CPP, TASK_H, GAME_CPP]
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
        # TaskInteriorSitAtDesk.cpp: include Probe.hpp and count calls to ProcessPed.
        content = TASK_CPP.read_text(encoding='utf-8')
        assert content.count('#include "StdInc.h"') == 1
        content = content.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../../../../reversing/CTaskInteriorSitAtDesk/Probe.hpp"')
        anchor = 'bool CTaskInteriorSitAtDesk::ProcessPed(CPed* ped) {'
        assert content.count(anchor) == 1
        content = content.replace(anchor, anchor + '\n    ++SitAtDeskProbe::calls;')
        TASK_CPP.write_text(content, encoding='utf-8')

        # TaskInteriorSitAtDesk.h: expose data members so the probe's independent
        # Fixture can set every branch of the state machine directly, the same way
        # CEntryExitTransition's Fixture reaches private members of the original task.
        header = TASK_H.read_text(encoding='utf-8')
        anchor_h = (
            'private:\n'
            '    int32                  m_Duration;'
        )
        assert header.count(anchor_h) == 1, 'anchor de miembros privados no encontrado'
        header = header.replace(anchor_h, 'public:\n    int32                  m_Duration;')
        TASK_H.write_text(header, encoding='utf-8')

        # Game.cpp: drive the probe from the normal per-frame update, like every
        # other LiveComparison bank in this repository.
        game = GAME_CPP.read_text(encoding='utf-8')
        assert game.count('#include "StdInc.h"') == 1
        game = game.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/CTaskInteriorSitAtDesk/LiveComparison.hpp"')
        anchor_g = 'void CGame::Process() {'
        assert game.count(anchor_g) == 1
        game = game.replace(anchor_g, anchor_g + '\n    SitAtDeskLive::Update();')
        GAME_CPP.write_text(game, encoding='utf-8')

        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE), '--build', str(ROOT/'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
        hashes = {}
        for ext in ('asi', 'pdb'):
            dest = artifacts/f'gta_reversed-sitatdesk-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(
            binaries=hashes,
            sources={str(p.relative_to(ROOT)): hashlib.sha256(data).hexdigest() for p, data in saved.items()},
            instrumentation={p.name: digest(p) for p in (HERE/'Probe.hpp', HERE/'LiveComparison.hpp', HERE/'build_probe.py')},
        )
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest, indent=2))
        print(json.dumps(hashes))
    finally:
        for p, data in saved.items():
            p.write_bytes(data)

if __name__ == '__main__':
    main()
