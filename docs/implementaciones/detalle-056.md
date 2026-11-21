# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1101"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eAEVehicleHornType.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eAEVehicleHornType v) | [source/game_sa/Audio/Enums/eAEVehicleHornType.h:25](../../source/game_sa/Audio/Enums/eAEVehicleHornType.h#L25) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1102"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eBassSetting.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eBassSetting v) | [source/game_sa/Audio/Enums/eBassSetting.h:14](../../source/game_sa/Audio/Enums/eBassSetting.h#L14) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1103"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBank.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBank v) | [source/game_sa/Audio/Enums/eSoundBank.h:740](../../source/game_sa/Audio/Enums/eSoundBank.h#L740) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1104"></a>

## Globales y helpers: source/game_sa/Audio/Enums/eSoundBankSlot.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| EnumToString(eSoundBankSlot v) | [source/game_sa/Audio/Enums/eSoundBankSlot.h:87](../../source/game_sa/Audio/Enums/eSoundBankSlot.h#L87) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1105"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ExeGainToDecibels(const float* gain) | [source/game_sa/Audio/Hardware/AEAudioHardware.cpp:487](../../source/game_sa/Audio/Hardware/AEAudioHardware.cpp#L487) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1106"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEAudioHardware.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyFromAESound(const CAESound& sound) | [source/game_sa/Audio/Hardware/AEAudioHardware.h:29](../../source/game_sa/Audio/Hardware/AEAudioHardware.h#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1107"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEStaticChannel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ChannelReleaseSequence(uint8* b, uint32 vtable) | [source/game_sa/Audio/Hardware/AEStaticChannel.cpp:29](../../source/game_sa/Audio/Hardware/AEStaticChannel.cpp#L29) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_numSoundChannelsUsed: source/game_sa/Audio/Hardware/AEAudioChannel.cpp:27 (0x00B5F898) |
| Lote142AStaticChannelDestructorBody() | [source/game_sa/Audio/Hardware/AEStaticChannel.cpp:59](../../source/game_sa/Audio/Hardware/AEStaticChannel.cpp#L59) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1108"></a>

## Globales y helpers: source/game_sa/Audio/Hardware/AEStreamingChannel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AEStreamingChannelDestructorBody(CAEStreamingChannel* self, void* /*edx*/) | [source/game_sa/Audio/Hardware/AEStreamingChannel.cpp:600](../../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp#L600) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CAEStreamingChannel::Destructor 0x004F2200 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote150ABody_CAEStreamingChannel() | [source/game_sa/Audio/Hardware/AEStreamingChannel.cpp:614](../../source/game_sa/Audio/Hardware/AEStreamingChannel.cpp#L614) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1109"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEDataStream.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * __fastcall AEDataStreamScalarDeletingDestructor(CAEDataStream* self, void* /*edx*/, uint8 flags) | [source/game_sa/Audio/Loaders/AEDataStream.cpp:227](../../source/game_sa/Audio/Loaders/AEDataStream.cpp#L227) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAEDataStream::ScalarDeletingDestructor 0x004DC550 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1110"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEMFDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MFDecoderDestructorBody(CAEMFDecoder* self, void* /*edx*/) | [source/game_sa/Audio/Loaders/AEMFDecoder.cpp:159](../../source/game_sa/Audio/Loaders/AEMFDecoder.cpp#L159) | C++ · Cuerpo fuente con errores de análisis | Pendiente en catálogo: CAEMFDecoder::Destructor 0x004E77C0 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote146AMFDecoderDestructorBody() | [source/game_sa/Audio/Loaders/AEMFDecoder.cpp:173](../../source/game_sa/Audio/Loaders/AEMFDecoder.cpp#L173) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1111"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| SecondsToMs(double seconds) | [source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp:34](../../source/game_sa/Audio/Loaders/AEVorbisDecoder.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1112"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEWMADecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CAEWMADecoderDestructorBody(CAEWMADecoder* self, void* /*edx*/) | [source/game_sa/Audio/Loaders/AEWMADecoder.cpp:325](../../source/game_sa/Audio/Loaders/AEWMADecoder.cpp#L325) | C++ · Cuerpo fuente con errores de análisis | Histórica del catálogo: CAEWMADecoder::Destructor 0x00502760 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Lote145ABody_CAEWMADecoder() | [source/game_sa/Audio/Loaders/AEWMADecoder.cpp:340](../../source/game_sa/Audio/Loaders/AEWMADecoder.cpp#L340) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1113"></a>

## Globales y helpers: source/game_sa/Audio/Loaders/AEWaveDecoder.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| BytesToMs(uint32 bytes, uint32 bytesPerSecond) | [source/game_sa/Audio/Loaders/AEWaveDecoder.cpp:32](../../source/game_sa/Audio/Loaders/AEWaveDecoder.cpp#L32) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1114"></a>

## Globales y helpers: source/game_sa/Audio/Managers/AERadioTrackManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ResetCommon(CAERadioTrackManager& self) | [source/game_sa/Audio/Managers/AERadioTrackManager.cpp:103](../../source/game_sa/Audio/Managers/AERadioTrackManager.cpp#L103) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo m_nAdvertIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:169 (0x00B620C0); Posible dato fijo m_nDJBanterIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:168 (0x00B61D78); Posible dato fijo m_nIdentIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:170 (0x00B62980); Posible dato fijo m_nMusicTrackIndexHistory: source/game_sa/Audio/Managers/AERadioTrackManager.h:171 (0x00B62B40); Posible dato fijo m_nSpecialDJBanterIndex: source/game_sa/Audio/Managers/AERadioTrackManager.h:199 (0x00B62C72); Posible dato fijo m_nSpecialDJBanterPending: source/game_sa/Audio/Managers/AERadioTrackManager.h:200 (0x00B62C73) |
| CalculateRadioVolume355() | [source/game_sa/Audio/Managers/AERadioTrackManager.cpp:599](../../source/game_sa/Audio/Managers/AERadioTrackManager.cpp#L599) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CAERadioTrackManager::UpdateRadioVolumes 0x004EA010 | 0 / 0 | Posible dato fijo AEAudioHardware: source/game_sa/Audio/Hardware/AEAudioHardware.cpp:30 (0x00B5F8B8) |

<a id="grupo-1115"></a>

## Globales y helpers: source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CopyBytesLikeExe(char* dst, const char* src, uint32 n) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:736](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L736) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringXlen() | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:741](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L741) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringXran() | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:745](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L745) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| StringCopyGrow(LegacyString* self, uint32 newSize, uint32 oldLength) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:751](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L751) | C++ · Envoltura al EXE | Revalidada en migración: UserTracksLegacyString::Reallocate 0x004F39C0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Delegación por dirección 0x004F39C0: UserTracksLegacyString::Reallocate; ruta efectiva sin resolver |
| StringTidy(LegacyString* self, void* /*edx*/) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:757](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L757) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyStdString::Tidy 0x004F3BA0 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall StringAssignString(LegacyString* self, void* /*edx*/, const LegacyString* str, uint32 pos, uint32 n) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:827](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L827) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyStdString::AssignString 0x004F3E80 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall StringAssignChars(LegacyString* self, void* /*edx*/, const char* text, uint32 n) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:850](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L850) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyStdString::AssignChars 0x004F3F60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall StringFromChars(LegacyString* self, void* /*edx*/, const char* text) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:866](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L866) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyStdString::ConstructFromChars 0x004F4170 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall RuntimeErrorFromString(LegacyRuntimeError* self, void* /*edx*/, const LegacyString* message) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:877](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L877) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyRuntimeError::ConstructFromString 0x004F4090 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall RuntimeErrorCopy(LegacyRuntimeError* self, void* /*edx*/, const LegacyRuntimeError* other) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:889](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L889) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyRuntimeError::CopyConstructor 0x004F4230 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| RuntimeErrorDestructorBody(LegacyRuntimeError* self, void* /*edx*/) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:901](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L901) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyRuntimeError::DestructorBodyA 0x004F3D10 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall RuntimeErrorWhat(const LegacyRuntimeError* self, void* /*edx*/) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:913](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L913) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyRuntimeError::What 0x004F3D50 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| * __fastcall RuntimeErrorScalarDeletingDestructor(LegacyRuntimeError* self, void* /*edx*/, uint8 flags) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:919](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L919) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: LegacyRuntimeError::ScalarDeletingDestructorA 0x004F3D60 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| VectorXlenImpl(void* /*self*/) | [source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp:930](../../source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp#L930) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00860AB0, 0x00893634 |

<a id="grupo-1116"></a>

## Globales y helpers: source/game_sa/Audio/OpenAL/OALBase.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| OALCheckErrors(std::string_view file, int32 line) | [source/game_sa/Audio/OpenAL/OALBase.cpp:26](../../source/game_sa/Audio/OpenAL/OALBase.cpp#L26) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1117"></a>

## Globales y helpers: source/game_sa/AutoPilot.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| AutoPilotSetCarMissionUnlessCrashAndBurn(CAutoPilot* self, void* /*edx*/, uint32 mission) | [source/game_sa/AutoPilot.cpp:34](../../source/game_sa/AutoPilot.cpp#L34) | C++ · Cuerpo fuente; delegación indirecta no descartada | Revalidada en migración: CAutoPilot::SetCarMissionUnlessCrashAndBurn 0x00463490 · [registro](../../reversing/CppOnlyMigration/approvals.json) | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SubtractRoundedOnce(double a, double b) | [source/game_sa/AutoPilot.cpp:128](../../source/game_sa/AutoPilot.cpp#L128) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1118"></a>

## Globales y helpers: source/game_sa/Birds.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetMaxDistance(eBirdsBiome biome) | [source/game_sa/Birds.cpp:65](../../source/game_sa/Birds.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1119"></a>

## Globales y helpers: source/game_sa/BouncingPanel.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_MV_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/BouncingPanel.cpp:52](../../source/game_sa/BouncingPanel.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| Multiply3x3_VM_59C810(const CVector& v, const CMatrix& m) | [source/game_sa/BouncingPanel.cpp:61](../../source/game_sa/BouncingPanel.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyByPow(CVector& pos, float damp, float step) | [source/game_sa/BouncingPanel.cpp:71](../../source/game_sa/BouncingPanel.cpp#L71) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1120"></a>

## Globales y helpers: source/game_sa/BreakObject_c.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) | [source/game_sa/BreakObject_c.cpp:49](../../source/game_sa/BreakObject_c.cpp#L49) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CMatrix::operator*-Vec 0x0059C890 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
