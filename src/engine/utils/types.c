#pragma once

#include <stdint.h>

typedef uintptr_t uPtr;
typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint8_t u8;
typedef int8_t i8;
typedef float f32;
typedef double f64;

typedef u16 id;

typedef f32 percent; //NOTE(VictorGallet): Goes from 0.0 to 1.0 for [0%-100%]

typedef struct { u32 m_x, m_y; } vec2u32;
typedef struct { i32 m_x, m_y; } vec2i32;

// Helper macros
#define concat(a, b) (a ## b)
#define concat2(a, b) concat(a, b)
#define macro_var(name) concat2(name, __LINE__)
#define scopeVar(start, ...) for(start;;) { __VA_ARGS__ }
#define deferVar(start, end) for(start;;end)
#define defer(start, end) for(i32 macro_var(_i_) = (start, 0); !macro_var(_i_); (macro_var(_i_) += 1), end)