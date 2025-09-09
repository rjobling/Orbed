////////////////////////////////////////////////////////////////////////////////
// bigendian.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline s16 swap_endian(s16 i) { return (((i << 8) & 0xff00) | ((i >> 8) & 0x00ff)); }
inline u16 swap_endian(u16 i) { return (((i << 8) & 0xff00) | ((i >> 8) & 0x00ff)); }
inline s32 swap_endian(s32 i) { return (((i << 24) & 0xff000000) | ((i << 8) & 0x00ff0000) | ((i >> 8) & 0x0000ff00) | ((i >> 24) & 0x000000ff)); }
inline u32 swap_endian(u32 i) { return (((i << 24) & 0xff000000) | ((i << 8) & 0x00ff0000) | ((i >> 8) & 0x0000ff00) | ((i >> 24) & 0x000000ff)); }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct big_s16
{
	s16 value;

	big_s16() {}
	big_s16(const big_s16& b) : value(b.value) {}
	explicit big_s16(s16 i) : value(swap_endian(i)) {}
	explicit big_s16(u16 i) : value(swap_endian((s16) i)) {}
	explicit big_s16(s32 i) : value(swap_endian((s16) i)) {}
	explicit big_s16(u32 i) : value(swap_endian((s16) i)) {}

	explicit operator int() const { return swap_endian(value); }
	explicit operator char() const { return ((char) swap_endian(value)); }
	explicit operator short() const { return ((short) swap_endian(value)); }
	explicit operator unsigned int() const { return ((unsigned int) swap_endian(value)); }
	explicit operator unsigned char() const { return ((unsigned char) swap_endian(value)); }
	explicit operator unsigned short() const { return ((unsigned short) swap_endian(value)); }

	big_s16& operator=(big_s16 b) { value = b.value; return *this; }
	big_s16& operator+=(big_s16 b) { value = swap_endian((s16) (swap_endian(b.value) + swap_endian(value))); return *this; }
	big_s16& operator-=(big_s16 b) { value = swap_endian((s16) (swap_endian(b.value) - swap_endian(value))); return *this; }
	big_s16& operator*=(big_s16 b) { value = swap_endian((s16) (swap_endian(b.value) * swap_endian(value))); return *this; }
	big_s16& operator/=(big_s16 b) { value = swap_endian((s16) (swap_endian(b.value) / swap_endian(value))); return *this; }

	big_s16 operator-() const { big_s16 r; r.value = swap_endian((s16) -swap_endian(value)); return r; }
	big_s16 operator+(const big_s16& b) const { big_s16 r; r.value = swap_endian((s16) (swap_endian(value) + swap_endian(b.value))); return r; }
	big_s16 operator-(const big_s16& b) const { big_s16 r; r.value = swap_endian((s16) (swap_endian(value) - swap_endian(b.value))); return r; }
	big_s16 operator*(const big_s16& b) const { big_s16 r; r.value = swap_endian((s16) (swap_endian(value) * swap_endian(b.value))); return r; }
	big_s16 operator/(const big_s16& b) const { big_s16 r; r.value = swap_endian((s16) (swap_endian(value) / swap_endian(b.value))); return r; }

	bool operator==(big_s16 b) const { return (value == b.value); }
	bool operator!=(big_s16 b) const { return (value != b.value); }
	bool operator>(big_s16 b) const { return (swap_endian(value) > swap_endian(b.value)); }
	bool operator<(big_s16 b) const { return (swap_endian(value) < swap_endian(b.value)); }
	bool operator>=(big_s16 b) const { return (swap_endian(value) >= swap_endian(b.value)); }
	bool operator<=(big_s16 b) const { return (swap_endian(value) <= swap_endian(b.value)); }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct big_u16
{
	u16 value;

	big_u16() {}
	big_u16(const big_u16& b) : value(b.value) {}
	explicit big_u16(u16 i) : value(swap_endian(i)) {}
	explicit big_u16(s16 i) : value(swap_endian((u16) i)) {}
	explicit big_u16(s32 i) : value(swap_endian((u16) i)) {}
	explicit big_u16(u32 i) : value(swap_endian((u16) i)) {}

	explicit operator int() const { return swap_endian(value); }
	explicit operator char() const { return ((char) swap_endian(value)); }
	explicit operator short() const { return ((short) swap_endian(value)); }
	explicit operator unsigned int() const { return ((unsigned int) swap_endian(value)); }
	explicit operator unsigned char() const { return ((unsigned char) swap_endian(value)); }
	explicit operator unsigned short() const { return ((unsigned short) swap_endian(value)); }

	big_u16& operator=(big_u16 b) { value = b.value; return *this; }
	big_u16& operator+=(big_u16 b) { value = swap_endian((u16) (swap_endian(b.value) + swap_endian(value))); return *this; }
	big_u16& operator-=(big_u16 b) { value = swap_endian((u16) (swap_endian(b.value) - swap_endian(value))); return *this; }
	big_u16& operator*=(big_u16 b) { value = swap_endian((u16) (swap_endian(b.value) * swap_endian(value))); return *this; }
	big_u16& operator/=(big_u16 b) { value = swap_endian((u16) (swap_endian(b.value) / swap_endian(value))); return *this; }

	big_u16 operator+(const big_u16& b) const { big_u16 r; r.value = swap_endian((u16) (swap_endian(value) + swap_endian(b.value))); return r; }
	big_u16 operator-(const big_u16& b) const { big_u16 r; r.value = swap_endian((u16) (swap_endian(value) - swap_endian(b.value))); return r; }
	big_u16 operator*(const big_u16& b) const { big_u16 r; r.value = swap_endian((u16) (swap_endian(value) * swap_endian(b.value))); return r; }
	big_u16 operator/(const big_u16& b) const { big_u16 r; r.value = swap_endian((u16) (swap_endian(value) / swap_endian(b.value))); return r; }

	bool operator==(big_u16 b) const { return (value == b.value); }
	bool operator!=(big_u16 b) const { return (value != b.value); }
	bool operator>(big_u16 b) const { return (swap_endian(value) > swap_endian(b.value)); }
	bool operator<(big_u16 b) const { return (swap_endian(value) < swap_endian(b.value)); }
	bool operator>=(big_u16 b) const { return (swap_endian(value) >= swap_endian(b.value)); }
	bool operator<=(big_u16 b) const { return (swap_endian(value) <= swap_endian(b.value)); }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct big_s32
{
	s32 value;

	big_s32() {}
	big_s32(const big_s32& b) : value(b.value) {}
	explicit big_s32(s32 i) : value(swap_endian(i)) {}
	explicit big_s32(s16 i) : value(swap_endian((s32) i)) {}
	explicit big_s32(u16 i) : value(swap_endian((s32) i)) {}
	explicit big_s32(u32 i) : value(swap_endian((s32) i)) {}

	explicit operator int() const { return swap_endian(value); }
	explicit operator char() const { return ((char) swap_endian(value)); }
	explicit operator short() const { return ((short) swap_endian(value)); }
	explicit operator unsigned int() const { return ((unsigned int) swap_endian(value)); }
	explicit operator unsigned char() const { return ((unsigned char) swap_endian(value)); }
	explicit operator unsigned short() const { return ((unsigned short) swap_endian(value)); }

	big_s32& operator=(big_s32 b) { value = b.value; return *this; }
	big_s32& operator+=(big_s32 b) { value = swap_endian(swap_endian(b.value) + swap_endian(value)); return *this; }
	big_s32& operator-=(big_s32 b) { value = swap_endian(swap_endian(b.value) - swap_endian(value)); return *this; }
	big_s32& operator*=(big_s32 b) { value = swap_endian(swap_endian(b.value) * swap_endian(value)); return *this; }
	big_s32& operator/=(big_s32 b) { value = swap_endian(swap_endian(b.value) / swap_endian(value)); return *this; }

	big_s32 operator-() const { big_s32 r; r.value = swap_endian(-swap_endian(value)); return r; }
	big_s32 operator+(const big_s32& b) const { big_s32 r; r.value = swap_endian(swap_endian(value) + swap_endian(b.value)); return r; }
	big_s32 operator-(const big_s32& b) const { big_s32 r; r.value = swap_endian(swap_endian(value) - swap_endian(b.value)); return r; }
	big_s32 operator*(const big_s32& b) const { big_s32 r; r.value = swap_endian(swap_endian(value) * swap_endian(b.value)); return r; }
	big_s32 operator/(const big_s32& b) const { big_s32 r; r.value = swap_endian(swap_endian(value) / swap_endian(b.value)); return r; }

	bool operator==(big_s32 b) const { return (value == b.value); }
	bool operator!=(big_s32 b) const { return (value != b.value); }
	bool operator>(big_s32 b) const { return (swap_endian(value) > swap_endian(b.value)); }
	bool operator<(big_s32 b) const { return (swap_endian(value) < swap_endian(b.value)); }
	bool operator>=(big_s32 b) const { return (swap_endian(value) >= swap_endian(b.value)); }
	bool operator<=(big_s32 b) const { return (swap_endian(value) <= swap_endian(b.value)); }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
struct big_u32
{
	u32 value;

	big_u32() {}
	big_u32(const big_u32& b) : value(b.value) {}
	explicit big_u32(u32 i) : value(swap_endian(i)) {}
	explicit big_u32(s16 i) : value(swap_endian((u32) i)) {}
	explicit big_u32(u16 i) : value(swap_endian((u32) i)) {}
	explicit big_u32(s32 i) : value(swap_endian((u32) i)) {}

	explicit operator int() const { return swap_endian(value); }
	explicit operator char() const { return ((char) swap_endian(value)); }
	explicit operator short() const { return ((short) swap_endian(value)); }
	explicit operator unsigned int() const { return ((unsigned int) swap_endian(value)); }
	explicit operator unsigned char() const { return ((unsigned char) swap_endian(value)); }
	explicit operator unsigned short() const { return ((unsigned short) swap_endian(value)); }

	big_u32& operator=(big_u32 b) { value = b.value; return *this; }
	big_u32& operator+=(big_u32 b) { value = swap_endian(swap_endian(b.value) + swap_endian(value)); return *this; }
	big_u32& operator-=(big_u32 b) { value = swap_endian(swap_endian(b.value) - swap_endian(value)); return *this; }
	big_u32& operator*=(big_u32 b) { value = swap_endian(swap_endian(b.value) * swap_endian(value)); return *this; }
	big_u32& operator/=(big_u32 b) { value = swap_endian(swap_endian(b.value) / swap_endian(value)); return *this; }

	big_u32 operator+(const big_u32& b) const { big_u32 r; r.value = swap_endian(swap_endian(value) + swap_endian(b.value)); return r; }
	big_u32 operator-(const big_u32& b) const { big_u32 r; r.value = swap_endian(swap_endian(value) - swap_endian(b.value)); return r; }
	big_u32 operator*(const big_u32& b) const { big_u32 r; r.value = swap_endian(swap_endian(value) * swap_endian(b.value)); return r; }
	big_u32 operator/(const big_u32& b) const { big_u32 r; r.value = swap_endian(swap_endian(value) / swap_endian(b.value)); return r; }

	bool operator==(big_u32 b) const { return (value == b.value); }
	bool operator!=(big_u32 b) const { return (value != b.value); }
	bool operator>(big_u32 b) const { return (swap_endian(value) > swap_endian(b.value)); }
	bool operator<(big_u32 b) const { return (swap_endian(value) < swap_endian(b.value)); }
	bool operator>=(big_u32 b) const { return (swap_endian(value) >= swap_endian(b.value)); }
	bool operator<=(big_u32 b) const { return (swap_endian(value) <= swap_endian(b.value)); }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef big_u32 big_size32;
typedef big_s32 big_p32;
