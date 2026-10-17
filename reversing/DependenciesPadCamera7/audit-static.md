<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría estática independiente — PadCamera7

Fuente aplicada; revisión de sólo lectura, sin build ni juego ni cambios a instrumentos. La admisión requiere corrida y auditoría dinámica completas. Identidades de registro coinciden entre CPad::InjectHooks, banco y declaraciones: GetCameraCycleUp5404A0, GetMouseCameraStickHorizontal540E80, GetMouseCameraStickVertical540F80.

## Fidelidad al EXE

Horizontal: 67 instrucciones normalizadas de la fuente coinciden con las 67 de disassembly.txt. Vertical: 69 coinciden con 69 direcciones únicas (el dump contiene 540F80 duplicada por ser inicio y límite del bloque anterior). Sólo se normalizan etiquetas, ceros de hex y destinos call/jmp: helper50A850 y CRT821B40. La fuente usa puntero miembro real para helper y dirección nativa para CRT. No se altera orden de x87, NEG/DI de vertical, ni limpieza de pila en tailcalls. Retornos explícitos540F7A,540FD7/DE,54103B; tailsCRT balancean pila original.

Ciclo usa dispatch local equivalente a tabla5404DC: 0/2/3→5404B3,1→5404C1, fuera0..3→5404D8. Conserva comparación no cero y old=0 y AL inválido. No salta a código de juego para sustituir la implementación. Alias semánticos documentados; no se presentan como nombres originales recuperados.

## Fixtures y contratos

CPad privado completo134+guardas, alineado; campos leídos están dentro de134. No requiere constructor ni pools porque estos cuerpos únicamente leen esos campos. Modos0/1/2/3/invalid y estados cero/no cero positivos/negativos ejercen todas las rutas del ciclo. Ejes cruzan 128 valores int16, extremos -32768/32767, ambos signos, igualdad/vecinos50/100, inversión, cámara0/4/7/FFFF, flag mouse y disable/botón. Retorno se compara AL para bool y AX para int16, respetando bits indefinidos.

El oracle usa int16 para inversión, incluyendo -32768; umbrales estrictos >100 y >50 y división hacia cero. Se exigen las tres rutas 0, magnitud>100 y mouse>50; signos/fronteras están en los fixtures aunque el resumen agrega categorías. Horizontal mantiene inversión irrelevante. Helper originalOFF/candidatoON; Resolve del puntero miembro debe coincidir con Body capturado y mismo módulo ASI. Esta comprobación evita tomar un ILThunk parcheado como cuerpo reconstruido. CRT real pertenece al EXE; no mocks.

ABI thiscall ECX pad y sin argumentos: se miden ESP/EBP y EBX/ESI/EDI; coste FP se compara en top/control x87. Guarda FNSAVE/FRSTOR y MXCSR; cámara real B6F19C completa, mouse B73400..5F y flag B6EC2E se restauran antes de publicar y al final, junto con cuatro estados RH. Entradas privadas/globales permanecen readonly. Ningún defecto bloqueante estático identificado; igualdad dinámica y restauración siguen necesarias. README aún dice propuesta sin aplicar: estado editorial anterior, no evidencia de ejecución.
