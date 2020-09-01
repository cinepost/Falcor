#ifndef RiTypesHelper_h
#define RiTypesHelper_h

/* $Revision: #17 $
# ------------------------------------------------------------------------------
#
# Copyright (c) 2012-2013 Pixar Animation Studios. All rights reserved.
#
# The information in this file (the "Software") is provided for the
# exclusive use of the software licensees of Pixar.  Licensees have
# the right to incorporate the Software into other products for use
# by other authorized software licensees of Pixar, without fee.
# Except as expressly permitted herein, the Software may not be
# disclosed to third parties, copied or duplicated in any form, in
# whole or in part, without the prior written permission of
# Pixar Animation Studios.
#
# The copyright notices in the Software and this entire statement,
# including the above license grant, this restriction and the
# following disclaimer, must be included in all copies of the
# Software, in whole or in part, and all permitted derivative works of
# the Software, unless such copies or derivative works are solely
# in the form of machine-executable object code generated by a
# source language processor.
#
# PIXAR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
# ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL PIXAR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
# SOFTWARE.
#
# Pixar
# 1200 Park Ave
# Emeryville CA 94608
#
# ------------------------------------------------------------------------------
*/

// This file is included via ri.h when using a C++ compiler.
// It represents a collection of classes that can be aliased
// atop the traditional "C" plain-ol-data representations for
// more programming expressivity without loss of performance or 
// compactness.
//
// Here's an example programming idiom (for simple scalar case):
//
//  RtPoint op = {1,2,3};
//  RtPoint3 &np = reinterpret_cast<RtPoint3&>(op);
//
// Now the standard collection of operator-overriding tricks eagerly await
// your beck and call.

#include <cassert>
#include <cstdio>
#include <vector>
#include <iostream>
#include <cmath> 
#include <float.h>
#include <algorithm> // std::max

class RtColorRGB;

// -------------------------------------------------------------------------
// RtFloat3 implements RtPoint3, RtVector3, RtNormal3.
//  it may be convenient/expressive to preserve the geometric
//  distinctions between these types in your code and so
//  we provide typedefs below.  This approach represents
//  a compromise between geometric type-safety and programmer 
//  convenience/efficiency.
class RtFloat3 
{
  public:
    // allow direct member access
    RtFloat x, y, z; 
    
    inline RtFloat3() {}
    inline RtFloat3(RtFloat xx, RtFloat yy, RtFloat zz)
    {
        assert(sizeof(*this) == sizeof(RtFloat[3])); 
        // ensure compat with legacy c-layout
        x = xx; y = yy; z = zz;
    }
    explicit inline RtFloat3(RtFloat v)
    {
	x = y = z = v;
    }
    // construct from a float array
    // nb: RtFloat3(0) is ambiguous (float and NULL)
    explicit inline RtFloat3(const RtFloat *d)
    {
        x = d[0];
        y = d[1];
        z = d[2];
    }
    explicit inline RtFloat3(RtColorRGB const &rgb);

    // offer access as array (original RtPoint was RtFloat[3])
    inline RtFloat& operator[] (int i) 
    {
        assert(i >= 0 && i<3);
        return (&x)[i];
    }
    inline const RtFloat& operator[] (int i) const
    {
        assert(i >= 0 && i<3);
        return (&x)[i];
    }

    // keep if we want to be able to compare point/vector/normal for equality
    // otherwise move to derived
    inline int operator==(const RtFloat3 &rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    inline int operator!=(const RtFloat3 &rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }

    // type cast  Too dangerous as the compiler does them automatically
    // and we can't make them explicit (not supported before C++ 11)
    // use &x[0]
    //inline operator const RtFloat *() const { return (RtFloat *) &x; }
    //inline operator RtFloat *() { return (RtFloat *) &x; }
 
    // serialize
    friend std::ostream& operator<<(std::ostream& o, const RtFloat3& v)
    {
        o << v.x << " " << v.y << " " << v.z;
        return o;
    }


    // addition
    inline RtFloat3 operator+(const RtFloat3 &rhs) const
    {
        return RtFloat3(x + rhs.x,
                        y + rhs.y,
                        z + rhs.z);
    }
    inline RtFloat3 &operator+=(const RtFloat3 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }


    // multiplication
    inline RtFloat3 operator*(const RtFloat3 &rhs) const
    {
        return RtFloat3(x * rhs.x,
                        y * rhs.y,
                        z * rhs.z);
    }
    inline RtFloat3 &operator*=(const RtFloat3 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }


    // subtraction
    inline RtFloat3 operator-(const RtFloat3 &rhs) const
    {
        return RtFloat3(x - rhs.x,
                        y - rhs.y,
                        z - rhs.z);
    }
    inline RtFloat3 &operator-=(const RtFloat3 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }


    // division
    inline RtFloat3 operator/(const RtFloat3 &rhs) const
    {
        return RtFloat3(x / rhs.x,
                        y / rhs.y,
                        z / rhs.z);
    }
    inline RtFloat3 &operator/=(const RtFloat3 &rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }


    // unary minus
    inline RtFloat3 operator-() const { return RtFloat3(-x, -y, -z); }

    // scalar addition
    inline RtFloat3 operator+(RtFloat rhs) const
    {
        return RtFloat3(x + rhs, y + rhs, z + rhs);
    }
    inline friend  RtFloat3 operator+(RtFloat lhs, const RtFloat3 &rhs) {
        return rhs + lhs;
    }
    inline RtFloat3 &operator+=(RtFloat rhs)
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    // scalar subtraction
    inline RtFloat3 operator-(RtFloat rhs) const
    {
        return RtFloat3(x - rhs, y - rhs, z - rhs);
    }
    inline friend  RtFloat3 operator-(RtFloat lhs, const RtFloat3 &rhs) {
        return rhs - lhs;
    }
    inline RtFloat3 &operator-=(RtFloat rhs)
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    // scalar multiplication
    inline RtFloat3 operator*(RtFloat rhs) const
    {
        return RtFloat3(x * rhs, y * rhs, z * rhs);
    }
    inline friend  RtFloat3 operator*(RtFloat lhs, const RtFloat3 &rhs) {
        return rhs * lhs;
    }
    inline RtFloat3 &operator*=(RtFloat rhs)
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    // scalar division
    inline RtFloat3 operator/(RtFloat rhs) const
    {
        RtFloat inv = 1.0f / rhs;
        return operator*(inv);
    }

    inline RtFloat3 & operator/=(RtFloat rhs)
    {
        RtFloat inv = 1.0f / rhs;
        return operator*=(inv);
    }

    // Dot product
    inline RtFloat Dot(const RtFloat3 &v2) const
    {
        return x * v2.x + y * v2.y + z * v2.z;
    }

    inline friend RtFloat Dot(const RtFloat3 &v1, const RtFloat3 &v2)
    {
        return v1.Dot(v2);
    }

    // Absolute value of dot product
    inline RtFloat AbsDot(const RtFloat3 &v2) const
    {
        float result =  x * v2.x + y * v2.y + z * v2.z;
        if(result < 0.f) 
            result = -result;
        return result;
    }

    inline friend RtFloat AbsDot(const RtFloat3 &v1, const RtFloat3 &v2)
    {
        return v1.AbsDot(v2);
    }

    // Length
    inline RtFloat LengthSq() const
    { 
        return (x*x + y*y + z*z);
    }

    inline RtFloat Length() const
    { 
        return std::sqrt(LengthSq());
    }

    inline bool IsUnitLength(float eps=.005f) const 
    {
        RtFloat len = Length();
        if(len > 1.f+eps || len < 1.f-eps) return false;
        else return true;
    }

    // normalize vector in place, return length. 
    inline RtFloat Normalize()
    {
        RtFloat len = Dot(*this);
        if (len > FLT_MIN)
        {
            len = std::sqrt(len);
            *this /= len;
        }
        else 
            x = y = z = 0.f;
        return len;
    }
    inline RtFloat NormalizeAndNegate()
    {
        RtFloat len = Dot(*this);
        assert(len > 0.f);

        if (len > FLT_MIN) 
        {
            len = std::sqrt(len);
            *this /= len;
            x = -x;
            y = -y;
            z = -z;
        } 
        else 
        {
            x = 0.f;
            y = 0.f;
            z = 0.f;
        }
        return len;
    }
    
    inline friend RtFloat3 NormalizeCopy(const RtFloat3 &v) 
    {
        RtFloat3 copy = v;
        copy.Normalize();
        return copy;
    }

    inline friend void Normalize(RtFloat3 &v) 
    {
        v.Normalize();
    }

    // cross product
    inline RtFloat3 Cross(const RtFloat3 &v2) const
    {
        return RtFloat3(y * v2.z - z * v2.y,
                        z * v2.x - x * v2.z,
                        x * v2.y - y * v2.x);
    }
    inline friend RtFloat3 Cross(const RtFloat3 &v1, const RtFloat3 &v2) 
    {
        return v1.Cross(v2);
    }

    // create orthonormal basis from a single vector.  The vector has to be
    // non-zero.
    inline void
    CreateOrthonormalBasis(RtFloat3 &v1, RtFloat3 &v2) const
    {
        RtFloat3 copy = *this;
        copy.Normalize();

        // Pick v1
        if (fabsf(copy.x) > 1e-3f || fabsf(copy.y) > 1e-3f)
            v1 = RtFloat3(copy.y, -copy.x, 0.0); // v1 = copy x Z
        else
            v1 = RtFloat3(0.0, copy.z, -copy.y); // v1 = copy x X

        v1.Normalize();

        // Compute v2
        v2 = copy.Cross(v1);   // v2 = copy x v1
    }

};

typedef RtFloat3 RtPoint3;
typedef RtFloat3 RtVector3;
typedef RtFloat3 RtNormal3;


// -------------------------------------------------------------------------
// RtFloat2 is the base class for RtPoint2, RtVector2, RtNormal2.
//  All geometrically insensitive operations are implemented here and
//  used by subclasses.  We use subclassing to preserve and convey 
//  geometric-type-sensitive behavior.
class RtFloat2 
{
  public:
    // allow direct member access
    RtFloat x, y; 
    
    inline RtFloat2() {}
    inline RtFloat2(RtFloat xx, RtFloat yy)
    {
        assert(sizeof(*this) == sizeof(RtFloat[2])); 
        // ensure compat with legacy c-layout
        x = xx; y = yy;
    }
    explicit inline RtFloat2(RtFloat v)
    {
	x = y = v;
    }
    // construct from a float array
    // nb: RtFloat2(0) is ambiguous (float and NULL)
    explicit inline RtFloat2(const RtFloat *d)
    {
        x = d[0];
        y = d[1];
    }

    // offer access as array (original RtPoint was RtFloat[3])
    inline RtFloat& operator[] (int i) 
    {
        assert(i >= 0 && i<2);
        return (&x)[i];
    }
    inline const RtFloat& operator[] (int i) const
    {
        assert(i >= 0 && i<2);
        return (&x)[i];
    }

    // keep if we want to be able to compare point/vector/normal for equality
    // otherwise move to derived
    inline int operator==(const RtFloat2 &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }
    inline int operator!=(const RtFloat2 &rhs) const
    {
        return x != rhs.x || y != rhs.y;
    }

    // type cast  Too dangerous as the compiler does them automatically
    // and we can't make them explicit (not supported before C++ 11)
    // use &x[0]
    //inline operator const RtFloat *() const { return (RtFloat *) &x; }
    //inline operator RtFloat *() { return (RtFloat *) &x; }
 
    // serialize
    friend std::ostream& operator<<(std::ostream& o, const RtFloat2& v)
    {
        o << v.x << " " << v.y ;
        return o;
    }

    // addition
    inline RtFloat2 operator+(const RtFloat2 &rhs) const
    {
        return RtFloat2(x + rhs.x, y + rhs.y);
    }
    inline RtFloat2 &operator+=(const RtFloat2 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    // subtraction
    inline RtFloat2 operator-(const RtFloat2 &rhs) const
    {
        return RtFloat2(x - rhs.x, y - rhs.y);
    }
    inline RtFloat2 &operator-=(const RtFloat2 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    // unary minus
    inline RtFloat2 operator-() const { return RtFloat2(-x, -y); }

    // scalar multiplication
    inline RtFloat2 operator*(RtFloat rhs) const
    {
        return RtFloat2(x * rhs, y * rhs);
    }
    inline friend  RtFloat2 operator*(RtFloat lhs, const RtFloat2 &rhs) {
        return rhs * lhs;
    }
    inline RtFloat2 &operator*=(RtFloat rhs)
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    // scalar division
    inline RtFloat2 operator/(RtFloat rhs) const
    {
        RtFloat inv = 1.0f / rhs;
        return operator*(inv);
    }
    inline RtFloat2 & operator/=(RtFloat rhs)
    {
        RtFloat inv = 1.0f / rhs;
        return operator*=(inv);
    }

    // length
    inline RtFloat LengthSq() const
    { 
        return (x*x + y*y);
    }
    inline RtFloat Length() const
    { 
        return std::sqrt(LengthSq());
    }

    // normalize vector in place, return length.  
    inline RtFloat Normalize()
    {
        RtFloat len = LengthSq();
        if (len > FLT_MIN) 
        {
             len = std::sqrt(len);
             *this /= len; 
        } 
        else 
            x = y = 0.f;
        return len;
    }

    inline friend RtFloat2 NormalizeCopy(const RtFloat2 &v)
    {
        RtFloat2 copy = v;
        copy.Normalize();
        return copy;
    }
    inline friend void Normalize(RtFloat2 &v)
    {
        v.Normalize();
    }

    // Dot product
    inline RtFloat Dot(const RtFloat2 &v2) const
    {
        return x * v2.x + y * v2.y;
    }
    inline friend RtFloat Dot(const RtFloat2 &v1, const RtFloat2 &v2)
    {
        return v1.Dot(v2);
    }
};

typedef RtFloat2 RtPoint2;
typedef RtFloat2 RtVector2;

/* RtColorRGB is a Float3 with special type to trigger alternate
 * transform semantics (color transformation)
 */
class RtColorRGB 
{
  public:
    RtFloat r,g,b;
    
    RtColorRGB() {}
    RtColorRGB(RtFloat rr, RtFloat gg, RtFloat bb ) : r(rr), g(gg), b(bb) { 
        assert(sizeof(*this) == sizeof(RtFloat[3]));
    } 
    explicit inline RtColorRGB(RtFloat v)
    {
	r = g = b = v;
    }
    // construct from a float array
    // nb: v = RtColorRGB(0) is ambiguous (float and NULL)
    explicit inline RtColorRGB(const RtFloat *d)
    {
        r = d[0];
        g = d[1];
        b = d[2];
    }

    explicit inline RtColorRGB(RtFloat3 const &rhs, bool asNormalVector = false)
    {
        if(asNormalVector)
        {
            r = rhs.x*.5f + .5f;
            g = rhs.y*.5f + .5f;
            b = rhs.z*.5f + .5f;
        }
        else
        {
            r = rhs.x;
            g = rhs.y;
            b = rhs.z;
        }
    }

    inline void Zero()
    {
        r = g = b = 0.f;
    } 

    inline void One() 
    {
        r = g = b = 1.f;
    }

    // IsBlack returns true if all components are small or negative
    inline bool IsBlack(float epsilon=1e-6f) const
    {
        return (r <= epsilon && g <= epsilon && b <= epsilon);
    }

    // IsZero returns true if all components are near zero.
    inline bool IsZero(RtFloat epsilon=1e-6f) const
    {
        return (fabsf(r)<=epsilon && fabsf(g)<=epsilon && fabsf(b)<=epsilon);
    }

    inline bool IsMonochrome(RtFloat epsilon=1e-6f) const
    {
        return ( fabsf(r-g)<epsilon && fabsf(g-b)<epsilon );
    }

    // offer access as array (original RtPoint was RtFloat[3])
    inline RtFloat& operator[] (int i) 
    {
        assert(i >= 0 && i<3);
        return (&r)[i];
    }

    inline const RtFloat& operator[] (int i) const
    {
        assert(i >= 0 && i<3);
        return (&r)[i];
    }

    // keep if we want to be able to compare point/vector/normal for equality
    // otherwise move to derived
    inline int operator==(const RtColorRGB &rhs) const
    {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }

    inline int operator!=(const RtColorRGB &rhs) const
    {
        return r != rhs.r || g != rhs.g || b != rhs.b;
    }

    // type cast too dangerous as the compiler does them automatically
    // and we can't make them explicit (not supported before C++ 11)
    // use &r[0]
    //inline operator const RtFloat *() const { return (RtFloat *) &r; }
    //inline operator RtFloat *() { return (RtFloat *) &r; }
 
    // serialize
    friend std::ostream& operator<<(std::ostream& o, const RtColorRGB& c)
    {
        o << c.r << " " << c.g << " " << c.b;
        return o;
    }

    // addition
    inline RtColorRGB operator+(const RtColorRGB &rhs) const
    {
        return RtColorRGB(r + rhs.r,
                          g + rhs.g,
                          b + rhs.b);
    }

    inline RtColorRGB &operator+=(const RtColorRGB &rhs)
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        return *this;
    }

    // subtraction
    inline RtColorRGB operator-(const RtColorRGB &rhs) const
    {
        return RtColorRGB(r - rhs.r,
                          g - rhs.g,
                          b - rhs.b);
    }

    inline RtColorRGB &operator-=(const RtColorRGB &rhs)
    {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
        return *this;
    }

    // unary minus
    inline RtColorRGB operator-() const { return RtColorRGB(-r, -g, -b); }

    // scalar multiplication, division
    inline RtColorRGB operator*(RtFloat rhs) const // scale this by scalar
    {
        return RtColorRGB(r * rhs, g * rhs, b * rhs);
    }

    inline friend RtColorRGB operator*(RtFloat lhs, const RtColorRGB &rhs) {
        return rhs * lhs;
    }

    inline RtColorRGB &operator*=(RtFloat rhs)
    {
        r *= rhs; g *= rhs; b *= rhs;
        return *this;
    }

    inline RtColorRGB operator/(RtFloat rhs) const
    {
        RtFloat inv = rhs != 0.f ? 1.0f / rhs : 1e-8f;
        return RtColorRGB(r * inv, g * inv, b * inv);
    }

    inline friend RtColorRGB operator/(RtFloat lhs, const RtColorRGB &rhs) 
    {
        return RtColorRGB(lhs/rhs.r, lhs/rhs.g, lhs/rhs.b);
    }

    inline RtColorRGB &operator/=(RtFloat rhs)
    {
        RtFloat inv = rhs != 0.f ? 1.0f / rhs : 1e-8f;
        r *= inv; g *= inv; b *= inv;
        return *this;
    }

    // scalar addition, subtraction
    inline RtColorRGB operator+(RtFloat rhs) const
    {
        return RtColorRGB(r + rhs, g + rhs, b + rhs);
    }

    inline friend  RtColorRGB operator+(RtFloat lhs, const RtColorRGB &rhs) 
    {
        return rhs + lhs;
    }

    inline RtColorRGB operator-(RtFloat rhs) const
    {
        return RtColorRGB(r - rhs, g - rhs, b - rhs);
    }

    inline friend  RtColorRGB operator-(RtFloat lhs, const RtColorRGB &rhs) 
    {
        return RtColorRGB(lhs - rhs.r, lhs - rhs.g, lhs - rhs.b);
    }

    // component multiplication, division
    inline RtColorRGB operator*(const RtColorRGB &rhs) const 
    {
        return RtColorRGB(r * rhs.r, g * rhs.g, b * rhs.b);
    }

    inline RtColorRGB &operator*=(const RtColorRGB &rhs)
    {
        r *= rhs.r; g *= rhs.g; b *= rhs.b;
        return *this;
    }

    inline RtColorRGB operator/(const RtColorRGB &rhs) const 
    {
        RtFloat rr = r / (rhs.r == 0.0f ? 1e-8f : rhs.r);
        RtFloat gg = g / (rhs.g == 0.0f ? 1e-8f : rhs.g);
        RtFloat bb = b / (rhs.b == 0.0f ? 1e-8f : rhs.b);
        return RtColorRGB(rr, gg, bb);
    }

    inline RtColorRGB &operator/=(const RtColorRGB &rhs)
    {
        r /= rhs.r; g /= rhs.g; b /= rhs.b;
        return *this;
    }

    // misc cross-channel computations
    inline RtFloat Luminance() const
    {
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }

    inline RtFloat ChannelAvg() const
    {
        return (r + g + b) * .33333f;
    }

    inline RtFloat ChannelMin() const
    {
        RtFloat min = r;
        if(g < min) min = g;
        if(b < min) min = b;
        return min;
    }

    inline RtFloat ChannelMax() const
    {
        RtFloat max = r;
        if(g > max) max = g;
        if(b > max) max = b;
        return max;
    }

    void ConvertToRGB(RtFloat *spectrum, int nsamps, RtFloat *cvt)
    {
        RtFloat *p = cvt;
        r = g = b = 0.0f;
        for(int i=0;i<nsamps;i++)
        {
            r += *p++ * *spectrum;
            g += *p++ * *spectrum;
            b += *p++ * *spectrum++;
        }

    }

    RtColorRGB Exp(RtFloat val) const
    {
        RtColorRGB result;
        result.r = std::exp(r*val);
        result.g = std::exp(g*val);
        result.b = std::exp(b*val);
        return result;
    }

    void InvertAlbedo()
    {
        r = 1.0f - r;
        g = 1.0f - g;
        b = 1.0f - b;
    }

    void ClampAlbedo()
    {
        if(r > 1.f) r = 1.f;
        else if(r < 0.f) r = 0.f;
        if(g > 1.f) g = 1.f;
        else if(g < 0.f) g = 0.f;
        if(b > 1.f) b = 1.f;
        else if(b < 0.f) b = 0.f;
    }

};

inline RtFloat3::RtFloat3(RtColorRGB const &rgb)
    : x( rgb.r ), y( rgb.g ), z( rgb.b )
{
}

// RtMatrix4x4: we wrap an RtMatrix in a struct to facilitate use
// in templating scenarios.
struct RtMatrix4x4
{
    RtMatrix m;
};

#endif
