extern "C" __declspec(noinline) void SetBuffering(bool enabled) {
    *reinterpret_cast<volatile unsigned char*>(0x8D6084) = enabled;
}
extern "C" __declspec(noinline) bool ReturnFalse(float, float) {return false;}
