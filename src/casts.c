#include "milo.h"
#include "types.h"
#include "lauxlib.h"

void testx(int a, char* b, int c, double d)
{
	printf("%d, %s, %d, %f", a, b, c, d);
}

typedef void(*abc)();

__declspec(dllexport) void test2(float x, float y)
{
	printf("%f", x+y);
}

/*
__declspec(dllexport) void test() {

	milo_value_t *a = calloc(1, sizeof(milo_value_t) *4);

	a[0].val.v_i32 = 5;
	a[1].val.v_str = "fwwwwdwdw";
	a[2].val.v_i32 = 20;
	a[3].val.v_f64 = 20.077277278;

	abc z = testx;

	milo_variant_t x = *(milo_variant_t*)((size_t)a + 4);

	z(a[0].val.v_ptr, a[1].val.v_ptr, a[2].val.v_ptr, a[3].val.v_f64);
	//testx(a[0].val->v_ptr, a[1].val->v_ptr, a[2].val->v_ptr);
}
*/