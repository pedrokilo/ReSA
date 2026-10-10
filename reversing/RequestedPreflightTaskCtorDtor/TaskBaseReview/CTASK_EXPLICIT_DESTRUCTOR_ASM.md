<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Destructorbase explícito: comprobación del código emitido

Cambio aplicado porcoordinador, sinedicionesTask.h/Task.cpp por esteagente. BajoDebug MSVC /Od, declaración virtual~CTask() ydefiniciónoutofline vacía producen storevfptr antesretorno.

ASI SmallPreflightTasks c5feca9509d3388cddcf4b214505c385993545ccb8083ce91507757be35b9847: export??_7CTask@@6B@=0x131E05E4. Export??1CTask@@UAE@XZ resuelvebody0x124861D0; ECX[this] se guarda[ebp-8], cargaEAX y en0x124861E2 escribe dwordptr[EAX]=0x131E05E4; ret0 en0x124861EE. Evidencia new-task-asi/.

ASI TaskCtorDtor corregido 6f13e4930f64ddee35b8d1f66a080a5fa82cf4625667b47157b6f02ee52f9557: exportVTCTask=0x131BA5E4; bodyordinarionuevo escribeexactoVT en0x12463DA2. Evidencia corrected-task-ctor-dtor-asi/. ElASI viejo41fe3e... no tenía storeCTaskVT enexportordinarydtor ni thunkvtableinspeccionado. No se normalizaCTaskComplex aCTask.

PEslot0 correspondethunk MSVC dedeletingdestructor y soportavariantesflags1/2/4 (vector/free); no sustituirlo con ABIEXEscalarret4sincomprobar. La pruebaestáticaimportante esordinarydestructor export??1CTask@@UAE@XZ ydataflowECX→[this], no unnombreinferido. inspect_ctask_destructor.py nunca cargaASI niGTA; resuelveexports ythunksPE ydecodificaCapstonehasta primerret. Laidentificaciónthis se examinó manualmente. No certifica todasramasfree niRelease optimizado.

Sun cambio confirmado mínimo: dosreordenamientos, GetBlockantesGetIndex yCleanupantesobjectType3; ningunaotrafuncióneditada porautor. Hashfuentesnuevo enmanifestbuilt1536, aguardareplayroot paraaprobar. SourceTask.cpp ahora incluidoexplícitamente enmanifest.

Segundo rebuild produjoASI válido pero finallytuvo OSError22 alreponerWinMain. Se recuperócontenidoHEAD sólo despuéscompararSHAoriginal_winmain_sha256 idéntico; binariosbaselineASI/PDBrestaurados yhashesverificados. finally-repair.json preservaexcepciónyrecuperación. No se perdió fuentedirty ymutexliberado alLeaftercerturno. AutornoejecutóGTA.
