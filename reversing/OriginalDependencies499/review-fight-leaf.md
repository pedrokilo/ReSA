<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Próximo lote: CTaskSimpleFight (revisión de lectura)

No se modificaron fuentes ni bancos compilados durante PedGroups2. InjectHooksMain ya llama CTaskSimpleFight::InjectHooks en source/InjectHooksMain.cpp:1339; la clase tiene RH_ScopedVirtualClass(86D684,9), pero no registra las tres hojas.

## Tres hojas aislables

Todas reciben this en ECX, no argumentos de pila y terminan con ret sin limpieza. Sólo leen this+24 (combo signed int8), this+25 (movimiento byte) y la tabla CMeleeInfo[12] en C170D0, stride88h. GetComboAnimGroupID lee dword+0; IsComboSet/IsHitComboSet leen word+84h.

- GetComboAnimGroupID 4ABDA0..BDD: movsx eax,[ecx+24]; resta4; max(resultado,0) mediante setl/dec/and; multiplica88h; devuelve dword de tabla. ECX termina en 0 cuando combo<4 y FFFFFFFF cuando combo>=4; EDX intacto.
- IsComboSet 4ABDC0..DED: lee movimiento en CL, conserva los 24 bits superiores de ECX; calcula índice con combo signed; zero-extiende flags; EDX=10h<<(movimiento&31); test flags/mask y setne AL. El bool está en AL: los bits superiores de EAX conservan flags. No convertir el resultado asm a un bool C++ y asumir que EAX entero es idéntico.
- IsHitComboSet 4ABDF0..4ABE2B: cmp CL,2/jg firmado. Movimientos 3..127 retornan cero sin consultar tabla; negativos -128..-1 sí pasan y usan desplazamiento x86 modulo32. Rama evaluada: EDX=1000h<<(movimiento&31), devuelve EAX=0/1. Rama temprana preserva EDX; ECX conserva sus 24 bits superiores y sustituye CL por movimiento.

Banco propuesto DependenciesFightLeaf3: tres hooks RH_ScopedInstall con rutas OFF/ON mediante HookRoute, 2048 pares por función (6144 total). Buffer this raw de28h bytes con relleno determinista, sin llamar constructor/destructor porque las hojas no leen vtable ni referencias. Combo de -128..15 cubre clamp y los doce elementos; valores positivos >15 se excluyen cuando hay lectura de tabla por estar fuera del tamaño real. Para rama temprana de IsHitComboSet pueden añadirse combos >15 porque no acceden a tabla. Movimientos cubren todos los 256 bytes, incluidos negativos, 2/3 y alias de desplazamiento. Tabla original se fotografía y se repone tras cada llamada; casos con flags 0/FFFF, cada bit y mezcla, IDs enteros deterministas.

Capturar EAX/ECX/EDX/ESP inmediatamente después de call, sin helpers intermedios. Para IsComboSet el retorno semántico es AL, y el EAX completo se registra como efecto ABI original; los otros dos usan EAX entero. Comparar this y tabla completa, exigir route_ok y stack_restored; registrar function_summary por hoja, pair global único y original/candidate no nulos. No requiere colaboradores ni stubs. Implementación naked thiscall es la opción más directa para preservar también los efectos exactos de registros.

## Constructor/destructor: lote posterior

Ctor61C470 termina ret0C; usa base61A390, deja padding sin escribir, combo/movimiento FF, finished0/control1/animsReferenced0, requiredGroup21h, idleCounter0, strike/chain0, anims null, nextCommand byte y lastCommand0; almacena idlePeriod=min(argumento unsigned,60000) en word+10. Target no nulo registra referencia this+18 mediante571B70. Devuelve this en EAX y restaura ECX con el enlace SEH, no this.

Dtor61C530 termina ret sin argumentos: target no nulo limpia referencia571A00; cada anim no nula pone DeleteCallback(4D1390,null) mediante4CEBC0; si animsReferenced y requiredGroup!=21 calcula índice real de animblock desde tablaB4EA34/B5D4A0 y RemoveAnimBlockRef4D3FD0, limpia byte+0A; llama base61A3A0. El destructor no permite un buffer con punteros inventados o sólo probar ramas null.

Se pueden reconstruir después con fixtures de CEntity y CAnimBlendAssociation reales y un animblock válido cuya cuenta de referencias se snapshot/restaure; revisar constructores de esos objetos antes. No registrar aprobación del ctor/dtor con un banco restringido a target/anims nulos y animsReferenced=false. No se propone esa aprobación parcial.
