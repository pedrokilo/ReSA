<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditor independiente IntegerMinimal2

No importa runtime_runner ni probe_case. Lee JSONL completo; acepta exactamente2 funciones y4608 pares World4 o2funciones y4096pares World10. Verifica original/candidate dict no nulos e igualdad completa, casos y indices completos sin duplicados, rutas reales, todos gates anidados, resumen porfuncion0fallos, restauracion final. Recalcula seedsEAX, high24/AL, ECX/EDX, argumentos fisicos, flags definidos y bytesvecinos setter. AF de XOR no se compara porque indefinido; no omite ninguna bandera definida. raw_evidence.patch es obligatorio, ya entregado a root; no aplicado poreste subagente.

Pruebas: python -B reversing/CppIntegerMinimalStudy/test_audit_runtime.py
Resultado: dos campañas sinteticas completas admitidas y14corrupciones rechazadas. Ambas mitades se corrompen igual para evitar pruebas triviales de paridad.

Comando, reemplazando rutas de evidencia REAL:

    python -B reversing/CppIntegerMinimalStudy/audit_runtime.py COMPARISONS.jsonl --kind 4 --manifest BUILD-MANIFEST.json --asi COPIA-DEL-ASI-EJECUTADO.asi --repo D:/Original/gta-reversed-master --bank D:/Original/gta-reversed-master/reversing/CppIntegerWorld4

Para World10 cambiar kind10 y bankCppIntegerWorld10. El ASI debe coincidir SHA256 manifest; sources e instrumentation se comprueban contra archivos en disco. El auditor requiere CMake, hooks.csv, source/StdInc.h y ambos headers de TU ademas deTU/hooks/instrumentos. Es valido ejecutarlo inmediatamente antesotrasmodificaciones o sobre snapshot de fuentes exactas; NO contra checkout posterior fingiendoqueeselcompilado. Genera hashJSONL/manifest/ASI en resultado; conservar salida juntoevidencia.

Riesgos pendientes: opciones reales delobjetoTU/headers macro pueden cambiar cuerpo respectodelestudio aislado; exigir objeto REAL 10/3bytes yrelocs antesGTA. World4 bancoheredado sóloverifica TOP/CW alfinal yrestauraFP108; no demuestra todos bytesFP108postPump. El auditor preserva alcance existente yno inventa esa cobertura. Los bancos no publican cada registro no volatilenumero: boolABI está sustentado por instrumentationasm aunqueauditor sólopuede comprobar booleano. Mismatchdehashmanifestporfuentesmutadas necesita evidenciaarchivada, no override. No registra validacionni ejecutajuego.

Correccion de auditor: tabla REAL World4 exige512setter+4096garajes=4608pares; World10 exige2048+2048=4096. Total8704. Las pruebas sinteticas usan W4/W10 reales. La formula counter=case%51 ymatrix_position=bool(case&1) fue verificada contraHPP yregistroREAL. El fallo inicialcaseidentity fue conteo erroneo delauditor, no diferenciafuente/banco.

CLI/archivo requieren ahora --runner-summary SUMMARY.json --game "E:/JUEGOS/GTA San Andreas - CVehicle Test". Revisan inventorieshashsaves/configs con archivosactuales restaurados; ejecutar tras terminar restauracion yantes otra campaña los cambie temporalmente. El archive conserva runner-summary.json sin alteracion ypublica hash+inventoriesverificados.
