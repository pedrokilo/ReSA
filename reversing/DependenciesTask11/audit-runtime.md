<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría independiente — DependenciesTask11

Lectura directa del gzip admitido `20261010-213032-855203`, sin ejecutar juego/banco ni modificar sus cuatro instrumentos.

- 4096 pares únicos (0..4095); dos funciones con casos 0..2047. Todos original/candidate son diccionarios no nulos, estrictamente iguales y equal/accepted=true: cero diferencias.
- 81920 gates verdaderos: ESP, no volátiles, SEH, RH OFF/ON, colaboradores, contrato, recursos, objeto original intacto, estado restaurado y FP.
- GetInstance: 1024 pares con singleton real existente y 1024 con NULL, cuatro modos de semilla por rama. Todos los retornos son no NULL e iguales al singleton global observado; identidad con el objeto original únicamente en rama existente. 2048 vidas naturales new/ctor/delete entre ambos lados, todas equilibradas.
- Cada constructor NULL verifica m_NoOfDecisionMakers=0, 25 matrices completas/1025 decisiones y los 80 bytes globales exactos. Las observaciones declaran 125960192 bytes definidos comprobados en total. Se comprobó que los registros excluyen 384 bytes EventIndices no escritos por el constructor; no se inventa igualdad para ellos.
- CanSunbathe: 2048 perfiles diferentes, 16 horas × 32 climas × cuatro semillas. Recalculado por separado el conjunto de climas y rango [10,18): 384 retornos true y 1664 false por lado, EAX exacto0/1 y ECX/EDX conservados.
- Constructor nativo completo415 bytes y helper meteorológico49 bytes: arrays canónicos del HPP coinciden con metadatos, prefijos observados de ambos colaboradores y rutas resueltas al módulo EXE. El instrumento compara los cuerpos completos antes/después/final; su gate native_complete_bodies_ok y el terminal son ciertos. No se presenta el array constante de metadatos como captura dinámica independiente.
- Diez rutas compartidas tienen módulo, bytes y gate ciertos; fin completo con4096 pares, restauración, original singleton, colaboradores, cuerpos canónicos y x87/MXCSR ciertos.
- SHA del log recalculado igual al registro. Huellas actuales: 15/15 fuentes y 4/4 instrumentos coinciden con admisión.

Log SHA-256: `9bc789a856a699e8f8c17c13ea9abb46851850e05844e573127de809b042f2b5`. ASI SHA-256: `bbaa8052ba3bc66e05dd33fc1b06e0e871b7bcb8102f2a438f13417618e4f0ba`. Gzip SHA-256: `ca33fa1badbfc0b593beb5c38b14025bfff4e0ffa02d9847f765ff722864073c`.

Límites preservados: SEH sólo ruta normal/restauración de cadena; no excepción/OOM. Sin restauración prometida de metadatos internos del heap. El objeto existente de61888 bytes se conserva íntegro; el objeto nuevo comprueba61504 bytes definidos y excluye384 no inicializados. Colaboradores actuales compartidos, incluidos SetDefault reconstruido si activo, sin independencia transitiva. CanSunbathe es función C++ global; owner RH CTaskComplexSunbathe sólo identifica el inyector.
