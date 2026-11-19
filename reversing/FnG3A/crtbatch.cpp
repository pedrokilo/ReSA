// Lanza argvdump.exe una vez por caso con CreateProcessA y la linea de comandos EXACTA (bytes) del caso.
// Uso: crtbatch argvdump.exe cases.txt out.txt [hilos]   -> out.txt: una linea por caso (la de argvdump) en el mismo orden.
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

static int hexv(char c) { return c <= '9' ? c - '0' : (c | 32) - 'a' + 10; }

int main(int argc, char** argv) {
    if (argc < 4) return 2;
    std::string exe = argv[1];
    std::ifstream in(argv[2]);
    std::vector<std::string> cases;
    for (std::string l; std::getline(in, l);) {
        std::string s;
        for (size_t i = 0; i + 1 < l.size(); i += 2) s.push_back((char)(hexv(l[i]) * 16 + hexv(l[i + 1])));
        cases.push_back(s);
    }
    int nt = argc > 4 ? atoi(argv[4]) : 8;
    std::vector<std::string> res(cases.size());
    std::atomic<size_t> next{0};
    std::atomic<int> fails{0};
    char tmp[MAX_PATH];
    GetTempPathA(MAX_PATH, tmp);
    auto work = [&](int tid) {
        std::string outf = std::string(tmp) + "crtb_" + std::to_string(GetCurrentProcessId()) + "_" + std::to_string(tid) + ".txt";
        std::string env = "ARGOUT=" + outf;
        env.push_back('\0');
        for (size_t k = 0; (k = next++) < cases.size();) {
            std::string cl = cases[k];
            cl.push_back('\0');
            DeleteFileA(outf.c_str());
            STARTUPINFOA si{};
            si.cb = sizeof si;
            PROCESS_INFORMATION pi{};
            // El bloque de entorno es solo ARGOUT (suficiente para este exe con CRT dinamico? no: hace falta SystemRoot)
            std::string e2 = env;
            e2.pop_back();
            e2.push_back('\0');
            char sr[MAX_PATH];
            GetEnvironmentVariableA("SystemRoot", sr, MAX_PATH);
            std::string block = "ARGOUT=" + outf;
            block.push_back('\0');
            block += std::string("SystemRoot=") + sr;
            block.push_back('\0');
            block.push_back('\0');
            if (!CreateProcessA(exe.c_str(), &cl[0], nullptr, nullptr, FALSE, CREATE_NO_WINDOW, (LPVOID)block.data(), nullptr, &si, &pi)) {
                res[k] = "ERR_CREATE";
                fails++;
                continue;
            }
            WaitForSingleObject(pi.hProcess, 20000);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            std::ifstream r(outf, std::ios::binary);
            std::string line;
            std::getline(r, line);
            res[k] = line.empty() ? "ERR_NOOUT" : line;
            if (line.empty()) fails++;
        }
        DeleteFileA(outf.c_str());
    };
    std::vector<std::thread> th;
    for (int i = 0; i < nt; i++) th.emplace_back(work, i);
    for (auto& t : th) t.join();
    std::ofstream o(argv[3], std::ios::binary);
    for (auto& s : res) o << s << '\n';
    printf("%zu casos, %d fallos\n", cases.size(), fails.load());
    return fails ? 1 : 0;
}
