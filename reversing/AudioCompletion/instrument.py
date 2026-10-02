"""Substitute external providers only; retain all three production algorithm bodies."""
TARGETS = {
    'AEAmbienceTrackManager.cpp': ('void CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume() {', 'ambienceCalls', {
        'CCullZones::PlayerNoRain()': 'AudioCompletionProbe::PlayerNoRain()',
        'CCullZones::CamNoRain()': 'AudioCompletionProbe::CamNoRain()',
        'CStats::FindMostFavoriteRadioStation()': 'AudioCompletionProbe::Favorite()',
        'CGameLogic::LaRiotsActiveHere()': 'AudioCompletionProbe::Riots()',
        'AudioEngine.IsRadioOn()': 'AudioCompletionProbe::RadioOn()',
        'AudioEngine.IsVehicleRadioActive()': 'AudioCompletionProbe::VehicleRadio()',
        'AudioEngine.GetCurrentRadioStationID()': 'AudioCompletionProbe::Station()',
        'AudioEngine.StartRadio(': 'AudioCompletionProbe::Start(',
        'AudioEngine.StopRadio(': 'AudioCompletionProbe::Stop(',
        'AudioEngine.ReportFrontendAudioEvent(': 'AudioCompletionProbe::Frontend(',
        'CAEAudioEnvironment::GetDistanceAttenuation(': 'AudioCompletionProbe::Attenuation(',
        'CAEAudioEnvironment::GetPositionRelativeToCamera(': 'AudioCompletionProbe::Relative(',
        'StopAmbienceTrack()': 'AudioCompletionProbe::StopAmbience(this)',
    }),
    'AEStaticChannel.cpp': ('bool CAEStaticChannel::SetAudioBuffer(', 'staticCalls', {
        'ConvertFromBytesToMS(': 'AudioCompletionProbe::BytesToMS(this, ',
    }),
    'AEWeatherAudioEntity.cpp': ('void CAEWeatherAudioEntity::AddAudioEvent(eAudioEvents event) {', 'weatherCalls', {
        'CCullZones::PlayerNoRain()': 'AudioCompletionProbe::PlayerNoRain()',
        'CCullZones::CamNoRain()': 'AudioCompletionProbe::CamNoRain()',
        'AEAudioHardware.IsSoundBankLoaded(': 'AudioCompletionProbe::Loaded(',
        'AEAudioHardware.LoadSoundBank(': 'AudioCompletionProbe::Load(',
        'sound.Initialise(': 'AudioCompletionProbe::Initialise(&sound, ',
        'AESoundManager.RequestNewSound(': 'AudioCompletionProbe::Request(',
    }),
}


def instrument(path, text):
    if path.endswith('/Game.cpp'):
        anchor = 'void CGame::Process() {'
        assert text.count(anchor) == 1
        return text.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/AudioCompletion/LiveComparison.hpp"', 1).replace(anchor, anchor + '\n    AudioCompletionLive::Update();')
    name = path.rsplit('/', 1)[-1]
    anchor, counter, replacements = TARGETS[name]
    start = text.index(anchor)
    opening = text.index('{', start)
    depth, end = 1, opening + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    body = text[opening + 1:end - 1]
    for old, new in replacements.items():
        assert old in body, old
        body = body.replace(old, new)
    text = text[:opening + 1] + '\n    ++AudioCompletionProbe::' + counter + ';' + body + text[end - 1:]
    return text.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../../../reversing/AudioCompletion/Probe.hpp"', 1)
