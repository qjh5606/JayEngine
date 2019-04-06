#include "JayCamera.h"

// ��ʼ�������ͶӰ����
void Camera::camera_init_projection() {
	if (projection == perspective) {
		Matrix_set_perspective(&this->projection_matrix,
			fovy, aspect, zn, zf);
	} else if (projection == orthographic) {
		Matrix_set_ortho(&this->projection_matrix, left, right, bottom, top, zn, zf);
	}
}

// ����ŷ����ԭ����ʵ���������ת
void Camera::camera_init_by_euler(float yaw, float pitch) {
	this->front.x = sin(angle_to_radian(yaw)) * cos(angle_to_radian(pitch));
	this->front.y = -sin(angle_to_radian(pitch));
	this->front.z = cos(angle_to_radian(yaw)) * cos(angle_to_radian(pitch));
	Vector_normalize(&this->front);
}

// �������,���������view����
void Camera::camera_update() {
	Vector right, at, up, front = this->front;
	Vector_crossproduct(&right, &this->worldup, &this->front);
	Vector_normalize(&right);
	Vector_crossproduct(&up, &this->front, &right);
	Vector_normalize(&up);
	Vector_add(&at, &this->pos, &this->front);

	Matrix_set_lookat(&this->view_matrix, &this->pos, &at, &up);
	Vector_normalize(&front);
	Matrix_set_axis(&this->view_matrix_r, &right, &up, &front, &this->pos);
}