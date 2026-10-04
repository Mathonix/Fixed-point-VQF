#pragma once

/* Optional CH32 linker sections. The linker script must map these sections.
 * Default builds need no firmware linker script or compiler attributes. */
#if defined(FIXED_VQF_USE_CH32_SECTIONS) && FIXED_VQF_USE_CH32_SECTIONS
#if !defined(__GNUC__)
#error "CH32 section placement requires a GCC-compatible compiler"
#endif
#define FAST_CODE __attribute__((section(".fasttext"), noinline))
#define SLOW_CODE __attribute__((section(".slowtext"), noinline))
#define SLOW_RODATA __attribute__((section(".slowrodata")))
#define USED __attribute__((used))
#else
#define FAST_CODE
#define SLOW_CODE
#define SLOW_RODATA
#define USED
#endif
