<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# COFF: BSS y consumidores indirectos

El hash distinto no representa cambio real de staticrefs. Los destinos afectados residen en .bss, con PointerToRawData=0, flags0xC0300080, sin relocations: Birds40bytes y Bright20bytes. El parser previo extraia data[0:size], bytes del encabezado COFF; diferencias offsets8/9 son timestamp bigobj. BSS no contiene payload: debe representarse con ceros declarados. El coordinador ya corrigio parser compartido manteniendo gate hash completo; esta auditoria no cambia script.

bss-header-evidence.json conserva descriptores, simbolos y ausencia de relocations. staticref-initializer-proof.json compara ocho inicializadores dinamicos (3Birds+5Bright), bytes y relocations identicos. No hay motivo para omitir hashes ni reducir arbitrariamente al tamano de un simbolo en estos objetos.

spec-eight-callers-extra-jet.json contiene ocho consumidores unicos. Simbolo real ?Render@tBrightLight@@QBEXXZ contiene ambas llamadas; CBrightLights::Render conserva cero llamadas directas. JetProcessPed solo sinMul; RenderJetPack llama lambda con sinMul y cosMul.

Simbolos antiguos/nuevos observados:
?RenderJetPack@CTaskSimpleJetPack@@QAEXPAVCPed@@@Z
??R<lambda_1>@?4??RenderJetPack@CTaskSimpleJetPack@@QAEXPAVCPed@@@Z@QBE@PBD_N@Z

caller-proof-eight-extra-jet.json acredita igualdad COFF de ocho rangos: bytes y relocations salvo aliases autorizados. Lambda Jet mantiene calls offsets198/234. RenderJetPack conserva llamada lambda sin alias adicional. Evidencia estatica, no validacion juego ni FP universal.

Sin build, GTA, Git ni produccion. No se modifica comparador ni bancos durante corridas.
