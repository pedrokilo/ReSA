import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
EXPECTED = {'CAEAudioChannel::ConvertFromBytesToMS': 512, 'CAEAudioChannel::ConvertFromMsToBytes': 512, 'CAEAudioChannel::GetCurrentPlaybackPosition': 256, 'CAEAudioChannel::Lost': 64, 'CAEAudioChannel::SetFrequency': 256, 'CAEAudioChannel::SetFrequencyScalingFactor': 1024, 'CAEAudioChannel::SetOriginalFrequency': 256, 'CAEAudioChannel::SetPosition': 256, 'CAEAudioChannel::SetVolume': 1024, 'CAEAudioChannel::UpdateStatus': 256, 'CAEAudioChannel::SetReverbAndDepth': 512, 'CAEAudioChannel::SetNotInRoom': 256}
if __name__ == '__main__': sys.exit(main(Path(__file__).parent, 'channel', 'CAEAudioChannel', EXPECTED))
