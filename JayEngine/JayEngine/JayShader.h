#ifndef _JAYSHADER_H_
#define _JAYSHADER_H_
#include "JayHeader.h"

/*
	着色器类
*/
class A2V {
public:
	Vector pos; // 顶点位置
	Color color; // 颜色
	Vector normal; // 法向量
	Vector tangent; // 切线
	Vector binormal; // 副法线?
	Texcoord texcoord; // 纹理坐标
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