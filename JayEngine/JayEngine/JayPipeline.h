#ifndef _JAYPIPELINE_H_
#define _JAYPIPELINE_H_
#include "JayHeader.h"

#include "JayMath3D.h"
#include "JayCamera.h"
#include "JayShader.h"
#include "JayLight.h"
#include "JayObject.h"
#include "JayTransform.h"

/*
	渲染管线类
*/
typedef unsigned int IUINT32;

#define RENDER_STATE_WIREFRAME      1		// 渲染线框
#define RENDER_STATE_TEXTURE        2		// 渲染纹理
#define RENDER_STATE_COLOR          4		// 渲染颜色

class Pipeline {
public:

	// control 
	bool CLIP2D = false;
	bool CULLBACK = true;

	Pipeline() {
	}

	Pipeline(int width,int height,void *fb) {
		this->width = width;
		this->height = height;
		this->fb = fb;
		if (fb != NULL) {
			this->framebuffer = (IUINT32 *)fb;
		}
		
	//	this->background = 0xFFFFFF;
		this->background = 0x55555555;
		this->foreground = 0;
		this->render_state = RENDER_STATE_WIREFRAME;

		this->transform.setResolution(width, height);
	}

	~Pipeline() {

	}

	// 设置主摄像机
	void setCurCamera(Camera* camera);

	// 设置帧缓冲指针
	void setFrameBuffer(IUINT32* framebuffer) {
		this->framebuffer = framebuffer;
	}

	// 设置Z缓冲指针
	void setZBuffer(float* zbuffer) {
		this->zbuffer = zbuffer;
	}

	// 设置阴影指针
	void setShadowBuffer(float* shadowbuffer) {
		this->shadowbuffer = shadowbuffer;
	}

	// 检查是否有光源需要产生阴影
	//void checkLights();

	// 绘制图元
	void device_draw_primitive(Vertex* t1, Vertex* t2, Vertex* t3);

	// 裁剪多边形
	void clip_polys(Vertex* t1,Vertex* t2,Vertex* t3, bool world);

	// 对点集合进行逆时针排序
	void sortVertexs(std::vector<Vertex> &VertexSet, int size);

	void clip_lines(Vertex t1, Vertex t2,std::vector<Vertex> &VertexSet);

	// 绘制
	void Draw();

	// 主渲染函数

	void device_render_trap(Trapezoid *trap);
	void device_render_trap(Trapezoid *trap, Point *points, V2F* v2f);

	// 初始化扫描线
	void  trapezoid_init_scan_line(const Trapezoid *trap, Scanline *scanline, int y);

	void device_draw_scanline(Scanline* scanline);
	void device_draw_scanline(Scanline* scanline, Point* points, V2F* v2f);
		
	// 分解三角形
	int trapezoid_init_triangle(Trapezoid *trap, const Vertex *p1, const Vertex *p2, const Vertex *p3);

	// 顶点插值
	void vertex_interp(Vertex *y, const Vertex *x1, const Vertex *x2, float k);

	// 顶点加法
	void vertex_add(Vertex *y, const Vertex *x);

	// 顶点除法
	void vertex_division(Vertex *y, const Vertex *x1, const Vertex *x2, float w);

	// 边缘插值
	void trapezoid_edge_interp(Trapezoid *trap, float y);

	// 计算加权系数
	bool computeBarycentricCoords3d(Point *res, const Point *p0, const Point *p1, const Point *p2, const Point *p);

	// 
	void v2f_interpolating(V2F *dest, const V2F *src1, const V2F *src2, const V2F *src3, float a, float b, float c);

	// 画线 
	void draw_line(int x1, int y1, int x2, int y2, IUINT32 c);
	// 画点
	void device_pixel(int x, int y, IUINT32 color);

	// 清理缓存区
	void clear();

	int render_state;				// 渲染模式 
	
	int width, height;				// 分辨率

	vector<Camera*> cameras;		// 阴影相机
	Camera *mainCamera;				// 主相机
	Camera *curCamera;				// 目前正在使用的相机

	vector<Object*> objects;		// 物体	(模型 材质 纹理)
	Object* curObeject;

	Transform transform;			// 获取物体model和摄像机的camera进行Transform

	// 光源
	vector<DirLight*> dirLights;
	vector<PointLight*> pointLights;

	Shader shader;					// 着色器数据
	
	// 顶点着色器
	void vert_shader(A2V *av, V2F *vf);
	// 片段着色器
	void frag_shader(V2F *vf,Color *color);


	// ====================== 裁剪与剔除 ======================= //
	int cull;						// 0:不裁剪;1:裁剪反面;2:裁剪正面

	// ======================= 缓冲Buffer ======================= // 
	float *zbuffer;					// 深度缓存
	float *shadowbuffer;			// 阴影缓存 
	IUINT32 *framebuffer;			// 帧缓冲
	IUINT32 background;				// 背景颜色
	IUINT32 foreground;				// 前景颜色
	
	void *fb;						// 外部帧缓冲

	// ======================= Blend ======================= // 
	bool blend;						// 是否开启混合
	float blend_sfactor;			// 混合源因子
	float blend_dfactor;			// 混合目标因子

};

#endif // _JAYPIPELINE_H_