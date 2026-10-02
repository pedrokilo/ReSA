import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'BreakablePlugin::BreakableConstructor': 256, 'BreakablePlugin::BreakableDestructor': 512, 'BreakablePlugin::BreakablePluginAttach': 64, 'BreakablePlugin::BreakableStreamGetSize': 32, 'BreakablePlugin::BreakableStreamRead': 512, 'BreakablePlugin::BreakableStreamWrite': 512, 'BreakManager_c::Add': 512, 'BreakManager_c::Exit': 32, 'BreakManager_c::Init': 32, 'BreakManager_c::Render': 256, 'BreakManager_c::ResetAll': 256, 'BreakManager_c::Update': 256}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'break', 'BreakablePlugin y BreakManager_c', EXPECTED))
