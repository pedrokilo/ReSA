<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoria independiente de StatsRadio7

Corrida 20261010-202345-539585. Lectura integra del gzip archivado sin ejecutar el analizador, compilar ni ejecutar el juego.

- 4096 comparaciones con identificadores unicos exactamente 0..4095.
- Dos funciones, cada una con casos unicos 0..2047 y 16 perfiles de 128 variantes.
- Cero resultados nulos, cero desigualdades de objetos completos, cero equal/accepted falsos y cero restauraciones falsas por par.
- Ambos lados cumplen route_ok, helper_route_ok, stack_restored, nonvolatile_registers_preserved, protected_state_unchanged, expected_effects_correct y fpu_unchanged.
- 8192 raw_abi_return, uno por lado/par: mascara FF para Populate y FFFFFFFF para Most; EAX enmascarado coincide con return_bits en todos.
- Comienzo unico: dos hooks registrados, colaboradores sin stubs; helpers 0x507020 y 0x4E83E0 con cuerpos/modulo ASI identificados y helper_route_ok=true.
- Dos resumenes con 2048 casos y cero fallos; cierre unico con 4096 pares y estado restaurado.

SHA-256 calculado del JSONL descomprimido: `677d56d37d83adf401e75a7fe02b292e12fab8c99c896e645ccfea8d6168b11f`, coincide con resumen/aprobacion. ASI disponible en artifacts: `eb054a66b30aa9c9fe468f3fb384b71e4bde5ad6d75da97357273d20e3c49eb0`, coincide con manifest/ejecutado declarado. Los doce hashes de fuentes coinciden con manifest y archivos actuales al revisar; los cuatro instrumentos coinciden tambien. PDB ausente: su huella declarada no se verifico contra archivo.

La restauracion observada cubre engine/manager completos, lista de Stats y margenes, control x87 y rutas/flags/codigo de hooks. No acredita todos los globales del juego ni pila x87/MXCSR completos. El resumen declara saves_unchanged y configs_restored con tres huellas iniciales de partidas y dos de configuracion; no guarda huellas posteriores individuales. Estas propiedades se apoyan en el runner, sin volver a medir archivos vivos durante otra corrida. Tampoco se recalculo el EXE vivo.

El registro completo demuestra equivalencia de las dos entradas en todos los casos declarados, sin pases nulos ni diferencias ocultas. No amplifica la aprobacion a reproduccion, streaming, persistencia o clases completas de audio.
