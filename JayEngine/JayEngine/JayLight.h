#ifndef _JAYLIGHT_H_
#define _JAYLIGHT_H_
#include "JayHeader.h"
#include "JayMath3D.h"

/*
	��Դ��
*/

typedef enum {
	POINTLIGHT = 0, // ���Դ
	DIRLIGHT = 1,	// ƽ�й�
	SPOTLIGHT = 2  // �۹��
} LightType;

class Light {
public:
	int lightType;
	bool shadow;
};

// ���Դ
class PointLight :public Light {
public:
	PointLight() {
		this->lightType = POINTLIGHT;
	}

	PointLight(Point pos,float constant,float linear,float quadratic,Color ambi,Color diff,Color spec,bool shadow) {
		this->lightType = POINTLIGHT;

		this->pos = pos;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
		this->ambi = ambi;
		this->diff = diff;
		this->spec = spec;
		this->shadow = shadow;
	}
	Point pos;
	float constant;
	float linear;
	float quadratic;
	Color ambi;
	Color diff;
	Color spec;
};

// ƽ�й�
class DirLight :public Light {
public:

	DirLight() {
		this->lightType = DIRLIGHT;
	}

	DirLight(Color dir,Color ambi,Color diff,Color spec,bool shadow) {
		this->lightType = DIRLIGHT;
	
		this->dir = dir;
		this->ambi = ambi;
		this->diff = diff;
		this->spec = spec;
		this->shadow = shadow;
	}

	Color dir;
	Color ambi;
	Color diff;
	Color spec;
};



#endif // _JAYLIGHT_H_