# Lote IND0A: objetivo opcional "gta_reversed_exe" (prototipo de EXE caparazon). Se activa con
#   cmake -DGTASA_EXE_PROTOTIPO=ON ...
# No recompila nada: espera que gta_reversed ya este compilado y solo enlaza sus .obj como EXE de base fija 0x400000
# (con la reserva 0x401000-0xCB1000 para la imagen del gta_sa.exe original) y ejecuta la comprobacion estatica.
# No ejecuta el juego. Ver docs/ExeIndependiente/README.md.
if (NOT TARGET gta_reversed)
    message(WARNING "GTASA_EXE_PROTOTIPO: falta el objetivo gta_reversed; no se crea gta_reversed_exe")
    return()
endif()
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(IND0A_SALIDA "${CMAKE_BINARY_DIR}/gta_reversed_exe")
add_custom_target(gta_reversed_exe
    COMMAND cmd /c "${CMAKE_CURRENT_SOURCE_DIR}/enlazar_exe_prueba.cmd" "${PROJECT_SOURCE_DIR}" "${IND0A_SALIDA}" "${CMAKE_BINARY_DIR}/source/gta_reversed.dir/$<CONFIG>"
    COMMAND ${Python3_EXECUTABLE} -I "${CMAKE_CURRENT_SOURCE_DIR}/../verificar_exe.py" "${IND0A_SALIDA}"
    DEPENDS gta_reversed
    COMMENT "IND0A: enlazando gta_reversed_exe.exe (caparazon) y comprobando su layout"
    VERBATIM)
