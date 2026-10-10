<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# TaskFinal1: constructores y destructores de las 5 clases finales

Banco por pares (estilo Small2/PadMouse1, dentro del juego completo) para: CTaskUtilityLineUpPedWithCar (0x64FBB0 / 0x64FC00), CTaskGoToVehicleAndLean (0x660E60 / 0x660EE0), CTaskGangHassleVehicle (0x65FAC0 / 0x65FB60), CTaskGangHasslePed (0x65FED0 / 0x65FF60) y CTaskComplexPlayHandSignalAnim (0x61B2B0 / 0x61BDF0). **Compilado y ejecutado; aprobado en la corrida 20261009-005855-477249 (3200 pares, 0 diferencias) y registrado en `validation-summary.json` (historico, `dependency_review=null`).**

Uso: `bash reversing/_handoff/bench.sh TaskFinal1 3`. Filtro opcional: `taskfinal1-only.txt` en la carpeta del juego con `Clase::Funcion`.

## Corpus: 3200 pares

LineUp ctor 512 / dtor 128; GoToVehicleAndLean 256 / 256; GangHassleVehicle 256 / 512; GangHasslePed 256 / 256; PlayHandSignalAnim 256 / 512.

## Que se compara (original = hooks desactivados, candidato = activos)

- Constructores en la direccion del EXE sobre buffer de 0x80 bytes prellenado con 0xCD (mas guardas de 16 bytes): buffer completo sin vptr, tipo de tarea (GetTaskType) en lugar de vptr, retorno, referencias y estado de las entidades falsas (peaton y automovil en buffers propios, el automovil con el byte 0x51B de posiciones de acoso aleatorio).
- Destructores: campos aleatorios, referencias registradas (3 de 4) o no, subtarea falsa con vtable trazada (`delete_fake`), contadores de los 4 modelos de manos, bloques de animacion (0xB5D4A0), `ms_animBlock` (0x8D2E38) y reloj (0xB7CB84), todos restaurados antes de cada ejecucion y al final.
- Pool de tareas: no se reserva nada; se comprueba que la ocupacion no cambia y se restaura `m_LastFreeSlot` con los huecos libres a 0xCD.

## Colaboradores simulados (EntryPatch en EXE + simbolo de la DLL, solo hilo del banco)

`CBaseModelInfo::RemoveRef` (0x4C4BB0; decrementa +8 y anota el modelo; el contador del TXD no se toca) y `CStreaming::RemoveModel` (0x4089A0). `RemoveAnimBlockRef`, `GetAnimationBlockIndex("gangs")`, `SetHasslePosId` y las referencias se ejecutan de verdad.

## Correcciones de fuente que cubre

- LineUp ctor: `m_fTime = CTimer::GetTimeInMS() + time` (0x64FBCD-0x64FBE0), antes solo `time`.
- PlayHandSignalAnim dtor: solo quita la referencia de cada mano si el bit 0 / bit 1 esta activo y usa el indice izquierda=SHANDL/FHANDL (0xA9B6F0/F8), derecha=SHANDR/FHANDR (el enum estaba invertido); ctor: `and byte [this+14h],0F8h` literal, sin `{}` en los bitfields.
- Sin cambio de comportamiento: GoToVehicleAndLean y GangHassleVehicle (solo se quito `: CTaskComplex()` redundante). GangHasslePed no tiene cambios de fuente (git).

## Supuestos que se revisaron al compilar

- `CBaseModelInfo::RemoveRef` y `CStreaming::RemoveModel` deben ser publicos y sin sobrecargas (se toma su direccion).
- El relleno de los bytes sin escribir (0xCD) debe coincidir: si PlayHandSignalAnim difiere en el byte 0x14 el compilador esta reescribiendo el byte completo.
