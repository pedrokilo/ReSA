<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Voz de peatón: CAEPedSpeechAudioEntity (lista de 34)

Banco escrito el 04/10/2026: **21.760 pares original/candidato** previstos dentro de GTA para las 58 funciones de `CAEPedSpeechAudioEntity` (50 hooks normales y 8 virtuales de la vtable `0x85F310`, entradas 0..7 comprobadas con el volcado de la vtable). Pendiente de compilar y ejecutar.

| Grupo | Funciones |
|---|---|
| Ciclo de vida | `Constructor`, `Initialise`, `Terminate` (VMT 2), `StaticInitialise`, `Reset`, `Service` |
| Decir frases | `AddSayEvent`, `AddScriptSayEvent` (VMT 1), `LoadAndPlaySpeech`, `PlayLoadedSound` (VMT 3), `UpdateParameters` (VMT 0), `StopCurrentSpeech` |
| Contextos | `GetSpecificSpeechContext`, `GetSoundAndBankIDs`, `GetSoundAndBankIDsForScriptedSpeech`, `GetRepeatTime`, `GetNextPlayTime`, `SetNextPlayTime`, `GetNumSlotsPlayingContext`, `GetSpeechContextVolumeOffset`, `IsGlobalContextImportantForInterupting/Streaming/Widescreen`, `IsGlobalContextPain`, `IsGlobalContextUberImportant`, `CanPedSayGlobalContext`, `CanPedHoldConversation`, `WillPedChatAboutTopic` (VMT 5) |
| Huecos y conversaciones | `CanWePlayGlobalSpeechContext`, `CanWePlayScriptedSpeech`, `Reserve*`, `Request*`/`Release*` de peatón y jugador, `SetUpConversation` |
| Voces y humor de CJ | `GetAudioPedType`, `GetVoice`, `GetVoiceAndTypeFromModel`, `GetVoiceAndTypeForSpecialPed`, `GetSexFromModel`, `GetSexForSpecialPed`, `GetCurrentCJMood`, `GetNextMoodToUse`, `GetVoiceForMood`, `SetCJMood`, `IsCJDressedInForGangSpeech` |
| Estado | `Enable/Disable(All)PedSpeech(ForScriptSpeech)`, `GetPedTalking`, `GetAllocatedVoice` (VMT 4), `GetPedType` (VMT 6), `IsPedFemaleForAudio` (VMT 7) |

**Diferencias reales corregidas:**
- Tablas: las búsquedas leen las tablas de datos del EXE (contextos `0x8C6A68` con centinela -1, rangos y bancos por tipo, nombres de voces y de peatones especiales); los peatones genéricos usan rangos de 2 bytes (`uint8`), `CanPedSayGlobalContext` solo mira el byte bajo y nunca deja hablar a los GFD; `GetSpecificSpeechContext` con SPC devuelve `RepeatTime`.
- Convenciones: `GetSpecificSpeechContext`, `GetSpeechContextVolumeOffset` e `IsGlobalContextUberImportant` son `__stdcall` (`ret 8`/`ret 4`).
- `GetSoundAndBankIDs`: aleatorio `[0, último - primero]`, hasta 20 intentos con la búsqueda del EXE en la memoria de frases, sin comprobar la voz; humor de CJ con el bucle del EXE.
- `GetVoiceForMood` es voz base + aleatorio (estaba invertido); humor desconocido = CR. `SetUpConversation` usa `GetRandomNumberInRange(0, 1)` (también la tirada sin usar del caso 5..8).
- `GetCurrentCJMood`: evalúa todas las condiciones y estadísticas en el orden del EXE, recorre los 8 miembros del grupo, nivel de búsqueda del propio jugador, modo forzado solo si su tiempo es **mayor** y valores tomados como byte.
- `GetSpeechContextVolumeOffset` (esfuerzo): `(f(0xB61C14/18) - base) · k + base` con los floats estáticos del EXE y las estadísticas pedidas en cada comparación.
- `AddSayEvent`: desfase `0xB61C10`, estado del juego antes del chat, distancia `> 1600` (un NaN pasa), hueco antes del sonido, `FindPlayerPed(0)`, `FindVehicleOfPlayer` y 2D (no volumen forzado) en el vehículo del jugador. `AddScriptSayEvent`: volumen del evento 0x35 y 2D en el vehículo del jugador.
- `CanWePlayGlobalSpeechContext`: hueco libre (avanza `s_NextSpeechSlot`) antes del tiempo; `CanWePlayScriptedSpeech` con módulo con signo; `Reserve*` usan los primeros huecos libres (antes el mismo hueco dos veces); `GetNumSlotsPlayingContext` cuenta 5 huecos.
- `ReleasePedConversation` baja la bandera aunque un hueco no sea válido y solo para voces en estado 1, 2, 3 o 5; `ReleasePlayerConversation` solo quita la reserva del hueco.
- `UpdateParameters` compara con el hueco antes de soltarlo y usa el bit 15 (`bRemoveHead`); `PlayLoadedSound` inicializa sin banderas y escribe banderas, atenuación y volumen después, `SetRequest(7, 2800, -1, 0)`.
- Huecos reiniciados campo a campo (`0x4E3350`) en todas partes; `Constructor`, `Initialise` y `Terminate` solo escriben los campos del EXE; `StaticInitialise` pone los huecos de conversación a -1 y vuelve a sortear los tiempos; `Service` escribe `s_bForceAudible` al final.
- `GetNextPlayTime`/`SetNextPlayTime` comprueban el rango y suman en 16 bits; `GetVoice` devuelve -1 en tipos desconocidos; `GetSexFromModel`/`GetVoiceAndTypeFromModel` como el EXE (tipo escrito antes de comprobarlo, voz `< 0`); `IsCJDressedInForGangSpeech` usa la posición del propio peatón.

**Método:**
- Cuatro peatones falsos con su entidad de voz en `+0x294`, matriz, inteligencia y tarea facial falsas, datos de jugador (CWanted, ropa, grupo) e infos de modelo falsas en `ms_modelInfoPtrs[19990..19997]`.
- Se fijan por caso y se restauran: huecos de voz (con `0xB61C10..0xB61C37`), memoria de frases, estáticos `0xB613D0..0xB61418`, tiempos de contexto, cámara, tiempo, fotograma, estado del juego, `UnderWaterness`, peticiones de streaming, ajustes de radio y `LastMissionPassedTime`.
- El hilo de streaming de audio se aparca en `IsForegroundApp` durante todo el banco.
- Se simulan y trazan en el EXE y en la DLL: `rand`/`GetRandomNumber`, `ResolveProbability`, carga de sonidos del hardware, `CAESound` (`Initialise` deja un estado determinista), `RequestNewSound`, la tarea facial, `CPed::IsAlive/GetPedTalking/CanPedHoldConversation`, `IsVeryBusy`, `FindVehicleOfPlayer`, el banco de los scripts, `FindPlayerPed`, `GetStatValue`, `FindCityClosestToPoint`, los miembros del grupo e `IncrementVoice`.
- Fuera de los fixtures: hueco -1 donde el EXE lo indexa, voces fuera de su tabla en `GetSoundAndBankIDs`, SPC con contextos de dolor en `GetSoundAndBankIDs`/`AddSayEvent` (rango leído fuera de la tabla y pila sin inicializar si queda vacío) y punteros nulos que el EXE usa sin comprobar.

Ejecutar con `python reversing/PedSpeech58/run_local.py` (caso `pedspeech58`).
