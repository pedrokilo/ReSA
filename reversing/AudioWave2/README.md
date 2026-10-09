# AudioWave2 (CAEScriptAudioEntity, CAEPoliceScannerAudioEntity, CAEPedAudioEntity, CAEStreamingChannel)

Banco simple (estilo Small2/PadMouse1/AudioFinal1) **preparado el 09/10/2026, SIN compilar ni ejecutar** (pendiente de `bash reversing/_handoff/bench.sh AudioWave2 3`).
54 funciones (las `reversed=1, validated=0` de `docs/hooks.csv` de las cuatro clases), 18 496 pares original (EXE, hook desactivado) / candidato (DLL, hook activado).

| Clase | Funciones (casos) |
|---|---|
| `CAEScriptAudioEntity` (8) | Constructor 128, Initialise 128, Reset 256, GetMissionAudioEvent 128, Service 256, ReportMissionAudioEvent-1 256, ReportMissionAudioEvent-2 256, PlayResidentSoundEvent 1024 |
| `CAEPoliceScannerAudioEntity` (13) | Constructor 128, Destructor 512, StaticInitialise 128, Reset 512, EnableScanner 64, DisableScanner 512, StopScanner 512, FinishedPlayingScannerDialogue 512, PrepSlots 512, PopulateScannerDialogueLists 512, CanWePlayNewScannerDialogue 512, PlayPoliceScannerDialogue 512, UpdateParameters (vtable 0x85F368/0) 512 |
| `CAEPedAudioEntity` (15) | Initialise 128, StaticInitialise 64, Terminate 256, AddAudioEvent 1024, TurnOnJetPack 256, TurnOffJetPack 128, StopJetPackSound 256, UpdateJetPack 1024, PlayWindRush 64, HandleFootstepEvent 1024, HandleSkateEvent 512, HandleLandingEvent 512, HandleSwimSplash 256, HandleSwimWake 256, PlayShirtFlap 256 |
| `CAEStreamingChannel` (18) | AddFX 256, RemoveFX 128, SetBassEQ 512, Initialise 128, InitialiseSilence 64, Destructor 256, Pause 128, Stop-class 128, Stop-virtual (vtable 0x85F3F0/7) 128, SetReady 256, SetNextStream 128, GetActiveTrackID 64, GetPlayingTrackID 128, FillBuffer 512, UpdatePlayTime 1024, IsSoundPlaying (slot 2) 128, Play (slot 5) 128, SetFrequencyScalingFactor (slot 8) 512 |

## Correcciones de la fuente (direccion + instruccion del EXE)
**CAEScriptAudioEntity** (`AEScriptAudioEntity.cpp`)
- `Reset` 0x4EC150: `mov dword ptr [edi+110h],0` (0x4EC168) es `m_Physical`; la fuente anulaba `m_Entity` (+4). Corregido.
- `Constructor` 0x5074D0: `mov dword ptr [eax+110h],ecx` (0x507531) pone `m_Physical = nullptr`; faltaba.
- `GetMissionAudioEvent` 0x4EC020: `movzx/shl 5` sin comprobar rango; `std::array::operator[]` abortaba en debug, ahora `.data()[]`.
- `PlayResidentSoundEvent` 0x4EC550: el EXE usa el miembro `m_tempSound` (`lea esi,[edi+8]`, 0x4EC65E): `CAESound::Initialise(...,flags 0, playTime 0)` (0x4EFE50), `mov word [edi+64h],playPosn`, `mov word [edi+5Eh],26h`, `SetFlags(1,frontend)`, si hay fisico `SetFlags(80h,1)` + `RegisterWithPhysicalEntity`, `mov [edi+14h],evento(movzx)` y `RequestNewSound(&m_tempSound)`. La fuente usaba `PlaySound` con un CAESound local (otro `m_ClientVariable`, `m_tempSound` intacto, evento sin truncar a word). La tabla de volumenes se indexa con el evento como word (`movzx eax,word ptr [esp+30h]`, 0x4EC57A).

**CAEPoliceScannerAudioEntity** (`AEPoliceScannerAudioEntity.cpp`)
- `FinishedPlayingScannerDialogue` 0x4E6C30: `mov ecx,[B7CB84]; add ecx,[8C8140]; mov [B61D50],ecx` (reloj + dword 0x8C8140); la fuente sumaba el valor previo de `s_NextNewScannerDialogueTime`. Ademas `mov [B61D14],10001h / [B61D18],10001h / word [B61D1C],1`: los cinco `s_SlotState` valen 1 (la fuente los rellenaba con -1).
- `PrepSlots` 0x4E6BC0: `test eax,eax / je` (0x4E6BC7): sin `s_pCurrentSlots` no hace nada; la fuente desreferenciaba nulo.
- `PlayPoliceScannerDialogue` 0x4E6ED0: un solo `[B7CB84]` y retardos `[8C8144]`/`[8C8148]` (0x4E6F31..0x4E6F50) en lugar de 2000/5000 fijos.
- `UpdateParameters` 0x4E7590: `fsub [8C8150h]` y `movsx [8C814C]` (0x4E75D7/0x4E75F1) en lugar de 6.0 y 40 literales (hoy valen igual).

**CAEPedAudioEntity** (`AEPedAudioEntity.cpp`)
- `HandleFootstepEvent` 0x4E13A0 reescrita como el EXE: volumen `(tabla+volume)+rand(-3,3)` ANTES de `FindPlayerPed` (0x4E13C1..0x4E13F2); ped no jugador: `IsSoundBankLoaded(0,0x29)` y `fsub [858B3C]` (-3.0) (0x4E1747), la fuente no restaba; superficies del jugador: el aleatorio se pide DESPUES de `IsSoundBankLoaded(bank,0x1E)` y el agua usa playTime 0x32 (la fuente lo perdia y comprobaba otra vez con el banco 0); `AreSoundsPlayingInBankSlot` se compara con `dec/je/dec/jne` (solo 1 o 2), la fuente `-1 <= 1` con signo; sonido por `m_tempSound` (`Initialise` flags 0xA0, varianza 0x3D70D845, `RegisterWithPhysicalEntity`, `RequestNewSound`).
- `HandleSkateEvent` 0x4E17E0: orden `(tabla+volume)+rand` (0x4E1810..0x4E1839) (la fuente `(tabla+rand)+volume`), varianza 0x3D70D845 (faltaba), `m_tempSound`, flags 0x80.
- `HandleLandingEvent` 0x4E18E0: `m_tempSound`, flags 0xA0, varianza 0x3D70D845, agua `volumen<=0 o NaN -> 0` (0x4E1968).
- `HandleSwimSplash` 0x4E26A0: `inc cx / cmp cx,4 / jle` (0x4E26E7): el id vuelve a 0 al pasar de 4 (la fuente `max(0,id+1)`); flags 0 + `SetFlags(80h,1)` (0x4E2751..0x4E2761), no PLAY_PHYSICALLY|START_PERCENTAGE|DUCKABLE; varianza por `mov [esi+28h],3D70D845h`; `m_tempSound`.
- `HandleSwimWake` 0x4E2790: primero `AreSoundsOfThisEventPlayingForThisEntityAndPhysical` (0x4E27A7) y despues el banco (+`IsLoadingTuneActive`); `m_tempSound` con speed 0.75, `word [esi+5Eh]=84h`, evento a mano; la fuente comprobaba el banco antes.
- `AddAudioEvent` 0x4E2BB0 (rama AE_PED_CRUNCH): `CAESound::Initialise` deja `m_ClientVariable=-1`; `PlaySound` lo pisaba con 0.
- `TurnOnJetPack` 0x4E28A0: valores iniciales leidos de 0x8AE5F0/0x8AE600/0x8AE610/0x8AE618 y sonidos por `m_tempSound` (`mov [esi+24h],3F800000h`, `mov word [esi+5Eh],4`).
- `UpdateJetPack` 0x4E0EE0: `fsin` x87 completo (0x4E106C) con ST0 sin redondear para el sonido 0; `fmul [B613B0]` (= 0.93f-1.0f, calculado por el inicializador 0x84C7E0; la fuente usaba -0.07f); `fmul [8AE620]` (0.2) en vez de `/5`; `fld [B61384]` (global a 0) en vez de `+0.0f`; `thrustFwd` NaN va a la rama `<=0.5` (0x4E0F24).

**CAEStreamingChannel** (`AEStreamingChannel.cpp/.h`)
- `FillBuffer` 0x4F1E20: `m_nStreamPlayTimeMs = decoder->[vtable+0Ch]` (GetStreamPlayTimeMs, 0x4F1F17); la fuente llamaba a GetStreamLengthMs (+8); comprueba decoder nulo (0x4F1F0B).
- `UpdatePlayTime` 0x4F18A0: `freqFactor = freq * [859CE0]` (0.004, no `/250`); la mitad del buffer sale de `cursor/0x60000` (`mul 0AAAAAAABh / shr 12h`, 0x4F1943), no de `freqFactor`; casos `(c-u)+(S-u)`, `(c-u)+S`, `c+S`, `(S-u)+c` segun `m_lastWrittenSlot`; resta de `upperLimit` en float (`fcom/fsub`, 0x4F196D), no entera.
- `SetReady` 0x4F1FF0: dos `if` consecutivos (`-5` y luego `-6`); la fuente tenia un `switch` sin segunda comprobacion.
- `Pause` 0x4F2170: `call 4D7990 (IsBufferPlaying) / test al,al / je` (0x4F217A); faltaba.
- `SetFrequencyScalingFactor` 0x4F2060: `RequestFade(...,35,0)` (`push 0` en 0x4F212C; la fuente pasaba true) y `m_nState = Started` (0x4F2159) tambien sin buffer.
- `AddFX` 0x4F1AE0: `SetFX` recibe un array local de codigos de resultado (0x4F1B6D), no nullptr.
- `Initialise` 0x4F22F0: devuelve `bl` (`mov al,bl`, 0x4F23BE): 1 si `CreateSoundBuffer` tuvo exito. La fuente era `void` (ahora `bool`).

MATCH (sin cambios): Script Initialise, Service, ReportMissionAudioEvent-1/-2; Scanner Constructor/Destructor/StaticInitialise/Reset/Enable/Disable/StopScanner/Populate/CanWePlay; Ped Initialise, StaticInitialise, Terminate, TurnOffJetPack, StopJetPackSound, PlayWindRush, PlayShirtFlap; Streaming Destructor, InitialiseSilence, RemoveFX, SetBassEQ, Stop-class/virtual, SetNextStream, GetActiveTrackID, GetPlayingTrackID, IsSoundPlaying, Play.

## Como se prueba
- Objetos falsos en memoria propia (entidades, CAEStreamingChannel de 0x60098 bytes, ped/fisico/matriz falsos, 8 CAESound falsos, decodificadores y COM de DirectSound/FXParamEq con vtables propias). No se toca ningun singleton de audio.
- Dobles trazados (EntryPatch en la direccion del EXE y en el simbolo/cuerpo de la DLL; los cuerpos se calculan ANTES de crear parches): IsSoundBankLoaded, LoadSoundBank, CAESoundManager::{CancelSoundsInBankSlot, AreSoundsPlayingInBankSlot, AreSoundsOfThisEvent...(AndPhysical), CancelSoundsOwnedByAudioEntity, RequestNewSound}, CAESound::{RegisterWithPhysicalEntity, StopSoundAndForget, SetPosition}, GetRandomNumberInRange<int>, FindPlayerPed, ReportFrontendAudioEvent, IsLoadingTuneActive, CAESmoothFadeThread::{RequestFade, CancelFade}, CAEAudioChannel::{SetFrequency, SetOriginalFrequency}, CAEAudioUtility::GetCurrentTimeInMS, CAETwinLoopSoundEntity::{Initialise, PlayTwinLoopSound, UpdateTwinLoopSound, StopSoundAndForget}; y, solo donde la funcion probada los llama, ProcessMissionAudioEvent, PlayResidentSoundEvent, PlayLoadedDialogue y los Handle* de CAEPedAudioEntity. CoInitialize (IAT 0x858350) solo durante `CAEStreamingChannel::Initialise`.
- El hilo de streaming real se aparca en `IsForegroundApp` durante todo el banco (como AudioFinal1) y los dobles ignoran hilos ajenos (`Mine()`). La tabla de volumenes 0xBD00F8 se sustituye por una falsa y se restaura. x87 a 24 bits en las llamadas con coma flotante.
- Se comparan: valor de retorno, hash del objeto bajo prueba (con mascara en los casos de abajo), hash de fisico/ped/sonidos/buffer destino, hash de los 0x60 bytes de globales del scanner (0xB61CF8) y la traza completa de llamadas con argumentos. Los sonidos pasados a `RequestNewSound` se resumen con hash enmascarando relleno (+0x55, +0x6A..0x6B y +0x72..0x73: huecos de alineacion que el constructor 0x4EFE50 del EXE no escribe), camara/tiempos (+0x40..0x4F) y `m_IsAudioHardwareAware` (+0x5A), que `CAESound::Initialise` no escribe (en la rama CRUNCH el EXE usa un CAESound de pila sin inicializar).
- Gate positivo por funcion: al menos un caso con llamadas a dobles, retorno distinto de 0 o bytes cambiados.

- AddAudioEvent: la ejecucion 20261009-033518 ya uso la sonda recompilada (asi 03:35:07 > LiveComparison 02:38) y seguia fallando 16/1024 solo en AE_PED_CRUNCH con mascara de relleno. Causa probable: el EXE calcula `(float)tabla[0x77] + volume` con fild/fadd/fstp (0x4E2CDA..0x4E2CFB) a precision extendida (doble redondeo) y el banco no activaba X87() (24 bits) en esa funcion. Ahora se envuelve con X87()/X87off(). Ademas, ante una diferencia el jsonl incluye `raw_original`/`raw_candidate` (hex de los 0x74 bytes de cada CAESound pedido, separados por `|`) para diagnosticar si persiste.

## Limites documentados
- Constructores/destructores (`Script::Constructor`, `Scanner::Constructor/Destructor`, `Streaming::Destructor`): el vptr y el tramo de la base `CAEAudioEntity` (`m_Entity`, `m_tempSound`) se enmascaran. El EXE solo escribe +0x10 del `m_tempSound`; la DLL, por `CAEAudioEntity::m_Entity{}` / `CAESound m_tempSound{}` (clase base compartida, fuera de este encargo), pone a cero todo. El resto de campos (`+0x7C..`, subentidades puerta/explosion) se compara byte a byte; el retorno indica si el vptr quedo escrito.
- `CAEStreamingChannel::InitialiseSilence` con `CreateSoundBuffer` fallido no se prueba: la DLL escribe `NOTSA_LOG_WARN` (spdlog reserva memoria) y el EXE no hace nada. En `Initialise` el primer `CreateSoundBuffer` si falla (1 de cada 5) y el de silencio siempre se crea.
- Dominios: indices `sampleId` 0..3, `m_pPed` no nulo salvo en AddAudioEvent, frecuencias de canal no nulas, `FillBuffer` con decodificador que siempre avanza, floats finitos (sin NaN/sNaN).
- `HandleFootstepEvent` y `HandleSwimWake`/`Landing` leen `g_surfaceInfos` real (solo lectura) con superficies 0..179.

## Supuestos a revisar al compilar
- Nombres de hooks `Stop-class`, `Stop-virtual`, `ReportMissionAudioEvent-1/-2` (como en `docs/hooks.csv`).
- `A(&CAEAudioChannel::SetFrequency)`, `A(&CAESound::SetPosition)` y los punteros a miembro de los stubs no son virtuales ni sobrecargados.
- `Initialise` de `CAEStreamingChannel` ahora devuelve `bool` (cabecera y .cpp); ningun llamante la usa.
- Punteros de offsets: CAESound `m_Length` +0x70, flags +0x56; `CPed` flags +0x46C, `m_nContactSurface` +0xBA; gemelo `m_IsInUse` +0x88 del twin-loop (+0x130 del ped).

Repetir: `bash reversing/_handoff/bench.sh AudioWave2 3`
