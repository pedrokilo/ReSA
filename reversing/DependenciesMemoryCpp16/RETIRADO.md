# Banco retirado (07/11/2026)

Este banco comprobaba que `CMemoryMgr::Malloc/Free/Realloc` (0x72F420/430/440) de la DLL eran un simple reenvío al CRT. Desde el lote 100A esas funciones son C++ real con el prologo de depuracion de 0xC0 bytes del EXE, asi que la premisa ya no vale y el banco falla sin que haya un defecto de produccion (lote 112A). No tenia entradas en el registro de validacion. No se ejecuta ni se usa como evidencia; se conserva solo como historial. La semantica actual la cubre `reversing/Fn100A`.
