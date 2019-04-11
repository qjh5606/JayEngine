#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"
#include <algorithm>
using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define  STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define M_PI (3.141592683)
const int width = 1024;
const int height = 768;

std::vector<Vec3f> framebuffer;
unsigned char* output;

const float fov = M_PI / 2; // 视场角


// 点光源
struct Light {
	Light(const Vec3f& p, const float &i) :
		position(p), intensity(i) {
	}
	Vec3f position;
	float intensity;
};




struct Material {

	Material() : refractive_index(1),albedo(1,0, 0,0), diffuse_color(), specular_exponent() {}

	Material(const float &r, const Vec4f &a, const Vec3f &color, const float &spec) :
		refractive_index(r),albedo(a), 
		diffuse_color(color), specular_exponent(spec) {}
	
	float refractive_index; 
	// [0]:漫反射 [1]: 高光反射
	// [2]:反射   [3]: 折射
	Vec4f albedo;
	Vec3f diffuse_color;
	float specular_exponent;
};

// 球形
struct Sphere{
	Vec3f center;
	float radius;

	Material material;

	Sphere(const Vec3f &c, const float &r) :center(c), radius(r) {}

	Sphere(const Vec3f &c, const float &r,const Material &m) :
		center(c), radius(r),material(m) {}

	//http://www.lighthouse3d.com/tutorials/maths/ray-sphere-intersection/
	// 判断是否射线是否与这个求相交
	// orig 射线源点,
	// dir 射线的单位方向
	// t0 返回交点在orig的dir方向上多长
    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
        Vec3f L = center - orig;
        float tca = L*dir;
        float d2 = L*L - tca*tca;
        if (d2 > radius*radius) return false;
        float thc = sqrtf(radius*radius - d2);
        t0       = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }
};

bool scene_intersect(const Vec3f &orig, const Vec3f &dir,const  std::vector<Sphere> &spheres,Vec3f &hit,Vec3f &N,Material &material) {
	float spheres_dist = std::numeric_limits<float>::max();
	for (int i = 0; i < spheres.size(); i++) {
		float dist_i;
		// 若相交就显示球的材质颜色.
		// 取最近
		if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
			spheres_dist = dist_i;
			hit = orig + dir*dist_i;
			// 球心指向交点
			N = (hit - spheres[i].center).normalize();
			material = spheres[i].material;
		}
	}

	// 棋盘格
	float checkerboard_dist = std::numeric_limits<float>::max();
	if (fabs(dir.y) > 1e-3) {
		// the checkerboard plane has equation y = -4
		float d = -(orig.y + 4) / dir.y; 
		Vec3f pt = orig + dir*d;
		if (d > 0 && fabs(pt.x) < 10 && pt.z<-10 && pt.z>-30 && d < spheres_dist) {
			checkerboard_dist = d;
			hit = pt;
			N = Vec3f(0, 1, 0);
			material.diffuse_color = (int(.5*hit.x + 1000) + int(.5*hit.z)) & 1 ? Vec3f(1, 1, 1) : Vec3f(1, .7, .3);
			material.diffuse_color = material.diffuse_color*.3;
		}
	}

	return std::min(spheres_dist, checkerboard_dist)<1000;
}

/*
		I  N  reflect
		\  |  /
	     \ | /
		  \|/
	----------------------
	I向下.N向上,refect向上
*/
Vec3f reflect(const Vec3f& I, const Vec3f& N) {
	return I - N * 2.0 * (I*N);
}

// Snell's law
// https://blog.csdn.net/yinhun2012/article/details/79472364
Vec3f refract(const Vec3f& I, const Vec3f& N,float &refractive_index) {
	
	float cosi = -std::max(-1.0f, std::min(1.f, I*N));
	float etai = 1, etat = refractive_index;
	Vec3f n = N;
	if (cosi < 0) {
		// if the ray is inside the object, 
		// swap the indices and invert the normal to get the correct result
		cosi = -cosi;
		std::swap(etai, etat); n = -N;
	}
	float eta = etai / etat;
	float k = 1 - eta*eta*(1 - cosi*cosi);
	return k < 0 ? Vec3f(0, 0, 0) : I*eta + n*(eta * cosi - sqrtf(k));
}

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, 
	const std::vector<Sphere> &spheres,
	const std::vector<Light> lights,
	size_t depth = 0/*光线追踪的深度 */)
{
	Vec3f point, N;
	Material material;

	// 找到是否存在交点
	if (depth>4||!scene_intersect(orig, dir, spheres, point, N, material)) {
		return Vec3f(0.2, 0.7, 0.8); // background color
	}
	
	// 求dir的反射光
	Vec3f reflect_dir = reflect(dir, N).normalize();
	// offset the original point to avoid occlusion by the object itself
	Vec3f reflect_orig = reflect_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; 
	Vec3f reflect_color = cast_ray(reflect_orig, reflect_dir, spheres, lights, depth + 1);

	// 求dir的折射光
	Vec3f refract_dir = refract(dir, N,material.refractive_index).normalize();
	Vec3f refract_orig = refract_dir*N < 0 ? point - N*1e-3 : point + N*1e-3;
	Vec3f refract_color = cast_ray(refract_orig, refract_dir, spheres, lights, depth + 1);

	// 应用光照
	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for (size_t i = 0; i < lights.size(); i++) {
		// 光源的方向:交点指向光源
		Vec3f light_dir = (lights[i].position - point).normalize(); 
		float light_distance = light_dir.norm();  // 距离

		// 判断是否 light[i]这个光线找不到point
		Vec3f shdow_orig = light_dir*N<0 ? point - N*1e-3 : point + N*1e-3; 
		Vec3f shadow_pt, shadow_N;
		Material tempmaterial;
		// 就是说从这个点出发,看看能不能找到与它相交的sphere,如果可以,那么就是处于阴影.lights[i]找不到
		if (scene_intersect(shdow_orig, light_dir, spheres, shadow_pt, shadow_N, tempmaterial))
			continue;

		diffuse_light_intensity += lights[i].intensity * std::max(0.f, light_dir*N);
		specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, N)*dir), material.specular_exponent)
			*lights[i].intensity;
	}

	// 环境光+漫发射+高光反射+反射光
	return material.diffuse_color*diffuse_light_intensity * material.albedo[0]
		+ Vec3f(1., 1., 1.)*specular_light_intensity * material.albedo[1]
		+ reflect_color*material.albedo[2]
		+ refract_color*material.albedo[3];
}

void saveImg() {
	for (size_t i = 0; i < height*width; ++i) {
		for (size_t j = 0; j < 3; j++) {
			output[i * 3 + j] = (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}
	stbi_write_bmp("output.bmp", width, height, 3, &output[0]);
}

void render(const std::vector<Sphere> &spheres, const std::vector<Light> lights) {
	//int screen_camera_distance = 1;
	//float x_leftup = 0;
	//float y_rightbottom = 0;
	float aspect = width / (float)height; // 防止失真
	for (size_t j = 0; j < height; j++) {
		for (size_t i = 0; i < width; i++) {
			// 将像素位置转换成为真实世界坐标位置
			/*
				(i+0.5) 像素偏移量
				+(i+0.5)/(width/2.0)* tan(fov / 2.)*aspect 世界坐标系偏移量
				起始世界坐标 - 1*tan(fov / 2.)*aspect
			*/
			float x = (2* (i + 0.5) / (float)width - 1)*tan(fov / 2.)*aspect;
			float y = -(2 * (j + 0.5) / (float)height - 1)*tan(fov / 2.);
			// 发出的光线方向
			Vec3f dir = Vec3f(x, y, -1).normalize(); 
			framebuffer[i + j*width] = cast_ray(Vec3f(0, 0, 0), dir, spheres,lights);
		}
	}
	// 保存成为图片
	saveImg();
}

int main() {
	framebuffer = std::vector<Vec3f>(width*height);
	output = new unsigned char[width*height * 3];
	
	std::vector<Light>  lights;
	lights.push_back(Light(Vec3f(-20, 20, 20), 1.5));
	lights.push_back(Light(Vec3f(30, 50, -25), 1.8));
	lights.push_back(Light(Vec3f(30, 20, 30), 1.7));

	Material      ivory(1.0f,Vec4f(0.6, 0.3,0.0,0.0), Vec3f(0.4, 0.4, 0.3), 50.);
	Material      glass(1.5f, Vec4f(0.0, 0.5, 0.1, 0.8), Vec3f(0.6, 0.7, 0.8), 125.);
	Material red_rubber(1.0f,Vec4f(0.9, 0.1,0.0,0.0), Vec3f(0.3, 0.1, 0.1), 10.);
	Material	 mirror(1.0f,Vec4f(0.0, 10.0, 0.8,0.0), Vec3f(1.0, 1.0, 1.0), 1425.);

	std::vector<Sphere> spheres;
	spheres.push_back(Sphere(Vec3f(-3, 0, -16), 2, ivory));
	spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, glass));
	spheres.push_back(Sphere(Vec3f(1.5, -0.5, -18), 3, red_rubber));
	spheres.push_back(Sphere(Vec3f(7, 5, -18), 4, mirror));

	render(spheres,lights);

	return 0;
}