<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8929 funciones reconstruidas; 8929 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Alias adicional: CMentalState::Process 0x6008A0

Revisión estática del 10/10/2026, sin reconstrucción ni aprobación. Este destino está fuera de la cohorte congelada de 499 y no aumenta ni reduce sus pendientes.

El wrapper actual de MentalHealth.cpp pasa únicamente this. La entrada original necesita también un CPed*: 0x6008BB carga [esp+44h] después del prólogo, 0x6008BF lee ped+0x540, y 0x600B23 devuelve con RET 4. El contrato actual sin argumento puede leer una dirección arbitraria y desequilibrar la pila. A diferencia del alias IncrementAnger 0x421050, aquí el this sí corresponde a CMentalState: escribe +0x10/+0x12 y copia +0/+1/+0x11/+0x13.

El cuerpo completo 0x6008A0..0x600B25 contiene creación de eventos de salud y enfado, llamadas al constructor/destructor base 0x4AC450/0x4AC480 y al grupo de eventos 0x4AB420. El prólogo instala el manejador original 0x83E768. No basta validar la rama sin eventos, ni cambiar solamente la firma y marcar la función validada.

En las fuentes actuales no hay invocaciones de CMentalState::Process. Se conserva pendiente hasta poder reconstruir el contrato Process(CPed*) y probar eventos reales, referencias, efectos sobre el grupo de eventos, ABI y restauración. La revisión no activa el wrapper ni instala un hook.

Reproducción: reversing/_handoff/dis.sh 0x6008A0 0x600B60 sobre el EXE canónico. Se empezó en la entrada real para evitar desensamblar desde la mitad de una instrucción. La auditoría actual de llamadas explícitas contiene la fila 0x006008A0 con cero aristas desde el catálogo; es un hallazgo adicional y separado del progreso de las 235 funciones de juego de la cohorte.
