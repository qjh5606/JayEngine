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

	Color readTextureCorlor(float u, float v);

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
	Material() {

	}

	Material(std::string name, 
		Color ambient, Color diffuse, Color specular, Color transmittance, Color emission,
		float shininess, float ior, float dissolve,
		int illum, int pad0,
		std::string ambient_texname, int ambient_tex_id,
		std::string diffuse_texname,int diffuse_tex_id,
		std::string specular_texname,int specular_tex_id,
		std::string specular_highlight_texname,int specular_highlight_tex_id,
		std::string bump_texname,int bump_tex_id,
		std::string displacement_texname,int displacement_tex_id,
		std::string alpha_texname,int alpha_tex_id			
	) {
		this->name = name;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->transmittance = transmittance;
		this->emission = emission;
		this->shininess = shininess;
		this->ior = ior;
		this->dissolve = dissolve;
		this->illum = illum;
		this->pad0 = pad0;
		this->ambient_texname = ambient_texname;
		this->ambient_tex_id = ambient_tex_id;
		this->diffuse_texname = diffuse_texname;
		this->diffuse_tex_id = diffuse_tex_id;
		this->specular_texname = specular_texname;
		this->specular_tex_id = specular_tex_id;
		this->specular_highlight_texname = specular_highlight_texname;
		this->specular_highlight_tex_id = specular_highlight_tex_id;
		this->bump_texname = bump_texname;
		this->bump_tex_id = bump_tex_id;
		this->displacement_texname = displacement_texname;
		this->displacement_tex_id = displacement_tex_id;
		this->alpha_texname = alpha_texname;
		this->alpha_tex_id = this->alpha_tex_id;
	}

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
	Object(Model* model,Material* tm) {
		if (model != NULL) {
			models.push_back(model);
			this->Curmodel = model;
			this->Curmaterial = tm;

			if (tm->ambient_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->ambient_texname)) 					{
					tm->ambient_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm->diffuse_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->diffuse_texname)) {
					tm->diffuse_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm->specular_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->specular_texname)) 					{
					tm->specular_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm->specular_highlight_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->specular_highlight_texname)) {
					tm->specular_highlight_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm->bump_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->bump_texname)) {
					tm->bump_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm->displacement_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->displacement_texname)) {
					tm->displacement_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm->alpha_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm->alpha_texname)) {
					tm->alpha_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
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
	//std::vector<Material*> materials; // ���еĲ���

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