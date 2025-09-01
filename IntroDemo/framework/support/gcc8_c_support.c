#include "gcc8_c_support.h"
#include <proto/exec.h>
extern struct ExecBase* SysBase;

unsigned long strlen(const char* s) {
	unsigned long t=0;
	while(*s++)
		t++;
	return t;
}

void memclr(void* dest, unsigned long len) { // dest: 16bit-aligned, len: multiple of 2
	__asm volatile (
		"add.l %[len], %[dest]\n"
		"moveq #0, %%d0\n"
		"moveq #0, %%d1\n"
		"moveq #0, %%d2\n"
		"moveq #0, %%d3\n"
		"cmp.l #256, %[len]\n"
		"blt 2f\n"
		"1:\n"
		".rept 256/16\n"
		"movem.l %%d0-%%d3, -(%[dest])\n"
		".endr\n"
		"sub.l #256, %[len]\n"
		"cmp.l #256, %[len]\n"
		"bge 1b\n"
		"2:\n" // <256
		"cmp.w #64, %[len]\n"
		"blt 3f\n"
		".rept 64/16\n"
		"movem.l %%d0-%%d3, -(%[dest])\n"
		".endr\n"
		"sub.w #64, %[len]\n"
		"bra 2b\n"
		"3:\n" // <64
		"lsr.w #2, %[len]\n" // 4
		"bcc 4f\n" // stray word
		"move.w %%d0,-(%[dest])\n"
		"4:\n"
		"moveq #64>>2, %%d1\n"
		"sub.w %[len], %%d1\n"
		"add.w %%d1, %%d1\n"
		"jmp (2, %%d1.w, %%pc)\n"
		".rept 64/4\n"
		"move.l %%d0,-(%[dest])\n"
		".endr\n"
	: [dest]"+a"(dest), [len]"+d"(len)
	:
	: "memory", "d0", "d1", "d2", "d3", "cc");
}

__attribute__((optimize("no-tree-loop-distribute-patterns")))
void* memset(void *dest, int val, unsigned long len) {
	unsigned char *ptr = (unsigned char *)dest;
	while(len-- > 0)
		*ptr++ = val;
	return dest;
}

__attribute__((optimize("no-tree-loop-distribute-patterns")))
void* memcpy(void *dest, const void *src, unsigned long len) {
	char *d = (char *)dest;
	const char *s = (const char *)src;
	while(len--)
		*d++ = *s++;
	return dest;
}

__attribute__((optimize("no-tree-loop-distribute-patterns")))
void* memmove(void *dest, const void *src, unsigned long len) {
	char *d = dest;
	const char *s = src;
	if (d < s) {
		while (len--)
			*d++ = *s++;
	} else {
		const char *lasts = s + (len - 1);
		char *lastd = d + (len - 1);
		while (len--)
			*lastd-- = *lasts--;
	}
	return dest;
}

#if defined(DEBUG)

// vbcc
typedef unsigned char *va_list;
#define va_start(ap, lastarg) ((ap)=(va_list)(&lastarg+1))

void KPutCharX();
void PutChar();

__attribute__((noipa))
void KPrintF(const char* fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	union { long(*func)(long mode, const char* string); ULONG ulong; UWORD* puword; } UaeDbgLog;
	UaeDbgLog.ulong = 0xf0ff60;
	if(*UaeDbgLog.puword == 0x4eb9 || *UaeDbgLog.puword == 0xa00e) {
		char temp[128];
		RawDoFmt((CONST_STRPTR)fmt, vl, PutChar, temp);
		UaeDbgLog.func(86, temp);
	} else {
		RawDoFmt((CONST_STRPTR)fmt, vl, KPutCharX, 0);
	}
}

#endif

int main();

extern void (*__preinit_array_start[])() __attribute__((weak));
extern void (*__preinit_array_end[])() __attribute__((weak));
extern void (*__init_array_start[])() __attribute__((weak));
extern void (*__init_array_end[])() __attribute__((weak));
extern void (*__fini_array_start[])() __attribute__((weak));
extern void (*__fini_array_end[])() __attribute__((weak));

__attribute__((used)) __attribute__((section(".text.unlikely")))
void _start() {
	// initialize globals, ctors etc.
	unsigned long count;
	unsigned long i;

	count = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < count; i++)
		__preinit_array_start[i]();

	count = __init_array_end - __init_array_start;
	for (i = 0; i < count; i++)
		__init_array_start[i]();

	main();

	// call dtors
	count = __fini_array_end - __fini_array_start;
	for (i = count; i > 0; i--)
		__fini_array_start[i - 1]();
}

#if defined(DEBUG)

void warpmode(int on) { // bool
	union { long(*func)(long mode, int index, const char* param, int param_len, char* outbuf, int outbuf_len); ULONG ulong; UWORD* puword; } UaeConf;
	UaeConf.ulong = 0xf0ff60;
	if(*UaeConf.puword == 0x4eb9 || *UaeConf.puword == 0xa00e) {
		char outbuf;
		UaeConf.func(82, -1, on ? "cpu_speed max" : "cpu_speed real", 0, &outbuf, 1);
		UaeConf.func(82, -1, on ? "cpu_cycle_exact false" : "cpu_cycle_exact true", 0, &outbuf, 1);
		UaeConf.func(82, -1, on ? "cpu_memory_cycle_exact false" : "cpu_memory_cycle_exact true", 0, &outbuf, 1);
		UaeConf.func(82, -1, on ? "blitter_cycle_exact false" : "blitter_cycle_exact true", 0, &outbuf, 1);
		UaeConf.func(82, -1, on ? "warp true" : "warp false", 0, &outbuf, 1);
	}
}

static void debug_cmd(unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4) {
	union { long(*func)(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4); ULONG ulong; UWORD* puword; } UaeLib;
	UaeLib.ulong = 0xf0ff60;
	if(*UaeLib.puword == 0x4eb9 || *UaeLib.puword == 0xa00e) {
		UaeLib.func(88, arg1, arg2, arg3, arg4);
	}
}

enum barto_cmd {
	barto_cmd_clear,
	barto_cmd_rect,
	barto_cmd_filled_rect,
	barto_cmd_text,
	barto_cmd_register_resource,
	barto_cmd_set_idle,
	barto_cmd_unregister_resource,
	barto_cmd_load,
	barto_cmd_save,
};

enum debug_resource_type {
	debug_resource_type_bitmap,
	debug_resource_type_palette,
	debug_resource_type_copperlist,
};

struct debug_resource {
	unsigned int address; // can't use void* because WinUAE is 64-bit
	unsigned int size;
	char name[32];
	unsigned short /*enum debug_resource_type*/ type;
	unsigned short /*enum debug_resource_flags*/ flags;

	union {
		struct bitmap {
			short width;
			short height;
			short numPlanes;
		} bitmap;
		struct palette {
			short numEntries;
		} palette;
	};
};

// debug overlay
void debug_clear() {
	debug_cmd(barto_cmd_clear, 0, 0, 0);
}

void debug_rect(short left, short top, short right, short bottom, unsigned int color) {
	debug_cmd(barto_cmd_rect, (((unsigned int)left) << 16) | ((unsigned int)top), (((unsigned int)right) << 16) | ((unsigned int)bottom), color);
}

void debug_filled_rect(short left, short top, short right, short bottom, unsigned int color) {
	debug_cmd(barto_cmd_filled_rect, (((unsigned int)left) << 16) | ((unsigned int)top), (((unsigned int)right) << 16) | ((unsigned int)bottom), color);
}

__attribute__((noipa))
void debug_text(short left, short top, unsigned int color, const char* fmt, ...) {
	va_list vl;
	va_start(vl, fmt);
	char temp[128];
	RawDoFmt((CONST_STRPTR)fmt, vl, PutChar, temp);
	debug_cmd(barto_cmd_text, (((unsigned int)left) << 16) | ((unsigned int)top), (unsigned int)temp, color);
}

// profiler
void debug_start_idle() {
	debug_cmd(barto_cmd_set_idle, 1, 0, 0);
}

void debug_stop_idle() {
	debug_cmd(barto_cmd_set_idle, 0, 0, 0);
}

// gfx debugger
static void my_strncpy(char* destination, const char* source, unsigned long num) {
	while(*source && --num > 0)
		*destination++ = *source++;
	*destination = '\0';
}

void debug_register_bitmap(const void* addr, const char* name, short width, short height, short numPlanes, unsigned short flags) {
	struct debug_resource resource = {
		.address = (unsigned int)addr,
		.size = mulsw32(width >> 3, mulsw16(height, numPlanes)),
		.type = debug_resource_type_bitmap,
		.flags = flags,
		.bitmap = { width, height, numPlanes }
	};

	if (flags & debug_resource_bitmap_masked)
		resource.size <<= 1;

	my_strncpy(resource.name, name, sizeof(resource.name));
	debug_cmd(barto_cmd_register_resource, (unsigned int)&resource, 0, 0);
}

void debug_register_palette(const void* addr, const char* name, short numEntries, unsigned short flags) {
	struct debug_resource resource = {
		.address = (unsigned int)addr,
		.size = numEntries << 1,
		.type = debug_resource_type_palette,
		.flags = flags,
		.palette = { numEntries }
	};
	my_strncpy(resource.name, name, sizeof(resource.name));
	debug_cmd(barto_cmd_register_resource, (unsigned int)&resource, 0, 0);
}

void debug_register_copperlist(const void* addr, const char* name, unsigned int size, unsigned short flags) {
	struct debug_resource resource = {
		.address = (unsigned int)addr,
		.size = size,
		.type = debug_resource_type_copperlist,
		.flags = flags,
	};
	my_strncpy(resource.name, name, sizeof(resource.name));
	debug_cmd(barto_cmd_register_resource, (unsigned int)&resource, 0, 0);
}

void debug_unregister(const void* addr) {
	debug_cmd(barto_cmd_unregister_resource, (unsigned int)addr, 0, 0);
}

// load/save
unsigned int debug_load(const void* addr, const char* name) {
	struct debug_resource resource = {
		.address = (unsigned int)addr,
		.size = 0,
	};
	my_strncpy(resource.name, name, sizeof(resource.name));
	debug_cmd(barto_cmd_load, (unsigned int)&resource, 0, 0);
	return resource.size;
}

void debug_save(const void* addr, unsigned int size, const char* name) {
	struct debug_resource resource = {
		.address = (unsigned int)addr,
		.size = size,
	};
	my_strncpy(resource.name, name, sizeof(resource.name));
	debug_cmd(barto_cmd_save, (unsigned int)&resource, 0, 0);
}

#endif