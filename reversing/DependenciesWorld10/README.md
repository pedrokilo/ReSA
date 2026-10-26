<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesWorld10 — aprobado y registrado

Dos destinos GAME de la cohorte, reconstruidos y validados dentro del juego en run 20261010-214328-227570. Propuesta aplicada por la raíz y banco aprobado/registrado.

| Dirección | Identidad descriptiva propuesta | Pares |
|---|---|---:|
|509CA0|CCam::GetBoatHeightForFirstPerson|2048|
|424100|CCarCtrl::Native_00424100|2048|

**4.096 pares verificados, cero diferencias y cero registros nulos.** Camera conserva18instrucciones completas hastaRET4 de509CD3/509CD8. Alias describe uso real por modo0x16 en Cam.cpp, no nombre binario recuperado. Es miembrothiscall, float*output; boolAL. Lee target21Ch, modelindex22h, modeltableA9B0C8, handlingid4Ah y filaGetBoatPointer6F5300 de managerC2B9C8. El cuerpo candidato llama al miembro fuente real mediante puntero; helperoriginalOFF/candidateON y Resolve(raw)==Body del mismoASI se demuestran, con32bytes ON/OFF antes/después. No se confunde ILThunk mutable con cuerpo reconstruido.

Fixture camera es copia completa238bytes de CCam realB6F19C, arena alineada y guardas, con targetNULL legal o vehículos existentes del pool. Nada fabrica entidades, modelos ni datos handling. Se requieren vehículos/modelinfos reales: si no existen, error explícito, sin aprobación de rama vacía. Vehículo, modelo completo308, cámara global y managerC624 se comparan readonly. NULLtarget conservaoutputsemilla; targetreal copia exactamente4bytes de altura en fila+20h. La rama de retornoNULL de GetBoatPointer es inalcanzable bajo su contrato: su EXE devuelve siempre LEA de la fila real [175..186] o fila0, manager fijo noNULL. Se conserva esa rama defensiva pero no se fuerza un helper falso para cubrirla. Esta validación no cuenta otra vez el helper ya catalogado ni promete su independencia transitiva.

424100 es exactamente XOR AL,AL / RET, no XOR EAX,EAX. Nombre únicamente por dirección; CCarCtrl es propietario **de instrumentación**, no una clase original inferida. Hookscopedglobal en CCarCtrl ya inyectado. Firma cdeclfloatx,y refleja Automobilecaller6B1D60..6F que apila posición2D y limpia8; cuerpo no lee argumentos. Se usan posiciones de vehículos reales del pool y ocho semillasEAX válidas para comprobar upper24bits intactos, AL0, slotsx/y intactos yABI. Callers reconstruidos ya eliminaron su efecto nulo; no se reintroduce conducta ni se simula una implementación desconocida.

Cada entrada registrada se compara EXE hookOFF contra fuente hookON. ABI ESP/EBP/EBX/ESI/EDI, fulloutput, readonly, guardas yFNSAVE108/MXCSR exactos antes de publicar y al final. Write preservaFP actual mientras espera y canónico antes/despuésPumpMessages/JSON. Restauración de tres rutasRH; no globals del juego modificados, sin recursos externos ni hooksCRT. Original/candidate siempreobjetos completos. Analyzer estricto exige4096pares/casos únicos,1024targetsNULL/real, ocho semillas, todosgates verdaderos ycomplete/restored.

La raíz aplicó la propuesta, compiló y ejecutó el banco en serie. La evidencia aprobada está en validation-summary.json y runs/20261010-214328-227570; auditoría independiente en audit-runtime.md. Garage449050 excluido hasta demostrar matrizNULL/allocador/modelos/esferas reales; no aprobación con conteo0. CanSunbathe632140 pertenece Task11 de otro agente, fuera de esta tanda. No tocar runtime_runner.py, frozen ni instalaciónpersonal.

## Corrección de precondición: exterior real

Los primeros dos intentos no produjeron pares: el jugador estaba en interior 3 de la casa de CJ y el pool real seguía vacío durante 60 segundos. No son diferencias de fuente y no se registran como validación. Las dos partidas disponibles en la copia aislada tienen posiciones de cámara en ese mismo interior.

El nuevo run_local usa NewGameConfiguration.py para configurar sólo este banco con FastLoader.SaveGameToLoad=0, opción documentada en source/extensions/Configs/FastLoader.hpp que inicia partida nueva mediante SimulateGameLoad(true,0). No se editan partidas ni runtime_runner.py. El helper guarda en memoria los bytes y hashes de ambos INI antes de ejecutar; el runner toma sus backups normales antes de solicitar los bytes temporales. En finally se restaura la función de configuración y se comprueban ambos hashes; si el runner fallara restaurando, el guard externo recupera los bytes y rechaza la ejecución. También permanece la verificación existente de saves sin cambios y configuraciones restauradas.

El banco espera jugador disponible, ausencia de cutscene, área exterior 0 y vehículos cargados naturalmente, con límite de 60 segundos desde esas precondiciones y diagnóstico cada 5 segundos. No construye actores ni carga modelos de forma artificial. Antes de cada devolución por espera restaura la imagen x87/MXCSR y desactiva el estado canónico para que el siguiente frame del juego avance naturalmente. El evento begin exige área 0 y documenta la configuración; el analizador los verifica. La creación de tráfico y recursos corresponde enteramente al juego normal de la partida nueva.

## Evidencia aprobada

- Run: `20261010-214328-227570`. Dos funciones, 4.096 pares, diferencias 0, registros nulos 0, estado/configuraciones/partidas restaurados.
- Log SHA-256: `617bdbc0a5347f96625580dc368d2da4143e621e7f4ca11263421d6a9372d235`.
- ASI SHA-256: `d3826d73a93d5453b1cb4fc8852bf142fc8c2ff00f69466e90c6c3c25af12313`.
- [Resumen](validation-summary.json) y [auditoría independiente](audit-runtime.md). Instrumentos aprobados inmutables; no reaplicar la propuesta.
