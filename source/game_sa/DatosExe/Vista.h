#pragma once

#include <cstddef>
#include <type_traits>

/*
 * Vistas tipadas sobre objetos DatosExe (auditoria de tamanos, docs/DatosExe/auditoria-tamanos.md).
 *
 * Los objetos `DatosExe::*` se definieron a veces con el tamano del ACCESO del EXE (1 o 4 bytes) y el codigo los
 * reinterpreta como el tipo real (arrays, estructuras). Si el objeto es mas pequeno que la vista, en el mapa de enlace del
 * independiente se solapa con los objetos vecinos y se pisan entre si (vehiculos negros: ms_lightsOn, ms_currentCol,
 * m_sfLogLookup). DATOSEXE_VISTA(obj, T...) es la unica forma permitida de ver un objeto DatosExe como otro tipo: da una
 * referencia T& y comprueba EN COMPILACION que sizeof(obj) >= sizeof(T). Es solo de tipos (no cambia el codigo generado ni el
 * orden de inicializacion). El script contrib/auditoria_datos/tamanos_objetos.py (orden `lint`) rechaza los reinterpret_cast
 * sobre objetos DatosExe que no pasen por aqui.
 */
namespace DatosExe {
template <class T, std::size_t TamObjeto>
struct VistaTipoImpl {
    static_assert(TamObjeto >= sizeof(T),
                  "DATOSEXE_VISTA: el objeto DatosExe es mas pequeno que el tipo con que se usa; agrandar su definicion (tamano real del EXE)");
    using tipo = T;
};
template <class T, std::size_t TamObjeto>
using VistaTipo = typename VistaTipoImpl<T, TamObjeto>::tipo;
} // namespace DatosExe

#define DATOSEXE_VISTA(obj, ...) \
    reinterpret_cast<std::add_lvalue_reference_t<::DatosExe::VistaTipo<__VA_ARGS__, sizeof(obj)>>>(obj)
