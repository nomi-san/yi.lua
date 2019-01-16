#ifndef _yi_api_
#define _yi_api_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern int yiL_Ref;

static char $__yi[]		= "yi";
static char $__yi_enum[]	= "enum";
static char $__yi_cfun[]	= "yi.cfun";
static char $__yi_clib[]	= "yi.clib";
static char $__yi_closure[]= "yi.closure";
static char $__yi_struct[] = "yi.struct";
static char $__yi_types[]	= "yi.types";
static char $__yi_thread[] = "yi.thread";

#if defined(hash)
#undef hash
#endif
#define hash(x) yiH__##x

#define yiH__types		0x06ba0236
#define yiH__type		0x048401c3
#define yiH__closure	0x0be302fe
#define yiH__i2p		0x0212010c
#define yiH__p2i		0x0220010c
#define yiH__check		0x05f401ff
#define yiH__load		0x042701a1
#define yiH__enum		0x043901b6
#define yiH__struct    0x095d02a6

// thread =======================================

#define yiH__thread    0x08e30279
#define yiH__data		0x0400019b
#define yiH__exit		0x044601bb
#define yiH__func		0x043a01ad
#define yiH__fn		0x013c00d5
#define yiH__routine	0x0c540307

// types ========================================

#define yiH__u8		0x012400ae
#define yiH__u16		0x01fa00dd
#define yiH__u32		0x01fa00db
#define yiH__u64		0x020200e0
#define yiH__i8		0x010c00a2
#define yiH__i16		0x01d600d1
#define yiH__i32		0x01d600cf
#define yiH__i64		0x01de00d4
#define yiH__int		0x028e014c
#define yiH__int32		0x05be01b1
#define yiH__uint		0x046301c1
#define yiH__uint32    0x087d0226
#define yiH__int64		0x05c601b6
#define yiH__uint64    0x0885022b
#define yiH__boolean   0x0b8902e1
#define yiH__bool		0x042301ad
#define yiH__str		0x02b6015a
#define yiH__string    0x09420298
#define yiH__wstr		0x049301d1
#define yiH__wstring   0x0c84030f
#define yiH__pointer   0x0c0e0302
#define yiH__ptr		0x02ad0157
#define yiH__none		0x044a01b1
#define yiH__void		0x045f01b3
#define yiH__long		0x044401b1
#define yiH__short		0x06890231
#define yiH__char		0x03fc019f
#define yiH__wchar		0x06500216
#define yiH__size		0x046401bc
#define yiH__ulong		0x068e0226
#define yiH__ushort    0x094802a6
#define yiH__uchar		0x06460214
#define yiH__float		0x06360217
#define yiH__f32		0x01cd00cc
#define yiH__num		0x02a40151
#define yiH__double    0x08c0027c
#define yiH__f64		0x01d500d1
#define yiH__llong		0x0661021d
#define yiH__ullong    0x09200292
#define yiH__longlong  0x0f480361
#define yiH__ulonglong 0x136603d6
#define yiH__unsigned  0x0f85035e
#define yiH__chr		0x026e013e
#define yiH__wchr		0x044b01b5
#define yiH__uchr		0x044301b3
#define yiH__signed    0x08d9027b
#define yiH__number    0x08f9028a
#define yiH__integer   0x0bc302ef

#ifdef __cplusplus
}
#endif

#endif // !_yi_api_