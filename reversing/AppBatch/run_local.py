import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'App::AppEventHandler': 512, 'App::DefinedState': 64, 'App::DefinedState2d': 64, 'App::DoRWStuffEndOfFrame': 64, 'App::DoRWStuffStartOfFrame': 512, 'App::DoRWStuffStartOfFrame_Horizon': 512, 'App::PluginAttach': 256, 'CIniFile::LoadIniFile': 512, 'CIplDefPool::New': 512}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'app', 'App, CIniFile y CIplDefPool', EXPECTED))
