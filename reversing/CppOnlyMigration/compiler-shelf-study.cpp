extern void __cdecl receiver(float,float);
#pragma optimize("gt", on)
void __cdecl shelf(int x,int y) { receiver(float(x), float(y)); }
#pragma optimize("", off)
