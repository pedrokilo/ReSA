import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import main
EXPECTED = {'CBirds::Render': 512}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'cpptrigbirdsrender', 'CppTrigBirdsRender', EXPECTED, timeout=1500))
