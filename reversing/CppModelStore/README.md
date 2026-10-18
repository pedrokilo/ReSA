<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Get2dEffectStore: conversión C++ aprobada

APROBADO: corrida 20261011-005646-878564, 1024 pares propios y 1024 acompañantes, cero diferencias y restauración verificada. La regresión CppModelInfoRegression pasó otros 5264 pares sobre 28 métodos. Evidencia archivada en runs; registro en CppOnlyMigration/approvals.json.

Conversión aplicada, compilada y comprobada; registro separado de migración, sin incremento de la cohorte.

## Alcance

| Función | Dirección | Pares | Papel |
|---|---|---:|---|
| CModelInfo::Get2dEffectStore | 0x4C5A60 | 1024 | Única conversión |
| CMemoryMgr::InitScratchPad | 0x72F480 | 1024 | Acompañante; no se convierte |

Total comprobado: 2048 pares. Solo 1024 corresponden a la migración. Se eliminaron dos apariciones del inventario (naked y asm) de una función; no se atribuye conversión a InitScratchPad.

## Evidencia y contrato

EXE canónico GTA SA 1.0 US, SHA256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Lectura con dis.sh: 004C5A60 MOV EAX,0B4C2D8h; 004C5A65 RET. El cuerpo no lee memoria, no toma argumentos, no cambia ECX/EDX ni x87 y devuelve el puntero completo en EAX.

La propuesta devuelve ese puntero absoluto mediante reinterpret_cast; no llama al cuerpo GAME ni a una biblioteca. La firma y el registro RH permanecen iguales. El compilador debe producir el retorno sin modificar ECX/EDX: la inspección final codegen-proof-final.json confirma PUSH/MOV EBP, MOV EAX constante, POP EBP y RET, sin aritmética ni cambios ECX/EDX/NV/flags/x87. Las 1024 semillas pasaron. El primer objeto codegen-proof.json se rechazó por SUB ESP bajo /ZI antes de GTA; /Zi con PCH resolvió ese efecto.

## Banco separado

Derivado de DependenciesLeaf2 sin modificar sus instrumentos. Prefijo cppmodelstore; namespace CppModelStoreLive. Conserva comparación par a par EAX/ECX/EDX, semillas variables, ESP, ruta RH EXE/DLL, bytes de heap/scratch del acompañante y restauración de ambos hooks y estados. Se exige también que el puntero devuelto sea 0xB4C2D8. Ambos registros original/candidate son objetos de datos, nunca placeholders nulos.

El acompañante conserva las mismas 1024 configuraciones privadas coherentes de lista circular y restaura heap/scratch antes de escribir cada comparación y bombear mensajes. Este banco heredado no certifica todos los registros no volátiles, flags ni estado x87: hay que inspeccionar el cuerpo compilado para comprobar ausencia de operaciones que los modifiquen. No se amplía artificialmente su alcance.

build_probe.py incluye las fuentes ModelInfo y MemoryMgr y sus declaraciones en el manifiesto. Los scripts redirigen GAME en memoria a la instalación de prueba establecida; runtime_runner.py no cambia. Ejecutarlos corresponde exclusivamente al coordinador y de forma serial.

source-proposal.patch es una propuesta offline que solo reemplaza Get2dEffectStore en ModelInfo.cpp. La producción permanece intacta.

## Verificación offline realizada

Contexto exacto de la sustitución encontrado una sola vez en fuente actual; scripts Python analizados con ast.parse sin importarlos ni ejecutar comandos de build/juego. Sin resultados dinámicos ni aprobación.
