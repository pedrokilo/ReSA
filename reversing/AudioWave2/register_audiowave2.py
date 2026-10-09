"""Registro historico de 54 funciones de CAEScriptAudioEntity (8), CAEPoliceScannerAudioEntity (13), CAEPedAudioEntity (15) y CAEStreamingChannel (18) via el banco simple AudioWave2. Re-audita la corrida aprobada, conserva la evidencia en runs/ y anade 54 entradas (dependency_review=null)."""
import csv,gzip,hashlib,json,os,shutil,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN_ID='20261009-094655-013393'
SRC=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-audiowave2')/RUN_ID
BASELINE=Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')
RUN=HERE/'runs'/RUN_ID
def sha(p):return hashlib.sha256(Path(p).read_bytes()).hexdigest()
def rel(p):return Path(p).relative_to(ROOT).as_posix()
summary=json.loads((SRC/'summary.json').read_text(encoding='utf-8'))
PAIRS=summary['pairs_per_function'];TOTAL=18496;NFUN=54
assert sum(PAIRS.values())==TOTAL and len(PAIRS)==NFUN
EXPECT={'CAEScriptAudioEntity':8,'CAEPoliceScannerAudioEntity':13,'CAEPedAudioEntity':15,'CAEStreamingChannel':18}
for c,n in EXPECT.items():assert sum(1 for k in PAIRS if k.split('::')[0]==c)==n,c
assert len({k.split('::')[0] for k in PAIRS})==4
# Mapping: class + name against docs/hooks.csv (must be unique and still pending); virtual functions have address 0.
rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
ADDR={};TYPE={}
for name in PAIRS:
    cls,fn=name.split('::');m=[r for r in rows if r['class']==cls and r['fn_name']==fn]
    assert len(m)==1,(name,len(m));assert m[0]['validated']=='0',(name,'already validated');ADDR[name]=m[0]['address'];TYPE[name]=m[0]['type']
    assert (TYPE[name]=='V')==(ADDR[name]=='0x00000000'),name
assert summary['passed'] is True and summary['pairs']==TOTAL and summary['all_equal'] is True and summary['state_restored'] is True and summary['saves_unchanged'] is True and summary['configs_restored'] is True
assert summary['executed_probe_sha256'].startswith('63570679') and summary['original_exe_sha256'].startswith('72ae59e4')
log_sha=sha(SRC/'comparisons.jsonl');assert log_sha==summary['log_sha256']
events={};per={};unequal=0;cases=set();complete=None;begin=None
with open(SRC/'comparisons.jsonl',encoding='utf-8') as stream:
    for line in stream:
        row=json.loads(line);ev=row.get('event');events[ev]=events.get(ev,0)+1
        if ev=='comparison':
            per[row['function']]=per.get(row['function'],0)+1;cases.add((row['function'],row['case']))
            if row.get('equal') is not True or row.get('accepted') is not True:unequal+=1
        elif ev=='function_summary':assert row['failures']==0 and row['cases']==PAIRS[row['function']],row
        elif ev=='complete':complete=row
        elif ev=='begin':begin=row
        else:assert ev=='waiting_for_player',ev
print('events',events)
assert events.get('comparison')==TOTAL and events.get('complete')==1 and events.get('begin')==1 and events.get('function_summary')==NFUN and unequal==0 and len(cases)==TOTAL and per==PAIRS
assert complete['pairs']==TOTAL and complete['state_restored'] is True and begin['known'] is True and all(x['ok'] is True for x in begin['sites']) and complete['failures']==0
manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
for p,h in manifest['sources'].items():assert sha(ROOT/p)==h,p
assert sha(ROOT/'source/game_sa/Game.cpp')==manifest['game_cpp']
for name,h in manifest['instrumentation'].items():assert sha(HERE/name)==h,name
assert sha(HERE/'artifacts/gta_reversed-audiowave2-probe.asi')==manifest['binaries']['asi']==summary['executed_probe_sha256']
pdb=BASELINE/'gta_reversed-audiowave2-probe.pdb';assert sha(pdb)==manifest['binaries']['pdb']
assert not RUN.exists();RUN.mkdir(parents=True)
shutil.copy2(SRC/'summary.json',RUN/'summary.json');shutil.copy2(SRC/'RESULTADO.txt',RUN/'RESULTADO.txt')
raw=(SRC/'comparisons.jsonl').read_bytes()
with open(RUN/'comparisons.jsonl.gz','wb') as out:
    with gzip.GzipFile(filename='',mode='wb',fileobj=out,compresslevel=9,mtime=0) as z:z.write(raw)
digest=hashlib.sha256()
with gzip.open(RUN/'comparisons.jsonl.gz','rb') as stream:
    for block in iter(lambda:stream.read(1<<20),b''):digest.update(block)
assert digest.hexdigest()==log_sha
assert sha(RUN/'summary.json')==sha(SRC/'summary.json') and sha(RUN/'RESULTADO.txt')==sha(SRC/'RESULTADO.txt')
storage=dict(uncompressed_sha256=log_sha,uncompressed_bytes=len(raw),compressed_path=rel(RUN/'comparisons.jsonl.gz'),compressed_sha256=sha(RUN/'comparisons.jsonl.gz'),compression='gzip',gzip_mtime=0,lossless_roundtrip_sha256=digest.hexdigest(),original_location=str(SRC/'comparisons.jsonl'))
COMMON=('Simple pair bench inside the full game with a loaded save (original with the class hooks off vs candidate with the hooks on), 18496 isolated CPU pairs for 54 functions of CAEScriptAudioEntity (8), CAEPoliceScannerAudioEntity (13), CAEPedAudioEntity (15) and CAEStreamingChannel (18), zero differences, natural completion event with state_restored, saves unchanged, configs restored. '
 'Each case rebuilds the same deterministic fixture in both legs (fake entities, fake CAEStreamingChannel of 0x60098 bytes, fake ped/physical/matrix, 8 fake CAESound, fake decoders and FAKE DirectSound/FXParamEq COM objects with own vtables; no audio singleton is touched) and compares: return value, the hash of the object under test, the hash of the physical/ped/sounds/destination buffer, the hash of the 0x60 bytes of scanner globals (0xB61CF8), the full trace of collaborator calls with arguments, the x87 control word (24-bit precision) and the restored globals (volume table 0xBD00F8 replaced by a fake and restored). '
 'Traced stubs, which get NO credit: IsSoundBankLoaded, LoadSoundBank, CAESoundManager::{CancelSoundsInBankSlot, AreSoundsPlayingInBankSlot, AreSoundsOfThisEventPlayingForThisEntity(AndPhysical), CancelSoundsOwnedByAudioEntity, RequestNewSound}, CAESound::{RegisterWithPhysicalEntity, StopSoundAndForget, SetPosition}, GetRandomNumberInRange<int>, FindPlayerPed, ReportFrontendAudioEvent, IsLoadingTuneActive, CAESmoothFadeThread::{RequestFade, CancelFade}, CAEAudioChannel::{SetFrequency, SetOriginalFrequency}, CAEAudioUtility::GetCurrentTimeInMS, CAETwinLoopSoundEntity::{Initialise, PlayTwinLoopSound, UpdateTwinLoopSound, StopSoundAndForget} and, only where the tested function calls them, ProcessMissionAudioEvent, PlayResidentSoundEvent, PlayLoadedDialogue and the Handle* of CAEPedAudioEntity. All of it on FAKE DirectSound COM; the real streaming thread is parked in IsForegroundApp during the whole bench. '
 'Limits: constructors/destructors (Script::Constructor, Scanner::Constructor/Destructor, Streaming::Destructor) mask the vptr and the CAEAudioEntity base part (m_Entity, m_tempSound), because the EXE writes only +0x10 of m_tempSound while the DLL zeroes all of it through the shared base class; the rest is compared byte by byte. '
 'The SoundDigest of sounds passed to RequestNewSound masks padding/stack garbage (+0x55, +0x40..0x4F, +0x5A..+0x5B, +0x6A..+0x6B, +0x72..+0x73). '
 'CAEStreamingChannel::InitialiseSilence with a failing CreateSoundBuffer is NOT tested (the DLL writes NOTSA_LOG_WARN, the EXE does nothing). CoInitialize (IAT 0x858350) is stubbed only during Streaming::Initialise. '
 'CAEStreamingChannel::Destructor is tested with m_pDirectSoundBuffer always non-null (the EXE does not check for null). '
 'AddFX: the eqFail fixture only fails the 2nd iteration (if the 1st failed the EXE would use an uninitialised local). '
 'Domains: sampleId 0..3, m_pPed non-null except in AddAudioEvent, non-zero channel frequencies, FillBuffer with a decoder that always advances, finite floats (no NaN/sNaN), surfaces 0..179 read from the real g_surfaceInfos (read only). '
 'Only the own part of each function is validated. '
 'Source corrected to what the EXE does before the approved run (uncommitted at the time; see reversing/AudioWave2/README.md for addresses): see source_corrections_before_run. '
 'Historical evidence approved with explicit limits; no current dependency accreditation.')
SPEC_TXT={
 'CAEScriptAudioEntity::Reset':'source corrected: 0x4EC168 clears m_Physical (+110h), not m_Entity (+4).',
 'CAEScriptAudioEntity::Constructor':'source corrected: 0x507531 sets m_Physical = nullptr; vptr and CAEAudioEntity base part masked.',
 'CAEScriptAudioEntity::PlayResidentSoundEvent':'source corrected: uses the member m_tempSound (CAESound::Initialise flags 0, playTime 0, flags/SetFlags, RegisterWithPhysicalEntity, RequestNewSound), event truncated to word for the volume table.',
 'CAEScriptAudioEntity::GetMissionAudioEvent':'source corrected: unchecked index as the EXE (no std::array bounds abort).',
 'CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue':'source corrected: next time = clock + [8C8140]; the five s_SlotState = 1.',
 'CAEPoliceScannerAudioEntity::PrepSlots':'source corrected: does nothing without s_pCurrentSlots.',
 'CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue':'source corrected: single clock read and delays from [8C8144]/[8C8148].',
 'CAEPoliceScannerAudioEntity::UpdateParameters':'source corrected: reads [8C8150] and [8C814C] instead of literals; virtual (vtable 0x85F368/0).',
 'CAEPoliceScannerAudioEntity::Constructor':'vptr and CAEAudioEntity base part masked.',
 'CAEPoliceScannerAudioEntity::Destructor':'vptr and CAEAudioEntity base part masked.',
 'CAEPedAudioEntity::HandleFootstepEvent':'source corrected: volume order and random call order, non-player ped bank 0x29 and -3.0, water playTime 0x32, AreSoundsPlayingInBankSlot compared as 1 or 2, m_tempSound.',
 'CAEPedAudioEntity::HandleSkateEvent':'source corrected: (table+volume)+rand order, variance 0x3D70D845, m_tempSound, flags 0x80.',
 'CAEPedAudioEntity::HandleLandingEvent':'source corrected: m_tempSound, flags 0xA0, variance 0x3D70D845, water volume <=0 or NaN -> 0.',
 'CAEPedAudioEntity::HandleSwimSplash':'source corrected: id wraps to 0 after 4, flags 0 + SetFlags(80h,1), variance 0x3D70D845, m_tempSound.',
 'CAEPedAudioEntity::HandleSwimWake':'source corrected: event-playing check before the bank check, m_tempSound with speed 0.75.',
 'CAEPedAudioEntity::AddAudioEvent':'source corrected: CRUNCH branch leaves m_ClientVariable = -1 (CAESound::Initialise) and SpeedVariance is the literal 0x3D75C290 in CRUNCH and KNOCK_DOWN; the bench runs it with x87 at 24 bits (fild/fadd/fstp at 0x4E2CDA..0x4E2CFB).',
 'CAEPedAudioEntity::TurnOnJetPack':'source corrected: initial values read from 0x8AE5F0/0x8AE600/0x8AE610/0x8AE618, sounds through m_tempSound.',
 'CAEPedAudioEntity::UpdateJetPack':'source corrected: full x87 fsin, fmul by [B613B0], [8AE620], [B61384]; NaN thrustFwd goes to the <=0.5 branch.',
 'CAEStreamingChannel::FillBuffer':'source corrected: m_nStreamPlayTimeMs from decoder vtable +0Ch (GetStreamPlayTimeMs), null decoder check; decoder always advances.',
 'CAEStreamingChannel::UpdatePlayTime':'source corrected: freq * [859CE0], half buffer from cursor/0x60000, four cases by m_lastWrittenSlot, float subtraction of upperLimit.',
 'CAEStreamingChannel::SetReady':'source corrected: two consecutive ifs (-5 then -6).',
 'CAEStreamingChannel::Pause':'source corrected: IsBufferPlaying check added.',
 'CAEStreamingChannel::SetFrequencyScalingFactor':'source corrected: RequestFade(...,35,0) and m_nState = Started also without buffer; virtual (slot 8).',
 'CAEStreamingChannel::AddFX':'source corrected: SetFX receives a non-null results pointer and a single local fxParamEq shared outside the loop; eqFail only in the 2nd iteration.',
 'CAEStreamingChannel::Initialise':'source corrected: returns bool (bl, 1 if CreateSoundBuffer succeeded); CoInitialize stubbed only during this function; the first CreateSoundBuffer fails 1 in 5, the silence one always succeeds.',
 'CAEStreamingChannel::InitialiseSilence':'failing CreateSoundBuffer NOT tested (NOTSA_LOG_WARN in the DLL).',
 'CAEStreamingChannel::Destructor':'m_pDirectSoundBuffer always non-null; vptr and base part masked.',
 'CAEStreamingChannel::Stop-virtual':'virtual (vtable 0x85F3F0/7).',
 'CAEStreamingChannel::IsSoundPlaying':'virtual (slot 2).',
 'CAEStreamingChannel::Play':'virtual (slot 5).'}
SPECIFIC={n:'Corpus %d; '%PAIRS[n]+SPEC_TXT.get(n,'compared against the EXE as is (no source change).') for n in PAIRS}
SOURCE_CHANGES={'source/game_sa/Audio/Entities/AEScriptAudioEntity.cpp':'Reset (m_Physical), Constructor (m_Physical = nullptr), PlayResidentSoundEvent (m_tempSound), GetMissionAudioEvent (unchecked index)',
 'source/game_sa/Audio/Entities/AEPoliceScannerAudioEntity.cpp':'FinishedPlayingScannerDialogue, PrepSlots, PlayPoliceScannerDialogue, UpdateParameters',
 'source/game_sa/Audio/Entities/AEPedAudioEntity.cpp':'HandleFootstepEvent, HandleSkateEvent, HandleLandingEvent, HandleSwimSplash, HandleSwimWake, AddAudioEvent (CRUNCH m_ClientVariable = -1; SpeedVariance literal 0x3D75C290 in CRUNCH/KNOCK_DOWN), TurnOnJetPack, UpdateJetPack',
 'source/game_sa/Audio/Hardware/AEStreamingChannel.cpp (+ .h)':'FillBuffer (vtable +0Ch), UpdatePlayTime, SetReady, Pause, SetFrequencyScalingFactor, AddFX (non-null results pointer; single local fxParamEq outside the loop), Initialise (returns bool)'}
bank=dict(head_informational=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),exe_sha256=summary['original_exe_sha256'],expected_pairs=TOTAL,asi=manifest['binaries']['asi'],pdb=manifest['binaries']['pdb'],pdb_location=str(pdb)+' (not copied; hash verified)',tested_sources=manifest['sources'],game_cpp=manifest['game_cpp'],bank_sources=manifest['instrumentation'],uncommitted_source_changes=SOURCE_CHANGES)
out=dict(passed=True,pairs=TOTAL,differences=0,functions=PAIRS,covered_functions={k:True for k in PAIRS},scope=COMMON,function_scope=SPECIFIC,dependency_review=None,run=rel(RUN),summary_sha256=sha(RUN/'summary.json'),result_txt_sha256=sha(RUN/'RESULTADO.txt'),log_sha256=log_sha,comparison_storage=storage,all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,executed_probe_sha256=summary['executed_probe_sha256'],original_exe_sha256=summary['original_exe_sha256'],manifest=bank,
 reaudit=dict(comparison_events=TOTAL,complete_events=1,begin_events=1,waiting_for_player_events=events.get('waiting_for_player',0),function_summaries=NFUN,unequal=0,distinct_cases=len(cases),per_function=per),
 source_corrections_before_run=SOURCE_CHANGES,
 out_of_bench_not_credited=[],
 readme_discrepancies='reversing/AudioWave2/README.md said "SIN compilar ni ejecutar" (pre-run state at preparation) and described the AddAudioEvent failure of earlier runs (16/1024 until X87 24-bit); both superseded by the approval section added at registration. Earlier failed runs (previous.jsonl and older run folders) are not preserved. The README correction list does not mention AddAudioEvent SpeedVariance 0x3D75C290 (CRUNCH/KNOCK_DOWN) nor the shared local fxParamEq of AddFX; both are recorded here as stated by the requester.')
sp=HERE/'validation-summary.json';assert not sp.exists();sp.write_text(json.dumps(out,indent=2)+'\n',encoding='utf-8')
registry=ROOT/'reversing/validation-registry.json';before=sha(registry);backup=ROOT/'reversing/validation-registry.backup-before-audio-wave2.json'
assert not backup.exists();shutil.copy2(registry,backup);assert sha(backup)==before
data=json.loads(registry.read_text(encoding='utf-8'));n=len(data['functions']);print('registry count read now:',n)
for name,pairs in PAIRS.items():
    cls,fn=name.split('::');hook={'class':cls,'fn_name':fn,'address':ADDR[name],'type':TYPE[name]}
    assert not any(x['hook']['class']==cls and x['hook']['fn_name']==fn for x in data['functions']),name
    data['functions'].append(dict(hook=hook,evidence=dict(path=rel(sp),sha256=sha(sp),passed_path=['covered_functions',name],pairs=pairs,original_exe_sha256=summary['original_exe_sha256']),tested_sources=manifest['sources'],
     source_provenance='Exact compiled AudioWave2 probe (ASI/PDB hashes in build-manifest.json); sources hashed from the probe build manifest; full 18496-row log reaudited offline and preserved gzipped with the original hash. The four audio source files were corrected to the EXE before the run (uncommitted at the time).',
     dependency_review=None,scope=COMMON+' Function-specific: '+SPECIFIC[name],review_reference='reversing/AudioWave2/README.md'))
assert sha(registry)==before,'registry changed concurrently'
temp=registry.with_suffix('.audio-wave2.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==before;os.replace(temp,registry)
print(json.dumps(dict(before_count=n,after_count=len(json.loads(registry.read_text(encoding='utf-8'))['functions']),registry_sha256=sha(registry),backup=rel(backup),backup_sha256=before,summary_sha256=sha(sp)),indent=2))
