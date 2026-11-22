#pragma once

// Descripcion en tiempo de compilacion de la firma de cada manejador de comando registrado con REGISTER_COMMAND_HANDLER:
// lista aplanada de parametros que el manejador lee del script (Read<T>) y de variables que escribe (StoreArg del valor devuelto).
// La usa el banco de comandos (reversing/CmdBench) para generar scripts sinteticos con los tipos correctos; no tiene efecto en el juego.
// Debe seguir exactamente las ramas de ReadArg.hpp / StoreArg.hpp.

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <type_traits>
#include <tuple>
#include <string_view>

#include "Utility.hpp"
#include "ReadArg.hpp"
#include <Enums/eScriptCommands.h>

namespace notsa {
namespace script {
namespace sig {

enum Kind : uint8_t {
    K_END = 0,
    K_I8, K_U8, K_I16, K_U16, K_I32, K_U32, K_F32, K_BOOL,   // aritmeticos (por valor; en `out` el ancho de la variable escrita)
    K_STR,                                                    // texto (const char*, string_view, StringRef...)
    K_PED, K_PLAYERPED, K_VEH, K_OBJ, K_POOL_OTHER,           // manejadores de entidades del pool (K_PLAYERPED: indice de jugador)
    K_PLAYERINFO,                                             // indice de jugador
    K_MODEL, K_HASH,
    K_SCRIPTTHING,                                            // manejador empaquetado (indice | id << 16) de esferas, efectos, etc.
    K_VAR_I32, K_VAR_F32, K_VAR_OTHER,                        // referencia a variable (T& / T* aritmetico): el manejador la lee y escribe
    K_SCRIPT,                                                 // CRunningScript& (no lee nada del script)
    K_CMD,                                                    // eScriptCommands (no lee nada del script)
    K_ENUM,                                                   // enumerado (se lee como su tipo subyacente)
    K_UNKNOWN,
};

enum RetMode : uint8_t { RET_NONE = 0, RET_FLAG, RET_VALUES, RET_OPCODE };

struct CmdSig {
    bool    registered{};
    bool    known{true};          // false si algun tipo no tiene Kind
    uint8_t nIn{}, nOut{};
    RetMode ret{RET_NONE};
    uint8_t in[24]{};
    uint8_t inSub[24]{};          // K_VEH: subtipo (eVehicleType + 1, 0 = cualquiera); K_ENUM: ancho en bytes
    uint8_t out[12]{};
};

namespace detail {
struct Builder {
    CmdSig* s;
    void In(Kind k, uint8_t sub = 0) {
        if (s->nIn < 24) { s->in[s->nIn] = k; s->inSub[s->nIn] = sub; ++s->nIn; } else { s->known = false; }
        if (k == K_UNKNOWN) { s->known = false; }
    }
    void Out(Kind k) {
        if (s->nOut < 12) { s->out[s->nOut++] = k; } else { s->known = false; }
        if (k == K_UNKNOWN) { s->known = false; }
    }
};

template<typename T>
constexpr Kind ArithKind() {
    if constexpr (std::is_same_v<T, bool>) return K_BOOL;
    else if constexpr (std::is_floating_point_v<T>) return K_F32;
    else if constexpr (std::is_integral_v<T>) {
        if constexpr (sizeof(T) == 1) return std::is_signed_v<T> ? K_I8 : K_U8;
        else if constexpr (sizeof(T) == 2) return std::is_signed_v<T> ? K_I16 : K_U16;
        else return std::is_signed_v<T> ? K_I32 : K_U32;
    } else return K_UNKNOWN;
}

template<typename Y>
constexpr Kind EntityKind() {
    if constexpr (std::is_base_of_v<CPed, Y>) return K_PED;
    else if constexpr (std::is_base_of_v<CVehicle, Y>) return K_VEH;
    else if constexpr (std::is_base_of_v<CObject, Y>) return K_OBJ;
    else return K_POOL_OTHER;
}

template<typename T, bool FromRef = false>
inline void AddIn(Builder& b) {
    using Y = std::remove_pointer_t<std::remove_cvref_t<T>>;
    if constexpr (std::is_same_v<T, eScriptCommands>) {
        b.In(K_CMD);
    } else if constexpr (std::is_reference_v<T>) {
        AddIn<std::remove_reference_t<T>*, true>(b);
    } else if constexpr (std::is_same_v<Y, CVector>) {
        b.In(K_F32); b.In(K_F32); b.In(K_F32);
    } else if constexpr (std::is_same_v<Y, CVector2D>) {
        b.In(K_F32); b.In(K_F32);
    } else if constexpr (std::is_same_v<Y, CRect>) {
        b.In(K_F32); b.In(K_F32); b.In(K_F32); b.In(K_F32);
    } else if constexpr (std::is_same_v<Y, CRGBA>) {
        b.In(K_U8); b.In(K_U8); b.In(K_U8); b.In(K_U8);
    } else if constexpr (std::is_same_v<Y, scm::StringRef> || std::is_same_v<Y, notsa::ci_string_view> || std::is_same_v<Y, std::string_view> || std::is_same_v<T, const char*>) {
        b.In(K_STR);
    } else if constexpr (std::is_arithmetic_v<Y>) {
        if constexpr (std::is_pointer_v<T>) {
            if constexpr (std::is_floating_point_v<Y>) b.In(K_VAR_F32);
            else if constexpr (std::is_same_v<std::remove_cv_t<Y>, int32> || std::is_same_v<std::remove_cv_t<Y>, uint32>) b.In(K_VAR_I32);
            else b.In(K_VAR_OTHER);
        } else b.In(ArithKind<Y>());
    } else if constexpr (std::is_enum_v<Y>) {
        b.In(K_ENUM, (uint8_t)sizeof(std::underlying_type_t<Y>));
    } else if constexpr (std::is_same_v<Y, CPlayerPed>) {
        b.In(K_PLAYERPED);
    } else if constexpr (notsa::is_specialization_of<Y, ScriptEntity>) {
        b.In(EntityKind<typename Y::EntityType>(), (uint8_t)0x80);   // ScriptEntity: `e` puede ser nulo (el manejador lo comprueba)
    } else if constexpr (::notsa::PooledType<Y>) {
        constexpr auto k = EntityKind<Y>();
        int sub = 0;
        if constexpr (k == K_VEH) {
            if constexpr (requires { Y::Type; }) sub = static_cast<int>(Y::Type) + 1;
        }
        if constexpr (std::is_pointer_v<T> && !FromRef) sub |= 0x80;   // puntero: puede ser nulo (manejador invalido)
        b.In(k, static_cast<uint8_t>(sub));
    } else if constexpr (std::is_same_v<Y, CRunningScript>) {
        b.In(K_SCRIPT);
    } else if constexpr (std::is_same_v<Y, CPlayerInfo>) {
        b.In(K_PLAYERINFO);
    } else if constexpr (::notsa::script::detail::scriptthing::is_script_thing_v<Y>) {
        b.In(K_SCRIPTTHING);
    } else if constexpr (std::is_same_v<T, ::notsa::script::Model>) {
        b.In(K_MODEL);
    } else if constexpr (std::is_same_v<T, ::notsa::script::Hash>) {
        b.In(K_HASH);
    } else {
        b.In(K_UNKNOWN);
    }
}

template<typename R>
inline void AddOut(Builder& b);

template<typename... Ts>
inline void AddOutTuple(Builder& b, std::tuple<Ts...>*) { (AddOut<std::remove_cvref_t<Ts>>(b), ...); }

template<typename R>
inline void AddOut(Builder& b) {
    using Y = std::remove_pointer_t<std::remove_cvref_t<R>>;
    if constexpr (std::is_same_v<R, void>) {
        // nada
    } else if constexpr (std::is_same_v<R, bool> || std::is_same_v<R, CompareFlagUpdate>) {
        b.s->ret = RET_FLAG;
    } else if constexpr (std::is_same_v<R, OpcodeResult>) {
        b.s->ret = RET_OPCODE;
    } else if constexpr (std::is_same_v<Y, CVector>) {
        b.s->ret = RET_VALUES; b.Out(K_F32); b.Out(K_F32); b.Out(K_F32);
    } else if constexpr (std::is_same_v<Y, CVector2D>) {
        b.s->ret = RET_VALUES; b.Out(K_F32); b.Out(K_F32);
    } else if constexpr (requires { AddOutTuple(b, (R*)nullptr); } && !std::is_arithmetic_v<R> && !std::is_enum_v<R> && !std::is_pointer_v<R>) {
        b.s->ret = RET_VALUES; AddOutTuple(b, (R*)nullptr);
    } else if constexpr (std::is_arithmetic_v<R>) {
        b.s->ret = RET_VALUES; b.Out(ArithKind<R>());
    } else if constexpr (std::is_enum_v<R>) {
        b.s->ret = RET_VALUES; b.Out(K_I32);
    } else if constexpr (::notsa::PooledType<Y>) {
        b.s->ret = RET_VALUES; b.Out(EntityKind<Y>());
    } else {
        b.s->ret = RET_VALUES; b.Out(K_UNKNOWN);
    }
}

template<typename R, typename... A>
inline void FromFn(CmdSig& s, R (*)(A...)) {
    Builder b{&s};
    (AddIn<A>(b), ...);
    AddOut<R>(b);
}
} // namespace detail

inline std::array<CmdSig, COMMAND_HIGHEST_ID_TO_HOOK + 1>& Table() {
    static std::array<CmdSig, COMMAND_HIGHEST_ID_TO_HOOK + 1> t{};
    return t;
}

template<eScriptCommands Command, auto* CommandFn>
inline void Record() {
    auto& s = Table()[(size_t)Command];
    s = CmdSig{};
    s.registered = true;
    detail::FromFn(s, CommandFn);
}

//! Herramienta contrib/audit_script_params.py: si existe la variable de entorno GTASA_DUMP_SIGS=<archivo>, vuelca la firma de cada manejador
//! registrado (id,conocida,retorno,nIn,nOut,kinds_in;kinds_out) para compararla con los parametros que lee el EXE. Sin la variable no hace nada.
inline void DumpIfRequested() {
    char* env = nullptr; size_t len = 0;
    if (_dupenv_s(&env, &len, "GTASA_DUMP_SIGS") != 0 || !env) { return; }
    FILE* f = nullptr;
    const auto err = fopen_s(&f, env, "w");
    free(env);
    if (err != 0 || !f) { return; }
    std::fprintf(f, "id,conocida,retorno,nIn,nOut,in,out\n");
    const auto& t = Table();
    for (size_t i = 0; i < t.size(); i++) {
        const auto& s = t[i];
        if (!s.registered) { continue; }
        std::fprintf(f, "%zu,%d,%d,%d,%d,", i, s.known ? 1 : 0, (int)s.ret, (int)s.nIn, (int)s.nOut);
        for (int k = 0; k < s.nIn; k++) { std::fprintf(f, "%s%d", k ? ";" : "", (int)s.in[k]); }
        std::fprintf(f, ",");
        for (int k = 0; k < s.nOut; k++) { std::fprintf(f, "%s%d", k ? ";" : "", (int)s.out[k]); }
        std::fprintf(f, "\n");
    }
    std::fclose(f);
}
} // namespace sig
} // namespace script
} // namespace notsa
