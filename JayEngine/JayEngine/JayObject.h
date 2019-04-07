#ifndef _JAYMODEL_H_
#define _JAYMODEL_H_
#include "JayHeader.h"
#include "JayMath3D.h"



/*
	����
*/
class Texture {
public:
	// Ĭ������ �Խ���������
	Texture(bool mipmap=true);
	// ����ͼƬ��������
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
	����
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

	// Ӧ����Ҫ�޸�����,�ĳ������ָ��
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
	ģ�� // ���񶥵�����
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
	// ����������ģ��
	Vertex *mesh; // ��������
	unsigned long mesh_num; // ������� 
};

// ����
class Object {
public:
	Object() {
		Matrix_set_identity(&model_matrix);
	}

	// ͨ���Զ����ģ��,���ʺ�������г�ʼ��
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

	// ͨ������OBJ�ļ���ʼ��
	Object(const char* filename, const char* basepath = NULL, bool triangulate = true) {
		make_mesh_and_material_by_obj(filename, basepath, triangulate);
		Matrix_set_identity(&model_matrix);
	}

	// ͨ������OBJ�ļ���ʼ��
	bool make_mesh_and_material_by_obj(const char* filename, const char* basepath = NULL, bool triangulate = true);
	
	// ����ģ�;���
	void updateModelMatrix() {
		Matrix_set_rotate_translate_scale(&model_matrix, &axis, theta, &pos, &scale);
	}

	Model* Curmodel;			// ģ��:���񶥵�����
	std::vector<Model*> models; // ���еĲ���
	Material* Curmaterial;		// ��ǰ����
	std::vector<Material*> materials; // ���еĲ���

	std::vector<Texture*> textures; // ���еĲ���
	Texture* Curtexture;		// ����


	Point pos;				// ��ǰλ��
	Vector scale;			// ����
	
	Vector axis;float theta;// ��ĳ������תĳ���Ƕ�

	bool shadow;			// �Ƿ������Ӱ?

	bool dirty;
	Matrix model_matrix;	// ��ʾmodel->world�ı任����
};


#endif // _JAYMODEL_H_