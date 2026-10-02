import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'CAEAmbienceTrackManager::Reset': 64, 'CAEAmbienceTrackManager::IsAmbienceTrackActive': 64, 'CAEAmbienceTrackManager::IsAmbienceRadioActive': 64,
            'CAEAmbienceTrackManager::PlaySpecialMissionAmbienceTrack': 64, 'CAEAmbienceTrackManager::StopSpecialMissionAmbienceTrack': 64, 'CAEAmbienceTrackManager::Service': 4096}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'ambience', 'CAEAmbienceTrackManager (6)', EXPECTED))
