__declspec(noinline) bool less(float a, float b) { return a < b; }
__declspec(noinline) bool greater(float a, float b) { return a > b; }
__declspec(noinline) bool le(float a, float b) { return a <= b; }
__declspec(noinline) bool ge(float a, float b) { return a >= b; }
__declspec(noinline) bool notgreater(float a, float b) { return !(a > b); }
__declspec(noinline) bool equal_ordered(float a, float b) { return a <= b && a >= b; }
#pragma float_control(precise, on, push)
__declspec(noinline) bool equal_precise(float a, float b) { return a <= b && a >= b; }
#pragma float_control(pop)
__declspec(noinline) bool volatile_greater(float a, float b) {
    volatile float left = a;
    return left > b;
}
__declspec(noinline) bool volatile_notgreater(float a, float b) {
    volatile float left = a;
    return !(left > b);
}
