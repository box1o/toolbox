# cc::math usage examples

## Includes

```cpp
#include <cc/math/math.hpp>
```

## Basic types and aliases

```cpp
using namespace cc;

vec2f a2{1.0f, 2.0f};
vec3f a3{1.0f, 2.0f, 3.0f};
vec4f a4{1.0f, 2.0f, 3.0f, 4.0f};

vec2i i2{1, 2};
vec3d d3{1.0, 2.0, 3.0};

mat2f m2 = mat2f::identity();
mat3f m3 = mat3f::identity();
mat4f m4 = mat4f::identity();

quatf q = quatf::identity();
```

## Constructing vectors

```cpp
vec2f v2a;                 // {0, 0}
vec2f v2b{1.0f, 2.0f};
vec2f v2c{5.0f};           // all components 5

vec3f v3a;                 // {0, 0, 0}
vec3f v3b{1.0f, 2.0f, 3.0f};
vec3f v3c{2.0f};           // all components 2
vec3f v3d{vec2f{1.0f, 2.0f}, 3.0f};

vec4f v4a;                 // {0, 0, 0, 0}
vec4f v4b{1.0f, 2.0f, 3.0f, 4.0f};
vec4f v4c{1.0f};           // all components 1
vec4f v4d{vec2f{1.0f, 2.0f}, 3.0f, 4.0f};
vec4f v4e{vec3f{1.0f, 2.0f, 3.0f}, 4.0f};
```

## Accessing components

```cpp
float x = v3b.x;
float y = v3b[1];
float* p = v3b.data();

vec2f xy = v3b.xy();
vec2f yz = v3b.yz();
vec3f xyz = v4b.xyz();
```

## Vector arithmetic

```cpp
vec3f v = {1.0f, 2.0f, 3.0f};
vec3f u = {4.0f, 5.0f, 6.0f};

vec3f add  = v + u;
vec3f sub  = v - u;
vec3f mulC = v * 2.0f;
vec3f mulS = 2.0f * v;
vec3f mulV = v * u;        // component-wise
vec3f divC = v / 2.0f;
vec3f divV = v / u;        // component-wise

v += u;
v -= u;
v *= 2.0f;
v /= 2.0f;

vec3f neg = -v;
```

## Comparisons

```cpp
bool eq_vec  = (v == u);
bool neq_vec = (v != u);   // component-wise, with approx for floats
```

## Vector math

```cpp
float d    = v.dot(u);
vec3f cr   = v.cross(u);   // 3D cross
float len2 = v.length_squared();
float len  = v.length();
vec3f vn   = v.normalized();
v.normalize();             // in-place

float s2d  = v2b.cross(vec2f{3.0f, 4.0f});   // scalar 2D cross
```

## Scalar utilities

```cpp
using cc::radians;
using cc::degrees;
using cc::lerp;
using cc::sign;
using cc::abs;
using cc::min;
using cc::max;
using cc::clamp;
using cc::sqrt;
using cc::approx_equal;

float rad = radians(90.0f);
float deg = degrees(rad);

float l   = lerp(0.0f, 10.0f, 0.5f);        // 5.0
int   sg  = sign(-3);                       // -1
float av  = abs(-3.5f);                     // 3.5
float mn  = min(1.0f, 2.0f);
float mx  = max(1.0f, 2.0f);
float cl  = clamp(5.0f, 0.0f, 1.0f);        // 1.0
float sq  = sqrt(9.0f);                     // 3.0
bool ae   = approx_equal(1.0f, 1.0f + 1e-7f);
```

## Math function aliases

```cpp
using cc::sin;
using cc::cos;
using cc::tan;
using cc::asin;
using cc::acos;
using cc::atan;
using cc::atan2;
using cc::exp;
using cc::log;
using cc::log10;
using cc::pow;
using cc::ceil;
using cc::floor;
using cc::trunc;
using cc::round;

float s = sin(rad);
float c = cos(rad);
```

## Constructing matrices

```cpp
mat3f I3 = mat3f::identity();
mat4f I4 = mat4f::identity();

mat3f M3_fill{2.0f};       // all components = 2
mat4f M4_fill{1.0f};

mat3f M3_row = mat3f(
cc::layout::rowm,
1, 2, 3,
4, 5, 6,
7, 8, 9);

mat3f M3_col = mat3f(
cc::layout::colm,
1, 4, 7,
2, 5, 8,
3, 6, 9);

mat4f M4_row = mat4f(
cc::layout::rowm,
1,  2,  3,  4,
5,  6,  7,  8,
9, 10, 11, 12,
13, 14, 15, 16);
```

## Accessing matrices

```cpp
float m00 = M3_row(0, 0);   // row 0, col 0
float m12 = M3_row(1, 2);   // row 1, col 2

auto col0 = M3_row[0];      // first column
float* md = M4_row.data();  // contiguous column-major data
```

## Matrix arithmetic

```cpp
mat3f A = mat3f::identity();
mat3f B = M3_row;

mat3f C  = A + B;
mat3f D  = A - B;
mat3f Es = B * 2.0f;
mat3f Fs = 2.0f * B;
mat3f Gs = B / 2.0f;

A += B;
A -= B;
A *= 2.0f;
A /= 2.0f;

mat3f H = B * C;             // matrix-matrix multiply (3x3)
```

## Transpose, determinant, inverse

```cpp
mat3f Mt  = B.transpose();
float d3  = B.det();
mat3f Bi  = B.inverse();     // if nearly singular, returns identity

mat4f Mt4 = M4_row.transpose();
float d4  = M4_row.det();
mat4f Mi4 = M4_row.inverse(); // if nearly singular, returns identity

// Generic NxN
float dN = cc::det(M3_row);
auto  iN = cc::inverse(M4_row);
```

## Matrix-vector multiplication

```cpp
vec3f v3{1.0f, 2.0f, 3.0f};
vec4f v4{1.0f, 2.0f, 3.0f, 1.0f};

vec3f r3 = M3_row * v3;      // (3x3) * (3)
vec4f r4 = M4_row * v4;      // (4x4) * (4)

vec3f r3_row = v3 * M3_row;  // row-vector interpretation
vec4f r4_row = v4 * M4_row;

// Non-square
mat<3, 4, float> M34{cc::layout::rowm,
1, 2, 3, 4,
5, 6, 7, 8,
9,10,11,12};

vec4f v4b{1, 2, 3, 4};
vec3f p3 = M34 * v4b;        // (3x4) * (4) = (3)

// vec * mat also supported:
vec3f p3_row = v4b * M34;    // (4) * (3x4) = (3)
```

## Transforms (OpenGL-style, RH)

```cpp
vec3f t{1.0f, 2.0f, 3.0f};
mat4f Tm = cc::translate(t);

vec3f s3{2.0f, 2.0f, 2.0f};
mat4f Sm = cc::scale(s3);
mat4f Su = cc::scale(2.0f);  // uniform

float ang = radians(45.0f);
mat4f Rx = cc::rotate_x(ang);
mat4f Ry = cc::rotate_y(ang);
mat4f Rz = cc::rotate_z(ang);

vec3f axis{0.0f, 1.0f, 0.0f};
mat4f Raxis = cc::rotate(ang, axis);

vec3f euler{radians(30.0f), radians(45.0f), radians(60.0f)};
mat4f Reuler = cc::rotate(euler);

vec3f eye{0.0f, 0.0f, 5.0f};
vec3f center{0.0f, 0.0f, 0.0f};
vec3f up{0.0f, 1.0f, 0.0f};
mat4f View = cc::look_at(eye, center, up);

float fov    = radians(60.0f);
float aspect = 16.0f / 9.0f;
float zn     = 0.1f;
float zf     = 100.0f;
mat4f Proj   = cc::perspective(fov, aspect, zn, zf);

float left   = -1.0f;
float right  =  1.0f;
float bottom = -1.0f;
float top    =  1.0f;
mat4f Ortho  = cc::ortho(left, right, bottom, top, zn, zf);

vec4f pos   = {0.0f, 0.0f, 0.0f, 1.0f};
vec4f world = Tm * pos;
vec4f clip  = Proj * View * world;
```

## Quaternions

```cpp
quatf q_id = quatf::identity();

quatf q1{0.0f, 0.0f, 0.0f, 1.0f};
quatf q2{0.1f, 0.2f, 0.3f, 0.9f};

quatf q_axis = quatf::from_axis_angle(axis, ang);

quatf qsum   = q1 + q2;
quatf qdiff  = q1 - q2;
quatf qscaled= q1 * 2.0f;
quatf qprod  = q1 * q2;

float qlen   = q1.length();
float qlen2  = q1.length_squared();

quatf qnorm  = q1.normalized();
q1.normalize();

quatf qconj  = q1.conjugate();
quatf qinv   = q1.inverse();

bool qeq     = (q1 == q2);
bool qneq    = (q1 != q2);

vec3f vrot   = q1.rotate(vec3f{1.0f, 0.0f, 0.0f});

mat3f Rq3    = q1.to_mat3();
mat4f Rq4    = q1.to_mat4();
```

## Example pipeline

```cpp
vec3f position{0.0f, 0.0f, 0.0f};
vec3f scale_v{1.0f, 2.0f, 1.0f};
vec3f rot_axis{0.0f, 1.0f, 0.0f};
float rot_angle = radians(45.0f);

mat4f M = cc::translate(position)
* cc::rotate(rot_angle, rot_axis)
* cc::scale(scale_v);

vec4f local_pos{1.0f, 0.0f, 0.0f, 1.0f};
vec4f world_pos = M * local_pos;
```
