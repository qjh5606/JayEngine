#ifndef _JAYCAMERA_H_
#define _JAYCAMERA_H_
#include "JayHeader.h"
#include "JayMath3D.h"

/*
	相机类
*/

// 投影方式
typedef enum {
	perspective, // 透视投影
	orthographic // 正视投影
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
		// 初始化摄像机
		camera_init_projection();
		// 更新
		camera_update();
	}

	PROJECTION projection; // 投影方式
	bool main; // 是否是主相机
	bool dirty;

	Point pos;  // 相机的位置

	Vector front;
	Vector worldup;

	Matrix view_matrix; // view矩阵
	Matrix view_matrix_r;

	Matrix projection_matrix;  // 投影矩阵

	// 用于构造投影矩阵
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

	// 利用欧拉角原理来实现摄像机旋转
	void camera_init_by_euler(float yaw, float pitch);

	// 初始化投影矩阵
	void camera_init_projection();

	// 相机更新view矩阵
	void camera_update();
};




#endif // _JAYCAMERA_H_