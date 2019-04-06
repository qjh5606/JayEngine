#ifndef _JAYCAMERA_H_
#define _JAYCAMERA_H_
#include "JayHeader.h"
#include "JayMath3D.h"

/*
	�����
*/

// ͶӰ��ʽ
typedef enum {
	perspective, // ͸��ͶӰ
	orthographic // ����ͶӰ
} PROJECTION;
class Camera {
public:

	Camera(PROJECTION projection,
		bool main, Point pos, Vector front, Vector worldup,
		float fovy, float zn, float zf, int width, int height,
		float left,float right,float bottom ,float top,
		bool dirty
		) {
		this->projection = projection;
		this->main = main;
		this->pos = pos;
		this->front = front;
		this->worldup = worldup;
		this->fovy = fovy;
		this->zn = zn;
		this->zf = zf;
		this->width = width;
		this->height = height;
		this->aspect = (float)width / (float)height;
		this->left = left;
		this->right = right;
		this->bottom = bottom;
		this->top = top;

		this->dirty = dirty;
		// ��ʼ�������
		camera_init_projection();
		// ����
		camera_update();
	}

	PROJECTION projection; // ͶӰ��ʽ
	bool main; // �Ƿ��������
	bool dirty;

	Point pos;  // �����λ��

	Vector front;
	Vector worldup;

	Matrix view_matrix; // view����
	Matrix view_matrix_r;

	Matrix projection_matrix;  // ͶӰ����

	// ���ڹ���ͶӰ����
	int width;
	int height;
	float fovy;
	float zn;
	float zf;

	float aspect;

	float left;
	float right;
	float bottom;
	float top;

	// ����ŷ����ԭ����ʵ���������ת
	void camera_init_by_euler(float yaw, float pitch);

	// ��ʼ��ͶӰ����
	void camera_init_projection();

	// �������view����
	void camera_update();
};




#endif // _JAYCAMERA_H_