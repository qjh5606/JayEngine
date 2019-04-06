#ifndef _JAYMODEL_H_
#define _JAYMODEL_H_
#include "JayHeader.h"
#include "JayMath3D.h"




/*
	纹理
*/
class Texture {
public:
	// 默认纹理 自建棋盘纹理
	Texture() {
		width = 256,height = 256;
		IUINT32 *bits = new IUINT32[width * height];
		int i, j;
		for (j = 0; j < 256; j++) {
			for (i = 0; i < 256; i++) {
				int x = i / 32, y = j / 32;
				bits[j*width + i] = ((x + y) & 1) ? 0xffffff:0x3fbcef; // 生成网格块纹理
			}
		}
		datas_len = 1; // 第一层
		datas = new IUINT32 *[1]; //1?
		datas[0] = bits;
		//use_mipmap = true;// 生成mipmap待补充
		// 默认生成mipmap
		//generate_mipmaps(1.01);
	}

	IUINT32 readTexture(float u, float v) {
		// 纹理采样与插值
		// 先来一个简单采样:
		int x, y;
		u = u * (width-1);
		v = v * (height-1);
		x = (int)(u + 0.5f);
		y = (int)(v + 0.5f);
		x = CMID(x, 0, width - 1);
		y = CMID(y, 0, height - 1);
		return datas[0][y*width+x];
	}

	void generate_mipmaps(float gamma) {
		IUINT32** mipmaps = NULL;
		int num_mip_levels = logbase2ofx(this->width) + 1;
		this->datas_len = num_mip_levels;
		mipmaps = new IUINT32*[num_mip_levels];
		mipmaps[0] = datas[0];
		int mip_width = width;
		int mip_height = height;
		for (int mip_level = 1; mip_level < num_mip_levels; mip_level++) {
			mip_width = mip_width >> 1; // 除以2
			mip_height = mip_height >> 1; // 除以2
			// 开辟空间
			mipmaps[mip_level] = new IUINT32[mip_width*mip_height];
			// src
			IUINT32 *src_buffer = mipmaps[mip_level - 1];
			// dst
			IUINT32 *dest_buffer = mipmaps[mip_level];
			for (int x = 0; x < mip_width; x++) {
				for (int y = 0; y < mip_height; y++) {
					float r0,g0,b0,a0,
						r1, g1, b1, a1,
						r2, g2, b2, a2,
						r3, g3, b3, a3;
					// 平均值
					int r_avg, g_avg, b_avg, a_avg;
					IUINT32 c = src_buffer[(x * 2 + 0) + (y * 2 + 0) * 2 * mip_width];
					b0 = c & 0xff;
					g0 = (c >> 8) & 0xff;
					r0 = (c >> 16) & 0xff;
					a0 = (c >> 24) & 0xff;

					c = src_buffer[(x * 2 + 1) + (y * 2 + 0) * 2 * mip_width];
					b1 = c & 0xff;
					g1 = (c >> 8) & 0xff;
					r1 = (c >> 16) & 0xff;
					a1 = (c >> 24) & 0xff;

					c = src_buffer[(x * 2 + 0) + (y * 2 + 1) * 2 * mip_width];
					b2 = c & 0xff;
					g2 = (c >> 8) & 0xff;
					r2 = (c >> 16) & 0xff;
					a2 = (c >> 24) & 0xff;

					c = src_buffer[(x * 2 + 1) + (y * 2 + 1) * 2 * mip_width];
					b3 = c & 0xff;
					g3 = (c >> 8) & 0xff;
					r3 = (c >> 16) & 0xff;
					a3 = (c >> 24) & 0xff;

					r_avg = (IUINT32)(0.5f + gamma*(r0 + r1 + r2 + r3) / 4);
					g_avg = (IUINT32)(0.5f + gamma*(g0 + g1 + g2 + g3) / 4);
					b_avg = (IUINT32)(0.5f + gamma*(b0 + b1 + b2 + b3) / 4);
					a_avg = (IUINT32)(0.5f + gamma*(b0 + b1 + b2 + b3) / 4);

					int R = CMID(r_avg, 0, 255);
					int G = CMID(g_avg, 0, 255);
					int B = CMID(b_avg, 0, 255);
					int A = CMID(a_avg, 0, 255);

					// ARGB
					dest_buffer[y*mip_width + x] = (A << 24) | (R << 16) | (G << 8) | B;
				}
			}
		}
		free(datas);
		datas = mipmaps;
		datas_len = num_mip_levels;
	}

	~Texture() {
		for (int j = 0; j < this->datas_len; j++) {
			IUINT32 *data = this->datas[j];
			free(data);
		}
		free(this->datas);
		this->datas = NULL;
	}

	IUINT32 width;
	IUINT32 height;
	IUINT32 **datas;            // data

	IUINT32 datas_len;
	bool use_mipmap;            // able mipmap

};

/*	
	材质
*/
class Material {
public:
	char *name;
	Color ambient;
	Color diffuse;
	Color specular;
	Color transmittance;
	Color emission;
	float shininess;
	float ior;      /* index of refraction */
	float dissolve;
	int illum;

	int pad0;

	char *ambient_texname;            /* map_Ka */
	int ambient_tex_id;
	char *diffuse_texname;            /* map_Kd */
	int diffuse_tex_id;
	char *specular_texname;           /* map_Ks */
	int specular_tex_id;
	char *specular_highlight_texname; /* map_Ns */
	int specular_highlight_tex_id;
	char *bump_texname;               /* map_bump, bump */
	int bump_tex_id;
	char *displacement_texname;       /* disp */
	int displacement_tex_id;
	char *alpha_texname;              /* map_d */
	int alpha_tex_id;
};

/*
	模型
*/
class Model {
public:
	Model(){}

	Model(Vertex* mesh, unsigned long num) {
		if (mesh != NULL) {
			this->mesh = mesh;
			this->mesh_num = num;
		}
	}

	// 三角形网格模型
	Vertex *mesh; // 顶点数组
	unsigned long mesh_num; // 顶点个数 
};


// 物体
class Object {
public:
	Object() {
		Matrix_set_identity(&model_matrix);
	}

	Object(Model* model,Material* material,Texture* texture) {
		this->model = model;
		this->material = material;
		this->texture = texture;
		Matrix_set_identity(&model_matrix);
	}

	void updateModelMatrix() {
		Matrix_set_rotate_translate_scale(&model_matrix, &axis, theta, &pos, &scale);
	}

	Model* model;			// 模型
	Material* material;		// 材质
	Texture* texture;		// 纹理

	Point pos;				// 位置
	Vector scale;			// 缩放
	
	Vector axis;float theta;// 绕某个轴旋转某个角度

	bool shadow;			// 是否产生阴影?

	bool dirty;
	Matrix model_matrix;	// 表示model->world的变换矩阵
};



#endif // _JAYMODEL_H_