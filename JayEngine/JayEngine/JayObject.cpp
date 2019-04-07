#include "JayObject.h"

// 地面网格
Vertex ground_mesh[6] = {
	// Positions                  // Texture Coords  //color           //rhw // Normals
	{ { -0.5f,  0.0f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 0.75f, 0.75f, 0.75f, 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.0f,  0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.75f, 0.75f, 0.75f, 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.0f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 0.75f, 0.75f, 0.75f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.0f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 0.75f, 0.75f, 0.75f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.0f, -0.5f, 1.0f },{ 1.0f,  1.0f },{ 0.75f, 0.75f, 0.75f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.0f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 0.75f, 0.75f, 0.75f, 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } }
};

// 箱子,12个面
Vertex box_mesh[36] = {
	// Positions                  // Texture Coords  //color           //rhw // Normals
	{ { -0.5f, -0.5f, -0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 1.0f, 1.0f, 1.0f },{ 0.0f,  0.0f,-1.0f,0.0f } },
	{ { -0.5f,  0.5f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 1.0f, 0.3f, 0.3f, 1.0f },{ 0.0f,  0.0f,-1.0f,0.0f } },
	{ { 0.5f,  0.5f, -0.5f, 1.0f },{ 1.0f,  1.0f },{ 0.2f, 1.0f, 0.3f, 1.0f },{ 0.0f,  0.0f,-1.0f ,0.0f } },
	{ { 0.5f,  0.5f, -0.5f, 1.0f },{ 1.0f,  1.0f },{ 0.2f, 1.0f, 0.3f, 1.0f },{ 0.0f,  0.0f,-1.0f,0.0f } },
	{ { 0.5f, -0.5f, -0.5f, 1.0f },{ 1.0f,  0.0f },{ 1.0f, 1.0f, 0.2f, 1.0f },{ 0.0f,  0.0f,-1.0f ,0.0f } },
	{ { -0.5f, -0.5f, -0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 1.0f, 1.0f, 1.0f },{ 0.0f,  0.0f,-1.0f,0.0f } },

	{ { -0.5f, -0.5f,  0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 1.0f, 0.2f, 1.0f },{ 0.0f,  0.0f, 1.0f,0.0f } },
	{ { 0.5f, -0.5f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 0.2f, 1.0f },{ 0.0f,  0.0f,  1.0f,0.0f } },
	{ { 0.5f,  0.5f,  0.5f, 1.0f },{ 1.0f,  1.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 0.0f,  0.0f,  1.0f,0.0f } },
	{ { 0.5f,  0.5f,  0.5f, 1.0f },{ 1.0f,  1.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 0.0f,  0.0f,  1.0f,0.0f } },
	{ { -0.5f,  0.5f,  0.5f, 1.0f },{ 0.0f,  1.0f },{ 0.2f, 0.2f, 1.0f, 1.0f },{ 0.0f,  0.0f,  1.0f,0.0f } },
	{ { -0.5f, -0.5f,  0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 1.0f, 0.2f, 1.0f },{ 0.0f,  0.0f,  1.0f,0.0f } },

	{ { -0.5f,  0.5f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 0.2f, 0.2f, 1.0f, 1.0f },{ -1.0f,  0.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.5f, -0.5f, 1.0f },{ 1.0f,  1.0f },{ 1.0f, 0.3f, 0.3f, 1.0f },{ -1.0f,  0.0f,  0.0f,0.0f } },
	{ { -0.5f, -0.5f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 1.0f, 1.0f, 0.2f, 1.0f },{ -1.0f,  0.0f,  0.0f,0.0f } },
	{ { -0.5f, -0.5f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 1.0f, 1.0f, 0.2f, 1.0f },{ -1.0f,  0.0f,  0.0f,0.0f } },
	{ { -0.5f, -0.5f,  0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 1.0f, 0.2f, 1.0f },{ -1.0f,  0.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.5f,  0.5f,1.0f },{ 1.0f,  0.0f },{ 0.2f, 0.2f, 1.0f, 1.0f },{ -1.0f,  0.0f,  0.0f,0.0f } },

	{ { 0.5f,  0.5f,  0.5f,1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 1.0f,  0.0f,  0.0f,0.0f } },
	{ { 0.5f, -0.5f,  0.5f,1.0f },{ 0.0f,  0.0f },{ 1.0f, 0.2f, 0.2f, 1.0f },{ 1.0f,  0.0f,  0.0f,0.0f } },
	{ { 0.5f, -0.5f, -0.5f,1.0f },{ 0.0f,  1.0f },{ 1.0f, 1.0f, 0.2f, 1.0f },{ 1.0f,  0.0f,  0.0f,0.0f } },
	{ { 0.5f, -0.5f, -0.5f,1.0f },{ 0.0f,  1.0f },{ 1.0f, 1.0f, 0.2f, 1.0f },{ 1.0f,  0.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.5f, -0.5f,1.0f },{ 1.0f,  1.0f },{ 0.2f, 1.0f, 0.3f, 1.0f },{ 1.0f,  0.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.5f,  0.5f,1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 1.0f,  0.0f,  0.0f,0.0f } },

	{ { -0.5f, -0.5f, -0.5f,1.0f },{ 0.0f,  1.0f },{ 0.2f, 1.0f, 1.0f, 1.0f },{ 0.0f, -1.0f,  0.0f,0.0f } },
	{ { 0.5f, -0.5f, -0.5f,1.0f },{ 1.0f,  1.0f },{ 1.0f, 1.0f, 0.2f , 1.0f },{ 0.0f, -1.0f,  0.0f,0.0f } },
	{ { 0.5f, -0.5f,  0.5f,1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 0.2f, 1.0f },{ 0.0f, -1.0f,  0.0f,0.0f } },
	{ { 0.5f, -0.5f,  0.5f,1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 0.2f, 1.0f },{ 0.0f, -1.0f,  0.0f,0.0f } },
	{ { -0.5f, -0.5f,  0.5f,1.0f },{ 0.0f,  0.0f },{ 0.2f, 1.0f, 0.2f, 1.0f },{ 0.0f, -1.0f,  0.0f,0.0f } },
	{ { -0.5f, -0.5f, -0.5f,1.0f },{ 0.0f,  1.0f },{ 0.2f, 1.0f, 1.0f, 1.0f },{ 0.0f, -1.0f,  0.0f,0.0f } },

	{ { -0.5f,  0.5f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 1.0f, 0.3f, 0.3f, 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.5f,  0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 0.2f, 1.0f , 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.5f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.5f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.5f, -0.5f, 1.0f },{ 1.0f,  1.0f },{ 0.2f, 1.0f, 0.3f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.5f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 1.0f, 0.3f, 0.3f, 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } }
};

Vertex triangle_mesh[3] = {
	{ { -0.5f,  0.5f, -0.5f, 1.0f },{ 0.0f,  1.0f },{ 1.0f, 0.3f, 0.3f, 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } },
	{ { -0.5f,  0.5f,  0.5f, 1.0f },{ 0.0f,  0.0f },{ 0.2f, 0.2f, 1.0f , 1.0f },{ 0.0f, 1.0f,  0.0f,0.0f } },
	{ { 0.5f,  0.5f,  0.5f, 1.0f },{ 1.0f,  0.0f },{ 1.0f, 0.2f, 1.0f, 1.0f },{ 0.0f,1.0f,  0.0f,0.0f } }
};


// 材质
Material m1 = { "",{ 0.2f, 0.2f, 0.2f },{ 0.5f, 0.5f, 0.5f },{ 0.2f, 0.2f, 0.2f },{ 0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f }, 32.0f, 1.0f, 1.0f, 1, 1, "", -1, "", 2, "", -1, "", -1, "", -1, "", -1, "", -1 };
//Material m2 = { "mabu", { 0.2f, 0.2f, 0.2f }, { 0.5f, 0.5f, 0.5f }, { 0.2f, 0.2f, 0.2f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, 32.0f, 1.0f, 1.0f, 1, 1, NULL, -1, "", 1, NULL, -1, NULL, -1, NULL, -1, NULL, -1, NULL, -1 };


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// ============================================= 纹理 ============================================= // 


Texture::Texture(bool mipmap) {
	// 默认纹理 自建棋盘纹理
	width = 256, height = 256;
	IUINT32 *bits = new IUINT32[width * height];
	int i, j;
	for (j = 0; j < 256; j++) {
		for (i = 0; i < 256; i++) {
			int x = i / 32, y = j / 32;
			bits[j*width + i] = ((x + y) & 1) ? 0xffffff : 0x3fbcef; // 生成网格块纹理
		}
	}
	datas_len = 1; // 第一层
	datas = new IUINT32 *[1]; //1?
	datas[0] = bits;
	use_mipmap = mipmap;// 生成mipmap待补充
	if(use_mipmap)
		generate_mipmaps(1.01);
}

Texture::Texture(string &path, bool mimap) {
	// 加载数据
	if (!make_texture_by_stb(path, mimap)) {
		cout << "Texture Loading ERROR" << endl;
	}
}

bool Texture::make_texture_by_stb(string &path, bool mipmap) {
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	this->width = width;
	this->height = height;

	stbi_set_flip_vertically_on_load(true);
	if (data) {
		//将unsigned char *data 转成 IUINT32*
		IUINT32 *bits = new IUINT32[width * height];
		int x, y;
		for (y = 0; y < height; ++y) {
			for (x = 0; x < width; ++x) {
				/* 以下是RGBA数据，需要自己补充代码，保存RGBA数据 */
				bits[y*width + x] = 0;
				// RGB
				bits[y*width + x] |= *(data + y*width*nrChannels + 0) << 16; //R
				bits[y*width + x] |= *(data + y*width*nrChannels + 1) << 8;//G
				bits[y*width + x] |= *(data + y*width*nrChannels + 2);//B
				if (nrChannels == 4) {
					bits[y*width + x] |= *(data + y*width*nrChannels + 3) << 24; // alpha
				}
			}
		}
		datas_len = 1; // 第一层
		datas = new IUINT32 *[1]; //1?
		datas[0] = bits;
		use_mipmap = mipmap;// 生成mipmap待补充
		if (use_mipmap)
			generate_mipmaps(1.01);
		return true;
	}
	else {
		return false;
	}
}


Texture::~Texture() {
	for (int j = 0; j < this->datas_len; j++) {
		IUINT32 *data = this->datas[j];
		free(data);
	}
	free(this->datas);
	this->datas = NULL;
}

IUINT32 Texture::readTexture(float u, float v) {
	// 纹理采样与插值
	// 先来一个简单采样:
	int x, y;
	u = u * (width - 1);
	v = v * (height - 1);
	x = (int)(u + 0.5f);
	y = (int)(v + 0.5f);
	x = CMID(x, 0, width - 1);
	y = CMID(y, 0, height - 1);
	return datas[0][y*width + x];
}

void Texture::generate_mipmaps(float gamma) {
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
				float r0, g0, b0, a0,
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

// ============================================= Model ============================================= // 

// ============================================= Object ============================================= // 

// 计算法线向量
void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
	float v10[3];
	float v20[3];
	float len2;

	v10[0] = v1[0] - v0[0];
	v10[1] = v1[1] - v0[1];
	v10[2] = v1[2] - v0[2];

	v20[0] = v2[0] - v0[0];
	v20[1] = v2[1] - v0[1];
	v20[2] = v2[2] - v0[2];

	N[0] = v20[1] * v10[2] - v20[2] * v10[1];
	N[1] = v20[2] * v10[0] - v20[0] * v10[2];
	N[2] = v20[0] * v10[1] - v20[1] * v10[0];

	len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
	if (len2 > 0.0f) {
		float len = (float)sqrt((double)len2);

		N[0] /= len;
		N[1] /= len;
	}
}

// 加载OBJ模型加载器
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// 打印加载的数据信息
static void PrintInfo(const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes,
	const std::vector<tinyobj::material_t>& materials) {
	std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
	std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
	std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
		<< std::endl;

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
		printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.vertices[3 * v + 0]),
			static_cast<const double>(attrib.vertices[3 * v + 1]),
			static_cast<const double>(attrib.vertices[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
		printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.normals[3 * v + 0]),
			static_cast<const double>(attrib.normals[3 * v + 1]),
			static_cast<const double>(attrib.normals[3 * v + 2]));
	}

	for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
		printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
			static_cast<const double>(attrib.texcoords[2 * v + 0]),
			static_cast<const double>(attrib.texcoords[2 * v + 1]));
	}

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", static_cast<long>(i),
			shapes[i].name.c_str());
		printf("Size of shape[%ld].mesh.indices: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.indices.size()));
		printf("Size of shape[%ld].path.indices: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].path.indices.size()));

		size_t index_offset = 0;

		assert(shapes[i].mesh.num_face_vertices.size() ==
			shapes[i].mesh.material_ids.size());

		assert(shapes[i].mesh.num_face_vertices.size() ==
			shapes[i].mesh.smoothing_group_ids.size());

		printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

		// For each face
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
				static_cast<unsigned long>(fnum));

			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
					static_cast<long>(v), idx.vertex_index, idx.normal_index,
					idx.texcoord_index);
			}

			printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
				shapes[i].mesh.material_ids[f]);
			printf("  face[%ld].smoothing_group_id = %d\n", static_cast<long>(f),
				shapes[i].mesh.smoothing_group_ids[f]);

			index_offset += fnum;
		}

		printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
			static_cast<unsigned long>(shapes[i].mesh.tags.size()));
		for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
			printf("  tag[%ld] = %s ", static_cast<long>(t),
				shapes[i].mesh.tags[t].name.c_str());
			printf(" ints: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
				printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
				if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");

			printf(" floats: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
				printf("%f", static_cast<const double>(
					shapes[i].mesh.tags[t].floatValues[j]));
				if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");

			printf(" strings: [");
			for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
				printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
				if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
					printf(", ");
				}
			}
			printf("]");
			printf("\n");
		}
	}

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%ld].name = %s\n", static_cast<long>(i),
			materials[i].name.c_str());
		printf("  material.Ka = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].ambient[0]),
			static_cast<const double>(materials[i].ambient[1]),
			static_cast<const double>(materials[i].ambient[2]));
		printf("  material.Kd = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].diffuse[0]),
			static_cast<const double>(materials[i].diffuse[1]),
			static_cast<const double>(materials[i].diffuse[2]));
		printf("  material.Ks = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].specular[0]),
			static_cast<const double>(materials[i].specular[1]),
			static_cast<const double>(materials[i].specular[2]));
		printf("  material.Tr = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].transmittance[0]),
			static_cast<const double>(materials[i].transmittance[1]),
			static_cast<const double>(materials[i].transmittance[2]));
		printf("  material.Ke = (%f, %f ,%f)\n",
			static_cast<const double>(materials[i].emission[0]),
			static_cast<const double>(materials[i].emission[1]),
			static_cast<const double>(materials[i].emission[2]));
		printf("  material.Ns = %f\n",
			static_cast<const double>(materials[i].shininess));
		printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
		printf("  material.dissolve = %f\n",
			static_cast<const double>(materials[i].dissolve));
		printf("  material.illum = %d\n", materials[i].illum);
		printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		printf("  material.map_Ns = %s\n",
			materials[i].specular_highlight_texname.c_str());
		printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
		printf("    bump_multiplier = %f\n", static_cast<const double>(materials[i].bump_texopt.bump_multiplier));
		printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
		printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
		printf("  <<PBR>>\n");
		printf("  material.Pr     = %f\n", static_cast<const double>(materials[i].roughness));
		printf("  material.Pm     = %f\n", static_cast<const double>(materials[i].metallic));
		printf("  material.Ps     = %f\n", static_cast<const double>(materials[i].sheen));
		printf("  material.Pc     = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
		printf("  material.Pcr    = %f\n", static_cast<const double>(materials[i].clearcoat_thickness));
		printf("  material.aniso  = %f\n", static_cast<const double>(materials[i].anisotropy));
		printf("  material.anisor = %f\n", static_cast<const double>(materials[i].anisotropy_rotation));
		printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
		printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
		printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
		printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
		printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
		std::map<std::string, std::string>::const_iterator it(
			materials[i].unknown_parameter.begin());
		std::map<std::string, std::string>::const_iterator itEnd(
			materials[i].unknown_parameter.end());

		for (; it != itEnd; it++) {
			printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		}
		printf("\n");
	}
}

// 载入Mesh和材质
bool Object::make_mesh_and_material_by_obj(const char* filename, const char* basepath,bool triangulate){

	std::cout << "Loading " << filename << std::endl;

	tinyobj::attrib_t attrib; // 所有的数据放在这里
	std::vector<tinyobj::shape_t> shapes; 
	// 一个shape,表示一个部分,
	// 其中主要存的是索引坐标 mesh_t类,
	// 放在indices中
	/*
	// -1 means not used.
	typedef struct {
	  int vertex_index;
	  int normal_index;
	  int texcoord_index;
	} index_t;
	*/
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename,
		basepath, triangulate);

	// 接下里就是从上面的属性中取值了
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "ERR: " << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return false;
	}

	// ========================== 将读入的模型数据存入自己定义的数据结构中 ========================
	
	std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
	std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
	std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2)
		<< std::endl;

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	///1. 获取各种材质和纹理
	{
		for (int i = 0; i < materials.size(); i++) {
			Material* m = new Material();
			tinyobj::material_t tm = materials[i];
			string name = tm.name;
			if (name.size()) {
				m->name = name;
			}
			m->ambient.r = tm.ambient[0];
			m->ambient.g = tm.ambient[1];
			m->ambient.b = tm.ambient[2];

			m->diffuse.r = tm.diffuse[0];
			m->diffuse.g = tm.diffuse[1];
			m->diffuse.b = tm.diffuse[2];

			m->specular.r = tm.specular[0];
			m->specular.g = tm.specular[1];
			m->specular.b = tm.specular[2];

			m->transmittance.r = tm.transmittance[0];
			m->transmittance.g = tm.transmittance[1];
			m->transmittance.b = tm.transmittance[2];

			m->emission.r = tm.emission[0];
			m->emission.g = tm.emission[1];
			m->emission.b = tm.emission[2];

			m->shininess = tm.shininess;
			m->ior = tm.ior;
			m->dissolve = tm.dissolve;
			m->illum = tm.illum;
			m->pad0 = tm.pad0;

			m->ambient_tex_id = -1;
			m->diffuse_tex_id = -1;
			m->specular_tex_id = -1;
			m->specular_highlight_tex_id = -1;
			m->bump_tex_id = -1;
			m->displacement_tex_id = -1;
			m->alpha_tex_id = -1;

			m->ambient_texname = "";
			m->diffuse_texname = "";
			m->specular_texname = "";
			m->specular_highlight_texname = "";
			m->bump_texname = "";
			m->displacement_texname = "";
			m->alpha_texname = "";

			if (tm.ambient_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.ambient_texname)) 					{
					m->ambient_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm.diffuse_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.diffuse_texname)) {
					m->diffuse_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm.specular_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.specular_texname)) 					{
					m->specular_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm.specular_highlight_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.specular_highlight_texname)) {
					m->specular_highlight_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm.bump_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.bump_texname)) {
					m->bump_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm.displacement_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.displacement_texname)) {
					m->displacement_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}
			if (tm.alpha_texname.size()) {
				Texture* tmp = new Texture();
				if (tmp->make_texture_by_stb(tm.alpha_texname)) {
					m->alpha_tex_id = textures.size();
					this->textures.push_back(tmp);
				}
			}

			this->materials.push_back(m);
		}


	}

	/// 2.顶点数据
	{
		// For each shape 遍历每一个部分
		for (size_t i = 0; i < shapes.size(); i++) {

			if (i == 2) {
				int debug = 1;
			}

			// 这部分的名称
			printf("shape[%ld].name = %s\n", static_cast<long>(i),
				shapes[i].name.c_str());
			// 网格的点数
			printf("Size of shape[%ld].mesh.indices: %lu\n", static_cast<long>(i),
				static_cast<unsigned long>(shapes[i].mesh.indices.size()));
			//printf("Size of shape[%ld].path.indices: %lu\n", static_cast<long>(i),static_cast<unsigned long>(shapes[i].path.indices.size()));

			//assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.material_ids.size());
			//assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.smoothing_group_ids.size());

			printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
				static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

			Model* model = new Model(); // 每一部分的模型数据
			// 顶点数量  = face的数量x3 
			model->mesh_num = shapes[i].mesh.num_face_vertices.size() * 3; 
			// 开辟空间
			Vertex *mesh_data = new Vertex[model->mesh_num];
			size_t index_offset = 0;

			// For each face
			for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
				size_t fnum = shapes[i].mesh.num_face_vertices[f];

				// 获得所索引下标
				tinyobj::index_t idx;
				int vertex_index[3];
				int normal_index[3];
				int texcoord_index[3];
				for (size_t v = 0; v < fnum; v++) {
					idx = shapes[i].mesh.indices[index_offset + v];
					vertex_index[v] = idx.vertex_index;
					texcoord_index[v] = idx.texcoord_index;
					normal_index[v] = idx.normal_index;
				}
				for (size_t v = 0; v < fnum; v++) {
					// v
					mesh_data[index_offset + v].pos.x = attrib.vertices[(vertex_index[v]) * 3 + 0];
					mesh_data[index_offset + v].pos.y = attrib.vertices[(vertex_index[v]) * 3 + 1];
					mesh_data[index_offset + v].pos.z = attrib.vertices[(vertex_index[v]) * 3 + 2];
					mesh_data[index_offset + v].pos.w = 1.0f;

					// vt
					mesh_data[index_offset + v].tc.u = attrib.texcoords[texcoord_index[v] * 2 + 0];
					mesh_data[index_offset + v].tc.v = attrib.texcoords[texcoord_index[v] * 2 + 1];

					// vn
					mesh_data[index_offset + v].normal.x = attrib.normals[normal_index[v] * 3 + 0];
					mesh_data[index_offset + v].normal.y = attrib.normals[normal_index[v] * 3 + 1];
					mesh_data[index_offset + v].normal.z = attrib.normals[normal_index[v] * 3 + 2];
					mesh_data[index_offset + v].normal.w = 1.0f;

					// color
					mesh_data[index_offset + v].color.r = 1.0f;
					mesh_data[index_offset + v].color.g = 1.0f;
					mesh_data[index_offset + v].color.b = 1.0f;
					mesh_data[index_offset + v].color.a = 1.0f;
				}
			
				// 偏移
				index_offset += fnum;
			}
			model->mesh = mesh_data;
			models.push_back(model);
		}
	}

	std::cout << "# Loading Complete #"<< std::endl;
	//PrintInfo(attrib, shapes, materials);
	return true;
}
























