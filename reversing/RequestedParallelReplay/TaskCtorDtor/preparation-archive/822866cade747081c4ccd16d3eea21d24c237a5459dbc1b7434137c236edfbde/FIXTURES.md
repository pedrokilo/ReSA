# Constructor/destructor de tres tareas: banco preparado

Sin compilar ni ejecutar. No modifica fuentes o registro. Activador requested-task-ctor-dtor.start; JSONL requested-task-ctor-dtor.jsonl. Namespace RequestedTaskCtorDtorLive::Update para FrontendIdle temporal. Seis funciones x256 seeds =1536 pares.

| Clase | Constructor / tamaÃ±o cuerpo | Destructor / tamaÃ±o cuerpo | Vtable / sizeof |
|---|---|---|---|
| CTaskComplexKillPedFromBoat | 0x6227C0..0x622816 /87 |0x622830..0x622889 /90 |0x86DA98 /0x10 |
| CTaskComplexHitPedWithCar |0x6539A0..0x653A11 /114 |0x653A30..0x653A89 /90 |0x86F294 /0x28 |
| CTaskComplexSunbathe |0x631F80..0x632035 /182 |0x632050..0x6320E2 /147 |0x86E0AC /0x38 |

Todas seis son S y validated=0 en hooks.csv al preparar. Combo completo de cada identidad guardado, pseudocÃ³digo y ensamblado exacto. Retornos constructor EAX=this, this ECX; ret4 Boat y ret8 Hit/Sun. Destructor de objeto (no scalar-deleting), ECX this, ret0: no se compara EAX porque no contrato EXE. Fuente wrapper retorna this incidentalmente.

Fixtures de almacenamiento privado alineado. Null/nonnull entidad en constructor/destructor; subtask null/nonnull en destructor (vtable falsa de7slots con scalar deleting destructor en slot0, registra flag1 sin liberar pool); flags de referenciasBeach/Sun en todas4combinaciones, towel object type explÃ­cito, Ã­ndicesanim3/5. Fill previo depende seed: comparar todos los bytes de objeto excepto vptr, cuyo valor se normaliza exclusivamente a pares de direcciones GTA/DLL acreditados derived/CTask/CTaskComplex. Vptr desconocido falla. Payload conserva padding y bytes no inicializados. Se comparan entidad privada completa, trace ordenado y retorno vÃ¡lido.

El banco sustituye simÃ©tricamente RegisterReference, CleanUpOldReference y GetAnimationBlock/GetAnimationBlockIndex/RemoveAnimBlockRef por stubs EXE+sÃ­mboloDLL+cuerpo tras saltos. Son contratos locales; NO equivalencia de gestores de referencia/animaciÃ³n. Stub Cleanup registra objectType observado y offset campo para detectar orden de efectos. Objetos fake no circulan por pools, streams o mundo real. No modifica globales de gameplay; Hit ctor EXE lee constante float86F118 y fuente usa DownTime1000, pendiente comprobar esa constante en evidencia hashBound antes replay.

## Diferencias estÃ¡ticas que se espera detectar

- Sun ctor EXE hace GetAnimationBlock(beach), GetAnimationBlockIndex(beach), GetAnimationBlock(sunbathe), GetAnimationBlockIndex(sunbathe). Fuente hace index antesblock para cada nombre. Trace lo conserva y falla si no corregido.
- Sun destructor EXE Cleanup(towel) antes escribir objectType3. Fuente escribe type3 antesCleanup. StubCleanup captura type para que se vea diferencia.
- Hit ctor EXE guarda ftol(float86F118) en DownTime; fuente fija1000. No aceptar equivalencia global si la constante o alcance de memoria es distinto.

## RestauraciÃ³n

Se capturan flags de todoshooks y 52bytes entradasGTA/candidato simples+virtuales; 11slots completos de vtablesGTA/DLL de cada clase, mÃ¡s direcciones de stubs5bytes. ExactSwitch verifica memcmp contra buffers Original/Hook del mecanismoSimple cada llamada. DestructorEntryPatch ocurre mediante patches.clear ANTES recuperar todosflags y ANTES complete. RestauraciÃ³n compara rango por rango y registra direcciones fallidas. No se modifica slot virtual alguno ni se llama destructor de fixtures vivos automÃ¡ticos; storage privado se descarta al terminar cada comparaciÃ³n. No cambios fuentes compartidas.

## Pendiente antes aceptar

Integrar build/runner Ãºnico del coordinador, corregir desvÃ­os acreditados si autorizados, congelar fuente/instrumentos/dependencias antesbuild, _CL_=/MP6 serie, REAfresh de estas identidades, manifesthash realEXE/ASI y anÃ¡lisis de1536pares. Snapshot52bytes excede body real y sÃ³lo verifica restauraciÃ³n. Retener fallos, no editar resultados para ocultar diferencias. El constructor/destructor de subtask sÃ³lo se simula como collaborator; no confirma semÃ¡ntica del pool ni recuperaciÃ³n de referencias real.

## Instrumentos de ejecución preparados

build_probe.py integra Frontend.hpp sólo temporalmente, compila Debug en serie con _CL_=/MP6, comprueba el congelado y recupera app_game.cpp/dllmain.cpp y binarios anteriores. run_local.py instala copia porhash del ASI/PDB, usa entradaWinMain original y SaveGameToLoad=-2, ejecuta una sola sonda1536 en proceso nuevo y preserva resultados. No se ejecutaron estos scripts.

El analizador exige los256seeds de cadaidentidad,1536pairIDs distintos, retorno constructor=this, conmutación exacta y trace esperado (incluye orden de consultas y cleanup observando toweltype5). Constructor debe terminar en vptrderived y destructor en vptrCTask: CTaskComplex intermedio NO se acepta como terminal aunque se conozca su dirección. No se ignora vptr ni padding.

REA nuevo: las seis identidades obtuvieron dossier ok=true; referencias exactas y hashesraw en rea-evidence-references.json. Es evidencia estática, no replay. bank-freeze.json congela12archivos preparados; ningún resultado validado nuevo se afirma aquí.
