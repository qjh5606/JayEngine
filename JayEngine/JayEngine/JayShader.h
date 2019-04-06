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
	Vector normal; // ������
	Vector tangent; // ����
	Vector binormal; // ������?
	Texcoord texcoord; // ��������
};

class V2F {
public:
	Vector pos;
	Texcoord texcoord;
	Color color;
	Vector normal;
	Vector storage0;
	Vector storage1;
	Vector storage2;
};

class Shader {
public:
	
	A2V a2v;
	V2F v2f;

	void vert_shader() {

	}
	void frag_shader() {

	}
};




#endif // _JAYSHADER_H_