; Lote IND0A, prototipo de la estrategia (a) "caparazon": reserva en el propio EXE el rango de direcciones que ocupa
; la imagen del gta_sa.exe original (0x401000-0xCB1000, 0x8B0000 bytes).
;
; El enlazador de MSVC siempre pone ".text" como primera seccion (una seccion propia ".orig" salia segunda y el codigo
; propio caia en 0x401000; un array con __declspec(allocate) en ".text$a" salia como segunda ".text" con otros atributos),
; asi que la reserva se declara en ensamblador como seccion ".text" y es el PRIMER objeto del enlace, asi que queda antes que el resto de ".text"
; y el resto. Cuesta 9 MB de ceros en el archivo del prototipo. El cargador cambia los permisos de la region a
; lectura/escritura/ejecucion antes de copiar la imagen original.
; Resultado esperado del enlace: ".text" en VA 0x401000 y el primer codigo propio en VA >= 0xCB1000.
.386
.model flat
OPTION DOTNAME
.text SEGMENT PARA PUBLIC 'CODE'
PUBLIC _g_ImagenOriginalReservada
_g_ImagenOriginalReservada LABEL BYTE
    db 8B0000h dup (0)
.text ENDS
END
