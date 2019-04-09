#ifndef _JAYTRANSFORM_H_
#define _JAYTRANSFORM_H_
#include "JayHeader.h"
#include "JayMath3D.h"

/*
	�ռ�任��
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

	// ����mv vp mvp ����
	void update_transform() {
		Matrix_mul(&this->mv, &this->model, &this->view);
		Matrix_mul(&this->vp, &this->view, &this->projection);
		Matrix_mul(&this->mvp, &this->mv, &this->projection);
	}

	// Ӧ��mvp����
	void transform_apply(Vector *y, const Vector *x) {
		Matrix_apply(y, x, &mvp);
	}

	// ����������ͬ cvv �ı߽�������׶�ü�
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

	// ��һ�����õ���Ļ����
	void transform_homogenize(Vector *y, const Vector *x) {
		// ͸�ӳ���
		float rhw = 1.0f / x->w;
		y->x = (x->x * rhw + 1.0f) * w * 0.5f;
		y->y = (1.0f - x->y * rhw) * h * 0.5f;
		//y->y = (1.0f + x->y * rhw) * h * 0.5f;
		y->z = x->z * rhw;
		y->w = rhw; // 1/w 
		// w��z�����Թ�ϵ,���������Ѿ��Ǽ��� ����ӳ��������
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

	Matrix model;		// �����model����
	Matrix view;		// �����view����
	Matrix view_r;		// view reverse
	Matrix projection;	// ͶӰ����
	Matrix vp;			// view * projection
	Matrix mv;			// model * view
	Matrix mvp;			// model * view * projection

	int w, h;
};

#endif // _JAYTRANSFORM_H_