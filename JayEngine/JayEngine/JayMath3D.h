#ifndef _JAYMATH3D_H_
#define _JAYMATH3D_H_

#include <stdlib.h>
#include <math.h>

/*
	基本的3D数学库,向量计算,矩阵计算
	基于D3D坐标系
*/

//=====================================================================
// 几何计算：顶点、扫描线、边缘、矩形、步长计算
//=====================================================================

typedef unsigned int IUINT32;
#define PI 3.141592653
#define angle_to_radian(X) ((X)/180*PI)
#define radian_to_angle(X) ((X)/PI*180)
#define CMID(x, min, max) (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))
// 计算插值：t 为 [0, 1] 之间的数值
#define interp(a, b, t) ((a) + ((b) - (a)) * (t))


int logbase2ofx(int n);

// D3D的4x4齐次矩阵
class Matrix {
public:
	float m[4][4];
};

class Vector {
public:
	float x, y, z, w;
};
typedef Vector Point;

// | v |
float Vector_length(const Vector *v);

// z = x + y
void Vector_add(Vector *z, const Vector *x, const Vector *y);

// z = x - y
void Vector_sub(Vector *z, const Vector *x, const Vector *y);

// 矢量点乘
float Vector_dotproduct(const Vector *x, const Vector *y);
// 矢量叉乘
void Vector_crossproduct(Vector *z, const Vector *x, const Vector *y);

// 矢量插值，t取值 [0, 1]
void Vector_interp(Vector *z, const Vector *x1, const Vector *x2, float t);
// 矢量归一化
void Vector_normalize(Vector *v);

// cale
void Vector_scale(Vector *v, float k);


// c = a + b
void Matrix_add(Matrix *c, const Matrix *a, const Matrix *b);

// c = a - b
void Matrix_sub(Matrix *c, const Matrix *a, const Matrix *b);
// c = a * b
void Matrix_mul(Matrix *c, const Matrix *a, const Matrix *b);

// c = a * f
void Matrix_scale(Matrix *c, const Matrix *a, float f);
// y = x * m
void Matrix_apply(Vector *y, const Vector *x, const Matrix *m);

// 单位矩阵
void Matrix_set_identity(Matrix *m);

// 零矩阵
void Matrix_set_zero(Matrix *m);

// 
void Matrix_set_axis(Matrix *m, const Vector *xaxis, const Vector *yaxis, const Vector *zaxis, const Point *pos);


//  set_scale 设置缩放系数
void Matrix_set_scale(Matrix *m, float sx, float sy, float sz);

void Matrix_set_rotate(Matrix *m, const Vector *v, float theta);


// clone 
void Matrix_clone(Matrix *dest, const Matrix *src);

//  transpose
void Matrix_transpose(Matrix *m);

// inverse
void Matrix_inverse(Matrix *m);



// 设置模型矩阵
void Matrix_set_rotate_translate_scale(Matrix *m, const Vector *axis, float theta, const Point *pos, const Vector *scale);


// D3DXMatrixPerspectiveFovLH
// https://blog.csdn.net/zzmkljd/article/details/52870807
void Matrix_set_perspective(Matrix *m, float fovy, float aspect, float zn, float zf);

void Matrix_set_ortho(Matrix *m, float l, float r, float b, float t, float zn, float zf);

// 设置摄像机
void Matrix_set_lookat(Matrix *m, const Vector *eye, const Vector *at, const Vector *up);




// 颜色
class Color {
public:
	Color() {}
	Color(float r, float g, float b) :r(r), g(g), b(b), a(0.0f) {}
	Color(float r, float g, float b, float a) :r(r), g(g), b(b), a(a){}

	float r, g, b, a;
};

// 纹理坐标
class Texcoord {
public:
	float u, v;
};

// 顶点属性
class Vertex {
public:
	Point pos;   // 顶点位置
	Texcoord tc; // 纹理坐标
	Color color; // 颜色值
	Vector normal; // 法线向量

	bool operator()(const Vertex& _Right) const {
		return this->pos.x < _Right.pos.x&this->pos.y < _Right.pos.y;
	}
	bool operator()(const Vertex& _Right){
		return this->pos.x < _Right.pos.x&this->pos.y < _Right.pos.y;
	}

	bool operator==(const Vertex& _Right) {
		return abs(this->pos.x - _Right.pos.x)<= 1e-6 &&abs(this->pos.y- _Right.pos.y) <= 1e-6;
	}

};

void Vertex_rhw_init(Vertex* v);


// 边
class Edge {
public:
	// v是插值的结果,v1起点,v2终点
	Vertex v, v1, v2;
};



// 矩形
class Trapezoid {
public:
	float top, bottom; Edge left, right;
};

// 扫描线
class Scanline {
public:
	Vertex v, step; int x, y, w;
};








#endif // _JAYMATH3D_H_

