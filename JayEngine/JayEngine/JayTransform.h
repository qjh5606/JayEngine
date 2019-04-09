#ifndef _JAYTRANSFORM_H_
#define _JAYTRANSFORM_H_
#include "JayHeader.h"
#include "JayMath3D.h"

/*
	空间变换类
*/
class Transform {
public:

	Transform() {
		reset();
	}

	Transform(int width, int height) {
		this->w = width;
		this->h = height;
	}

	void setResolution(int width, int height) {
		this->w = width;
		this->h = height;
	}

	void reset() {
		Matrix_set_identity(&model);
		Matrix_set_identity(&view);
		Matrix_set_identity(&view_r);
		Matrix_set_identity(&projection);
		Matrix_set_identity(&vp);
		Matrix_set_identity(&mv);
		Matrix_set_identity(&mvp);
	}

	// 更新mv vp mvp 矩阵
	void update_transform() {
		Matrix_mul(&this->mv, &this->model, &this->view);
		Matrix_mul(&this->vp, &this->view, &this->projection);
		Matrix_mul(&this->mvp, &this->mv, &this->projection);
	}

	// 应用mvp矩阵
	void transform_apply(Vector *y, const Vector *x) {
		Matrix_apply(y, x, &mvp);
	}

	// 检查齐次坐标同 cvv 的边界用于视锥裁剪
	int transform_check_cvv(const Vector *v) {
		float w = v->w;
		int check = 0;
		if (v->z < 0.0f) check |= 1;
		if (v->z > w) check |= 2;
		if (v->x < -w) check |= 4;
		if (v->x > w) check |= 8;
		if (v->y < -w) check |= 16;
		if (v->y > w) check |= 32;
		return check;
	}

	// 归一化，得到屏幕坐标
	void transform_homogenize(Vector *y, const Vector *x) {
		// 透视除法
		float rhw = 1.0f / x->w;
		y->x = (x->x * rhw + 1.0f) * w * 0.5f;
		y->y = (1.0f - x->y * rhw) * h * 0.5f;
		//y->y = (1.0f + x->y * rhw) * h * 0.5f;
		y->z = x->z * rhw;
		y->w = rhw; // 1/w 
		// w和z是线性关系,所以这里已经是加了 仿射映射修正的
	}

	void transform_homogenize_reverse(Vector *y, const Vector *x, float w, float width, float height) {
		y->x = (x->x * 2 / width - 1.0f) * w;
		y->y = (1.0f - x->y * 2 / height) * w;
		y->z = x->z * w;
		y->w = w;
	}

	void project_divese(Vector* v) {
		float rhw = 1.0f / v->w;
		v->x *= rhw;
		v->y *= rhw;
		v->z *= rhw;
		v->w = 1.0f;
	}

	void viewpoint(Vector* out,Vector* in) {
		out->x = (in->x + 1.0f) * w * 0.5f;
		out->y = (1.0f-in->y) * h * 0.5f;
		//out->y = (in->y + 1.0f) * h * 0.5f;
		out->z = in->z;
		out->w = 1.0f;
	}

	Matrix model;		// 物体的model矩阵
	Matrix view;		// 相机的view矩阵
	Matrix view_r;		// view reverse
	Matrix projection;	// 投影矩阵
	Matrix vp;			// view * projection
	Matrix mv;			// model * view
	Matrix mvp;			// model * view * projection

	int w, h;
};

#endif // _JAYTRANSFORM_H_