from pathlib import Path
import re
bodies={
('Audio/Entities/AEScriptAudioEntity.cpp','PreloadMissionAudio'):'''void CAEScriptAudioEntity::PreloadMissionAudio(uint8 slotId, int32 sampleId) {
    if (slotId >= MISSION_AUDIO_COUNT || !IsMissionAudioSampleFinished(slotId)) return;
    auto& link = wavLinks[slotId];
    auto event = static_cast<eAudioEvents>(sampleId);
    if (!CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent(event, reinterpret_cast<eSoundBankS32&>(link.m_nBankId), link.m_nBankSlotId, slotId)) return;
    const auto bank = static_cast<eSoundBank>(uint16(link.m_nBankId));
    const auto slot = static_cast<eSoundBankSlot>(slotId + 26);
    if (link.m_nBankSlotId < 0) AEAudioHardware.LoadSoundBank(bank, slot);
    else AEAudioHardware.LoadSound(bank, static_cast<eSoundID>(uint16(link.m_nBankSlotId)), slot);
    link.m_nAudioEvent = int32(event);
    link.m_pEntity = nullptr;
    link.m_vPosition = CVector{-1000.f, -1000.f, -1000.f};
}''',
('Entity/Vehicle/Plane.cpp','SwitchAmbientPlanes'):'''void CPlane::SwitchAmbientPlanes(bool enable) {
    if (GenPlane_Active && !enable) {
        auto* pool = GetVehiclePool();
        for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
            auto* vehicle = pool->GetAt(i);
            if (vehicle && (vehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE || vehicle->m_nVehicleSubType == VEHICLE_TYPE_HELI) && vehicle->m_nCreatedBy == RANDOM_VEHICLE) {
                CWorld::Remove(vehicle);
                delete vehicle;
            }
        }
    }
    GenPlane_Active = enable;
}''',
('Entity/Vehicle/Train.cpp','RemoveAllTrains'):'''void CTrain::RemoveAllTrains() {
    auto* pool = GetVehiclePool();
    for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
        auto* vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsTrain()) continue;
        auto* train = vehicle->AsTrain();
        bool playerInside{};
        for (auto* carriage = train; carriage; carriage = carriage->m_pPrevCarriage) {
            if (carriage == FindPlayerVehicle(-1, false)) playerInside = true;
        }
        for (auto* carriage = train; carriage; carriage = carriage->m_pNextCarriage) {
            if (carriage == FindPlayerVehicle(-1, false)) playerInside = true;
        }
        if (!playerInside) {
            CWorld::Remove(train);
            delete train;
        }
    }
}''',
('Entity/Vehicle/Train.cpp','FindNearestTrain'):'''CTrain* CTrain::FindNearestTrain(CVector posn, bool mustBeMainTrain) {
    auto* pool = GetVehiclePool();
    auto bestDistance = 10000000.f;
    CTrain* best{};
    for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
        auto* vehicle = pool->GetAt(i);
        if (!vehicle || !vehicle->IsTrain()) continue;
        auto* train = vehicle->AsTrain();
        const auto dx = train->GetPosition().x - posn.x;
        const auto dy = train->GetPosition().y - posn.y;
        const auto distance = std::sqrt(dy * dy + dx * dx);
        if (distance < bestDistance && (!mustBeMainTrain || train->trainFlags.bIsFrontCarriage)) {
            bestDistance = distance;
            best = train;
        }
    }
    return best;
}''',
('Entity/Vehicle/Train.cpp','FindCoorsFromPositionOnTrack'):'''void CTrain::FindCoorsFromPositionOnTrack(float railDistance, int32 trackId, CVector* outCoors) {
    const auto* nodes = pTrackNodes[trackId];
    const auto count = NumTrackNodes[trackId];
    for (int32 i = 0; i < count; ++i) {
        const auto next = (i + 1) % count;
        const auto before = railDistance - float(nodes[i].m_nDistanceFromStart) * StaticRef<float>(0x859040);
        const auto after = float(nodes[next].m_nDistanceFromStart) * StaticRef<float>(0x859040) - railDistance;
        if (before >= 0.f && after >= 0.f) {
            const auto inverse = 1.f / (after + before);
            outCoors->x = (float(nodes[i].x) * .125f * after + float(nodes[next].x) * .125f * before) * inverse;
            outCoors->y = (float(nodes[i].y) * .125f * after + float(nodes[next].y) * .125f * before) * inverse;
            outCoors->z = (float(nodes[i].z) * .125f * after + float(nodes[next].z) * .125f * before) * inverse;
            return;
        }
    }
}'''
}
for (rel,fn),body in bodies.items():
 p=Path('source/game_sa')/rel;s=p.read_text();m=re.search(r'[^\n]*::'+fn+r'\([^\n]*\)\s*\{',s);assert m
 a=s.index('{',m.start());b=a+1;d=1
 while d:d+=(s[b]=='{')-(s[b]=='}');b+=1
 p.write_text(s[:m.start()]+body+s[b:]);print(fn)
