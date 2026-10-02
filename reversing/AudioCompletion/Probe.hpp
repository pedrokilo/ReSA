#pragma once
#include "AEAmbienceTrackManager.h"
#include "AEStaticChannel.h"
#include "AEWeatherAudioEntity.h"
#include "AEAudioHardware.h"
#include "AudioZones.h"
#include "../ProbeSupport.hpp"

namespace AudioCompletionProbe {
inline bool active{};
inline uint32 ambienceCalls{}, staticCalls{}, weatherCalls{};
inline json trace;
struct Spec {
    std::string name;
    int function{}, area{}, zone{-1}, zoneKind{}, previousRadio{-1}, station{3}, favorite{5};
    bool playerNoRain{}, cameraNoRain{}, radio{}, vehicleRadio{}, riots{}, loaded{true}, requestSucceeds{true};
    int status{8}, special{-1}, track{174};
    bool lastOverride{}, overrideRadio{}, startAtBeginning{};
    float sandstorm{}, underwater{}, volume{-20.f}, frequency{1.f}, cameraZ{10.f}, waterZ{}, waterDistance{10.f}, attenuation{-50.f};
    CVector spherePosition{12.f, 24.f, 48.f}, relativePosition{3.f, 4.f, 0.f};
    uint32 size{8000}, channels{19}, lightning{10}, convertResult{123};
    uint16 sampleFrequency{22050};
    int16 loop{-1}, f88{-21}, f8c{-17};
    int failure{}, oldBuffers{3}, event{141}, counter{}, defaultVolume{-3};
    bool hardware{};
};
inline Spec spec;
inline CAEAmbienceTrackManager* ambience{};
inline CAEWeatherAudioEntity* weather{};
inline CAEStaticChannel* channel{};
inline uint32 Bits(float value) { return std::bit_cast<uint32>(value); }
inline json Vector(const CVector& p) { return {Bits(p.x),Bits(p.y),Bits(p.z)}; }
inline void Record(const char* name,json args=json::array()) { trace.push_back({name,args}); }
inline bool PlayerNoRain() { Record("player-no-rain"); return spec.playerNoRain; }
inline bool CamNoRain() { Record("camera-no-rain"); return spec.cameraNoRain; }
inline bool RadioOn() { Record("radio-on"); return spec.radio; }
inline bool VehicleRadio() { Record("vehicle-radio"); return spec.vehicleRadio; }
inline bool Riots() { Record("riots"); return spec.riots; }
inline eRadioID Station() { Record("station"); return eRadioID(spec.station); }
inline eRadioID Favorite() { Record("favorite"); return eRadioID(spec.favorite); }
inline void Start(eRadioID station,eBassSetting bass) { Record("start-radio",{int(station),int(bass)}); }
inline void Stop(tVehicleAudioSettings* settings,bool pause) { Record("stop-radio",{settings!=nullptr,pause}); }
inline void Frontend(eAudioEvents event,float volume=0.f,float speed=1.f) { Record("frontend",{int(event),Bits(volume),Bits(speed)}); }
inline float Attenuation(float distance) { Record("attenuation",{Bits(distance)}); return spec.attenuation; }
inline CVector Relative(const CVector& pos) { Record("relative",Vector(pos)); return spec.relativePosition; }
inline void RelativeABI(CVector* out,const CVector* pos) { *out=Relative(*pos); }
inline void StopAmbience(CAEAmbienceTrackManager* manager) {
    Record("stop-ambience",{int(manager->m_AmbienceStatus),manager->m_RequestedSettings.TrackID,Bits(manager->m_Volume),manager->m_LastAmbienceOverrodeRadio});
    if(uint32(manager->m_AmbienceStatus)<=3)manager->m_AmbienceStatus=CAEAmbienceTrackManager::STOPPING;
    manager->m_OverrideRadio=false;manager->m_LastAmbienceOverrodeRadio=false;manager->m_StartAmbienceAtBeginning=false;
}
inline bool Loaded(eSoundBank bank,eSoundBankSlot slot) { Record("loaded",{int(bank),int(slot)});return spec.loaded; }
inline void Load(eSoundBank bank,eSoundBankSlot slot) { Record("load",{int(bank),int(slot)}); }
inline void Initialise(CAESound* sound,eSoundBankSlot bank,eSoundID id,CAEAudioEntity* entity,CVector pos,float volume,float rolloff=1.f,float speed=1.f,float doppler=1.f,uint8 delay=0,uint16 flags=0,float variance=0.f,int16 time=0) {
    Record("initialise",{int(bank),int(id),entity==weather,Vector(pos),Bits(volume),Bits(rolloff),Bits(speed),Bits(doppler),delay,flags,Bits(variance),time});
    std::memset(sound,0,sizeof(*sound));sound->m_Event=-1;
}
inline CAESound* Request(CAESound* sound) { Record("request",{sound->m_Event});return spec.requestSucceeds ? sound : nullptr; }
inline uint32 BytesToMS(CAEAudioChannel* p,uint32 bytes) { Record("bytes-to-ms",{bytes,p->m_WaveFormat.nSamplesPerSec,p->m_WaveFormat.nChannels});return spec.convertResult; }
inline bool __fastcall RadioOnABI(void*,void*) { return RadioOn(); }
inline bool __fastcall VehicleRadioABI(void*,void*) { return VehicleRadio(); }
inline eRadioID __fastcall StationABI(void*,void*) { return Station(); }
inline void __fastcall StartABI(void*,void*,eRadioID station,eBassSetting bass) { Start(station,bass); }
inline void __fastcall StopABI(void*,void*,tVehicleAudioSettings* settings,bool pause) { Stop(settings,pause); }
inline void __fastcall FrontendABI(void*,void*,eAudioEvents event,float volume,float speed) { Frontend(event,volume,speed); }
inline void __fastcall StopAmbienceABI(CAEAmbienceTrackManager* p,void*) { StopAmbience(p); }
inline bool __fastcall LoadedABI(void*,void*,eSoundBank bank,eSoundBankSlot slot) { return Loaded(bank,slot); }
inline void __fastcall LoadABI(void*,void*,eSoundBank bank,eSoundBankSlot slot) { Load(bank,slot); }
inline void __fastcall InitialiseABI(CAESound* sound,void*,eSoundBankSlot bank,eSoundID id,CAEAudioEntity* entity,CVector pos,float volume,float rolloff,float speed,float doppler,uint8 delay,uint16 flags,float variance,int16 time) { Initialise(sound,bank,id,entity,pos,volume,rolloff,speed,doppler,delay,flags,variance,time); }
inline CAESound* __fastcall RequestABI(void*,void*,CAESound* sound) { return Request(sound); }
inline uint32 __fastcall BytesToMSABI(CAEAudioChannel* p,void*,uint32 bytes) { return BytesToMS(p,bytes); }

struct ComObject { void** table{};int id{}; };
inline std::array<void*,32> bufferTable{},deviceTable{};
inline ComObject* newBuffer{};
inline ComObject* new3D{};
inline std::vector<uint8>* output{};
inline ULONG __stdcall Release(ComObject* object) { Record("release",{object->id,g_numSoundChannelsUsed});return 0; }
inline HRESULT __stdcall Create(ComObject* object,const DSBUFFERDESC* desc,IDirectSoundBuffer** result,IUnknown* outer) {
    const auto& f=*desc->lpwfxFormat;
    Record("create",{object->id,desc->dwSize,desc->dwFlags,desc->dwBufferBytes,desc->dwReserved,ReversingProbe::Bytes(&desc->guid3DAlgorithm,sizeof(GUID)),outer!=nullptr,
        {f.wFormatTag,f.nChannels,f.nSamplesPerSec,f.nAvgBytesPerSec,f.nBlockAlign,f.wBitsPerSample,f.cbSize},g_numSoundChannelsUsed});
    if(spec.failure==1)return E_FAIL;
    *result=reinterpret_cast<IDirectSoundBuffer*>(newBuffer);output->assign(desc->dwBufferBytes+32,0xCD);return S_OK;
}
inline HRESULT __stdcall Lock(ComObject* object,DWORD offset,DWORD size,void** a,DWORD* bytes,void** b,DWORD* bytes2,DWORD flags) {
    Record("lock",{object->id,offset,size,b!=nullptr,bytes2!=nullptr,flags,g_numSoundChannelsUsed});
    if(spec.failure==2)return E_FAIL;
    *a=output->data();*bytes=uint32(output->size()-32);return S_OK;
}
inline HRESULT __stdcall Unlock(ComObject* object,void* a,DWORD bytes,void* b,DWORD bytes2) {
    Record("unlock",{object->id,a==output->data(),bytes,b!=nullptr,bytes2});return spec.failure==3 ? E_FAIL : S_OK;
}
inline HRESULT __stdcall Position(ComObject* object,DWORD position) { Record("position",{object->id,position});return spec.failure==4 ? E_FAIL : S_OK; }
inline HRESULT __stdcall Query(ComObject* object,REFIID guid,void** result) {
    Record("query",{object->id,ReversingProbe::Bytes(&guid,sizeof(guid))});
    if(spec.failure==5)return E_FAIL;*result=new3D;return S_OK;
}
inline HRESULT __stdcall Volume(ComObject* object,LONG volume) { Record("volume",{object->id,volume});return spec.failure==6 ? E_FAIL : S_OK; }
inline void SetTables() {
    bufferTable[0]=reinterpret_cast<void*>(&Query);bufferTable[2]=reinterpret_cast<void*>(&Release);bufferTable[11]=reinterpret_cast<void*>(&Lock);
    bufferTable[13]=reinterpret_cast<void*>(&Position);bufferTable[15]=reinterpret_cast<void*>(&Volume);bufferTable[19]=reinterpret_cast<void*>(&Unlock);
    deviceTable[3]=reinterpret_cast<void*>(&Create);
}
}
