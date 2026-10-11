# COFF: BSS y consumidores indirectos

El hash distinto no representa cambio real de staticrefs. Los destinos afectados residen en .bss, con PointerToRawData=0, flags0xC0300080, sin relocations: Birds40bytes y Bright20bytes. El parser previo extraia data[0:size], bytes del encabezado COFF; diferencias offsets8/9 son timestamp bigobj. BSS no contiene payload: debe representarse con ceros declarados. El coordinador ya corrigio parser compartido manteniendo gate hash completo; esta auditoria no cambia script.

bss-header-evidence.json conserva descriptores, simbolos y ausencia de relocations. staticref-initializer-proof.json compara ocho inicializadores dinamicos (3Birds+5Bright), bytes y relocations identicos. No hay motivo para omitir hashes ni reducir arbitrariamente al tamano de un simbolo en estos objetos.

spec-eight-callers-extra-jet.json contiene ocho consumidores unicos. Simbolo real ?Render@tBrightLight@@QBEXXZ contiene ambas llamadas; CBrightLights::Render conserva cero llamadas directas. JetProcessPed solo sinMul; RenderJetPack llama lambda con sinMul y cosMul.

Simbolos antiguos/nuevos observados:
?RenderJetPack@CTaskSimpleJetPack@@QAEXPAVCPed@@@Z
??R<lambda_1>@?4??RenderJetPack@CTaskSimpleJetPack@@QAEXPAVCPed@@@Z@QBE@PBD_N@Z

caller-proof-eight-extra-jet.json acredita igualdad COFF de ocho rangos: bytes y relocations salvo aliases autorizados. Lambda Jet mantiene calls offsets198/234. RenderJetPack conserva llamada lambda sin alias adicional. Evidencia estatica, no validacion juego ni FP universal.

Sin build, GTA, Git ni produccion. No se modifica comparador ni bancos durante corridas.
