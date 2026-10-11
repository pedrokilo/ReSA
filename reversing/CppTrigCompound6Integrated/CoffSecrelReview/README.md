# Soporte SECREL estricto separado

0xB = IMAGE_REL_I386_SECREL: desplazamiento32 del símbolo desde inicio de sección, usado por TLS/debug. En C3dMarkers::Render aparece en código real section1783/328bytes, offsets77/172, símbolo externo __Init_thread_epoch y cmp registro-memoria TLS. No proviene de metadata confundida ni límites erróneos. Ambos objetos contienen mismos campos.

compare_coff_secrel.py separado acepta ancho4 y conserva bytes crudos para0xB (no los normaliza). Sigue comparando offset/type/symbol/addend/destination/referent y sólo admite aliases de helpers E8 REL32. No se omite relocation ni se amplía igualdad semántica. Patch separado, compare_coff.py original intacto.

Corrida offline read-only spec13: 11/13 iguales. Render ya pasa. PlaceMarker y CreateNumberOfBirds difieren en bytes y hashes de sección propia: no se acepta ni normaliza; coordinación debe corregir fuente/codegen. Evidencia completa caller-proof-secrel.json.

[Microsoft PE Format](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format) tabla Intel386 define0x000B32bitsectionoffset/TLS. Sin producción/build/GTA/Git.
