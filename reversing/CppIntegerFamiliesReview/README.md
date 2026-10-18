<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revision offline: familias enteras y estructuras

Sin cambios de produccion, bancos, compilacion, juego o Git. Inventario leido como orientacion; contrastado con fuente actual porque contiene filas de trig ya convertido.

## Primera familia: Decision / Event / Relationship

- CDecisionMakerTypes::AddDecisionMaker 0x607050: recorre slots [0,15) o [15,20), primer libre; escribe flags de active/group, copia 41 decisiones de 0x3C bytes mediante operador assignment real, incrementa contador y devuelve indice o -1. Propuesta C++: bucle con limites exactos, tipos unsigned para offsets, assignment miembro por miembro o llamada tipada al assignment reconstruido. No delegar cuerpo Add al EXE. Banco DependenciesDecisionRelationship6: 2048 pares.
- RemoveDecisionMaker 0x6043A0: slot inactivo no opera; rama ped recorre pool de mayor a menor, salta marca negativa y llama SetDecisionMakerType con tipo firmado del ped; rama grupo recorre 8 objetos de 0x2D4 y actualiza coincidencias. Finalmente limpia flags y decrementa contador. Propuesta C++ mantiene recorrido descendente y orden de llamadas/stores. Mismo banco: 2048.
- LoadEventIndices 0x600840: llamada tipada ReadEventIndices(m_EventIndices, literal 0x86CD44). Es wrapper de colaborador, no reconstruccion adicional. Mismo banco: 128. Debe inspeccionarse firma exacta y retorno residual antes de convertir.
- CPlayerRelationshipRecorder::RecordRelationshipWithPlayer 0x61A1D0: busca primera coincidencia en 16 relaciones, la limpia; consulta tarea activa; conserva orden Kill/Follow/Kill y stores byte/puntero. Propuesta C++ con flags locales independientes y cortocircuitos identicos. Mismo banco: 2048.
- CEventSource::ComputeEventSourceType 0x4ABAC0: GetSourceEntity virtual, tipo ped, prioridad Threatened -> Friendly -> IsPlayer, retorno int32 3/2/1/0. Propuesta C++ directa con llamadas reales y orden exacto. Banco DependenciesTaskPed5: 2048.

Total seleccionable 8320 pares de consumidores. Riesgo decisivo: ambos bancos capturan EAX/ECX/EDX completos incluso void, ESP, no volatiles y SEH; comparan snapshots sin normalizacion. Remove/Add restauran ECX=this por push/pop; un cuerpo C++ ordinario no garantiza esto. No declarar estos cinco listos para aplicar. Primer ensayo aconsejable: EventSource (cdecl con retorno int32 y sin registro keeper explicito), conservando bancos y comprobando objeto frente EXE antes de ejecutar. Add/Remove necesitan estrategia compilador valida para registro this o codegen exacto; no wrapper ASM.

## Segunda familia: FightLeaf

GetComboAnimGroupID 0x4ABDA0, IsComboSet 0x4ABDC0, IsHitComboSet 0x4ABDF0; DependenciesFightLeaf2: 2048 cada una, 6144 total. Dis EXE 0x4ABDA0..0x4ABE2C coincide con asm fuente actual.

Calculo C++ propuesto: index=max(int(int8(combo))-4,0); group tabla[index].group; shift=uint8(move)&31; mask=0x10u<<shift o 0x1000u<<shift; hit solo si int8(move)<=2. Usar uint32 evita UB de shift/desbordamiento. Los fixtures incluyen byte firmado negativo y valores de shift grandes, no limitar dominio.

Bloqueo ABI real: GetCombo deja ECX=0 o FFFFFFFF y conserva EDX inicial; IsComboSet conserva parte alta EAX=flags&FFFFFF00 y sustituye solo AL por bool, ECX=this alto|move, EDX=mask; IsHit devuelve EAX canonico pero EDX queda seed si move firmado>2 o mask en otro caso. Banco verifica estas formas explicitamente. bool C++ canonico perderia EAX alto. Sin demostracion de codegen o estrategia compilador no presentar estas tres como faciles.

## Excluidos

MemoryMgr y allocator: investigacion root en paralelo. KeepEdx/KeepEcx wrappers de Clothes, EntryExit, Pickup, modelos, vehiculo y audio no son candidatos ordinarios: contrato de preservacion explicito. SEH manual de Vehicle/Plane/Bike y singleton Decision no son lote corto. Stats FindMostFavoriteRadioStation ya tiene propuesta previa pendiente, ECX max y EDX final15 contra asm fuente13: no duplicar.

No cambia ningun gate ni atribuye aprobacion runtime. La siguiente familia amplia utilizable es control entero Decision/Event/Relationship, pero la exactitud de registros sigue siendo condicion de admision, no detalle eliminable.
