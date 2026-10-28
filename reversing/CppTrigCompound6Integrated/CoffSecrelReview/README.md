<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Soporte SECREL estricto separado

0xB = IMAGE_REL_I386_SECREL: desplazamiento32 del símbolo desde inicio de sección, usado por TLS/debug. En C3dMarkers::Render aparece en código real section1783/328bytes, offsets77/172, símbolo externo __Init_thread_epoch y cmp registro-memoria TLS. No proviene de metadata confundida ni límites erróneos. Ambos objetos contienen mismos campos.

compare_coff_secrel.py separado acepta ancho4 y conserva bytes crudos para0xB (no los normaliza). Sigue comparando offset/type/symbol/addend/destination/referent y sólo admite aliases de helpers E8 REL32. No se omite relocation ni se amplía igualdad semántica. Patch separado, compare_coff.py original intacto.

Corrida offline read-only spec13: 11/13 iguales. Render ya pasa. PlaceMarker y CreateNumberOfBirds difieren en bytes y hashes de sección propia: no se acepta ni normaliza; coordinación debe corregir fuente/codegen. Evidencia completa caller-proof-secrel.json.

[Microsoft PE Format](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format) tabla Intel386 define0x000B32bitsectionoffset/TLS. Sin producción/build/GTA/Git.
