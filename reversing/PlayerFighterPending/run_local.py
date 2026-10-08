import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
if __name__ == '__main__':
    sys.exit(main(Path(__file__).parent, 'player-fighter-pending', 'PlayerControlFighter', {'CTaskSimplePlayerOnFoot::PlayerControlFighter':4096}, timeout=1500))
