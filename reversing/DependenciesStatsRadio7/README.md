<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Estadisticas de radio: validacion aprobada

Dos entradas reconstruidas, validadas y admitidas: PopulateFavoriteRadioStationList (0x558EC0) y FindMostFavoriteRadioStation (0x558FA0). Corrida 20261010-202345-539585: **4096 pares**, 2048 por funcion, **cero diferencias y cero resultados nulos**, rutas RH OFF/ON verificadas y estado restaurado. FindLeastFavoriteRadioStation (0x559010) queda fuera.

## Evidencia del EXE y reconstrucciÃ³n

`dis-populate.txt` muestra la llamada en 0x558ECE a `CAudioEngine::GetRadioStationListenTimes` (0x507020) sobre el engine global 0xB6BC90. Ese colaborador redirige al manager global 0x8CB6F8 y a 0x4E83E0, que devuelve `this+0x18` (`dis-helpers-*.txt`). El original copia **14 dwords**, incluidas emisoras de emergencia y radio apagada, a 0xB78E58..0xB78E8F. Retorna AL=1 solamente cuando los catorce son cero. Un entero negativo tambiÃ©n hace AL=0. La propuesta utiliza el colaborador local real y copia cada entero sin convertirlo ni reinterpretar su signo.

`dis-most.txt` prueba que la bÃºsqueda lee Ãºnicamente Ã­ndices **1..12** de esa lista. Comienza con mÃ¡ximo signed=0 y resultado=1. Las ramas `jle` conservan el primer Ã­ndice en caso de empate; una lista de valores cero/negativos retorna 1 aunque otra posiciÃ³n tenga un cero. Los Ã­ndices 0 y 13 no participan. La propuesta reproduce la bÃºsqueda signed y no depende de una llamada original.

Aunque la declaraciÃ³n de la fuente retorna `eRadioID` con base int8, el EXE devuelve el Ã­ndice completo en EAX. La propuesta usa una definiciÃ³n naked con bÃºsqueda entera y `ret` sin argumentos para conservar ese resultado completo. No introduce colaboradores adicionales ni cambia el layout o el tipo del header. Los mÃ©todos son static/cdecl; el banco mide ESP y EBX/ESI/EDI/EBP en ambas rutas.

## Colaboradores reales y conmutaciÃ³n

Los dos colaboradores de audio ya reconstruidos permanecen RH ON+locked durante los dos lados. `HelperHookRoute.hpp` identifica sus cuerpos usando el contenido guardado de RH, demuestra que entrada y sÃ­mbolo resuelven al cuerpo del mismo ASI, comprueba sus bytes antes/despuÃ©s de cada llamada y restaura enabled/locked y cÃ³digo al terminar. La entrada de CStats se invoca siempre en la misma direcciÃ³n del EXE, con su RH OFF para original y ON para candidato. No hay stubs ni sustituciÃ³n del array retornado por el manager.

El banco usa el engine y manager reales del juego cargado, guardando sus bloques completos: CAudioEngine de 0x1FD8 bytes y CAERadioTrackManager de 0x370. SÃ³lo prepara los catorce tiempos del manager y la lista de CStats; los demÃ¡s bytes de ambos objetos deben permanecer idÃ©nticos. Se guardan/restauran ademÃ¡s la lista y mÃ¡rgenes de 16 bytes, el control x87 y las rutas. No inicia mÃºsica, retunea la emisora, llama Service ni recurre a archivos o asignadores del juego.

## Cobertura

Cada funciÃ³n tiene 16 perfiles Ã— 128 variantes = **2048 casos**. Los patrones incluyen cero, Â±1, INT_MIN, INT_MAX y otros valores signed de ambos signos. No se limita a punteros nulos ni a ceros.

| FunciÃ³n | Perfiles |
| --- | --- |
| Populate | Array completo en cero; Ãºnico valor distinto en cada una de las catorce posiciones, incluida emergencia/off; mezcla de todos los campos. Destino previamente envenenado con 0xA5/0x5A. |
| Most | NingÃºn candidato positivo; cada uno de los doce Ã­ndices como candidato; empate de todos; empate de dos Ã­ndices adyacentes; mezcla signed. Los Ã­ndices excluidos 0 y 13 se fijan a INT_MAX para detectar una inclusiÃ³n incorrecta. |

El oracle de Populate exige los 56 bytes de destino iguales al array de entrada y AL correcto. El oracle de Most calcula una bÃºsqueda independiente signed con comparaciÃ³n estricta y exige EAX completo. Ambas funciones comprueban que engine/manager no cambian, mÃ¡rgenes de CStats intactos, ABI y restauraciÃ³n. El registro contiene array de entrada, bloque crudo de CStats y retorno definido por funciÃ³n.

En Populate los 24 bits superiores de EAX no pertenecen al retorno bool. No se comparan como resultado: se conserva EAX entero en un evento `raw_abi_return` y se aplica exclusivamente mÃ¡scara 0xFF a AL. En Most se aplica mÃ¡scara 0xFFFFFFFF, sin excluir ningÃºn bit. El analizador exige exactamente 8192 observaciones ABI, una por lado de cada par, y comprueba las mÃ¡scaras contra los resultados.

## Llamadores y alcance ABI

`dis-callers.txt` contiene los cuatro llamadores directos de Populate: 0x55B344 y 0x55B388 consumen AL con `test al,al`; 0x55C12C y 0x5D3B9E no consumen el retorno. Los tres llamadores de Most son 0x47BED3, 0x4D7184 y 0x55B35F. 0x4D7184 guarda AL; los otros empujan EAX a mÃ©todos de audio. Se incluye el receptor original de nombres 0x4E8380 y de retune 0x4E8290, con lecturas de byte y extensiÃ³n de signo posteriores. El banco conserva EAX entero para Most aun cuando esos receptores consuman AL. No se encontrÃ³ consumo posterior de flags ni de un registro volÃ¡til anterior sin recarga.

Esta prueba acredita sÃ³lo copia/selecciÃ³n de tiempos y las rutas de colaboradores empleadas. No acredita reproducciÃ³n, streaming de audio, persistencia de estadÃ­sticas ni las clases completas. No se ejecuta mientras el administrador de radio avanza un frame: todos los pares corren dentro de la actualizaciÃ³n instrumentada, con eventos escritos despuÃ©s de restaurar los bloques preparados.

## Evidencia final

La propuesta ya esta aplicada y registrada; no volver a aplicar source-proposal.patch.

[Registro](runs/20261010-202345-539585/comparisons.jsonl.gz), [resumen](runs/20261010-202345-539585/summary.json), [aprobacion](runs/20261010-202345-539585/approval-audit.json), [resultado](runs/20261010-202345-539585/RESULTADO.txt) y [auditoria independiente](audit-runtime.md).

SHA-256 del registro descomprimido: `677d56d37d83adf401e75a7fe02b292e12fab8c99c896e645ccfea8d6168b11f`.
SHA-256 del ASI ejecutado: `eb054a66b30aa9c9fe468f3fb384b71e4bde5ad6d75da97357273d20e3c49eb0`.
SHA-256 del EXE declarado: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Las 8192 observaciones ABI y todos los controles pasan. Partidas sin cambios y configuraciones restauradas segun el runner. El control x87 comprobado no equivale a guardar toda su pila o MXCSR. El alcance sigue limitado a copia/seleccion de tiempos y colaboradores empleados; no acredita reproduccion ni clases completas.
