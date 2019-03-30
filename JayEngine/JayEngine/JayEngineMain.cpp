#include <iostream>
#include <fstream> 
#include <sstream> // istringstream 必须包含这个头文件
#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <deque>
#include <stack>
#include <bitset>
#include <utility> // pair
#include <functional> //greater<>
#include <mutex>
#include <cmath>
#include <ctime>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <exception>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <tchar.h>

using namespace std;

// 类型名
typedef unsigned int IUINT32;

#include <windows.h> // windows窗口
//=====================================================================
// Win32 窗口及图形绘制：为 device 提供一个 DibSection 的 FB
//=====================================================================
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];	// 当前键盘按下状态
static HWND screen_handle = NULL;		// 主窗口 HWND
static HDC screen_dc = NULL;			// 配套的 HDC
static HBITMAP screen_hb = NULL;		// DIB
static HBITMAP screen_ob = NULL;		// 老的 BITMAP
unsigned char *screen_fb = NULL;		// frame buffer 往屏幕里写数据
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);	// 屏幕初始化
int screen_close(void);								// 关闭屏幕
void screen_dispatch(void);							// 处理消息
void screen_update(void);							// 显示 FrameBuffer

// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif

// 初始化窗口并设置标题
int screen_init(int w, int h, const TCHAR *title) {
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
		NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;

	screen_close();

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc)) return -1;

	screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if (screen_handle == NULL) return -2;

	screen_exit = 0;
	hDC = GetDC(screen_handle);
	screen_dc = CreateCompatibleDC(hDC);
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if (screen_hb == NULL) return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
	screen_fb = (unsigned char*)ptr;
	screen_w = w;
	screen_h = h;
	screen_pitch = w * 4;

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(screen_handle);

	ShowWindow(screen_handle, SW_NORMAL);
	screen_dispatch();

	memset(screen_keys, 0, sizeof(int) * 512);
	// 空间w*h*4
	memset(screen_fb, 0, w * h * 4);

	return 0;
}

int screen_close(void) {
	if (screen_dc) {
		if (screen_ob) {
			SelectObject(screen_dc, screen_ob);
			screen_ob = NULL;
		}
		DeleteDC(screen_dc);
		screen_dc = NULL;
	}
	if (screen_hb) {
		DeleteObject(screen_hb);
		screen_hb = NULL;
	}
	if (screen_handle) {
		CloseWindow(screen_handle);
		screen_handle = NULL;
	}
	return 0;
}

static LRESULT screen_events(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE: screen_exit = 1; break;
	case WM_KEYDOWN: screen_keys[wParam & 511] = 1; break;
	case WM_KEYUP: screen_keys[wParam & 511] = 0; break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void screen_dispatch(void) {
	MSG msg;
	while (1) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, NULL, 0, 0)) break;
		DispatchMessage(&msg);
	}
}

void screen_update(void) {
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	screen_dispatch();
}


//=====================================================================
// 数学库：此部分应该不用详解，熟悉 D3D 矩阵变换即可
//=====================================================================
class matrix_t {
public:
	float m[4][4];
};

class vector_t {
public:
	float x, y, z, w;
};
typedef vector_t point_t;

int CMID(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }

// 计算插值：t 为 [0, 1] 之间的数值
float interp(float x1, float x2, float t) { return x1 + (x2 - x1) * t; }

// | v |
float vector_length(const vector_t *v) {
	float sq = v->x * v->x + v->y * v->y + v->z * v->z;
	return (float)sqrt(sq);
}

// z = x + y
void vector_add(vector_t *z, const vector_t *x, const vector_t *y) {
	z->x = x->x + y->x;
	z->y = x->y + y->y;
	z->z = x->z + y->z;
	z->w = 1.0;
}

// z = x - y
void vector_sub(vector_t *z, const vector_t *x, const vector_t *y) {
	z->x = x->x - y->x;
	z->y = x->y - y->y;
	z->z = x->z - y->z;
	z->w = 1.0;
}

// 矢量点乘
float vector_dotproduct(const vector_t *x, const vector_t *y) {
	return x->x * y->x + x->y * y->y + x->z * y->z;
}

// 矢量叉乘
void vector_crossproduct(vector_t *z, const vector_t *x, const vector_t *y) {
	float m1, m2, m3;
	m1 = x->y * y->z - x->z * y->y;
	m2 = x->z * y->x - x->x * y->z;
	m3 = x->x * y->y - x->y * y->x;
	z->x = m1;
	z->y = m2;
	z->z = m3;
	z->w = 1.0f;
}

// 矢量插值，t取值 [0, 1]
void vector_interp(vector_t *z, const vector_t *x1, const vector_t *x2, float t) {
	z->x = interp(x1->x, x2->x, t);
	z->y = interp(x1->y, x2->y, t);
	z->z = interp(x1->z, x2->z, t);
	z->w = 1.0f;
}

// 矢量归一化
void vector_normalize(vector_t *v) {
	float length = vector_length(v);
	if (length != 0.0f) {
		float inv = 1.0f / length;
		v->x *= inv;
		v->y *= inv;
		v->z *= inv;
	}
}

// c = a + b
void matrix_add(matrix_t *c, const matrix_t *a, const matrix_t *b) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] + b->m[i][j];
	}
}

// c = a - b
void matrix_sub(matrix_t *c, const matrix_t *a, const matrix_t *b) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] - b->m[i][j];
	}
}

// c = a * b
void matrix_mul(matrix_t *c, const matrix_t *a, const matrix_t *b) {
	matrix_t z;
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			z.m[j][i] = (a->m[j][0] * b->m[0][i]) +
				(a->m[j][1] * b->m[1][i]) +
				(a->m[j][2] * b->m[2][i]) +
				(a->m[j][3] * b->m[3][i]);
		}
	}
	c[0] = z;
}

// c = a * f
void matrix_scale(matrix_t *c, const matrix_t *a, float f) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			c->m[i][j] = a->m[i][j] * f;
	}
}

// y = x * m
void matrix_apply(vector_t *y, const vector_t *x, const matrix_t *m) {
	float X = x->x, Y = x->y, Z = x->z, W = x->w;
	y->x = X * m->m[0][0] + Y * m->m[1][0] + Z * m->m[2][0] + W * m->m[3][0];
	y->y = X * m->m[0][1] + Y * m->m[1][1] + Z * m->m[2][1] + W * m->m[3][1];
	y->z = X * m->m[0][2] + Y * m->m[1][2] + Z * m->m[2][2] + W * m->m[3][2];
	y->w = X * m->m[0][3] + Y * m->m[1][3] + Z * m->m[2][3] + W * m->m[3][3];
}

// 单位矩阵
void matrix_set_identity(matrix_t *m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

// 零矩阵
void matrix_set_zero(matrix_t *m) {
	m->m[0][0] = m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][1] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][2] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = m->m[3][3] = 0.0f;
}

// D3DXMatrixPerspectiveFovLH
// https://blog.csdn.net/zzmkljd/article/details/52870807
void matrix_set_perspective(matrix_t *m, float fovy, float aspect, float zn, float zf) {
	float fax = 1.0f / (float)tan(fovy * 0.5f);
	matrix_set_zero(m);
	m->m[0][0] = (float)(fax / aspect);
	m->m[1][1] = (float)(fax);
	m->m[2][2] = zf / (zf - zn);
	m->m[3][2] = -zn * zf / (zf - zn);
	m->m[2][3] = 1;
}

// 设置摄像机
void matrix_set_lookat(matrix_t *m, const vector_t *eye, const vector_t *at, const vector_t *up) {
	vector_t xaxis, yaxis, zaxis;

	vector_sub(&zaxis, at, eye);
	vector_normalize(&zaxis);
	vector_crossproduct(&xaxis, up, &zaxis);
	vector_normalize(&xaxis);
	vector_crossproduct(&yaxis, &zaxis, &xaxis);

	m->m[0][0] = xaxis.x;
	m->m[1][0] = xaxis.y;
	m->m[2][0] = xaxis.z;
	m->m[3][0] = -vector_dotproduct(&xaxis, eye);

	m->m[0][1] = yaxis.x;
	m->m[1][1] = yaxis.y;
	m->m[2][1] = yaxis.z;
	m->m[3][1] = -vector_dotproduct(&yaxis, eye);

	m->m[0][2] = zaxis.x;
	m->m[1][2] = zaxis.y;
	m->m[2][2] = zaxis.z;
	m->m[3][2] = -vector_dotproduct(&zaxis, eye);

	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
	m->m[3][3] = 1.0f;
}

//=====================================================================
// 坐标变换
//=====================================================================
class transform_t {
public:
	// 初始化，设置屏幕长宽
	void transform_init(int width, int height) {
		float aspect = (float)width / ((float)height); // 宽广比
		matrix_set_identity(&this->world);
		matrix_set_identity(&this->view);
		// 投影矩阵的设置 projection 相机坐标系->齐次裁剪空间
		matrix_set_perspective(&this->projection, 3.1415926f * 0.5f, aspect, 1.0f, 500.0f);
		this->w = width;
		this->h = height;
		transform_update();
	}

	// https://blog.csdn.net/you_lan_hai/article/details/37992123 D3D和OpenGL坐标系
	// 矩阵更新，计算 transform = world * view * projection
	void transform_update() {
		matrix_t m;
		matrix_mul(&m, &this->world, &this->view);
		matrix_mul(&this->transform, &m, &this->projection);
	}

	// 引用变换矩阵
	void transform_apply(point_t* y, point_t* x) {
		matrix_apply(y, x, &this->transform);
	}

	// 检查齐次坐标同 cvv 的边界用于视锥裁剪
	int transform_check_cvv(const vector_t *v) {
		float w = v->w;
		int check = 0;
		if (v->z < 0.0f) check |= 1;
		if (v->z > w) check |= 2;
		if (v->x < -w) check |= 4;
		if (v->x > w) check |= 8;
		if (v->y < -w) check |= 16;
		if (v->y > w) check |= 32;
		return check;
	}

	// 归一化，得到屏幕坐标
	void transform_homogenize(vector_t *y, const vector_t *x) {
		// 透视除法+视口变换
		float rhw = 1.0f / x->w;
		y->x = (x->x * rhw + 1.0f) * w * 0.5f;
		y->y = (1.0f - x->y * rhw) * h * 0.5f;
		y->z = x->z * rhw;
		y->w = 1.0f;
	}

	matrix_t world;			// world矩阵 model->world
	matrix_t view;			// view矩阵  world->view
	matrix_t projection;	// 投射变换 view->裁剪空间
	matrix_t transform;		// transform = world * view * projection
	float w, h;				// 屏幕大小
};

//=====================================================================
// 几何计算：顶点、扫描线、边缘、矩形、步长计算
//=====================================================================
typedef struct { float r, g, b; } color_t;
typedef struct { float u, v; } texcoord_t;
// 顶点坐标,纹理坐标,颜色值,
typedef struct { point_t pos; texcoord_t tc; color_t color; float rhw; } vertex_t;

typedef struct { vertex_t v, v1, v2; } edge_t; // v是插值的结果,v1起点,v2终点
// 矩形
typedef struct { float top, bottom; edge_t left, right; } trapezoid_t;
// 扫描线
typedef struct { vertex_t v, step; int x, y, w; } scanline_t;

// 顶点插值
void vertex_interp(vertex_t *y, const vertex_t* x1, const vertex_t* x2, float t) {
	vector_interp(&y->pos, &x1->pos, &x2->pos, t);
	y->tc.u = interp(x1->tc.u, x2->tc.u, t);
	y->tc.v = interp(x1->tc.v, x2->tc.v, t);
	y->color.r = interp(x1->color.r, x2->color.r, t);
	y->color.g = interp(x1->color.g, x2->color.g, t);
	y->color.b = interp(x1->color.b, x2->color.b, t);
	y->rhw = interp(x1->rhw, x2->rhw, t);
}

// 计算step,位移的每一步分量
void vertex_division(vertex_t *y, const vertex_t *x1, const vertex_t *x2, float w) {
	float inv = 1.0f / w;
	y->pos.x = (x2->pos.x - x1->pos.x) * inv;
	y->pos.y = (x2->pos.y - x1->pos.y) * inv;
	y->pos.z = (x2->pos.z - x1->pos.z) * inv;
	y->pos.w = (x2->pos.w - x1->pos.w) * inv;
	y->tc.u = (x2->tc.u - x1->tc.u) * inv;
	y->tc.v = (x2->tc.v - x1->tc.v) * inv;
	y->color.r = (x2->color.r - x1->color.r) * inv;
	y->color.g = (x2->color.g - x1->color.g) * inv;
	y->color.b = (x2->color.b - x1->color.b) * inv;
	y->rhw = (x2->rhw - x1->rhw) * inv;
}

void vertex_add(vertex_t *y, const vertex_t *x) {
	y->pos.x += x->pos.x;
	y->pos.y += x->pos.y;
	y->pos.z += x->pos.z;
	y->pos.w += x->pos.w;
	y->rhw += x->rhw;
	y->tc.u += x->tc.u;
	y->tc.v += x->tc.v;
	y->color.r += x->color.r;
	y->color.g += x->color.g;
	y->color.b += x->color.b;
}

//=====================================================================
// 渲染设备
//=====================================================================
#define RENDER_STATE_WIREFRAME      1		// 渲染线框
#define RENDER_STATE_TEXTURE        2		// 渲染纹理
#define RENDER_STATE_COLOR          4		// 渲染颜色

class device_t {
public:
	device_t() {
	}

	device_t(int width, int height, void *fb) {
		this->width = width;
		this->height = height;
		this->fb = fb;
	}

	// 设备初始化，fb为外部帧缓存，非 NULL 将引用外部帧缓存（每行 4字节对齐）
	void device_init() {
		// 开辟了一块大空间
		int need = sizeof(void*) * (height * 2 + 1024) + width * height * 8;
		char *ptr = (char*)malloc(need + 64);

		char *framebuf, *zbuf;
		int j;
		assert(ptr); // 空间开辟成功

		// 存储每个行 framebuffer帧缓冲 的指针
		this->framebuffer = (IUINT32 **)ptr;
		// 存储每一行 Z缓冲zbuffer 的指针
		this->zbuffer = (float**)(ptr + sizeof(void*) * height);

		// 存储每一行texture纹理的指针
		ptr += sizeof(void*) * height * 2;
		this->texture = (IUINT32**)ptr;

		ptr += sizeof(void*) * 1024; // 为了对齐

		// 帧缓冲的区间大小 width * height * 4(RGBA)
		framebuf = (char*)ptr;  // framebuf 指向帧缓冲内存
		// z缓冲实际内存大小 width * height * 4(float是4个字节)
		zbuf = (char*)ptr + width * height * 4; // zbuf 指向zbuf内存
		ptr += width * height * 8;

		if (fb != NULL) framebuf = (char*)fb; // 如果有外部的帧缓冲就用外部的,比如传递一块纹理内存

		// 每一行分别指向内存空间
		for (j = 0; j < height; j++) {
			this->framebuffer[j] = (IUINT32*)(framebuf + width * 4 * j);
			this->zbuffer[j] = (float*)(zbuf + width * 4 * j);
		}

		this->texture[0] = (IUINT32*)ptr;
		this->texture[1] = (IUINT32*)(ptr + 16);
		memset(this->texture[0], 0, 64);
		this->tex_width = 2;
		this->tex_height = 2;
		this->max_u = 1.0f;
		this->max_v = 1.0f;
		this->width = width;
		this->height = height;
		this->background = 0xc0c0c0;
		this->foreground = 0;
		this->transform.transform_init(width, height);
		// 绘制线框
		this->render_state = RENDER_STATE_WIREFRAME;
	}

	void device_init_mesh(vertex_t *mesh) {
		if (mesh != NULL)
			this->mesh = mesh;
	}

	// D3D使用的是左手坐标系 
	void device_set_camera(float x, float y, float z) {
		point_t eye = { x, y, z, 1 }, at = { 0, 0, 0, 1 }, up = { 0, 0, 1, 1 };
		// view矩阵的计算,世界坐标系到相机坐标系
		matrix_set_lookat(&this->transform.view, &eye, &at, &up);
		this->transform.transform_update();
	}

	// 初始化纹理
	void device_init_texture() {
		static IUINT32 texture[256][256];
		int i, j;
		for (j = 0; j < 256; j++) {
			for (i = 0; i < 256; i++) {
				int x = i / 32, y = j / 32;
				texture[j][i] = ((x + y) & 1) ? 0xffffff : 0x3fbcef; // 生成网格块纹理
			}
		}
		device_set_texture(texture, 256 * 4, 256, 256);
	}

	// 设置纹理
	void device_set_texture(void *bits, long pitch, int w, int h) {
		char *ptr = (char*)bits;
		int j;
		assert(w <= 1024 && h <= 1024);
		for (j = 0; j < h; ptr += pitch, j++) 	// 重新计算每行纹理的指针
			this->texture[j] = (IUINT32*)ptr;
		this->tex_width = w;
		this->tex_height = h;
		this->max_u = (float)(w - 1);
		this->max_v = (float)(h - 1);
	}

	// 清空 framebuffer 和 zbuffer
	void device_clear(int mode) {
		int y, x;
		for (y = 0; y < height; y++) {
			IUINT32 *dst = framebuffer[y];
			IUINT32 cc = (height - 1 - y) * 230 / (height - 1);
			cc = (cc << 16) | (cc << 8) | cc;
			if (mode == 0) cc = background;
			for (x = width; x > 0; dst++, x--) dst[0] = cc;
		}
		for (y = 0; y < height; y++) {
			float *dst = zbuffer[y];
			for (x = width; x > 0; dst++, x--) dst[0] = 0.0f;
		}
	}

	// 设置旋转矩阵
	// http://www.cnblogs.com/graphics/archive/2012/08/10/2627458.html
	void matrix_set_rotate(matrix_t *m, float x, float y, float z, float theta) {
		float qsin = (float)sin(theta * 0.5f);
		float qcos = (float)cos(theta * 0.5f);
		vector_t vec = { x, y, z, 1.0f };
		float w = qcos;
		vector_normalize(&vec);
		x = vec.x * qsin;
		y = vec.y * qsin;
		z = vec.z * qsin;
		m->m[0][0] = 1 - 2 * y * y - 2 * z * z;
		m->m[1][0] = 2 * x * y - 2 * w * z;
		m->m[2][0] = 2 * x * z + 2 * w * y;
		m->m[0][1] = 2 * x * y + 2 * w * z;
		m->m[1][1] = 1 - 2 * x * x - 2 * z * z;
		m->m[2][1] = 2 * y * z - 2 * w * x;
		m->m[0][2] = 2 * x * z - 2 * w * y;
		m->m[1][2] = 2 * y * z + 2 * w * x;
		m->m[2][2] = 1 - 2 * x * x - 2 * y * y;
		m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0f;
		m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
		m->m[3][3] = 1.0f;
	}

	// 核心函数画盒子
	void draw_box(float theta) {
		matrix_t m;
		matrix_set_rotate(&m, -1, -0.5, 1, theta); // 更新旋转矩阵
		this->transform.world = m;
		this->transform.transform_update(); // 更新transform矩阵
		
		// 画6个平面
		draw_plane(0, 1, 2, 3);
		draw_plane(4, 5, 6, 7);
		draw_plane(0, 4, 5, 1);
		draw_plane(1, 5, 6, 2);
		draw_plane(2, 6, 7, 3);
		draw_plane(3, 7, 4, 0);
	}

	// 画平面
	void draw_plane(int a, int b, int c, int d) {
		// 一个立方体平面由四个点构成
		vertex_t p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];
		// 设置纹理坐标
		p1.tc.u = 0, p1.tc.v = 0, p2.tc.u = 0, p2.tc.v = 1;
		p3.tc.u = 1, p3.tc.v = 1, p4.tc.u = 1, p4.tc.v = 0;
		// 画两个三角形
		device_draw_primitive(&p1, &p2, &p3);
		device_draw_primitive(&p3, &p4, &p1);
	}

	void vertex_rhw_init(vertex_t* v) {
		float rhw = 1.0f / v->pos.w;	// 1/w
		v->rhw = rhw;
		v->tc.u *= rhw;
		v->tc.v *= rhw;
		v->color.r *= rhw;
		v->color.g *= rhw;
		v->color.b *= rhw;
	}

	// 将三角形拆成 平三角形
	// https://blog.csdn.net/cppyin/article/details/6232453
	int trapezoid_init_triangle(trapezoid_t *trap, const vertex_t *p1,
		const vertex_t *p2, const vertex_t *p3) {

		const vertex_t *p;
		float k, x;

		// 排序一下 p1.y<p2.y<p3.y
		if (p1->pos.y > p2->pos.y) p = p1, p1 = p2, p2 = p;
		if (p1->pos.y > p3->pos.y) p = p1, p1 = p3, p3 = p;
		if (p2->pos.y > p3->pos.y) p = p2, p2 = p3, p3 = p;

		// 构不成三角形,返回0
		if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y) return 0;
		if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x) return 0;

		// 平顶,向下的三角形
		if (p1->pos.y == p2->pos.y) {
			// 保证 p1.pos.x<p2.pos.x
			if (p1->pos.x > p2->pos.x) p = p1, p1 = p2, p2 = p;
			trap[0].top = p1->pos.y; // 上面
			trap[0].bottom = p3->pos.y; // 底部
			// 三角形的左边 p1->p3
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p3;
			// 三角形的右边 p2->p3
			trap[0].right.v1 = *p2;
			trap[0].right.v2 = *p3;
			return (trap[0].top < trap[0].bottom) ? 1 : 0;
		}

		// 平底三角,向上的三角形
		if (p2->pos.y == p3->pos.y) {
			// 保证 p2.pos.x<p3.pos.x
			if (p2->pos.x > p3->pos.x) p = p2, p2 = p3, p3 = p;
			trap[0].top = p1->pos.y;
			trap[0].bottom = p2->pos.y;
			// 三角形的左边 p1->p2
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p2;
			// 三角形的右边 p1->p3
			trap[0].right.v1 = *p1;
			trap[0].right.v2 = *p3;
			return (trap[0].top < trap[0].bottom) ? 1 : 0;
		}

		// 需要对三角形进行划分,0:平底,1:平顶
		trap[0].top = p1->pos.y;
		trap[0].bottom = p2->pos.y;
		trap[1].top = p2->pos.y;
		trap[1].bottom = p3->pos.y;

		k = (p3->pos.y - p1->pos.y) / (p2->pos.y - p1->pos.y);
		x = p1->pos.x + (p2->pos.x - p1->pos.x) * k;

		// 三角形分为右边为主和左边为主
		if (x <= p3->pos.x) {		// 右边为主
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p2;
			trap[0].right.v1 = *p1;
			trap[0].right.v2 = *p3;

			trap[1].left.v1 = *p2;
			trap[1].left.v2 = *p3;
			trap[1].right.v1 = *p1;
			trap[1].right.v2 = *p3;
		}
		else {					// 左边为主
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p3;
			trap[0].right.v1 = *p1;
			trap[0].right.v2 = *p2;

			trap[1].left.v1 = *p1;
			trap[1].left.v2 = *p3;
			trap[1].right.v1 = *p2;
			trap[1].right.v2 = *p3;
		}
		return 2;
	}

	// 边缘线插值
	void trapezoid_edge_interp(trapezoid_t *trap, float y) {
		float s1 = trap->left.v2.pos.y - trap->left.v1.pos.y;
		float s2 = trap->right.v2.pos.y - trap->right.v1.pos.y;
		float t1 = (y - trap->left.v1.pos.y) / s1;
		float t2 = (y - trap->right.v1.pos.y) / s2;
		// 插值,用edge的两个顶点插值出v点,即扫描线上的点
		vertex_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1);
		vertex_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);
	}


	void trapezoid_init_scan_line(trapezoid_t* trap, scanline_t* scanline, float y) {

		float width = trap->right.v.pos.x - trap->left.v.pos.x;
		// 起始x
		scanline->x = (int)(trap->left.v.pos.x + 0.5f);
		// 哪一行
		scanline->y = y;
		// 扫描线长度
		scanline->w = (int)(trap->right.v.pos.x + 0.5f) - scanline->x;
		// 扫描线的起始点
		scanline->v = trap->left.v;
		if (trap->left.v.pos.x >= trap->right.v.pos.x) scanline->w = 0; // ?
		// 计算每种属性的每一步偏移量
		vertex_division(&scanline->step, &trap->left.v, &trap->right.v, width);
	}

	void device_draw_scanline(scanline_t* scanline) {
		IUINT32 *f = framebuffer[scanline->y];
		float *z = zbuffer[scanline->y];
		int x = scanline->x; // 起始x
		int w = scanline->w; // 宽度

		for (; w > 0; x++, w--) {
			if (x >= 0 && x < width) {
				float rhw = scanline->v.rhw;
				// 1/w buffer
				if (rhw >= z[x]) {
					float _w = 1.0f / rhw;
					z[x] = rhw;

					if (render_state&RENDER_STATE_COLOR) {
						float r = scanline->v.color.r*_w;
						float g = scanline->v.color.g*_w;
						float b = scanline->v.color.b*_w;
						int R = (int)(r * 255.0f);
						int G = (int)(g * 255.0f);
						int B = (int)(b * 255.0f);
						R = CMID(R, 0, 255);
						G = CMID(G, 0, 255);
						B = CMID(B, 0, 255);
						f[x] = (R << 16) | (G << 8) | (B);// 填充颜色
					}
					// 渲染纹理
					if (render_state& RENDER_STATE_TEXTURE) {
						// u,v恢复到0,1,乘以分辨率,进行纹理采样
						float u = scanline->v.tc.u*_w;  
						float v = scanline->v.tc.v*_w;
						IUINT32 cc = device_texture_read(u, v);
						f[x] = cc;// 填充纹理
					}
				}

			}
			vertex_add(&scanline->v, &scanline->step);
			if (x >= width) break;
		}
	}


	// 主渲染函数
	void device_render_trap(trapezoid_t *trap) {
		// 扫描线插值
		scanline_t scanline;
		int j, top, bottom;
		top = (int)(trap->top + 0.5f);
		bottom = (int)(trap->bottom + 0.5f);

		for (j = top; j < bottom; ++j) {
			// 合法
			if (j >= 0 && j < height) {
				// 1 根据 j坐标计算出left和right线上的两个顶点
				trapezoid_edge_interp(trap, (float)j + 0.5f);
				// 2. 根据左右两边的端点，初始化计算出扫描线的起点和步长
				trapezoid_init_scan_line(trap, &scanline, j);
				// 3. 绘制扫描线
				device_draw_scanline(&scanline);
			}
			if (j >= height) break;
		}
	}

	// 根据 render_state 绘制原始三角形
	void device_draw_primitive(vertex_t* v1, vertex_t* v2, vertex_t* v3) {
		// c表示从模型坐标系->裁剪坐标系
		// p表示屏幕坐标系坐标
		point_t p1, p2, p3, c1, c2, c3;

		// 1. 进行坐标变换  
		transform.transform_apply(&c1, &v1->pos);
		transform.transform_apply(&c2, &v2->pos);
		transform.transform_apply(&c3, &v3->pos);

		// 2. 进行裁剪
		// 注意此处可以完善为具体判断几个点在 cvv内以及同cvv相交平面的坐标比例
		// 进行进一步精细裁剪，将一个分解为几个完全处在 cvv内的三角形
		if (transform.transform_check_cvv(&c1) != 0) return;
		if (transform.transform_check_cvv(&c2) != 0) return;
		if (transform.transform_check_cvv(&c3) != 0) return;

		// 3. 归一化,透视除法,加视口变化,变换到屏幕坐标
		transform.transform_homogenize(&p1, &c1);
		transform.transform_homogenize(&p2, &c2);
		transform.transform_homogenize(&p3, &c3);

		// 4. 根据 render_state 绘制原始三角形,光栅化
		/// 绘制纹理或者根据顶点色彩绘制
		if (render_state & RENDER_STATE_COLOR | RENDER_STATE_TEXTURE) {
			// t1,t2,t3为顶点,属性有:pos,纹理坐标,颜色,rhw
			vertex_t t1 = *v1, t2 = *v2, t3 = *v3; 
			trapezoid_t traps[2];
			int n;

			t1.pos = p1; // 都是二维的坐标了,p是经过透视除法的,p中w为1.0
			t2.pos = p2;
			t3.pos = p3;
			t1.pos.w = c1.w; // c没有经过透视除法,w是有的,后面加权需要
			t2.pos.w = c2.w;
			t3.pos.w = c3.w;

			vertex_rhw_init(&t1);
			vertex_rhw_init(&t2);
			vertex_rhw_init(&t3);

			// 拆分三角形为0-2个梯形，并且返回可用梯形数量
			n = trapezoid_init_triangle(traps, &t1, &t2, &t3);
			// 光栅化平的三角形
			if (n >= 1) device_render_trap(&traps[0]);
			if (n >= 2) device_render_trap(&traps[1]);

		}

		/// 线框绘制
		if (render_state & RENDER_STATE_WIREFRAME) {
			// 绘制三条直线
			device_draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, foreground);
			device_draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, foreground);
			device_draw_line((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y, foreground);
		}
	}

	// 绘制线段 Bresenham算法(布雷森漢姆直線)
	void device_draw_line(int x1, int y1, int x2, int y2, IUINT32 c) {
#define Bresenham 1
#if !Bresenham
		// DDA 数字微分分析
		if (x1 == x2&&y1 == y2) {
			device_pixel(x1 + 0.5, y1 + 0.5, c);
			return;
		}

		int dx = x2 - x1, dy = y2 - y1;
		int step, k;
		float xIncrement, yIncrement, x = x1, y = y1;
		if (fabs(dx) > fabs(dy)) {
			step = fabs(dx);
		} else {
			step = fabs(dy);
		}
		xIncrement = (float)dx / (float)step;
		yIncrement = (float)dy / (float)step;
		device_pixel(x + 0.5, y + 0.5, c);
		for (k = 0; k < step; k++) {
			x += xIncrement;
			y += yIncrement;
			device_pixel(x + 0.5, y + 0.5, c);
		}
#else
		// Bresenham 算法
		int x = 0, y = 0, rem = 0;
		if (x1 == x2&&y1 == y2) {
			device_pixel(x1, y1, c);
		} else if (x1 == x2) {
			int inc = (y1 <= y2) ? 1 : -1;
			for (y = y1; y != y2; y += inc) device_pixel(x, y, c);
			device_pixel(x2, y2, c);
		} else if (y1 == y2) {
			int inc = (x1 <= x2) ? 1 : -1;
			for (x = x1; x != x2; x += inc) device_pixel(x, y, c);
			device_pixel(x2, y2, c);
		} else {
			// 判断直线是近x轴还是近y轴
			int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
			int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
			if (dx >= dy) {
				// 靠近x轴
				if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
				for (x = x1, y = y1; x <= x2; x++) {
					device_pixel(x, y, c);
					rem += dy;
					if (rem >= dx) {
						rem -= dx;
						y += (y2 >= y1) ? 1 : -1;
						device_pixel(x, y, c);
					}
				}
				device_pixel(x2, y2, c);
			} else {
				// 靠近y轴
				if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
				for (x = x1, y = y1; y <= y2; y++) {
					device_pixel(x, y, c);
					rem += dx;
					if (rem >= dy) {
						rem -= dy;
						x += (x2 >= x1) ? 1 : -1;
						device_pixel(x, y, c);
					}
				}
				device_pixel(x2, y2, c);
			}
		}
#endif
	}

	// 画点
	void device_pixel(int x, int y, IUINT32 color) {
		if (((IUINT32)x) < (IUINT32)width && ((IUINT32)y) < (IUINT32)height) {
			framebuffer[y][x] = color;
		}
	}

	// 根据坐标读取纹理
	IUINT32 device_texture_read(float u, float v) {
		int x, y;
		u = u * max_u;
		v = v * max_v;
		x = (int)(u + 0.5f);
		y = (int)(v + 0.5f);
		x = CMID(x, 0, tex_width - 1);
		y = CMID(y, 0, tex_height - 1);
		return texture[y][x];
	}

	// ===============================  成员变量 =============================== //
	transform_t transform;      // 坐标变换器
	int width;                  // 窗口宽度
	int height;                 // 窗口高度
	IUINT32 **framebuffer;      // 像素缓存：framebuffer[y] 代表第 y行
	float **zbuffer;            // 深度缓存：zbuffer[y] 为第 y行指针
	IUINT32 **texture;          // 纹理：同样是每行索引
	int tex_width;              // 纹理宽度
	int tex_height;             // 纹理高度
	float max_u;                // 纹理最大宽度：tex_width - 1
	float max_v;                // 纹理最大高度：tex_height - 1
	int render_state;           // 渲染状态
	IUINT32 background;         // 背景颜色
	IUINT32 foreground;         // 线框颜色
	void *fb;					// 外部帧缓冲
	vertex_t *mesh;				// 网格模型
};


//=====================================================================
// 主程序
//=====================================================================

// 网格
vertex_t mesh[8] = {
	// 模型顶点,纹理坐标,颜色值
	{ { 1, -1,  1, 1 },{ 0, 0 },{ 1.0f, 0.2f, 0.2f }, 1 },
	{ { -1, -1,  1, 1 },{ 0, 1 },{ 0.2f, 1.0f, 0.2f }, 1 },
	{ { -1,  1,  1, 1 },{ 1, 1 },{ 0.2f, 0.2f, 1.0f }, 1 },
	{ { 1,  1,  1, 1 },{ 1, 0 },{ 1.0f, 0.2f, 1.0f }, 1 },
	{ { 1, -1, -1, 1 },{ 0, 0 },{ 1.0f, 1.0f, 0.2f }, 1 },
	{ { -1, -1, -1, 1 },{ 0, 1 },{ 0.2f, 1.0f, 1.0f }, 1 },
	{ { -1,  1, -1, 1 },{ 1, 1 },{ 1.0f, 0.3f, 0.3f }, 1 },
	{ { 1,  1, -1, 1 },{ 1, 0 },{ 0.2f, 1.0f, 0.3f }, 1 },
};


int main() {

	TCHAR *title = _T("JayEngine-参考自 mini3d | ")
		_T("Left/Right: rotation, Up/Down: forward/backward, Space: switch state");

	int width = 800, height = 600;

	// 初始化窗口
	if (screen_init(width, height, title))
		return -1;

	device_t device(width, height, screen_fb); // 渲染设备创建
	int states[] = { RENDER_STATE_TEXTURE, RENDER_STATE_COLOR, RENDER_STATE_WIREFRAME };
	int indicator = 0; // 切换渲染状态
	int kbhit = 0;
	float theta = 1; // 旋转
	float pos = 3.5; // x位置

	device.device_init();
	device.device_set_camera(3, 0, 0); // view矩阵的计算
	device.device_init_texture();  // 初始化纹理
	device.device_init_mesh(mesh); // 初始化模型

	// 渲染状态
	device.render_state = RENDER_STATE_WIREFRAME; // 线框

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		screen_dispatch(); // 
		device.device_clear(1); // 清空数据
		device.device_set_camera(pos, 0, 0); // 更新位置pos
		
		if (screen_keys[VK_UP]) pos -= 0.01f; // pos
		if (screen_keys[VK_DOWN]) pos += 0.01f;
		if (screen_keys[VK_LEFT]) theta += 0.01f; // theta,旋转
		if (screen_keys[VK_RIGHT]) theta -= 0.01f;

		if (screen_keys[VK_SPACE]) {
			if (kbhit == 0) {
				kbhit = 1;
				if (++indicator >= 3) indicator = 0;
				device.render_state = states[indicator];
			}
		} else {
			kbhit = 0;
		}

		device.draw_box(theta);// 核心函数
		screen_update();
		Sleep(1);
	}

	return 0;
}

