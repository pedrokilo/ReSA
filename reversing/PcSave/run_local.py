import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'C_PcSave::SetSaveDirectory': 128, 'C_PcSave::GenerateGameFilename': 256, 'C_PcSave::PopulateSlotInfo': 256, 'C_PcSave::SaveSlot': 64, 'C_PcSave::DeleteSlot': 128}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'pcsavebatch', 'C_PcSave', EXPECTED))
