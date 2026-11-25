/*
 * Configuracion de las correcciones de SilentPatch (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent); docs/THIRD_PARTY_NOTICES.md).
 */
#include "StdInc.h"
#include "SilentPatchConfig.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
// Valor por defecto del interruptor general (cambiar aqui para que SilentPatch quede apagado salvo que se encienda por entorno o ini).
constexpr bool kSilentPatchPorDefecto = true;

std::string Minusculas(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

std::string Recortar(const std::string& s) {
    const auto a = s.find_first_not_of(" \t\r\n");
    return a == std::string::npos ? std::string{} : s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}

std::optional<bool> ValorBool(const std::string& v) {
    const auto m = Minusculas(Recortar(v));
    if (m == "1" || m == "true" || m == "yes" || m == "on") { return true; }
    if (m == "0" || m == "false" || m == "no" || m == "off") { return false; }
    return std::nullopt;
}

std::optional<int> EnteroDe(const std::string& v) {
    const auto t = Recortar(v);
    if (t.empty()) { return std::nullopt; }
    size_t i = (t[0] == '-' || t[0] == '+') ? 1 : 0;
    if (i >= t.size()) { return std::nullopt; }
    int r = 0;
    for (size_t k = i; k < t.size(); ++k) {
        if (t[k] < '0' || t[k] > '9') { return std::nullopt; }
        r = r * 10 + (t[k] - '0');
    }
    return t[0] == '-' ? -r : r;
}

std::optional<bool> DeEntorno(const std::string& nombre) {
    char buf[16]{};
    const DWORD n = GetEnvironmentVariableA(nombre.c_str(), buf, sizeof(buf));
    return (n > 0 && n < sizeof(buf)) ? ValorBool(buf) : std::nullopt;
}

std::optional<bool> DeIni(const std::string& clave) {
    std::ifstream in("gta-reversed.ini");
    std::string linea;
    const auto buscada = Minusculas(clave);
    while (std::getline(in, linea)) {
        if (const auto c = linea.find(';'); c != std::string::npos) { linea.erase(c); }
        const auto e = linea.find('=');
        if (e == std::string::npos) { continue; }
        if (Minusculas(Recortar(linea.substr(0, e))) == buscada) { return ValorBool(linea.substr(e + 1)); }
    }
    return std::nullopt;
}
// Registro de las opciones ya consultadas (solo para el log de diagnostico: SilentPatchLogState).
std::vector<std::pair<std::string, int>> g_consultadas;
bool g_informado = false;

void Registrar(const std::string& nombre, int valor) {
    g_consultadas.emplace_back(nombre, valor);
    if (g_informado) { SPDLOG_INFO("SilentPatch: opcion {} = {}", nombre, valor); }
}
} // namespace

bool SilentPatchEnabled() {
    static const bool activo = DeEntorno("GTASA_SILENTPATCH").value_or(DeIni("SilentPatch").value_or(kSilentPatchPorDefecto));
    return activo;
}

bool SilentPatchOption(const char* name, bool defaultValue) {
    if (!SilentPatchEnabled()) {
        return false;
    }
    // Cache por nombre: varios sitios consultan la opcion en cada llamada y leer el ini cada vez seria caro (el valor es fijo durante el proceso).
    static std::unordered_map<std::string, bool> cache;
    static std::mutex                            mtx; // el streaming consulta opciones desde su hilo
    std::lock_guard                              lock{mtx};
    const std::string clave = std::string(name) + (defaultValue ? "#1" : "#0");
    if (const auto it = cache.find(clave); it != cache.end()) {
        return it->second;
    }
    std::string mayus{name};
    std::transform(mayus.begin(), mayus.end(), mayus.begin(), [](unsigned char c) { return (char)std::toupper(c); });
    const bool r = DeEntorno("GTASA_SILENTPATCH_" + mayus).value_or(DeIni(std::string("SilentPatch_") + name).value_or(defaultValue));
    cache.emplace(clave, r);
    Registrar(name, r ? 1 : 0);
    return r;
}

void SilentPatchLogState() {
    SPDLOG_INFO("SilentPatch: interruptor general = {} (GTASA_SILENTPATCH / SilentPatch= en gta-reversed.ini; por defecto {})", SilentPatchEnabled() ? 1 : 0, kSilentPatchPorDefecto ? 1 : 0);
    g_informado = true;
    for (const auto& [n, v] : g_consultadas) { SPDLOG_INFO("SilentPatch: opcion {} = {}", n, v); }
}

int SilentPatchIntOption(const char* name, int defaultValue, int disabledValue) {
    if (!SilentPatchEnabled()) {
        return disabledValue;
    }
    std::string mayus{name};
    std::transform(mayus.begin(), mayus.end(), mayus.begin(), [](unsigned char c) { return (char)std::toupper(c); });
    char buf[16]{};
    if (const DWORD n = GetEnvironmentVariableA(("GTASA_SILENTPATCH_" + mayus).c_str(), buf, sizeof(buf)); n > 0 && n < sizeof(buf)) {
        if (const auto v = EnteroDe(buf)) { return *v; }
    }
    std::ifstream in("gta-reversed.ini");
    std::string linea;
    const auto buscada = Minusculas(std::string("SilentPatch_") + name);
    while (std::getline(in, linea)) {
        if (const auto c = linea.find(';'); c != std::string::npos) { linea.erase(c); }
        const auto e = linea.find('=');
        if (e == std::string::npos) { continue; }
        if (Minusculas(Recortar(linea.substr(0, e))) == buscada) {
            if (const auto v = EnteroDe(linea.substr(e + 1))) { return *v; }
        }
    }
    return defaultValue;
}

// Desviacion propia (no de SilentPatch): ver SilentPatchConfig.h
bool OwnDeviationOption(const char* envName, const char* iniKey, bool defaultValue) {
    static std::mutex mtx;
    std::lock_guard   lock{mtx};
    static std::unordered_map<std::string, bool> cache;
    const std::string clave = std::string(envName) + (defaultValue ? "#1" : "#0");
    if (const auto it = cache.find(clave); it != cache.end()) { return it->second; }
    const bool r = DeEntorno(envName).value_or(DeIni(iniKey).value_or(defaultValue));
    cache.emplace(clave, r);
    SPDLOG_INFO("Desviacion propia: opcion {} = {}", iniKey, r ? 1 : 0);
    return r;
}

// Desviacion propia de texto (p. ej. PadButtons=auto|ps|xbox): entorno > gta-reversed.ini > defaultValue; el valor se devuelve recortado y en minusculas.
std::string OwnDeviationString(const char* envName, const char* iniKey, const char* defaultValue) {
    static std::mutex mtx;
    std::lock_guard   lock{mtx};
    char              buf[64]{};
    if (const DWORD n = GetEnvironmentVariableA(envName, buf, sizeof(buf)); n > 0 && n < sizeof(buf)) {
        return Minusculas(Recortar(buf));
    }
    std::ifstream in("gta-reversed.ini");
    std::string   linea;
    const auto    buscada = Minusculas(iniKey);
    while (std::getline(in, linea)) {
        if (const auto c = linea.find(';'); c != std::string::npos) { linea.erase(c); }
        const auto e = linea.find('=');
        if (e == std::string::npos) { continue; }
        if (Minusculas(Recortar(linea.substr(0, e))) == buscada) { return Minusculas(Recortar(linea.substr(e + 1))); }
    }
    return defaultValue;
}
