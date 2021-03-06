extern inline const int plus32 (int arg1, int arg2)
{
	asm(" addl %2,%0					\n\
	jo plus_err": "=r" (arg2): "0" (arg2), "r" (arg1));
	return arg2;
}
#define plus_err_label() asm("plus_err:")

extern inline const int iplus32 (int arg1, int arg2)
{
	asm(" addl %2,%0					\n\
	jo iplus_err":  "=r" (arg2): "0" (arg2), "r" (arg1));
	return arg2;
}
#define iplus_err_label() asm("iplus_err:")

extern inline const int sub32 (int arg1, int arg2)
{
	asm("subl %2,%0						\n\
	jo diff_err": "=r" (arg1): "0" (arg1), "r" (arg2));
	return arg1;
}
#define diff_err_label() asm("diff_err:")

extern inline const int isub32 (int arg1, int arg2)
{
	asm(" subl %2,%0						\n\
	jo idiff_err": "=r" (arg1): "0" (arg1), "r" (arg2));
	return arg1;
}
#define idiff_err_label() asm("idiff_err:")

extern inline const int iplus32n(int arg1, int arg2)
{
	asm("addl %2,%0						\n\
	jo iplusn_err": "=r" (arg2): "0" (arg2), "r" (arg1));
	return arg2;
}
#define iplusn_err_label() asm("iplusn_err:")

extern inline const int sub32n (int arg1, int arg2)
{
	asm(" subl %2,%0						\n\
	jo idiffn_err": "=r" (arg1): "0" (arg1), "r" (arg2));
	return arg1;
}
#define idiffn_err_label() asm("idiffn_err:")

