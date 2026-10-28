<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Lote entero corto: candidatos y restricciones exactas

Revisión offline del inventario vigente: 951 apariciones asm y 298 naked, conteos léxicos, no funciones. No se aplica producción, no se compila ni se ejecuta GTA. Los wrappers push-dirección/ret se excluyen porque delegan GAME; AttractorScanner::Clear se excluye de lote entero porque ejecuta FLD/FMUL/FSTP incluso para slots luego sobrescritos.

## Candidatos propios sin FP ni llamadas

DependenciesFightLeaf2 ya mide tres identidades, 2048 pares cada una (6144 total), ret completo EAX, ECX, EDX, ESP, objeto/tabla sin cambios, rutas y restauración. Usar banco derivado separado conservando estos gates, nunca reutilizarlo debilitando registros.

- GetComboAnimGroupID 0x4ABDA0: MOVSX EAX,[ECX+24] en 4ABDA0; ADD -4 en 4ABDA4; XOR ECX en 4ABDA7; TEST/SETL/DEC/AND en 4ABDA9..4ABDAF; IMUL 0x88 en 4ABDB1; MOV EAX,[EAX+C170D0] en 4ABDB7; RET 4ABDBD. Valor C++: leer uint32 de tabla + max(int(int8 combo)-4,0)*0x88. EDX queda intacto; ECX final vale 0 si combo<4, FFFFFFFF si combo>=4. Una función C++ ordinaria puede devolver valor correcto pero NO promete ECX exacto: necesita estudio de código emitido antes de admitir.
- IsComboSet 0x4ABDC0: carga signed combo y CL movimiento; tabla uint16 en 4ABDDA; EDX=0x10 << (move&31) en 4ABDE1/4ABDE6; TEST 4ABDE8; SETNE AL 4ABDEA. EAX conserva byte alto de flags, solo AL se reemplaza. ECX conserva tres bytes altos del this y CL=move. Retornar bool limpio en EAX rompería gate histórico. La propuesta semántica debe conservar retorno parcial sin alterar firma oficial; requiere prueba concreta del compilador, no supuesto.
- IsHitComboSet 0x4ABDF0: CMP CL,2 signed en 4ABDF5, JG false 4ABDF8; máscara EDX=0x1000 << (move&31), TEST 4ABE1F. Devuelve EAX 0/1; ECX conserva upper-this + move y EDX conserva semilla en early false, máscara en rama consultada. Mismo obstáculo de registro observable.

Son el lote entero más corto encontrado con banco ABI explícito. Se proponen para estudio de compilador, no como conversiones ya viables. C++ estándar no proporciona acceso al registro ECX ni especifica el byte alto EAX en retorno bool; no introducir helpers asm para sortearlo. Si no se obtiene código y banco exactos, conservar naked y registrar el impedimento.

## Reserva más extensa

CMemoryMgr::InitScratchPad 0x72F480: puramente enteros, sin calls ni FP; inicializa heap y enlaza lista circular con lecturas de estado previo. DependenciesLeaf2 y CppModelStore contienen 1024 pares de esta función y comparan EAX/ECX/EDX/ESP más memoria. No sustituir por CHeap::Init sin demostrar orden y bytes sin inicializar. Es más largo y conserva resultados de registros accidentales; inferior prioridad a los tres getters para estudio.

## Documentación

model-regression-readme.patch elimina únicamente la contradicción pendiente/offline frente al encabezado APROBADO del README actual. Se deja para coordinación, sin editar el documento aprobado durante pruebas activas.

Evidencia EXE: dis.sh fue invocado pero no produjo texto en esta shell; se corroboró con el mismo dumpbin canónico /DISASM /RANGE 0x4ABDA0..0x4ABE25. No se usa el texto fuente como sustituto del EXE.
