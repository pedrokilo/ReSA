<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Preflight TaskCtorDtor:1536 pares

Instrumento Debug compilado el8octubre2026. ASI SHA256 41fe3e63571149f0d0901865b9848ce870537a7ee2afaea345b4130f530d76da; fuentes,instrumentos yEXE hashados en artifacts/manifest.json. WinMain.cpp y binariosDebug anteriores restaurados yverificados porfinally. Autor noejecutóGTA; replayserial correspondecoordinador.

WinMain instrumentado llama harness antes inicializargráficos/input y retorna0: hilo inicial, DLLattachcompleto, loaderliberado yCRTdisponible. No acreditaarranque niWinMain original. Runner SaveGameToLoad=-2, sinselectors, wait_for_exit0 despuéscomplete.

Fixtures privados cubren ctor/dtorBoat,HitCar,Sunbathe x256. Referencias/animaciónstubs simétricos EXE/DLL/cuerpo; subtask vtablefake ydeletingstub, no taskpool ni entidadesreales. ConstanteHit86F118=1000.0boundEXE en hit-downtime-constant.json. Toweltype5 inicialválido y4combinacionesrefsanim.

Harness snapshotallflags, entradas52bytes yvtablesGTA/DLL de todacategoríavirtual, antesdisableall salvoWinMain ejecutando. VTableRanges.hpp454metadatas cubre397categoríasvirtuales runtimeactual; unknown abortaantesmutar. Banco cambia sólo targettemporal yrestaura. Harnessrestauraall+verificabytes/vtables/flags antesreturn0. CopiaTaskCtorDtorBank.hpp arregla namespace std::bit_cast einclude paths, preservandoheaderinstrumentaloriginal.

Primerbuildfallóbit_cast(no namespace); build-first-failure.log preservado. Segundobuildsucceeded. FuenteSunbathe aún tiene dosdesvíosdeorden y se espera repro; no se modificó fuentefuncional ni flagsvalidación.

Integridadoldbank: hpp/py congeladosintactos. FIXTURES.md cambió por actualizacióndocumental central y no coincidecon hashantiguo: se documentaexcepción en postbuild-integrity.json, sinactualizarelhashsilenciosamente. Manifest nuevo usa sólo instrumentos/fuentes, no eseMD. Analyzer exige1536pairIDs,256seeds/identidad,traceexacto,vptrterminalderivedctor/CTaskdtor ymemcmprestoration. Noejecuciónaprobada significa no validaciónfuncionalnueva.
