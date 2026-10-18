import json,glob,os
R='reversing/CppAudioSixUnified/runs'
def run(bank,notbefore=None):
    ds=sorted(d for d in os.listdir(R) if d.startswith(bank+'-'))
    return ds[-1]
def ref(bank):
    d=run(bank); return d.split('-',1)[1], f'{R}\{d}/summary.json'.replace('/','/',1)
def pairs(bank):
    return json.load(open(f'{R}/{run(bank)}/summary.json',encoding='utf-8')).get('pairs')
def add(bank): 
    r,s=ref(bank); return {'bank':bank,'run':r,'pairs':pairs(bank),'summary':s}
S='reversing/CppAudioSixStudy/README.md'
a=json.load(open('reversing/CppOnlyMigration/approvals.json',encoding='utf-8'))
def entry(addr,ident,n,src,bank,own,scope,extra=()):
    r,s=ref(bank)
    e={'identity':ident,'converted_source_functions':n,'source':src,'shared_source':'source/game_sa/X87Math.cpp','bank':bank,'run':r,'pairs':pairs(bank),'own_pairs':own,'audit':s,'differences':0,'null_comparison_records':0,'state_restored':True,'helper_codegen':S}
    if extra: e['additional_banks']=[add(b) for b in extra]
    e['scope']=scope+' Revalidación del banco histórico completo sobre la fuente nueva (sin script de auditoría propio: la evidencia es el resumen y el registro de la corrida). Sin cobertura exhaustiva de x87 más allá de los fixtures del banco.'
    a[addr]=e
E='source/game_sa/Audio/'
entry('0x004D80B0','CAEAudioEnvironment::GetPositionRelativeToCamera_Vector',1,E+'AEAudioEnvironment.cpp','Small2',1024,'Aritmética float en el mismo orden que el x87 del EXE (24 bits), sin ensamblador ni ayudante.',['UnlockAudio','Audio14','Audio18','RadioVolumes355'])
entry('0x004EED10','CAESmoothFadeThread::Service (DsVolumeToDb y fundido)',2,E+'AESmoothFadeThread.cpp','Sys10',1024,'DsVolumeToDb con IntTimesFloatCpp y el volumen del fundido con SmoothFadeVolumeCpp (pow del EXE 0x822130, fyl2x).')
entry('0x004DF210','CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire (AddCosMulX87 y AddSinMulX87)',2,E+'Entities/AEGlobalWeaponAudioEntity.cpp','AnimAudio6',1024,'TrigCompoundCosMulAdd y TrigCompoundSinMulAdd. Para que el banco aprobara se añadió la interceptación de la entrada de la DLL de CMemoryMgr::FreeAlign (naked desde d26e3a5d); antes fallaba igual en HEAD (CAnimBlendClumpData, 85/128 y 174/256).')
entry('0x00502670','CAEVorbisDecoder::SetCursor (y SecondsToMs de GetStreamLengthMs/GetStreamPlayTimeMs)',2,E+'Loaders/AEVorbisDecoder.cpp','Audio13',512,'SetCursor con UnsignedTimesFloatCpp(pos, 0.001f) y SecondsToMsCpp.')
entry('0x005033C0','CAEWaveDecoder::SetCursor (y BytesToMs de GetStreamLengthMs/GetStreamPlayTimeMs)',2,E+'Loaders/AEWaveDecoder.cpp','Core9',1024,'WaveCursorBytesCpp con float_control(precise) (la primera versión, con /fp:fast, reasociaba (pos·0,001)·tasa y Core9 dio 206/1024 diferencias en SetCursor) y BytesToMsCpp.')
entry('0x004D6E60','CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume',1,E+'Managers/AEAmbienceTrackManager.cpp','AudioCompletion',7846,'Volumen de tormenta de arena con AudioLog10Times20Cpp(x) - 6 y atenuación bajo el agua con sqrt(d·d + dz·dz)·0,5714286f en float.',['AmbienceTrack'])
json.dump(a,open('reversing/CppOnlyMigration/approvals.json','w',encoding='utf-8',newline=''),ensure_ascii=False,indent=2)
