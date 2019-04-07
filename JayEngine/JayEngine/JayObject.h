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
	Texture(bool mipmap=true);
	// 读入图片创建纹理
	Texture(string &path,bool mimap=true);

	IUINT32 readTexture(float u, float v);

	void generate_mipmaps(float gamma);

	bool make_texture_by_stb(string &path, bool mipmap=true);

	~Texture();

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
	std::string name;
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

	// 应该需要修改数据,改成纹理的指针
	std::string ambient_texname;            /* map_Ka */
	int ambient_tex_id;
	std::string diffuse_texname;            /* map_Kd */
	int diffuse_tex_id;
	std::string specular_texname;           /* map_Ks */
	int specular_tex_id;
	std::string specular_highlight_texname; /* map_Ns */
	int specular_highlight_tex_id;
	std::string bump_texname;               /* map_bump, bump */
	int bump_tex_id;
	std::string displacement_texname;       /* disp */
	int displacement_tex_id;
	std::string alpha_texname;              /* map_d */
	int alpha_tex_id;
};

/*
	模型 // 网格顶点数据
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

	// 通过自定义的模型,材质和纹理进行初始化
	Object(Model* model,Material* material,Texture* texture) {
		if (model != NULL) {
			models.push_back(model);
			materials.push_back(material);
			textures.push_back(texture);

			this->Curmodel = model;
			this->Curmaterial = material;
			this->Curtexture = texture;
			Matrix_set_identity(&model_matrix);
		}
	}

	// 通过加载OBJ文件初始化
	Object(const char* filename, const char* basepath = NULL, bool triangulate = true) {
		make_mesh_and_material_by_obj(filename, basepath, triangulate);
		Matrix_set_identity(&model_matrix);
	}

	// 通过加载OBJ文件初始化
	bool make_mesh_and_material_by_obj(const char* filename, const char* basepath = NULL, bool triangulate = true);
	
	// 更新模型矩阵
	void updateModelMatrix() {
		Matrix_set_rotate_translate_scale(&model_matrix, &axis, theta, &pos, &scale);
	}

	Model* Curmodel;			// 模型:网格顶点数据
	std::vector<Model*> models; // 所有的材质
	Material* Curmaterial;		// 当前材质
	std::vector<Material*> materials; // 所有的材质

	std::vector<Texture*> textures; // 所有的材质
	Texture* Curtexture;		// 纹理


	Point pos;				// 当前位置
	Vector scale;			// 缩放
	
	Vector axis;float theta;// 绕某个轴旋转某个角度

	bool shadow;			// 是否产生阴影?

	bool dirty;
	Matrix model_matrix;	// 表示model->world的变换矩阵
};


#endif // _JAYMODEL_H_