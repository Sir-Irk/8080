#include <math.h>
#include <stdint.h>
#include <x86intrin.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i32 b32;

typedef float r32;
typedef double r64;

#define PI_32 3.14159265358979323846f

#ifdef _MSC_VER
#define SI_INLINE __forceinline
#else
#define SI_INLINE inline __attribute__((always_inline))
#endif

#define si_min(a, b) ((a) < (b) ? (a) : (b))
#define si_max(a, b) ((a) > (b) ? (a) : (b))

typedef union {
    struct {
        r32 x, y, z;
    };
    r32 v[3];
} si_v3;

typedef union {
    struct {
        r32 x, y;
    };
    r32 v[2];
} si_v2;

typedef struct {
    r32 v[4][4];
} si_mat4x4;

SI_INLINE si_v2
si_v3_to_v2(si_v3 v)
{
    si_v2 result = { v.x, v.y };
    return result;
}

SI_INLINE si_v3
si_v2_to_v3(si_v2 v)
{
    si_v3 result = { v.x, v.y, 0.0f };
    return result;
}

SI_INLINE si_v3
si_v3_f(r32 x, r32 y, r32 z)
{
    si_v3 result = { x, y, z };
    return result;
}

SI_INLINE si_v2
si_v2_add(si_v2 a, si_v2 b)
{
    si_v2 result = { a.x + b.x, a.y + b.y };
    return result;
}

SI_INLINE si_v2
si_v2_mul(si_v2 a, r32 s)
{
    si_v2 result = { a.x * s, a.y * s };
    return result;
}

SI_INLINE si_v2
si_v2_sub(si_v2 a, si_v2 b)
{
    si_v2 result = { a.x - b.x, a.y - b.y };
    return result;
}

SI_INLINE b32
si_v2_contains(si_v2 min, si_v2 max, si_v2 point)
{
    if (point.x > max.x || point.x < min.x)
        return 0;
    if (point.y > max.y || point.y < min.y)
        return 0;
    return 1;
}

SI_INLINE si_mat4x4
si_mat4x4_identity()
{
    si_mat4x4 result = {};
    result.v[0][0] = 1.0f;
    result.v[1][1] = 1.0f;
    result.v[2][2] = 1.0f;
    result.v[3][3] = 1.0f;
    return result;
}

SI_INLINE si_mat4x4
si_mat4x4_translate(si_mat4x4 m, si_v3 v)
{
    m.v[3][0] = v.x;
    m.v[3][1] = v.y;
    m.v[3][2] = v.z;
    return m;
}

SI_INLINE si_mat4x4
si_mat4x4_scale_f(float scale)
{
    si_mat4x4 result = si_mat4x4_identity();
    result.v[0][0] = scale;
    result.v[1][1] = scale;
    result.v[2][2] = scale;
    return result;
}

SI_INLINE si_mat4x4
si_orthographic(r32 left, r32 right, r32 bottom, r32 top, r32 nearDist, r32 farDist)
{
    si_mat4x4 result = {};

    result.v[0][0] = 2.0f / (right - left);
    result.v[1][1] = 2.0f / (top - bottom);
    result.v[2][2] = 2.0f / (nearDist - farDist);

    result.v[3][0] = (left + right) / (left - right);
    result.v[3][1] = (bottom + top) / (bottom - top);
    result.v[3][2] = (farDist + nearDist) / (nearDist - farDist);

    result.v[3][3] = 1.0f;

    return result;
}

SI_INLINE si_mat4x4
si_mat4x4_mul(si_mat4x4 a, si_mat4x4 b)
{
    si_mat4x4 result = {};
    for (i32 x = 0; x < 4; ++x) {
        for (i32 y = 0; y < 4; ++y) {
            r32 sum = 0;
            for (i32 i = 0; i < 4; ++i) {
                sum += a.v[i][y] * b.v[x][i];
            }
            result.v[x][y] = sum;
        }
    }
    return result;
}

SI_INLINE si_mat4x4
si_mat4x4_mul3(si_mat4x4 a, si_mat4x4 b, si_mat4x4 c)
{
    si_mat4x4 result = si_mat4x4_mul(a, b);
    result = si_mat4x4_mul(result, c);
    return result;
}

SI_INLINE si_v3
si_v3_invert(si_v3 a)
{
    si_v3 result = { -a.x, -a.y, -a.z };
    return result;
}

SI_INLINE si_v3
si_v3_add(si_v3 a, si_v3 b)
{
    si_v3 result = { a.x + b.x, a.y + b.y, b.z + b.z };
    return result;
}

SI_INLINE si_v3
si_v3_sub(si_v3 a, si_v3 b)
{
    si_v3 result = { a.x - b.x, a.y - b.y, b.z - b.z };
    return result;
}

SI_INLINE si_v3
si_v3_mul(si_v3 v, float s)
{
    si_v3 result = { v.x * s, v.y * s, v.z * s };
    return result;
}

SI_INLINE float
si_v3_sqr_len(si_v3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

SI_INLINE float
si_v3_len(si_v3 v)
{
    return sqrtf(si_v3_sqr_len(v));
}

SI_INLINE si_v3
si_v3_normalized(si_v3 v)
{
    float len = si_v3_len(v);
    if (len == 0.0f) {
        return v;
    }
    float invLen = 1.0f / len;
    v.x *= invLen;
    v.y *= invLen;
    v.z *= invLen;
    return v;
}

SI_INLINE si_v3
si_v3_cross(si_v3 a, si_v3 b)
{
    si_v3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

SI_INLINE float
si_v3_dot(si_v3 a, si_v3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

SI_INLINE si_mat4x4
si_look_at(si_v3 position, si_v3 target, si_v3 up)
{
    si_v3 f = si_v3_normalized(si_v3_sub(target, position));
    si_v3 r = si_v3_normalized(si_v3_cross(f, si_v3_normalized(up)));
    si_v3 u = si_v3_cross(r, f);

    si_mat4x4 result;

    result.v[0][0] = r.x;
    result.v[0][1] = u.x;
    result.v[0][2] = -f.x;
    result.v[0][3] = 0.0f;

    result.v[1][0] = r.y;
    result.v[1][1] = u.y;
    result.v[1][2] = -f.y;
    result.v[1][3] = 0.0f;

    result.v[2][0] = r.z;
    result.v[2][1] = u.z;
    result.v[2][2] = -f.z;
    result.v[2][3] = 0.0f;

    result.v[3][0] = -si_v3_dot(r, position);
    result.v[3][1] = -si_v3_dot(u, position);
    result.v[3][2] = si_v3_dot(f, position);
    result.v[3][3] = 1.0f;

    return result;
}

static inline __m256
si_v3_len_avx(__m256 x, __m256 y, __m256 z)
{
    __m256 result = _mm256_mul_ps(x, x);
    result = _mm256_add_ps(result, _mm256_mul_ps(y, y));
    result = _mm256_add_ps(result, _mm256_mul_ps(z, z));
    result = _mm256_sqrt_ps(result);
    return result;
}
