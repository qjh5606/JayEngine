#ifndef _JAYLIGHT_H_
#define _JAYLIGHT_H_
#include "JayHeader.h"
#include "JayMath3D.h"

/*
	光源类
*/

typedef enum {
	POINTLIGHT = 0, // 点光源
	DIRLIGHT = 1,	// 平行光
	SPOTLIGHT = 2  // 聚光灯
} LightType;

// 点光源
class PointLight{
public:
	PointLight(Point pos,float constant,float linear,float quadratic,Color ambi,Color diff,Color spec,bool shadow) {
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
	bool shadow;
};

// 平行光
class DirLight{
public:
	DirLight(Vector dir,Color ambi,Color diff,Color spec,bool shadow) {
		this->dir = dir;
		this->ambi = ambi;
		this->diff = diff;
		this->spec = spec;
		this->shadow = shadow;
	}

	Vector dir;
	Color ambi;
	Color diff;
	Color spec;
	bool shadow;
};



#endif // _JAYLIGHT_H_