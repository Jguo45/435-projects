// 3D vector class
#ifndef VEC3_HPP
#define VEC3_HPP

#define _USE_MATH_DEFINES
#include <math.h>

#include <istream>

#ifndef INFINITY
// don't complain arithmetic overflow to define infinity, I'm doing it on purpose
#ifdef _WIN32
#pragma warning( disable: 4056 )
#pragma warning( disable: 4756 )
#endif

#define INFINITY float(3.402823466e+38f + 3.402823466e+38f)
#endif

//////////////////////////////////////////////////////////////////////
// 3D vector
class Vec3 {
private: // private data
    float data[3];                          // array data

public: // constructors & destructors
    Vec3() { data[0] = data[1] = data[2] = 0; }
    Vec3(float _x, float _y, float _z) { 
        data[0] = _x; 
        data[1] = _y; 
        data[2] = _z;
    }
    // also can use default copy constructor

public:
    // access as an array vec[i] rather than vec.data[i]
    float operator[](int i) const { return data[i]; }
    float &operator[](int i) { return data[i]; }

    // as color, scaled and clamped to 0-255
    int r() const {
        return data[0]<0 ? 0 : (data[0]>1 ? 255 : int(255*data[0] + .5));
    }
    int g() const {
        return data[1]<0 ? 0 : (data[1]>1 ? 255 : int(255*data[1] + .5));
    }
    int b() const {
        return data[2]<0 ? 0 : (data[2]>1 ? 255 : int(255*data[2] + .5));
    }


};

//////////////////////////////
// component-wise operations: -v, v1+v2, v1-v2, v1*v2, v1/v2
// operations with a scalar: s*v, v*s, v/s
// vector operations: cross(v1,v2), dot(v1,v2), length(v), normalize(v)

// negate v
inline Vec3 operator-(const Vec3 &v) {
    return Vec3(-v[0], -v[1], -v[2]);
}

// vector addition, v1+v2
inline Vec3 operator+(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

// vector subtraction, v1-v2
inline Vec3 operator-(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

// vector component-wise multiplication, v1*v2
inline Vec3 operator*(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
}

// vector component-wise division, v1/v2
inline Vec3 operator/(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]);
}

// scalar multiplication, s*v
inline Vec3 operator*(float s, const Vec3 &v) {
    return Vec3(s*v[0], s*v[1], s*v[2]);
}

// scalar multiplication, v*s
inline Vec3 operator*(const Vec3 &v, float s) {
    return Vec3(s*v[0], s*v[1], s*v[2]);
}

// scalar division, v/s
inline Vec3 operator/(const Vec3 &v, float s) {
    return v*(1/s);
}

// cross product, cross(v1,v2)
inline Vec3 cross(const Vec3 &v1, const Vec3 &v2) {
    return Vec3(v1[1]*v2[2] - v1[2]*v2[1],
                v1[2]*v2[0] - v1[0]*v2[2],
                v1[0]*v2[1] - v1[1]*v2[0]);
}

// vector dot product, dot(v1,v2)
inline float dot(const Vec3 &v1, const Vec3 &v2) {
    return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

// return Euclidean vector length, length(v)
inline float length(const Vec3 &v) {
    return sqrtf(dot(v, v));
}

// return normalized vector, normalize(v)
inline Vec3 normalize(const Vec3 &v) {
    return v / length(v);
}

inline std::istream& operator>>(std::istream &stream, Vec3 &v) {
    std::streampos before = stream.tellg();
    if (!(stream >> v[0] >> v[1] >> v[2])) {
        // reset stream to position before read on failure
        stream.clear();
        stream.seekg(before);
        stream.setstate(std::ios::failbit);
    }
    return stream;
}

inline std::ostream& operator<<(std::ostream& sout, const Vec3 &v) {
    sout << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return sout;
}
#endif
