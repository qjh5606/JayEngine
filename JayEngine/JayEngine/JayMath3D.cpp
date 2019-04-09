#include "JayMath3D.h"


int logbase2ofx(int n) {
	if (n <= 0) return 0;
	int r = 0;
	while (n != 0) {
		n = n >> 1;
		r++;
	}
	return r - 1;
}

float Vector_length(const Vector *v) {
	float sq = v->x * v->x + v->y * v->y + v->z * v->z;
	return (float)sqrt(sq);
}

// z = x + y
void Vector_add(Vector *z, const Vector *x, const Vector *y) {
	z->x = x->x + y->x;
	z->y = x->y + y->y;
	z->z = x->z + y->z;
	z->w = 1.0;
}

// z = x - y
void Vector_sub(Vector *z, const Vector *x, const Vector *y) {
	z->x = x->x - y->x;
	z->y = x->y - y->y;
	z->z = x->z - y->z;
	z->w = 1.0;
}

// 矢量点乘
float Vector_dotproduct(const Vector *x, const Vector *y) {
	return x->x * y->x + x->y * y->y + x->z * y->z;
}

// 矢量叉乘
void Vector_crossproduct(Vector *z, const Vector *x, const Vector *y) {
	float m1, m2, m3;
	m1 = x->y * y->z - x->z * y->y;
	m2 = x->z * y->x - x->x * y->z;
	m3 = x->x * y->y - x->y * y->x;
	z->x = m1;
	z->y = m2;
	z->z = m3;
	z->w = 1.0f;
}

// 矢量插值，t取值 [0, 1]
void Vector_interp(Vector *z, const Vector *x1, const Vector *x2, float t) {
	z->x = interp(x1->x, x2->x, t);
	z->y = interp(x1->y, x2->y, t);
	z->z = interp(x1->z, x2->z, t);
	z->w = interp(x1->w, x2->w, t); // 1/w其实
}


void Vector_clone(Vector *dest, const Vector *src) {
	dest->x = src->x;
	dest->y = src->y;
	dest->z = src->z;
	dest->w = src->w;
}

void Vector_reflect(Vector* r, const Vector *v, const Vector* n) {
	Vector_clone(r, n);
	Vector_scale(r, -2 * Vector_dotproduct(v, n));
	Vector_add(r, r, v);
}

// 矢量归一化
void Vector_normalize(Vector *v) {
	float length = Vector_length(v);
	if (length != 0.0f) {
		float inv = 1.0f / length;
		v->x *= inv;
		v->y *= inv;
		v->z *= inv;
	}
}

void Vector_scale(Vector *v, float k) {
	v->x *= k;
	v->y *= k;
	v->z *= k;
	v->w *= k;
}

void Vector_inverse(Vector *v) {
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}


// c = a + b
void Matrix_add(Matrix *c, const Matrix *a, const Matrix *b) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] + b->m[i][j];
	}
}

// c = a - b
void Matrix_sub(Matrix *c, const Matrix *a, const Matrix *b) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] - b->m[i][j];
	}
}

// c = a * b
void Matrix_mul(Matrix *c, const Matrix *a, const Matrix *b) {
	Matrix z;
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			z.m[j][i] = (a->m[j][0] * b->m[0][i]) +
				(a->m[j][1] * b->m[1][i]) +
				(a->m[j][2] * b->m[2][i]) +
				(a->m[j][3] * b->m[3][i]);
		}
	}
	c[0] = z;
}

// c = a * f
void Matrix_scale(Matrix *c, const Matrix *a, float f) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] * f;
	}
}

// y = x * m
void Matrix_apply(Vector *y, const Vector *x, const Matrix *m) {
	float X = x->x, Y = x->y, Z = x->z, W = x->w;
	y->x = X * m->m[0][0] + Y * m->m[1][0] + Z * m->m[2][0] + W * m->m[3][0];
	y->y = X * m->m[0][1] + Y * m->m[1][1] + Z * m->m[2][1] + W * m->m[3][1];
	y->z = X * m->m[0][2] + Y * m->m[1][2] + Z * m->m[2][2] + W * m->m[3][2];
	y->w = X * m->m[0][3] + Y * m->m[1][3] + Z * m->m[2][3] + W * m->m[3][3];
}

void Matrix_clone(Matrix *dest, const Matrix *src) {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			dest->m[i][j] = src->m[i][j];
}

void Matrix_transpose(Matrix *m) {
	for (int i = 0; i < 3; i++)
		for (int j = i + 1; j < 3; j++) {
			float temp = m->m[i][j];
			m->m[i][j] = m->m[j][i];
			m->m[j][i] = temp;
		}
}

void Matrix_inverse(Matrix *m) {
	float t[3][6];
	int i, j, k;
	float f;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 6; j++) {
			if (j < 3)
				t[i][j] = m->m[i][j];
			else if (j == i + 3)
				t[i][j] = 1;
			else
				t[i][j] = 0;
		}

	for (i = 0; i < 3; i++) {
		f = t[i][i];
		for (j = 0; j < 6; j++)
			t[i][j] /= f;
		for (j = 0; j < 3; j++) {
			if (j != i) {
				f = t[j][i];
				for (k = 0; k < 6; k++)
					t[j][k] = t[j][k] - t[i][k] * f;
			}
		}
	}

	for (i = 0; i < 3; i++)
		for (j = 3; j < 6; j++)
			m->m[i][j - 3] = t[i][j];

	m->m[3][0] = -m->m[3][0];
	m->m[3][1] = -m->m[3][1];
	m->m[3][2] = -m->m[3][2];
}



// 单位矩阵
void Matrix_set_identity(Matrix *m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

// 零矩阵
void Matrix_set_zero(Matrix *m) {
	m->m[0][0] = m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][1] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][2] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = m->m[3][3] = 0.0f;
}

void Matrix_set_axis(Matrix *m, const Vector *xaxis, const Vector *yaxis, const Vector *zaxis, const Point *pos) {
	m->m[0][0] = xaxis->x;
	m->m[0][1] = xaxis->y;
	m->m[0][2] = xaxis->z;
	m->m[1][0] = yaxis->x;
	m->m[1][1] = yaxis->y;
	m->m[1][2] = yaxis->z;
	m->m[2][0] = zaxis->x;
	m->m[2][1] = zaxis->y;
	m->m[2][2] = zaxis->z;
	m->m[3][0] = pos->x;
	m->m[3][1] = pos->y;
	m->m[3][2] = pos->z;
	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][3] = 1.0f;
}

//  set_scale 设置缩放系数
void Matrix_set_scale(Matrix *m, float sx, float sy, float sz) {
	Matrix_set_identity(m);
	m->m[0][0] = sx;
	m->m[1][1] = sy;
	m->m[2][2] = sz;
}


// set_rotate 
// m, x, y, z, theta
// x*x*(1-cos0)+cos0    x*y*(1-cos0)+z*sin0   x*z*(1-cos0)-y*sin0
// x*y*(1-cos0)-z*sin0  y*y*(1-cos0)+cos0     y*z*(1-cos0)+x*sin0
// x*z*(1-cos0)+y*sin0  y*z*(1-cos0)-x*sin0   z*z*(1-cos0)+cos0
void Matrix_set_rotate(Matrix *m, const Vector *v, float theta) {
	float x = v->x, y = v->y, z = v->z;
	float qsin = (float)sin(theta * 0.5f);
	float qcos = (float)cos(theta * 0.5f);
	Vector vec = { x, y, z, 1.0f };
	float w = qcos;
	Vector_normalize(&vec);
	x = vec.x * qsin;
	y = vec.y * qsin;
	z = vec.z * qsin;
	m->m[0][0] = 1 - 2 * y * y - 2 * z * z;
	m->m[1][0] = 2 * x * y - 2 * w * z;
	m->m[2][0] = 2 * x * z + 2 * w * y;
	m->m[0][1] = 2 * x * y + 2 * w * z;
	m->m[1][1] = 1 - 2 * x * x - 2 * z * z;
	m->m[2][1] = 2 * y * z - 2 * w * x;
	m->m[0][2] = 2 * x * z - 2 * w * y;
	m->m[1][2] = 2 * y * z + 2 * w * x;
	m->m[2][2] = 1 - 2 * x * x - 2 * y * y;
	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
	m->m[3][3] = 1.0f;
}

// 设置旋转平移缩放矩阵
void Matrix_set_rotate_translate_scale(Matrix *m, const Vector *axis, float theta, const Point *pos, const Vector *scale) {
	Matrix_set_scale(m, scale->x, scale->y, scale->z);
	Matrix r, t = *m;
	Matrix_set_rotate(&r, axis, theta);
	Matrix_mul(m, &t, &r);
	m->m[3][0] = pos->x;
	m->m[3][1] = pos->y;
	m->m[3][2] = pos->z;
}



// D3DXMatrixPerspectiveFovLH
// https://blog.csdn.net/zzmkljd/article/details/52870807
// set_perspective 
// m, fovy, aspect, zn, zf
// zoom = 1 / tan(fov/2)
// zoomy = 1 / tan(fovy/2)
// zoomx = zoomy * aspect
// zoomx    0       0               0
// 0        zoomy   0               0
// 0        0       zf/(zf-zn)      1
// 0        0       zn*zf/(zn-zf)   0
void Matrix_set_perspective(Matrix *m, float fovy, float aspect, float zn, float zf) {
	float fax = 1.0f / (float)tan(fovy * 0.5f);
	Matrix_set_zero(m);
	m->m[0][0] = (float)(fax / aspect);
	m->m[1][1] = (float)(fax);
	m->m[2][2] = zf / (zf - zn);
	m->m[3][2] = -zn * zf / (zf - zn);
	m->m[2][3] = 1;
}


// set_ortho 
// m, left, right, bottom, top, near, far
// 2/(r-l)      0            0           0
// 0            2/(t-b)      0           0
// 0            0            1/(zf-zn)   0
// (l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1
void Matrix_set_ortho(Matrix *m, float l, float r, float b, float t, float zn, float zf) {
	Matrix_set_zero(m);
	m->m[0][0] = 2.0f / (r - l);
	m->m[1][1] = 2.0f / (t - b);
	m->m[2][2] = 1.0f / (zf - zn);
	m->m[3][0] = (l + r) / (l - r);
	m->m[3][1] = (t + b) / (b - t);
	m->m[3][2] = zn / (zn - zf);
	m->m[3][3] = 1.0f;
}


// 设置摄像机
void Matrix_set_lookat(Matrix *m, const Vector *eye, const Vector *at, const Vector *up) {
	Vector xaxis, yaxis, zaxis;

	Vector_sub(&zaxis, at, eye);
	Vector_normalize(&zaxis);
	Vector_crossproduct(&xaxis, up, &zaxis);
	Vector_normalize(&xaxis);
	Vector_crossproduct(&yaxis, &zaxis, &xaxis);

	m->m[0][0] = xaxis.x;
	m->m[1][0] = xaxis.y;
	m->m[2][0] = xaxis.z;
	m->m[3][0] = -Vector_dotproduct(&xaxis, eye);

	m->m[0][1] = yaxis.x;
	m->m[1][1] = yaxis.y;
	m->m[2][1] = yaxis.z;
	m->m[3][1] = -Vector_dotproduct(&yaxis, eye);

	m->m[0][2] = zaxis.x;
	m->m[1][2] = zaxis.y;
	m->m[2][2] = zaxis.z;
	m->m[3][2] = -Vector_dotproduct(&zaxis, eye);

	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][3] = 1.0f;
}

void Vertex_rhw_init(Vertex* v) {
	float rhw = v->pos.w;	// 1/w
	// 纹理坐标
	v->tc.u *= rhw;
	v->tc.v *= rhw;
	// 颜色
	v->color.r *= rhw;
	v->color.g *= rhw;
	v->color.b *= rhw;
	// 法向量
}

// 插值 3个点插值一个点
void Vector_interpolating(Vector *dest,
	const Vector *src1, const Vector *src2, const Vector *src3,
	float a, float b, float c) {
	dest->x = dest->y = dest->z = dest->w = 0.0f;
	Vector each = *src1;
	Vector_scale(&each, a);
	Vector_add(dest, dest, &each);
	each = *src2;
	Vector_scale(&each, b);
	Vector_add(dest, dest, &each);
	each = *src3;
	Vector_scale(&each, c);
	Vector_add(dest, dest, &each);
}

void Color_scale(Color* c, float k) {
	c->r *= k;
	c->g *= k;
	c->b *= k;
	c->a *= k;
}

void Color_add(Color* c, Color* a, Color *b) {
	c->r = a->r + b->r;
	c->g = a->g + b->g;
	c->b = a->b + b->b;
	c->a = a->a + b->a;
}

void Color_sub(Color *c, const Color *a, const Color *b) {
	c->r = a->r - b->r;
	c->g = a->g - b->g;
	c->b = a->b - b->b;
	c->a = a->a - b->a;
}

void Color_product(Color *c, const Color *a, const Color *b) {
	c->r = a->r * b->a;
	c->g = a->g * b->g;
	c->b = a->b * b->b;
	c->a = a->a * b->a;
}



void Color_interpolating(Color *dest, const Color *src1, const Color *src2, const Color *src3, float a, float b, float c) {
	dest->r = dest->g = dest->b = dest->a = 0.0f;
	Color each = *src1;
	Color_scale(&each, a);
	Color_add(dest, dest, &each);
	each = *src2;
	Color_scale(&each, b);
	Color_add(dest, dest, &each);
	each = *src3;
	Color_scale(&each, c);
	Color_add(dest, dest, &each);
}

// 纹理缩放
void Texcoord_scale(Texcoord *t, float k) {
	t->u *= k;
	t->v *= k;
}

// 纹理缩放
void Texcoord_add(Texcoord *c, const Texcoord *a, const Texcoord *b) {
	c->u = a->u + b->u;
	c->v = a->v + b->v;
}

// 纹理插值
void Texcoord_interpolating(Texcoord *dest, const Texcoord *src1, const Texcoord *src2, const Texcoord *src3, float a, float b, float c) {
	dest->u = dest->v = 0.0f;
	Texcoord each = *src1;
	Texcoord_scale(&each, a);
	Texcoord_add(dest, dest, &each);
	each = *src2;
	Texcoord_scale(&each, b);
	Texcoord_add(dest, dest, &each);
	each = *src3;
	Texcoord_scale(&each, c);
	Texcoord_add(dest, dest, &each);
}