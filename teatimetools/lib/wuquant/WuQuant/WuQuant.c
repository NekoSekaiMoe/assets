#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WuQuant.h"

#if WU_USE_SSE

#define MASK_AND      _mm_set_epi32(0xFF000000, 0x000000FF, 0x0000FF00, 0x00FF0000)
#define MASK_SHIFT    _mm_set_epi32(24, 0, 8, 16)
#define MASK_ONE      _mm_set1_epi32(1)
#define MASK_SHIFTIDX _mm_set_epi32(8 - INDEXALPHABITS, 8 - INDEXBITS, 8 - INDEXBITS, 8 - INDEXBITS)

#else

// Scalar helper functions for non-x86 platforms
static inline void v4i_add(V4i* result, const V4i* a, const V4i* b) {
    result->R = a->R + b->R;
    result->G = a->G + b->G;
    result->B = a->B + b->B;
    result->A = a->A + b->A;
}

static inline void v4i_sub(V4i* result, const V4i* a, const V4i* b) {
    result->R = a->R - b->R;
    result->G = a->G - b->G;
    result->B = a->B - b->B;
    result->A = a->A - b->A;
}

static inline void v4f_sub(V4f* result, const V4f* a, const V4f* b) {
    result->R = a->R - b->R;
    result->G = a->G - b->G;
    result->B = a->B - b->B;
    result->A = a->A - b->A;
}

static inline void v4i_to_v4f(V4f* result, const V4i* a) {
    result->R = (float)a->R;
    result->G = (float)a->G;
    result->B = (float)a->B;
    result->A = (float)a->A;
}

static inline float v4f_dot(const V4f* a, const V4f* b) {
    return a->R * b->R + a->G * b->G + a->B * b->B + a->A * b->A;
}

static inline void v4f_mul_scalar(V4f* result, const V4f* a, float s) {
    result->R = a->R * s;
    result->G = a->G * s;
    result->B = a->B * s;
    result->A = a->A * s;
}

static inline void v4f_div_scalar(V4f* result, const V4f* a, float s) {
    result->R = a->R / s;
    result->G = a->G / s;
    result->B = a->B / s;
    result->A = a->A / s;
}

static inline void v4i_setzero(V4i* v) {
    v->R = v->G = v->B = v->A = 0;
}

#endif

__forceinline static unsigned int GetIndex(
   const unsigned int r, 
   const unsigned int g, 
   const unsigned int b, 
   const unsigned int a)
{
   return 
        (r << ((INDEXBITS * 2) + INDEXALPHABITS))
      + (r << (INDEXBITS + INDEXALPHABITS + 1))
      + (g << (INDEXBITS + INDEXALPHABITS))
      + (r << (INDEXBITS * 2))
      + (r << (INDEXBITS + 1))
      + (g << INDEXBITS)
      + ((r + g + b) << INDEXALPHABITS)
      + r + g + b + a;
}

__forceinline static void Bottom(const Moment* m, 
   const unsigned int IDX1, const unsigned int IDX2, const unsigned int IDX3, const unsigned int IDX4,
   const unsigned int IDX5, const unsigned int IDX6, const unsigned int IDX7, const unsigned int IDX8,
   V4i* v, int* w)
{
#if WU_USE_SSE
   const __m128i s1 = _mm_setzero_si128();
   const __m128i s2 = _mm_sub_epi32(s1, m[IDX1].P.SSE);
   const __m128i s3 = _mm_add_epi32(s2, m[IDX2].P.SSE);
   const __m128i s4 = _mm_add_epi32(s3, m[IDX3].P.SSE);
   const __m128i s5 = _mm_sub_epi32(s4, m[IDX4].P.SSE);
   const __m128i s6 = _mm_add_epi32(s5, m[IDX5].P.SSE);
   const __m128i s7 = _mm_sub_epi32(s6, m[IDX6].P.SSE);
   const __m128i s8 = _mm_sub_epi32(s7, m[IDX7].P.SSE);
   v->SSE = _mm_add_epi32(s8, m[IDX8].P.SSE);
#else
   V4i t1, t2, t3, t4, t5, t6, t7;
   v4i_setzero(&t1);
   v4i_sub(&t2, &t1, &m[IDX1].P);
   v4i_add(&t3, &t2, &m[IDX2].P);
   v4i_add(&t4, &t3, &m[IDX3].P);
   v4i_sub(&t5, &t4, &m[IDX4].P);
   v4i_add(&t6, &t5, &m[IDX5].P);
   v4i_sub(&t7, &t6, &m[IDX6].P);
   v4i_sub(v, &t7, &m[IDX7].P);
   v4i_add(v, v, &m[IDX8].P);
#endif
   *w = -m[IDX1].V + m[IDX2].V + m[IDX3].V - m[IDX4].V + m[IDX5].V - m[IDX6].V - m[IDX7].V + m[IDX8].V;
}
__forceinline static void BottomR(const Box* c, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P0.R, c->P1.G, c->P1.B, c->P1.A);
   const unsigned int IDX2 = GetIndex(c->P0.R, c->P1.G, c->P1.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P1.A);
   const unsigned int IDX4 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P1.A);
   const unsigned int IDX6 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX8 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P0.A);
   Bottom(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void BottomG(const Box* c, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, c->P0.G, c->P1.B, c->P1.A);
   const unsigned int IDX2 = GetIndex(c->P1.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX4 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P1.A);
   const unsigned int IDX6 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX8 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P0.A);
   Bottom(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void BottomB(const Box* c, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, c->P1.G, c->P0.B, c->P1.A);
   const unsigned int IDX2 = GetIndex(c->P1.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX4 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P1.A);
   const unsigned int IDX6 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX8 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P0.A);
   Bottom(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void BottomA(const Box* c, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, c->P1.G, c->P1.B, c->P0.A);
   const unsigned int IDX2 = GetIndex(c->P1.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P1.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX4 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P0.R, c->P1.G, c->P1.B, c->P0.A);
   const unsigned int IDX6 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX8 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P0.A);
   Bottom(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}

__forceinline static void Top(const Moment* m,
   const unsigned int IDX1, const unsigned int IDX2, const unsigned int IDX3, const unsigned int IDX4,
   const unsigned int IDX5, const unsigned int IDX6, const unsigned int IDX7, const unsigned int IDX8,
   V4i* v, int* w)
{
#if WU_USE_SSE
   const __m128i s1 = _mm_sub_epi32(m[IDX1].P.SSE, m[IDX2].P.SSE);
   const __m128i s2 = _mm_sub_epi32(s1, m[IDX3].P.SSE);
   const __m128i s3 = _mm_add_epi32(s2, m[IDX4].P.SSE);
   const __m128i s4 = _mm_sub_epi32(s3, m[IDX5].P.SSE);
   const __m128i s5 = _mm_add_epi32(s4, m[IDX6].P.SSE);
   const __m128i s6 = _mm_add_epi32(s5, m[IDX7].P.SSE);
   v->SSE = _mm_sub_epi32(s6, m[IDX8].P.SSE);
#else
   V4i t1, t2, t3, t4, t5, t6;
   v4i_sub(&t1, &m[IDX1].P, &m[IDX2].P);
   v4i_sub(&t2, &t1, &m[IDX3].P);
   v4i_add(&t3, &t2, &m[IDX4].P);
   v4i_sub(&t4, &t3, &m[IDX5].P);
   v4i_add(&t5, &t4, &m[IDX6].P);
   v4i_add(&t6, &t5, &m[IDX7].P);
   v4i_sub(v, &t6, &m[IDX8].P);
#endif
   *w = m[IDX1].V - m[IDX2].V - m[IDX3].V + m[IDX4].V - m[IDX5].V + m[IDX6].V + m[IDX7].V - m[IDX8].V;
}
__forceinline static void TopR(const Box* c, const int position, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(position, c->P1.G, c->P1.B, c->P1.A);
   const unsigned int IDX2 = GetIndex(position, c->P1.G, c->P1.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(position, c->P1.G, c->P0.B, c->P1.A);
   const unsigned int IDX4 = GetIndex(position, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(position, c->P0.G, c->P1.B, c->P1.A);
   const unsigned int IDX6 = GetIndex(position, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(position, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX8 = GetIndex(position, c->P0.G, c->P0.B, c->P0.A);
   Top(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void TopG(const Box* c, const int position, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, position, c->P1.B, c->P1.A);
   const unsigned int IDX2 = GetIndex(c->P1.R, position, c->P1.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P1.R, position, c->P0.B, c->P1.A);
   const unsigned int IDX4 = GetIndex(c->P1.R, position, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P0.R, position, c->P1.B, c->P1.A);
   const unsigned int IDX6 = GetIndex(c->P0.R, position, c->P1.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P0.R, position, c->P0.B, c->P1.A);
   const unsigned int IDX8 = GetIndex(c->P0.R, position, c->P0.B, c->P0.A);
   Top(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void TopB(const Box* c, const int position, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, c->P1.G, position, c->P1.A);
   const unsigned int IDX2 = GetIndex(c->P1.R, c->P1.G, position, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P1.R, c->P0.G, position, c->P1.A);
   const unsigned int IDX4 = GetIndex(c->P1.R, c->P0.G, position, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P0.R, c->P1.G, position, c->P1.A);
   const unsigned int IDX6 = GetIndex(c->P0.R, c->P1.G, position, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P0.R, c->P0.G, position, c->P1.A);
   const unsigned int IDX8 = GetIndex(c->P0.R, c->P0.G, position, c->P0.A);
   Top(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void TopA(const Box* c, const int position, const Moment* m, V4i* v, int* w)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, c->P1.G, c->P1.B, position);
   const unsigned int IDX2 = GetIndex(c->P1.R, c->P1.G, c->P0.B, position);
   const unsigned int IDX3 = GetIndex(c->P1.R, c->P0.G, c->P1.B, position);
   const unsigned int IDX4 = GetIndex(c->P1.R, c->P0.G, c->P0.B, position);
   const unsigned int IDX5 = GetIndex(c->P0.R, c->P1.G, c->P1.B, position);
   const unsigned int IDX6 = GetIndex(c->P0.R, c->P1.G, c->P0.B, position);
   const unsigned int IDX7 = GetIndex(c->P0.R, c->P0.G, c->P1.B, position);
   const unsigned int IDX8 = GetIndex(c->P0.R, c->P0.G, c->P0.B, position);
   Top(m, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}

__forceinline static void Maximize(const Quantizer* quantizer, const Box* cube, V4i* cut, const V4f* whole, const float wholeW, V4f* ma)
{
   float max;
   V4i base; int baseW;
   
   // R
   BottomR(cube, quantizer->v, &base, &baseW);
   max = 0.0;
   cut->R = -1;
   for (int i = cube->P0.R + 1; i < cube->P1.R; i++)
   {
      V4i top; int topW; V4f half; float halfW; float tf;
      TopR(cube, i, quantizer->v, &top, &topW);
#if WU_USE_SSE
      __m128 temp;
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = _mm_cvtss_f32(temp);
#else
      V4i sum_vi; V4f sum_vf;
      v4i_add(&sum_vi, &base, &top);
      v4i_to_v4f(&half, &sum_vi);
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      float temp = v4f_dot(&half, &half) / halfW;
      V4f half_sub;
      v4f_sub(&half_sub, whole, &half);
      half = half_sub;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += v4f_dot(&half, &half) / halfW;
      tf = temp;
#endif
      if (tf > max)
      {
         max = tf;
         cut->R = i;
      }
   }
   ma->R = max;

   // G
   BottomG(cube, quantizer->v, &base, &baseW);
   max = 0.0;
   cut->G = -1;
   for (int i = cube->P0.G + 1; i < cube->P1.G; i++)
   {
      V4i top; int topW; V4f half; float halfW; float tf;
      TopG(cube, i, quantizer->v, &top, &topW);
#if WU_USE_SSE
      __m128 temp;
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = _mm_cvtss_f32(temp);
#else
      V4i sum_vi; V4f sum_vf;
      v4i_add(&sum_vi, &base, &top);
      v4i_to_v4f(&half, &sum_vi);
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      float temp = v4f_dot(&half, &half) / halfW;
      V4f half_sub;
      v4f_sub(&half_sub, whole, &half);
      half = half_sub;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += v4f_dot(&half, &half) / halfW;
      tf = temp;
#endif
      if (tf > max)
      {
         max = tf;
         cut->G = i;
      }
   }
   ma->G = max;

   // B
   BottomB(cube, quantizer->v, &base, &baseW);
   max = 0.0;
   cut->B = -1;
   for (int i = cube->P0.B + 1; i < cube->P1.B; i++)
   {
      V4i top; int topW; V4f half; float halfW; float tf;
      TopB(cube, i, quantizer->v, &top, &topW);
#if WU_USE_SSE
      __m128 temp;
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = _mm_cvtss_f32(temp);
#else
      V4i sum_vi; V4f sum_vf;
      v4i_add(&sum_vi, &base, &top);
      v4i_to_v4f(&half, &sum_vi);
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      float temp = v4f_dot(&half, &half) / halfW;
      V4f half_sub;
      v4f_sub(&half_sub, whole, &half);
      half = half_sub;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += v4f_dot(&half, &half) / halfW;
      tf = temp;
#endif
      if (tf > max)
      {
         max = tf;
         cut->B = i;
      }
   }
   ma->B = max;

   // A
   BottomA(cube, quantizer->v, &base, &baseW);
   max = 0.0;
   cut->A = -1;
   for (int i = cube->P0.A + 1; i < cube->P1.A; i++)
   {
      V4i top; int topW; V4f half; float halfW; float tf;
      TopA(cube, i, quantizer->v, &top, &topW);
#if WU_USE_SSE
      __m128 temp;
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = _mm_cvtss_f32(temp);
#else
      V4i sum_vi; V4f sum_vf;
      v4i_add(&sum_vi, &base, &top);
      v4i_to_v4f(&half, &sum_vi);
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      float temp = v4f_dot(&half, &half) / halfW;
      V4f half_sub;
      v4f_sub(&half_sub, whole, &half);
      half = half_sub;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += v4f_dot(&half, &half) / halfW;
      tf = temp;
#endif
      if (tf > max)
      {
         max = tf;
         cut->A = i;
      }
   }
   ma->A = max;
}

__forceinline static void _Volume(const Moment* m,
   const unsigned int IDX1, const unsigned int IDX2, const unsigned int IDX3, const unsigned int IDX4,
   const unsigned int IDX5, const unsigned int IDX6, const unsigned int IDX7, const unsigned int IDX8,
   const unsigned int IDX9, const unsigned int IDX10, const unsigned int IDX11, const unsigned int IDX12,
   const unsigned int IDX13, const unsigned int IDX14, const unsigned int IDX15, const unsigned int IDX16,
   V4f* v, float* w)
{
#if WU_USE_SSE
   __m128i s01 = _mm_sub_epi32(m[IDX1].P.SSE, m[IDX2].P.SSE);
   __m128i s02 = _mm_sub_epi32(s01, m[IDX3].P.SSE);
   __m128i s03 = _mm_add_epi32(s02, m[IDX4].P.SSE);
   __m128i s04 = _mm_sub_epi32(s03, m[IDX5].P.SSE);
   __m128i s05 = _mm_add_epi32(s04, m[IDX6].P.SSE);
   __m128i s06 = _mm_add_epi32(s05, m[IDX7].P.SSE);
   __m128i s07 = _mm_sub_epi32(s06, m[IDX8].P.SSE);
   __m128i s08 = _mm_sub_epi32(s07, m[IDX9].P.SSE);
   __m128i s09 = _mm_add_epi32(s08, m[IDX10].P.SSE);
   __m128i s10 = _mm_add_epi32(s09, m[IDX11].P.SSE);
   __m128i s11 = _mm_sub_epi32(s10, m[IDX12].P.SSE);
   __m128i s12 = _mm_add_epi32(s11, m[IDX13].P.SSE);
   __m128i s13 = _mm_sub_epi32(s12, m[IDX14].P.SSE);
   __m128i s14 = _mm_sub_epi32(s13, m[IDX15].P.SSE);
   __m128i s15 = _mm_add_epi32(s14, m[IDX16].P.SSE);
   v->SSE = _mm_cvtepi32_ps(s15);
#else
   V4i t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15;
   v4i_sub(&t1, &m[IDX1].P, &m[IDX2].P);
   v4i_sub(&t2, &t1, &m[IDX3].P);
   v4i_add(&t3, &t2, &m[IDX4].P);
   v4i_sub(&t4, &t3, &m[IDX5].P);
   v4i_add(&t5, &t4, &m[IDX6].P);
   v4i_add(&t6, &t5, &m[IDX7].P);
   v4i_sub(&t7, &t6, &m[IDX8].P);
   v4i_sub(&t8, &t7, &m[IDX9].P);
   v4i_add(&t9, &t8, &m[IDX10].P);
   v4i_add(&t10, &t9, &m[IDX11].P);
   v4i_sub(&t11, &t10, &m[IDX12].P);
   v4i_add(&t12, &t11, &m[IDX13].P);
   v4i_sub(&t13, &t12, &m[IDX14].P);
   v4i_sub(&t14, &t13, &m[IDX15].P);
   v4i_add(&t15, &t14, &m[IDX16].P);
   v4i_to_v4f(v, &t15);
#endif

   *w = (float)(
      m[IDX1].V  - m[IDX2].V  - m[IDX3].V  + m[IDX4].V  -
      m[IDX5].V  + m[IDX6].V  + m[IDX7].V  - m[IDX8].V  -
      m[IDX9].V  + m[IDX10].V + m[IDX11].V - m[IDX12].V +
      m[IDX13].V - m[IDX14].V - m[IDX15].V + m[IDX16].V);
}

__forceinline static void Volume(const Box* cube, const Moment* m, V4f* col, float* w)
{
   const unsigned int IDX1 = GetIndex(cube->P1.R, cube->P1.G, cube->P1.B, cube->P1.A);
   const unsigned int IDX2 = GetIndex(cube->P1.R, cube->P1.G, cube->P1.B, cube->P0.A);
   const unsigned int IDX3 = GetIndex(cube->P1.R, cube->P1.G, cube->P0.B, cube->P1.A);
   const unsigned int IDX4 = GetIndex(cube->P1.R, cube->P1.G, cube->P0.B, cube->P0.A);
   const unsigned int IDX5 = GetIndex(cube->P1.R, cube->P0.G, cube->P1.B, cube->P1.A);
   const unsigned int IDX6 = GetIndex(cube->P1.R, cube->P0.G, cube->P1.B, cube->P0.A);
   const unsigned int IDX7 = GetIndex(cube->P1.R, cube->P0.G, cube->P0.B, cube->P1.A);
   const unsigned int IDX8 = GetIndex(cube->P1.R, cube->P0.G, cube->P0.B, cube->P0.A);
   const unsigned int IDX9 = GetIndex(cube->P0.R, cube->P1.G, cube->P1.B, cube->P1.A);
   const unsigned int IDX10 = GetIndex(cube->P0.R, cube->P1.G, cube->P1.B, cube->P0.A);
   const unsigned int IDX11 = GetIndex(cube->P0.R, cube->P1.G, cube->P0.B, cube->P1.A);
   const unsigned int IDX12 = GetIndex(cube->P0.R, cube->P1.G, cube->P0.B, cube->P0.A);
   const unsigned int IDX13 = GetIndex(cube->P0.R, cube->P0.G, cube->P1.B, cube->P1.A);
   const unsigned int IDX14 = GetIndex(cube->P0.R, cube->P0.G, cube->P1.B, cube->P0.A);
   const unsigned int IDX15 = GetIndex(cube->P0.R, cube->P0.G, cube->P0.B, cube->P1.A);
   const unsigned int IDX16 = GetIndex(cube->P0.R, cube->P0.G, cube->P0.B, cube->P0.A);

   _Volume(m, 
      IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8,
      IDX9, IDX10, IDX11, IDX12, IDX13, IDX14, IDX15, IDX16,
      col, w);
}

__forceinline static float Variance(const Moment* m, const Box* c)
{
   const unsigned int IDX1 = GetIndex(c->P1.R, c->P1.G, c->P1.B, c->P1.A);
   const unsigned int IDX2 = GetIndex(c->P1.R, c->P1.G, c->P1.B, c->P0.A);
   const unsigned int IDX3 = GetIndex(c->P1.R, c->P1.G, c->P0.B, c->P1.A);
   const unsigned int IDX4 = GetIndex(c->P1.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX5 = GetIndex(c->P1.R, c->P0.G, c->P1.B, c->P1.A);
   const unsigned int IDX6 = GetIndex(c->P1.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX7 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX8 = GetIndex(c->P1.R, c->P0.G, c->P0.B, c->P0.A);
   const unsigned int IDX9 = GetIndex(c->P0.R, c->P1.G, c->P1.B, c->P1.A);
   const unsigned int IDX10 = GetIndex(c->P0.R, c->P1.G, c->P1.B, c->P0.A);
   const unsigned int IDX11 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P1.A);
   const unsigned int IDX12 = GetIndex(c->P0.R, c->P1.G, c->P0.B, c->P0.A);
   const unsigned int IDX13 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P1.A);
   const unsigned int IDX14 = GetIndex(c->P0.R, c->P0.G, c->P1.B, c->P0.A);
   const unsigned int IDX15 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P1.A);
   const unsigned int IDX16 = GetIndex(c->P0.R, c->P0.G, c->P0.B, c->P0.A);

   const float xx =
      + m[IDX1].V2  - m[IDX2].V2  - m[IDX3].V2  + m[IDX4].V2
      - m[IDX5].V2  + m[IDX6].V2  + m[IDX7].V2  - m[IDX8].V2
      - m[IDX9].V2  + m[IDX10].V2 + m[IDX11].V2 - m[IDX12].V2
      + m[IDX13].V2 - m[IDX14].V2 - m[IDX15].V2 + m[IDX16].V2;

   V4f d; 
   float vol;

   _Volume(m,
      IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8,
      IDX9, IDX10, IDX11, IDX12, IDX13, IDX14, IDX15, IDX16,
      &d, &vol);

#if WU_USE_SSE
   return xx - _mm_cvtss_f32(_mm_div_ss(_mm_dp_ps(d.SSE, d.SSE, 0xF1), _mm_set_ss(vol)));
#else
   return xx - v4f_dot(&d, &d) / vol;
#endif
}

__forceinline static int Cut(const Quantizer* quantizer, Box* set1, Box* set2)
{
   V4f whole; float wholeW;
   V4f max; V4i cut;
   V4i sub1; V4i sub2;

   Volume(set1, quantizer->v, &whole, &wholeW);
   Maximize(quantizer, set1, &cut, &whole, wholeW, &max);

#if WU_USE_SSE
   set2->P1.SSE = set1->P1.SSE;
   set2->P0.SSE = set1->P0.SSE;
#else
   set2->P1 = set1->P1;
   set2->P0 = set1->P0;
#endif

   // RED
   if ((max.R >= max.G) & (max.R >= max.B) & (max.R >= max.A))
   {
      if (cut.R < 0)
         return 0;

      set2->P0.R = set1->P1.R = cut.R;
   }

   // GREEN
   else if ((max.G >= max.R) & (max.G >= max.B) & (max.G >= max.A))
      set2->P0.G = set1->P1.G = cut.G;

   // BLUE
   else if ((max.B >= max.R) & (max.B >= max.G) & (max.B >= max.A))
      set2->P0.B = set1->P1.B = cut.B;

   // ALPHA
   else
      set2->P0.A = set1->P1.A = cut.A;

#if WU_USE_SSE
   sub1.SSE = _mm_sub_epi32(set1->P1.SSE, set1->P0.SSE);
   sub2.SSE = _mm_sub_epi32(set2->P1.SSE, set2->P0.SSE);
#else
   v4i_sub(&sub1, &set1->P1, &set1->P0);
   v4i_sub(&sub2, &set2->P1, &set2->P0);
#endif

   set1->Volume = sub1.R * sub1.G * sub1.B * sub1.A;
   set2->Volume = sub2.R * sub2.G * sub2.B * sub2.A;

   return 1;
}

__forceinline static void Mark(char* tags, const Box* c, const char label)
{
   for (int r = c->P0.R + 1; r <= c->P1.R; r++)
      for (int g = c->P0.G + 1; g <= c->P1.G; g++)
         for (int b = c->P0.B + 1; b <= c->P1.B; b++)
            for (int a = c->P0.A + 1; a <= c->P1.A; a++)
               tags[GetIndex(r, g, b, a)] = label;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS

static void Clear(Quantizer* quantizer)
{
   memset(quantizer->v, 0, sizeof(quantizer->v));
   memset(quantizer->tag, 0, sizeof(quantizer->tag));
   memset(quantizer->cube, 0, sizeof(quantizer->cube));
}

static void Build3DHistogram(Moment* m, unsigned int* image, int width, int height)
{
   const unsigned int PIXELS = width * height;
   for (unsigned int i = 0; i < PIXELS; i++)
   {
      V4i p, in;
      unsigned int pix = image[i];

      p.A = (pix & 0xFF000000) >> 24;
      p.R = (pix & 0x00FF0000) >> 16;
      p.G = (pix & 0x0000FF00) >> 8;
      p.B = pix & 0x000000FF;
      in.R = p.R >> (8 - INDEXBITS);
      in.G = p.G >> (8 - INDEXBITS);
      in.B = p.B >> (8 - INDEXBITS);
      in.A = p.A >> (8 - INDEXALPHABITS);
      const unsigned int IDX = GetIndex((int)in.R + 1, (int)in.G + 1, (int)in.B + 1, (int)in.A + 1);

#if WU_USE_SSE
      m[IDX].P.SSE = _mm_add_epi32(m[IDX].P.SSE, p.SSE);
#else
      v4i_add(&m[IDX].P, &m[IDX].P, &p);
#endif
      m[IDX].V++;
      m[IDX].V2 += (p.R * p.R) + (p.G * p.G) + (p.B * p.B) + (p.A * p.A);
   }
}

static void Get3DMoments(Quantizer* quantizer)
{
   V4i l;
   for (unsigned int r = 1; r < INDEXCOUNT; r++)
   {
      memset(quantizer->volume, 0, sizeof(quantizer->volume));

      for (unsigned int g = 1; g < INDEXCOUNT; g++)
      {
         memset(quantizer->area, 0, sizeof(quantizer->area));

         for (int b = 1; b < INDEXCOUNT; b++)
         {
#if WU_USE_SSE
            l.SSE = _mm_setzero_si128();
#else
            v4i_setzero(&l);
#endif

            int v = 0;
            float v2 = 0;

            for (unsigned int a = 1; a < INDEXALPHACOUNT; a++)
            {
               const unsigned int ind1 = GetIndex(r, g, b, a);

#if WU_USE_SSE
               l.SSE = _mm_add_epi32(l.SSE, quantizer->v[ind1].P.SSE);
#else
               v4i_add(&l, &l, &quantizer->v[ind1].P);
#endif
               v  += quantizer->v[ind1].V;
               v2 += quantizer->v[ind1].V2;

#if WU_USE_SSE
               quantizer->area[a].P.SSE = _mm_add_epi32(quantizer->area[a].P.SSE, l.SSE);
#else
               v4i_add(&quantizer->area[a].P, &quantizer->area[a].P, &l);
#endif
               quantizer->area[a].V  += v;
               quantizer->area[a].V2 += v2;

               const unsigned int inv = (b * INDEXALPHACOUNT) + a;

#if WU_USE_SSE
               quantizer->volume[inv].P.SSE = _mm_add_epi32(quantizer->volume[inv].P.SSE, quantizer->area[a].P.SSE);
#else
               v4i_add(&quantizer->volume[inv].P, &quantizer->volume[inv].P, &quantizer->area[a].P);
#endif
               quantizer->volume[inv].V  += quantizer->area[a].V;
               quantizer->volume[inv].V2 += quantizer->area[a].V2;

               const unsigned int ind2 = ind1 - GetIndex(1, 0, 0, 0);

#if WU_USE_SSE
               quantizer->v[ind1].P.SSE = _mm_add_epi32(quantizer->v[ind2].P.SSE, quantizer->volume[inv].P.SSE);
#else
               v4i_add(&quantizer->v[ind1].P, &quantizer->v[ind2].P, &quantizer->volume[inv].P);
#endif
               quantizer->v[ind1].V  = quantizer->v[ind2].V + quantizer->volume[inv].V;
               quantizer->v[ind1].V2 = quantizer->v[ind2].V2 + quantizer->volume[inv].V2;
            }
         }
      }
   }
}

static unsigned int BuildCube(Quantizer* q)
{
#if WU_USE_SSE
   q->cube[0].P0.SSE = _mm_setzero_si128();
   q->cube[0].P1.SSE = _mm_set_epi32(INDEXALPHACOUNT - 1, INDEXCOUNT - 1, INDEXCOUNT - 1, INDEXCOUNT - 1);
#else
   v4i_setzero(&q->cube[0].P0);
   q->cube[0].P1.R = INDEXCOUNT - 1;
   q->cube[0].P1.G = INDEXCOUNT - 1;
   q->cube[0].P1.B = INDEXCOUNT - 1;
   q->cube[0].P1.A = INDEXALPHACOUNT - 1;
#endif
   unsigned int next = 0;
   for (unsigned int i = 1; i < MAXCOLORS; i++)
   {
      Box* cubeNext = &q->cube[next];
      Box* cubeI    = &q->cube[i];
      if (Cut(q, cubeNext, cubeI))
      {
         cubeNext->vv = cubeNext->Volume > 1 ? Variance(q->v, cubeNext) : 0.0f;
         cubeI->vv    = cubeI->Volume    > 1 ? Variance(q->v, cubeI)    : 0.0f;
      }
      else
      {
         cubeNext->vv = 0.0f;
         i--;
      }
      next = 0;
      float temp = q->cube[0].vv;
      for (unsigned int k = 1; k <= i; k++)
      {
         if (q->cube[k].vv > temp)
         {
            temp = q->cube[k].vv;
            next = k;
         }
      }
      if (temp <= 0.0f)
         return i + 1;
   }
   return MAXCOLORS;
}

static void GenerateResult(
   Quantizer*    quantizer, 
   unsigned int* image, 
   unsigned int* palette, 
   unsigned int  colorCount, 
   unsigned int  width, 
   unsigned int  height, 
   char*         destPixels, 
   int           padMultiple4)
{
   V4f d; float weight;
   V4i di; 
   //V4i in;

   // rows must be a multiple of 4, hence padding up to 3 bytes for 8-bit indexed pixels
   const unsigned int WIDTHMOD4 = width % 4;
   const unsigned int WIDTHZEROS = WIDTHMOD4 != 0 ? 4 - WIDTHMOD4 : 0;
   
   // create palette
   for (unsigned int k = 0; k < colorCount; k++)
   {
      Mark(quantizer->tag, &quantizer->cube[k], (char)k);
      Volume(&quantizer->cube[k], quantizer->v, &d, &weight);

      if (weight > 0.01 || weight < -0.01)
      {
#if WU_USE_SSE
         di.SSE = _mm_cvtps_epi32(_mm_mul_ps(d.SSE, _mm_set1_ps(1.0f / weight)));
#else
         di.R = (int)(d.R / weight);
         di.G = (int)(d.G / weight);
         di.B = (int)(d.B / weight);
         di.A = (int)(d.A / weight);
#endif
         palette[k] = (di.A << 24) | (di.R << 16) | (di.G << 8) | di.B;
      }
      else
      {
         palette[k] = 0xFF000000;
      }
   }

   // create pixels
   for (unsigned int ri = 0; ri < height; ri++)
   {
      for (unsigned int ci = 0; ci < width; ci++)
      {
         const unsigned int pix = image[0];

         const unsigned int a = ((pix & 0xFF000000) >> 24) >> (8 - INDEXALPHABITS);
         const unsigned int r = ((pix & 0x00FF0000) >> 16) >> (8 - INDEXBITS);
         const unsigned int g = ((pix & 0x0000FF00) >> 8) >> (8 - INDEXBITS);
         const unsigned int b = (pix & 0x000000FF) >> (8 - INDEXBITS);
         const unsigned int IDX = GetIndex(r + 1, g + 1, b + 1, a + 1);

         destPixels[0] = quantizer->tag[IDX];
         destPixels++;
         image++;
      }

      // write additional zero bytes if requested
      if (padMultiple4)
      {
         for (unsigned int c = 0; c < WIDTHZEROS; c++)
         {
            destPixels[0] = 0x00;
            destPixels++;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC

Quantizer* Create()
{
   // allocate
   Quantizer* quantizer = (Quantizer*)malloc(sizeof(Quantizer));

   // zero by default
   memset(quantizer, 0, sizeof(Quantizer));

   // return
   return quantizer;
}

void Destroy(Quantizer* quantizer)
{
   free(quantizer);
}

int Quantize(
   Quantizer*    quantizer, 
   unsigned int* image, 
   unsigned int* palette, 
   int*          colorCount, 
   int           width, 
   int           height, 
   char*         destPixels, 
   int           padMultiple4)
{
   // nullptr check
   if ((quantizer == 0) | (image == 0) | (palette == 0) | (colorCount == 0) | (destPixels == 0))
      return 0;

   // range check
   const int COLCOUNT = *colorCount;
   if ((COLCOUNT < 1) | (COLCOUNT > MAXCOLORS))
      return 0;

   // execute
   Clear(quantizer);
   Build3DHistogram(quantizer->v, image, width, height);
   Get3DMoments(quantizer);

   // build cube
   *colorCount = (int)BuildCube(quantizer);

   // generate result
   GenerateResult(quantizer, image, palette, *colorCount, width, height, destPixels, padMultiple4);

   return 1;
}