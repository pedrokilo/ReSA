import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'CInformFriendsEventQueue::Init': 256, 'CInformFriendsEventQueue::Flush': 256, 'CInformFriendsEventQueue::Add': 1024,
            'CInformFriendsEventQueue::Process': 1024, 'CInformGroupEventQueue::Init': 256, 'CInformGroupEventQueue::Flush': 256,
            'CInformGroupEventQueue::Add': 1024, 'CInformGroupEventQueue::Process': 1024, 'CInformGroupEvent::Constructor': 64,
            'CInformGroupEvent::Destructor': 256, 'CInformGroupEvent::Process': 1024}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'inform', 'Colas de eventos de grupo (CInformFriendsEventQueue, CInformGroupEvent, CInformGroupEventQueue)', EXPECTED))
