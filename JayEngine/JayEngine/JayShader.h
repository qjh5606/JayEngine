#ifndef _JAYSHADER_H_
#define _JAYSHADER_H_
#include "JayHeader.h"

/*
	��ɫ����
*/
class A2V {
public:
	Vector pos; // ����λ��
	Color color; // ��ɫ

	/* ������ͼ��Ҫ */
	Vector normal; // ������
	Vector tangent; // ����
	Vector binormal; // normal x tangent

	Texcoord texcoord; // ��������
};

class V2F {
public:
	Vector pos; // ����λ��
	Texcoord texcoord;// ��������
	Color color;// ��ɫ
	Vector normal;   // ����
	Vector storage0;
	Vector storage1;
	Vector storage2;
};

class Shader {
public:
	
	A2V a2v[3];
	V2F v2f[3];
};




#endif // _JAYSHADER_H_