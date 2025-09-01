////////////////////////////////////////////////////////////////////////////////
// core.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
// Basic types.
////////////////////////////////////////////////////////////////////////////////
typedef char s8;
typedef short s16;
typedef int s32;
typedef unsigned int uint;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

////////////////////////////////////////////////////////////////////////////////
// Type sizes.
////////////////////////////////////////////////////////////////////////////////
template<typename T> inline T private_minof();
template<typename T> inline T private_maxof();
template<> constexpr char private_minof<char>() { return -128; }
template<> constexpr char private_maxof<char>() { return 127; }
template<> constexpr short private_minof<short>() { return -32768; }
template<> constexpr short private_maxof<short>() { return 32767; }
template<> constexpr int private_minof<int>() { return -2147483648LL; }
template<> constexpr int private_maxof<int>() { return 2147483647; }
template<> constexpr long private_minof<long>() { return -2147483648LL; }
template<> constexpr long private_maxof<long>() { return 2147483647; }
template<> constexpr unsigned char private_minof<unsigned char>() { return 0; }
template<> constexpr unsigned char private_maxof<unsigned char>() { return 255; }
template<> constexpr unsigned short private_minof<unsigned short>() { return 0; }
template<> constexpr unsigned short private_maxof<unsigned short>() { return 65535; }
template<> constexpr unsigned int private_minof<unsigned int>() { return 0; }
template<> constexpr unsigned int private_maxof<unsigned int>() { return 4294967295; }
template<> constexpr unsigned long private_minof<unsigned long>() { return 0; }
template<> constexpr unsigned long private_maxof<unsigned long>() { return 4294967295; }
template<> constexpr float private_minof<float>() { return -3.402823466e38f; }
template<> constexpr float private_maxof<float>() { return 3.402823466e38f; }
#define minof(a) private_minof<a>()
#define maxof(a) private_maxof<a>()

////////////////////////////////////////////////////////////////////////////////
// Compiler hints.
////////////////////////////////////////////////////////////////////////////////
#define unused(...) ((void) __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////
// Asserts.
////////////////////////////////////////////////////////////////////////////////
#if defined(DEBUG)
#define assert_message(b, ...) { char str[1024]; if (!(b)) { sprintf(str, "Assert: %s %s:%d\n", __FUNCTION__, __FILE__, __LINE__); OutputDebugString(str); sprintf(str, __VA_ARGS__); OutputDebugString(str); OutputDebugString("\n"); assert(b); } }
#define assert_pointer(p) { assert((p) != nullptr); }
#define assert_pointer_message(p, ...) { assert_message((p) != nullptr, __VA_ARGS__); }
#else
#define assert_message(...)
#define assert_pointer(...)
#define assert_pointer_message(...)
#endif

////////////////////////////////////////////////////////////////////////////////
// Debug.
////////////////////////////////////////////////////////////////////////////////
void dprintf(const char* format, ...);

////////////////////////////////////////////////////////////////////////////////
// Alignment.
////////////////////////////////////////////////////////////////////////////////
template<typename T> constexpr bool isaligned(T a, T b) { return ((a & (b - 1)) == 0); }
template<typename T> constexpr T alignup(T a, T b) { return ((a + b - 1) & ~(b - 1)); }
template<typename T> constexpr T aligndown(T a, T b) { return (a & ~(b - 1)); }
template<typename T> constexpr T alignnext(T a) { a--; a |= a >> 1; a |= a >> 2; a |= a >> 4; a |= a >> 8; a |= a >> 16; a++; return a; }

////////////////////////////////////////////////////////////////////////////////
// Arrays.
////////////////////////////////////////////////////////////////////////////////
#define countof(a) ((int) (sizeof(a) / sizeof((a)[0])))

////////////////////////////////////////////////////////////////////////////////
// Math.
////////////////////////////////////////////////////////////////////////////////
template<typename T> constexpr T sqr(T a) { return (a * a); }
template<typename T> constexpr T cube(T a) { return (a * a * a); }
template<typename T> constexpr T log2(T a) { return __lzcnt(a); }
template<typename T> constexpr T is_log2(T a) { return ((a != 0) && ((a & (a - 1)) == 0)); }
template<typename T> constexpr T abs(T a) { return ((a < 0) ? -a : a); }
template<typename T> constexpr T sign(T a) { return ((a < 0) ? ((T) -1) : ((T) 1)); }
template<typename T> constexpr T min(T a, T b) { return ((a < b) ? a : b); }
template<typename T> constexpr T max(T a, T b) { return ((a > b) ? a : b); }
template<typename T> constexpr T clamp(T a, T amin, T amax) { T t = ((a > amin) ? a : amin); t = ((t < amax) ? t : amax); return t; }
template<typename T> inline void swap(T& a, T& b) { T t = a; a = b; b = t; }
template<typename T, typename TBlend> inline T lerp(T a, T b, TBlend blend) { return (a + (b - a) * blend); }
constexpr float Pi = 3.14159274f;
constexpr float degrees_to_radians(float degrees) { return (degrees * Pi / 180.0f); }
constexpr float radians_to_degrees(float radians) { return (radians * 180.0f / Pi); }
template<typename T> constexpr T mul40(T a) { a <<= 3; return (a + (a << 2)); }
template<typename T> constexpr T mul96(T a) { a <<= 5; return (a + a + a); }
template<typename T> constexpr T mul112(T a) { a <<= 4; return (a + a + a + (a << 2)); }
template<typename T> constexpr T mul128(T a) { return (a << 7); }
template<typename T> constexpr T mul144(T a) { a <<= 4; return (a + (a << 3)); }
template<typename T> constexpr T mul160(T a) { a <<= 5; return (a + (a << 2)); }

////////////////////////////////////////////////////////////////////////////////
// Rounding.
////////////////////////////////////////////////////////////////////////////////
inline constexpr float ceil(float f) { int i = (int) f; return (float) ((f > i) ? i + 1 : i); }
inline constexpr float floor(float f) { int i = (int) f; return (float) ((f < i) ? i - 1 : i); }
inline constexpr float round(float f) { return (f > 0.0f) ? (float) ((int) (f + 0.5f)) :  (float) ((int) (f - 0.5f)); }

////////////////////////////////////////////////////////////////////////////////
// Rand.
////////////////////////////////////////////////////////////////////////////////
inline u16 rand(int& value) { value = value * 1325 + 715136305; return ((value >> 16) & 0x7fff); }
