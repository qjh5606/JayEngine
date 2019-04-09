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
	��Ⱦ������
*/
typedef unsigned int IUINT32;

#define RENDER_STATE_WIREFRAME      1		// ��Ⱦ�߿�
#define RENDER_STATE_TEXTURE        2		// ��Ⱦ����
#define RENDER_STATE_COLOR          4		// ��Ⱦ��ɫ

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

	// �����������
	void setCurCamera(Camera* camera);

	// ����֡����ָ��
	void setFrameBuffer(IUINT32* framebuffer) {
		this->framebuffer = framebuffer;
	}

	// ����Z����ָ��
	void setZBuffer(float* zbuffer) {
		this->zbuffer = zbuffer;
	}

	// ������Ӱָ��
	void setShadowBuffer(float* shadowbuffer) {
		this->shadowbuffer = shadowbuffer;
	}

	// ����Ƿ��й�Դ��Ҫ������Ӱ
	//void checkLights();

	// ����ͼԪ
	void device_draw_primitive(Vertex* t1, Vertex* t2, Vertex* t3);

	// �ü������
	void clip_polys(Vertex* t1,Vertex* t2,Vertex* t3, bool world);

	// �Ե㼯�Ͻ�����ʱ������
	void sortVertexs(std::vector<Vertex> &VertexSet, int size);

	void clip_lines(Vertex t1, Vertex t2,std::vector<Vertex> &VertexSet);

	// ����
	void Draw();

	// ����Ⱦ����

	void device_render_trap(Trapezoid *trap);
	void device_render_trap(Trapezoid *trap, Point *points, V2F* v2f);

	// ��ʼ��ɨ����
	void  trapezoid_init_scan_line(const Trapezoid *trap, Scanline *scanline, int y);

	void device_draw_scanline(Scanline* scanline);
	void device_draw_scanline(Scanline* scanline, Point* points, V2F* v2f);
		
	// �ֽ�������
	int trapezoid_init_triangle(Trapezoid *trap, const Vertex *p1, const Vertex *p2, const Vertex *p3);

	// �����ֵ
	void vertex_interp(Vertex *y, const Vertex *x1, const Vertex *x2, float k);

	// ����ӷ�
	void vertex_add(Vertex *y, const Vertex *x);

	// �������
	void vertex_division(Vertex *y, const Vertex *x1, const Vertex *x2, float w);

	// ��Ե��ֵ
	void trapezoid_edge_interp(Trapezoid *trap, float y);

	// �����Ȩϵ��
	bool computeBarycentricCoords3d(Point *res, const Point *p0, const Point *p1, const Point *p2, const Point *p);

	// 
	void v2f_interpolating(V2F *dest, const V2F *src1, const V2F *src2, const V2F *src3, float a, float b, float c);

	// ���� 
	void draw_line(int x1, int y1, int x2, int y2, IUINT32 c);
	// ����
	void device_pixel(int x, int y, IUINT32 color);

	// ��������
	void clear();

	int render_state;				// ��Ⱦģʽ 
	
	int width, height;				// �ֱ���

	vector<Camera*> cameras;		// ��Ӱ���
	Camera *mainCamera;				// �����
	Camera *curCamera;				// Ŀǰ����ʹ�õ����

	vector<Object*> objects;		// ����	(ģ�� ���� ����)
	Object* curObeject;

	Transform transform;			// ��ȡ����model���������camera����Transform

	// ��Դ
	vector<DirLight*> dirLights;
	vector<PointLight*> pointLights;

	Shader shader;					// ��ɫ������
	
	// ������ɫ��
	void vert_shader(A2V *av, V2F *vf);
	// Ƭ����ɫ��
	void frag_shader(V2F *vf,Color *color);


	// ====================== �ü����޳� ======================= //
	int cull;						// 0:���ü�;1:�ü�����;2:�ü�����

	// ======================= ����Buffer ======================= // 
	float *zbuffer;					// ��Ȼ���
	float *shadowbuffer;			// ��Ӱ���� 
	IUINT32 *framebuffer;			// ֡����
	IUINT32 background;				// ������ɫ
	IUINT32 foreground;				// ǰ����ɫ
	
	void *fb;						// �ⲿ֡����

	// ======================= Blend ======================= // 
	bool blend;						// �Ƿ������
	float blend_sfactor;			// ���Դ����
	float blend_dfactor;			// ���Ŀ������

};

#endif // _JAYPIPELINE_H_