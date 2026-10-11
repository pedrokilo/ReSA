<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Siguiente lote de tareas/peatones: revisión de lectura

TaskLeaf3 está en compilación/prueba; no se cambiaron sus fuentes ni instrumentación. Sólo se actualizó su README para reflejar la propuesta aplicada, sin aprobación.

## Prioridad alta: constructores completos que sí son aislables

**CTaskInteriorSitInChair::Constructor 675C30..675C79.** Una sola ruta, ret0C. Base61A390 sólo escribe parent+4=0/vptr; ctor cambia vptr870314, almacena argumentos en8/C/10 (último byte), pone anim14=0, campo18=-1, timer1C/20=0 y bytes24/25/28/29/2A=0. Conserva padding11..13,26..27,2B. Devuelve this en EAX, preserva ESI y demás no volátiles. No registra referencias ni lee los argumentos almacenados: buffer2Ch prellenado y argumentos apuntando a registros propios válidos permiten cubrir TODA la función, sin stubs. Clase fuente es aún plantilla sin campos/constructor/VALIDATE_SIZE: antes del hook debe declararse layout2Ch exacto y wrapper Constructor. Un banco1024 pares compararía objeto entero normalizando sólo vptr, devolución this, ESP y registros. Puede validar esta función sin aprobar la clase completa; no incluir el destructor sólo porque constructor deja anim nulo.

**CTaskComplexWaitForPed::Constructor 683340..6833B3.** Una ruta normal con ped NO nulo, ret10h. Base61A3B0 escribe parent+4=0, subtask+8=0; ctor vptr8706F8, target+C, radius+10, time+14, bool+18; timer1C/20=0 y bytes24/25=0; padding19..1B/26..27 y frames+28 quedan sin escribir. 683399 llama RegisterReference571B70 sin test de null: NULL no es caso válido de entrada y causaría acceso nulo en original. Fixture ped real por layout con type3 y arena privada CReference (como TaskLeaf3), pEmptyList snapshot/restauración y disponibles garantizados; variaciones de referencias preexistentes/duplicadas/tipos aplicables. Comparar bytes2Ch, referencia apuntando a this+C, nodos y todos registros/ESP. Constructor actual no está declarado; CTaskTimer tiene constructor/inicializadores que deben revisarse para no borrar padding. Puede cubrirse íntegramente sin ejecutar Process ni esperar frames.

## Destructor requiere más trabajo antes de admitirlo

WaitForPed dtor6833D0..68342A limpia target mediante571A00 si no nulo y llama basecomplex61A3D0: puede destruir subtask por virtual. Fixture sólo recién construido (subtask nulo) no acredita esa rama de vida útil. Necesita subtarea REAL de tipo conocido, referencia limpia, pool y destrucción exactos y restaurables; no un vtable stub.

SitInChair dtor675C90..675CED cambia callback de anim14 no nulo mediante4CEBC0(4D1390,null), luego basesimple61A3A0. Todos los caminos serían aislables con CAnimBlendAssociation real sin enlazar a clump, constructor/layout validado y snapshot del callback. Aún no se propone aprobarlo usando anim nula exclusivamente.

## Prioridad posterior

ControlButtonSprint60A610..60A81B es mayor: playerData+480, player index+598, PlayerInfo53FB70/GetPad5407F0/5407A0, tabla8D2460 stride20h, timestepB7CB5C, x87 clamp y ResetSprintEnergy60A550. Esta última puede tocar tareas/animaciones por ped. Banco con playerData nula solamente sería parcial. Requiere datos de jugador/pad reales o temporalmente intercambiados, todas ramas de sprint y reset, snapshots y ausencia de stubs. Dificultad media/alta.

CPedSaveStructure::Extract5D44B0 continúa después de lo revisado hasta modelos/armas y createdBy5E47E0, requests743C60: no es la inversa puramente copiable de Construct. Requiere streaming y ciclo de armas reales; alta dificultad, no proponemos nulificar las13 armas para ocultar colaboradores.

Helpers GangDriveBy627B20/627CC0 y selector406E80 siguen sin identidad confirmada; no elegirlos por tamaño antes de demostrar destino y nombre. Formation69B1B0 tiene datos de clase confirmados pero nombre semántico; revisar declaración/API y dominio antes de tratarlo como función nominada.
