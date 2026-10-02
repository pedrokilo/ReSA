import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'CLoadMonitor::Constructor': 64, 'CLoadMonitor::Destructor': 32, 'CLoadMonitor::BeginFrame': 128, 'CLoadMonitor::EndFrame': 4096,
            'CLoadMonitor::StartTimer': 256, 'CLoadMonitor::EndTimer': 256, 'CLoadMonitor::Render': 32}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'loadmon', 'CLoadMonitor', EXPECTED))
