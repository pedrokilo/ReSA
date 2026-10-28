<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco Dependencies26A (grupo g26a de BRIEF_21): CAEAudioHardware::PlaySound

Cierra la clase CAEAudioHardware (era la unica funcion sin hook; las otras 54 estaban reconstruidas y validadas). Pares EXE (hook RH apagado) frente a DLL (hook RH encendido), dentro del juego, con
colaboradores sustituidos por los mismos falsos trazados. **La corrida dentro del juego NO se ha ejecutado** (encargo de solo analisis). La evidencia previa es el ensayo sin juego de `offline/`
(bytes reales del EXE frente al C++ de la DLL), que si se ejecuto.

| Funcion | Entrada | Convencion | Pares |
| --- | --- | --- | --- |
| CAEAudioHardware::PlaySound | 0x4D86B0 | `thiscall`, 7 palabras de pila, `ret 0x1C`, sin retorno | 16 familias x 128 = 2048 |

`void PlaySound(int16 channel, uint16 channelSlot, uint16 soundIdInSlot, uint16 bankSlot, int16 playPosition, int16 flags, float speed)`. Clasificacion y evidencias: `g26a-classification.md`. Residuo de registros: `g26a-audit.txt`.

## Que hace la funcion (desensamblado, `original-disassembly.txt`)
Sale sin hacer nada si el canal es negativo (`test di, di`) o la ranura no es menor que `m_anNumChannelsInSlot[canal]`. Pide a `CAEMP3BankLoader::GetSoundBuffer` (0x4E0280) el buffer, el tamano y la tasa del sonido (banco con `movsx` de
16 bits); sin buffer sale. Llama a `GetLoopOffset` (0x4E0380) ANTES de comprobar el indice; `indice = (ranura + canal) & 0xFFFF` debe ser <= 0x40 (el 64 lee el primer dword de gBeatInfo, tras la tabla). Llama a
`CAEStaticChannel::SetAudioBuffer` (0x4F0C40) sobre `m_aChannels[indice]`; si devuelve falso, sale. Posicion = `max(playPosition, 0)`; con el bit 3 de flags y posicion > 0 es un porcentaje de `GetLength()` (vtable +0x10):
`fild; fmul 0.01f; fimul longitud; fstp float; floor del CRT; _ftol2`. Se recorta con `movsx` de 16 bits contra la longitud de 32 bits con signo. Con `m_n3dEffectsQueryResult != 0` llama a `SetNotInRoom(!(flags & 0x200))`;
llama a `Play(posicion, flags, speed)` (vtable +0x14) y escribe `m_awChannelFlags[canal + ranura] = flags` (suma de 32 bits) y `canal->m_nFlags = movsx(flags)`.

## Familias (16 familias x 128 casos, `Fixtures.hpp`)
0 valido sin porcentaje | 1 porcentaje | 2 canal negativo | 3 ranura fuera de rango | 4 sin buffer | 5 indice > 64 | 6 SetAudioBuffer falla | 7 indice 63 y 64 | 8 posicion extrema (negativa, == longitud, +-1, recorte)
| 9 velocidad especial (NaN con carga y senalizante, +-inf, +-0, subnormales, +-1e30, FLT_MAX) | 10 los 16 bits altos de los seis primeros argumentos sucios | 11 flags especiales (bit 3, bit 9, signo, todos) | 12 suma de
32 bits distinta de la de 16 (canal hasta 0x7FFF + ranura que dan 0x10000 + indice) | 13 buffer, tamano, tasa y bucle extremos | 14 porcentaje extremo (longitudes negativas, 0, 2^24, 0x7FFFFFFF; productos exactos e inexactos) con
8 FSW iniciales | 15 mezcla. Ademas los 8 FSW iniciales (banderas acumuladas y C0..C3) se reparten por todos los casos.

## Objetos falsos (nunca se toca AEAudioHardware, DirectSound, EAX, los cargadores ni los hilos de audio)
- `CAEAudioHardware` falso: arena de 0x24000 bytes relleno al azar por caso (las lecturas fuera de la tabla, hasta 0x1008E, y las escrituras de la tabla de flags con la suma de 32 bits, hasta 0x20088, caen dentro). Las palabras que
  la funcion escribe se rellenan con el complemento de lo que escribira, de modo que el cambio es siempre visible. `static_assert(offsetof(...))` de los seis desplazamientos en `LiveComparison.hpp`.
- 65 canales falsos (64 de la tabla + el del indice 64) con tabla de metodos virtuales falsa de 9 huecos: `GetLength` guionizado, `Play` trazado y trampa (suceso 7) en el resto. `CAEMP3BankLoader` falso solo como `this`.
- Colaboradores no virtuales sustituidos en AMBOS lados por el mismo falso trazado (`EntryPatch` en la direccion del EXE y en el cuerpo de la DLL, resuelto con `Resolve()` ANTES de crear HookRoute/CollabHooks/parches):
  `GetSoundBuffer` 0x4E0280, `GetLoopOffset` 0x4E0380, `SetAudioBuffer` 0x4F0C40, `SetNotInRoom` 0x4D7B50. Sus hooks RH (otros bancos) se apagan durante la corrida y se restauran (`CollabHooks`).
- NO se sustituyen `floor` del CRT del EXE (0x8219F0, ruta SSE2: el banco exige `[0xC9C400] != 0`, como en el juego) ni `_ftol2` (0x821B40): sus efectos se comparan por el estado x87 completo.

## Que se compara (en bits; el contenido de `original` y `candidate` ha de ser identico)
- Traza completa de colaboradores con todos sus argumentos (los de 16 bits enmascarados a su ancho real, porque los llamados solo leen 16 bits; `Play` recibe `posicion & FFFF`, `flags & FF` y los bits exactos de la velocidad).
- Cambios del arena de CAEAudioHardware (lista de bytes cambiados) y de los 65 canales falsos.
- ESP (`ret 0x1C` balanceado) y EBX/ESI/EDI/EBP. EAX/ECX/EDX al volver NO se exigen: el unico llamador (0x4F07F8) los sobrescribe antes de leerlos (`g26a-audit.txt`); se registran (`original_residue`/`candidate_residue`).
- Estado x87 completo: FNSAVE de 108 bytes (FCW, FSW con banderas acumuladas y C0..C3, FTW/TOP, FOP/FCS/FDP/FDS y los diez bytes de los ocho registros, VACIOS incluidos; FIP a cero y su modulo validado aparte) y MXCSR.
  Antes de cada lado se carga un estado conocido (FCW 0x7F: 24 bits, redondeo al par, excepciones enmascaradas; FSW del caso; ocho residuos distintos) y se vacian los ocho registros. `kCompareDeadX87Registers = true`, no se relaja.
- Restauracion: vigilancia de las entradas parcheadas (PlaySound, los cuatro colaboradores en el EXE y en la DLL, floor y _ftol2), `[0xC9C400]`, estado de los hooks (PlaySound y colaboradores) y x87/MXCSR del juego.

## Gate independiente (`run_local.py` + `fixture_oracle.py`)
Ademas del analizador comun, cada lado se compara con un **oraculo hecho a partir del desensamblado** que solo usa la entrada completa registrada en la fila: traza de colaboradores, cambios del arena y de los canales,
FCW/TOP/FTW, MXCSR, banderas acumuladas (IE/DE/ZE/OE/UE sin cambios y PE exacto tras el porcentaje: redondeo al par a 24 bits de `p * 0.01f` y del producto por la longitud con fracciones exactas) y los diez bytes de ST0..ST7
(los seis primeros con el residuo inicial; tras el porcentaje T-2 = R = floor en formato de 80 bits y T-1 = fraccion = +0). Cobertura minima por clase (60 % de lo medido) y final con restauracion.
`python -B run_local.py --analyze offline/obj/sim.jsonl` acepta el registro que escribe el ensayo sin juego.

## Estado x87
El unico codigo x87 es el porcentaje (`flags & 8` y posicion > 0). La DLL usa `AudioHwPercentPositionCpp` (anexo `g26a-x87.*.txt` de X87Math; compilado con /arch:IA32 /fp:fast): `fild; fmul 0.01f; fimul; fstp float` con
`double(...)` en cadena bajo `float_control(precise)` (la x87 a 24 bits redondea una vez por operacion, igual que el EXE, y se levanta PE por los mismos productos), el floor del CRT (SSE2) con enteros desde los bits del float (sin
tocar banderas) y `Ftol2RegsDblCpp` (T-2 = R, T-1 = fraccion). Ver `g26a-audit.txt`.

## Ensayo sin juego (`offline/`) — SI ejecutado
`build.bat [raiz] [gta-sa.exe] [casos] [registro.jsonl]` mapea los bytes del EXE (proceso hijo suspendido, imagen en 0x400000) y llama a la entrada 0x4D86B0 de esa copia frente a `PlaySound` tal como esta en
`AEAudioHardware.cpp` (`prepare.py` extrae el texto de la funcion byte a byte y lo compila con las opciones del proyecto) y a `AudioHwPercentPositionCpp` de `X87Math.cpp` (/arch:IA32), con los mismos falsos y las mismas
fixtures (`Core.hpp`, `Fixtures.hpp` compartidos con el banco del juego). `floor` y `_ftol2` son los del EXE (`[0xC9C400] = 1`). Resultado al entregar (sobre el arbol con `g26a.patch` y el anexo aplicados):
**2048 pares, 0 diferencias** (traza, arena, canales, FNSAVE completo, MXCSR, ESP, no volatiles), y el registro pasa `run_local.py --analyze`. `mutations.py`: ver el resultado en la seccion siguiente.

## Mutaciones (`offline/mutations.py`)
19 mutaciones del cuerpo de la DLL (15 de `PlaySound`, 4 del ayudante x87; compilan solo el objeto afectado): canal <= 0, ranura >, indice >= 64, indice sin sumar el canal, comprobacion del indice con la suma de 32 bits,
porcentaje con posicion >= 0 / con el bit equivocado, `onStreet` invertido, sin `SetNotInRoom`, tabla de flags con el indice de 16 bits, `m_nFlags` sin extension de signo, `Play` con flags de 7 bits, posicion negativa sin poner a cero,
recorte sin signo, floor hacia cero, menor que 1 negativo, constante 0.01 en doble, producto en float. **18 detectadas (de 3 a 1524 pares con diferencias) y 1 equivalente correctamente no detectada** (bucle como `uint16`: mismos 16 bits).
Resultado: `mutaciones sin el resultado esperado: 0`.

## Limites declarados
- Fuera del banco y sin credito: los cuatro colaboradores sustituidos (cada uno tiene su propio banco) y la vtable real de los canales (`GetLength`, `Play`).
- El banco registra los argumentos de los colaboradores enmascarados a 16 bits porque los llamados reales (GetSoundBuffer: `movzx`/`movsx` de palabra; SetAudioBuffer: `cmp word`/`mov ax, word`; Play: `mov ax, word`) solo leen 16 bits.
  Por eso el banco NO distingue `bankSlot` extendido con signo de sin signo al reenviarlo a `GetSoundBuffer`: esa fidelidad (`static_cast<int16>(bankSlot)`) se apoya en el desensamblado de 0x4E0280/0x4E0380 (`movsx`).
- El indice de canal 64 y las sumas de 32 bits mayores que 64 solo existen en el arena falso; con el `AEAudioHardware` real la lectura de `m_aChannels[64]` es el primer dword de `gBeatInfo`.
- Si `[0xC9C400]` fuera 0 el floor del CRT tomaria la ruta x87 (0x828A60), que no se cubre: `run_local.py` aborta.
- Las compuertas FOP/FDP/FDS heredan el tratamiento de 22A/24A (FIP a cero; el resto exigido).

## Ejecucion
```
python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies26A
python -B reversing/Dependencies26A/build_probe.py
python -B reversing/Dependencies26A/run_local.py
python -B reversing/Dependencies26A/run_local.py --analyze <comparisons.jsonl>
reversing\Dependencies26A\offline\build.bat [raiz] [gta-sa.exe]        (ensayo sin juego)
```
