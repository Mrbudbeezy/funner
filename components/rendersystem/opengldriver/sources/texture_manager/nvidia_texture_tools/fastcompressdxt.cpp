// Copyright NVIDIA Corporation 2007 -- Ignacio Castano <icastano@nvidia.com>
// 
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include "Color.h"
#include "ColorBlock.h"
#include "BlockDXT.h"

#include "FastCompressDXT.h"

#include <algorithm>

#if defined(__SSE2__)
#include <emmintrin.h>
#endif

#if defined(__MMX__)
#include <mmintrin.h>
#include <xmmintrin.h>
#endif

#undef __VEC__
#if defined(__VEC__)
#include <altivec.h>
#undef bool
#endif
// Online Resources:
// - http://www.jasondorie.com/ImageLib.zip
// - http://homepage.hispeed.ch/rscheidegger/dri_experimental/s3tc_index.html
// - http://www.sjbrown.co.uk/?article=dxt

using namespace nv;
using namespace std;

#if defined(__SSE2__) && 0

// @@ TODO

typedef __m128i VectorColor;

inline static __m128i loadColor(Color32 c)
{
  return ...;
}

inline static __m128i absoluteDifference(__m128i a, __m128i b)
{
  return ...;
}
  
inline uint colorDistance(__m128i a, __m128i b)
{
  return 0;
}

#elif defined(__MMX__) && 0

typedef __m64 VectorColor;

inline static __m64 loadColor(Color32 c)
{
  return _mm_unpacklo_pi8(_mm_cvtsi32_si64(c), _mm_setzero_si64());
}

inline static __m64 absoluteDifference(__m64 a, __m64 b)
{
  // = |a-b| or |b-a|
  return _mm_or_si64(_mm_subs_pu16(a, b), _mm_subs_pu16(b, a));
}
  
inline uint colorDistance(__m64 a, __m64 b)
{
  union {
    __m64 v;
    uint16 part[4];
  } s;
      
  s.v = absoluteDifference(a, b);
    
  // @@ This is very slow!
  return s.part[0] + s.part[1] + s.part[2] + s.part[3];
}

#define vectorEnd _mm_empty

#elif defined(__VEC__)

typedef vector signed int VectorColor;

inline static vector signed int loadColor(Color32 c)
{
  return (vector signed int) (c.r, c.g, c.b, c.a);
}

// Get the absolute distance between the given colors.
inline static uint colorDistance(vector signed int c0, vector signed int c1)
{
  int result;
  vector signed int v = vec_sums(vec_abs(vec_sub(c0, c1)), (vector signed int)0);
  vec_ste(vec_splat(v, 3), 0, &result);
  return result;
}

inline void vectorEnd()
{
}

#else

typedef Color32 VectorColor;

inline static Color32 loadColor(Color32 c)
{
  return c;
}

inline static uint sqr(uint s)
{
  return s*s;
}

// Get the absolute distance between the given colors.
inline static uint colorDistance(Color32 c0, Color32 c1)
{
  return sqr(c0.r - c1.r) + sqr(c0.g - c1.g) + sqr(c0.b - c1.b);
  //return abs(c0.r - c1.r) + abs(c0.g - c1.g) + abs(c0.b - c1.b);
}

inline void vectorEnd()
{
}

#endif  
  
inline static uint computeIndices(const ColorBlock & rgba, const Color32 palette[4])
{
  const VectorColor vcolor0 = loadColor(palette[0]);
  const VectorColor vcolor1 = loadColor(palette[1]);
  const VectorColor vcolor2 = loadColor(palette[2]);
  const VectorColor vcolor3 = loadColor(palette[3]);
  
  uint indices = 0;
  for(int i = 0; i < 16; i++) {
    const VectorColor vcolor = loadColor(rgba.color(i));
    
    uint d0 = colorDistance(vcolor0, vcolor);
    uint d1 = colorDistance(vcolor1, vcolor);
    uint d2 = colorDistance(vcolor2, vcolor);
    uint d3 = colorDistance(vcolor3, vcolor);
    
    /*if (d0 < d1 && d0 < d2 && d0 < d3) {
      indices |= 0 << (2 * i);
    }
    else if (d1 < d2 && d1 < d3) {
      indices |= 1 << (2 * i);
    }
    else if (d2 < d3) {
      indices |= 2 << (2 * i);
    }
    else {
      indices |= 3 << (2 * i);
    }*/
    
    /*
    uint b0 = d0 > d2;
    uint b1 = d1 > d3;
    uint b2 = d0 > d3;
    uint b3 = d1 > d2;
    uint b4 = d0 > d1;
    uint b5 = d2 > d3;
    
    uint x0 = b1 & b2;
    uint x1 = b0 & b3;
    uint x2 = b2 & b5;
    uint x3 = !b3 & b4;
    
    indices |= ((x3 | x2) | ((x1 | x0) << 1)) << (2 * i);
    */

    uint b0 = d0 > d3;
    uint b1 = d1 > d2;
    uint b2 = d0 > d2;
    uint b3 = d1 > d3;
    uint b4 = d2 > d3;
    
    uint x0 = b1 & b2;
    uint x1 = b0 & b3;
    uint x2 = b0 & b4;
    
    indices |= (x2 | ((x0 | x1) << 1)) << (2 * i);
  }

  vectorEnd();
  return indices;
}

// Encode DXT3 alpha block.
void nv::compressBlock(const ColorBlock & rgba, AlphaBlockDXT3 * block)
{
  block->alpha0 = rgba.color(0).a >> 4;
  block->alpha1 = rgba.color(1).a >> 4;
  block->alpha2 = rgba.color(2).a >> 4;
  block->alpha3 = rgba.color(3).a >> 4;
  block->alpha4 = rgba.color(4).a >> 4;
  block->alpha5 = rgba.color(5).a >> 4;
  block->alpha6 = rgba.color(6).a >> 4;
  block->alpha7 = rgba.color(7).a >> 4;
  block->alpha8 = rgba.color(8).a >> 4;
  block->alpha9 = rgba.color(9).a >> 4;
  block->alphaA = rgba.color(10).a >> 4;
  block->alphaB = rgba.color(11).a >> 4;
  block->alphaC = rgba.color(12).a >> 4;
  block->alphaD = rgba.color(13).a >> 4;
  block->alphaE = rgba.color(14).a >> 4;
  block->alphaF = rgba.color(15).a >> 4;
}



static uint computeAlphaIndices(const ColorBlock & rgba, AlphaBlockDXT5 * block)
{
  uint8 alphas[8];
  block->evaluatePalette(alphas);

  uint totalError = 0;

  for (uint i = 0; i < 16; i++)
  {
    uint8 alpha = rgba.color(i).a;

    uint besterror = 256*256;
    uint best;
    for(uint p = 0; p < 8; p++)
    {
      int d = alphas[p] - alpha;
      uint error = d * d;

      if (error < besterror)
      {
        besterror = error;
        best = p;
      }
    }

    totalError += besterror;
    block->setIndex(i, best);
  }

  return totalError;
}

static uint computeAlphaError(const ColorBlock & rgba, const AlphaBlockDXT5 * block)
{
  uint8 alphas[8];
  block->evaluatePalette(alphas);

  uint totalError = 0;

  for (uint i = 0; i < 16; i++)
  {
    uint8 alpha = rgba.color(i).a;

    uint besterror = 256*256;
    uint best;
    for(uint p = 0; p < 8; p++)
    {
      int d = alphas[p] - alpha;
      uint error = d * d;

      if (error < besterror)
      {
        besterror = error;
        best = p;
      }
    }

    totalError += besterror;
  }

  return totalError;
}

static void optimizeAlpha8(const ColorBlock & rgba, AlphaBlockDXT5 * block)
{
  float alpha2_sum = 0;
  float beta2_sum = 0;
  float alphabeta_sum = 0;
  float alphax_sum = 0;
  float betax_sum = 0;

  for (int i = 0; i < 16; i++)
  {
    uint idx = block->index(i);
    float alpha;
    if (idx < 2) alpha = 1.0f - idx;
    else alpha = (8.0f - idx) / 7.0f;
    
    float beta = 1 - alpha;

        alpha2_sum += alpha * alpha;
        beta2_sum += beta * beta;
        alphabeta_sum += alpha * beta;
        alphax_sum += alpha * rgba.color(i).a;
        betax_sum += beta * rgba.color(i).a;
  }

    const float factor = 1.0f / (alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum);

    float a = (alphax_sum * beta2_sum - betax_sum * alphabeta_sum) * factor;
  float b = (betax_sum * alpha2_sum - alphax_sum * alphabeta_sum) * factor;

  uint alpha0 = uint(min(max(a, 0.0f), 255.0f));
  uint alpha1 = uint(min(max(b, 0.0f), 255.0f));

  if (alpha0 < alpha1)
  {
    swap(alpha0, alpha1);

    // Flip indices:
    for (int i = 0; i < 16; i++)
    {
      uint idx = block->index(i);
      if (idx < 2) block->setIndex(i, 1 - idx);
      else block->setIndex(i, 9 - idx);
    }
  }
  else if (alpha0 == alpha1)
  {
    for (int i = 0; i < 16; i++)
    {
      block->setIndex(i, 0);
    }
  }

  block->alpha0 = alpha0;
  block->alpha1 = alpha1;
}


static void optimizeAlpha6(const ColorBlock & rgba, AlphaBlockDXT5 * block)
{
  float alpha2_sum = 0;
  float beta2_sum = 0;
  float alphabeta_sum = 0;
  float alphax_sum = 0;
  float betax_sum = 0;

  for (int i = 0; i < 16; i++)
  {
    uint8 x = rgba.color(i).a;
    if (x == 0 || x == 255) continue;

    uint bits = block->index(i);
    if (bits == 6 || bits == 7) continue;

    float alpha;
    if (bits == 0) alpha = 1.0f;
    else if (bits == 1) alpha = 0.0f;
    else alpha = (6.0f - block->index(i)) / 5.0f;
    
    float beta = 1 - alpha;

        alpha2_sum += alpha * alpha;
        beta2_sum += beta * beta;
        alphabeta_sum += alpha * beta;
        alphax_sum += alpha * x;
        betax_sum += beta * x;
  }

    const float factor = 1.0f / (alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum);

    float a = (alphax_sum * beta2_sum - betax_sum * alphabeta_sum) * factor;
  float b = (betax_sum * alpha2_sum - alphax_sum * alphabeta_sum) * factor;

  uint alpha0 = uint(min(max(a, 0.0f), 255.0f));
  uint alpha1 = uint(min(max(b, 0.0f), 255.0f));

  if (alpha0 > alpha1)
  {
    swap(alpha0, alpha1);
  }

  block->alpha0 = alpha0;
  block->alpha1 = alpha1;
}



static bool sameIndices(const AlphaBlockDXT5 & block0, const AlphaBlockDXT5 & block1)
{
  const uint64 mask = ~uint64(0xFFFF);
  return (block0.u | mask) == (block1.u | mask);
}


uint nv::compressBlock_Iterative(const ColorBlock & rgba, AlphaBlockDXT5 * resultblock)
{
  uint8 alpha0 = 0;
  uint8 alpha1 = 255;
  
  // Get min/max alpha.
  for (uint i = 0; i < 16; i++)
  {
    uint8 alpha = rgba.color(i).a;
    alpha0 = max(alpha0, alpha);
    alpha1 = min(alpha1, alpha);
  }
  
  AlphaBlockDXT5 block;
  block.alpha0 = alpha0 - (alpha0 - alpha1) / 34;
  block.alpha1 = alpha1 + (alpha0 - alpha1) / 34;
  uint besterror = computeAlphaIndices(rgba, &block);
  
  AlphaBlockDXT5 bestblock = block;
  
  while(true)
  {
    optimizeAlpha8(rgba, &block);
    uint error = computeAlphaIndices(rgba, &block);
    
    if (error >= besterror)
    {
      // No improvement, stop.
      break;
    }
    if (sameIndices(block, bestblock))
    {
      bestblock = block;
      break;
    }
    
    besterror = error;
    bestblock = block;
  };
  
  // Copy best block to result;
  *resultblock = bestblock;

  return besterror;
}
