#include <iostream>
#include <fstream> 
#include <sstream> // istringstream ����������ͷ�ļ�
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

// ������
typedef unsigned int IUINT32;

#include <windows.h> // windows����
//=====================================================================
// Win32 ���ڼ�ͼ�λ��ƣ�Ϊ device �ṩһ�� DibSection �� FB
//=====================================================================
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];	// ��ǰ���̰���״̬
static HWND screen_handle = NULL;		// ������ HWND
static HDC screen_dc = NULL;			// ���׵� HDC
static HBITMAP screen_hb = NULL;		// DIB
static HBITMAP screen_ob = NULL;		// �ϵ� BITMAP
unsigned char *screen_fb = NULL;		// frame buffer ����Ļ��д����
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);	// ��Ļ��ʼ��
int screen_close(void);								// �ر���Ļ
void screen_dispatch(void);							// ������Ϣ
void screen_update(void);							// ��ʾ FrameBuffer

// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif

// ��ʼ�����ڲ����ñ���
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
	// �ռ�w*h*4
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
// ��ѧ�⣺�˲���Ӧ�ò�����⣬��Ϥ D3D ����任����
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

// �����ֵ��t Ϊ [0, 1] ֮�����ֵ
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

// ʸ�����
float vector_dotproduct(const vector_t *x, const vector_t *y) {
	return x->x * y->x + x->y * y->y + x->z * y->z;
}

// ʸ�����
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

// ʸ����ֵ��tȡֵ [0, 1]
void vector_interp(vector_t *z, const vector_t *x1, const vector_t *x2, float t) {
	z->x = interp(x1->x, x2->x, t);
	z->y = interp(x1->y, x2->y, t);
	z->z = interp(x1->z, x2->z, t);
	z->w = 1.0f;
}

// ʸ����һ��
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

// ��λ����
void matrix_set_identity(matrix_t *m) {
	m->m[0][0] = m->m[1][1] = m->m[2][2] = m->m[3][3] = 1.0f;
	m->m[0][1] = m->m[0][2] = m->m[0][3] = 0.0f;
	m->m[1][0] = m->m[1][2] = m->m[1][3] = 0.0f;
	m->m[2][0] = m->m[2][1] = m->m[2][3] = 0.0f;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0f;
}

// �����
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

// ���������
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
// ����任
//=====================================================================
class transform_t {
public:
	// ��ʼ����������Ļ����
	void transform_init(int width, int height) {
		float aspect = (float)width / ((float)height); // ����
		matrix_set_identity(&this->world);
		matrix_set_identity(&this->view);
		// ͶӰ��������� projection �������ϵ->��βü��ռ�
		matrix_set_perspective(&this->projection, 3.1415926f * 0.5f, aspect, 1.0f, 500.0f);
		this->w = width;
		this->h = height;
		transform_update();
	}

	// https://blog.csdn.net/you_lan_hai/article/details/37992123 D3D��OpenGL����ϵ
	// ������£����� transform = world * view * projection
	void transform_update() {
		matrix_t m;
		matrix_mul(&m, &this->world, &this->view);
		matrix_mul(&this->transform, &m, &this->projection);
	}

	// ���ñ任����
	void transform_apply(point_t* y, point_t* x) {
		matrix_apply(y, x, &this->transform);
	}

	// ����������ͬ cvv �ı߽�������׶�ü�
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

	// ��һ�����õ���Ļ����
	void transform_homogenize(vector_t *y, const vector_t *x) {
		// ͸�ӳ���+�ӿڱ任
		float rhw = 1.0f / x->w;
		y->x = (x->x * rhw + 1.0f) * w * 0.5f;
		y->y = (1.0f - x->y * rhw) * h * 0.5f;
		y->z = x->z * rhw;
		y->w = 1.0f;
	}

	matrix_t world;			// world���� model->world
	matrix_t view;			// view����  world->view
	matrix_t projection;	// Ͷ��任 view->�ü��ռ�
	matrix_t transform;		// transform = world * view * projection
	float w, h;				// ��Ļ��С
};

//=====================================================================
// ���μ��㣺���㡢ɨ���ߡ���Ե�����Ρ���������
//=====================================================================
typedef struct { float r, g, b; } color_t;
typedef struct { float u, v; } texcoord_t;
// ��������,��������,��ɫֵ,
typedef struct { point_t pos; texcoord_t tc; color_t color; float rhw; } vertex_t;

typedef struct { vertex_t v, v1, v2; } edge_t; // v�ǲ�ֵ�Ľ��,v1���,v2�յ�
// ����
typedef struct { float top, bottom; edge_t left, right; } trapezoid_t;
// ɨ����
typedef struct { vertex_t v, step; int x, y, w; } scanline_t;

// �����ֵ
void vertex_interp(vertex_t *y, const vertex_t* x1, const vertex_t* x2, float t) {
	vector_interp(&y->pos, &x1->pos, &x2->pos, t);
	y->tc.u = interp(x1->tc.u, x2->tc.u, t);
	y->tc.v = interp(x1->tc.v, x2->tc.v, t);
	y->color.r = interp(x1->color.r, x2->color.r, t);
	y->color.g = interp(x1->color.g, x2->color.g, t);
	y->color.b = interp(x1->color.b, x2->color.b, t);
	y->rhw = interp(x1->rhw, x2->rhw, t);
}

// ����step,λ�Ƶ�ÿһ������
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
// ��Ⱦ�豸
//=====================================================================
#define RENDER_STATE_WIREFRAME      1		// ��Ⱦ�߿�
#define RENDER_STATE_TEXTURE        2		// ��Ⱦ����
#define RENDER_STATE_COLOR          4		// ��Ⱦ��ɫ

class device_t {
public:
	device_t() {
	}

	device_t(int width, int height, void *fb) {
		this->width = width;
		this->height = height;
		this->fb = fb;
	}

	// �豸��ʼ����fbΪ�ⲿ֡���棬�� NULL �������ⲿ֡���棨ÿ�� 4�ֽڶ��룩
	void device_init() {
		// ������һ���ռ�
		int need = sizeof(void*) * (height * 2 + 1024) + width * height * 8;
		char *ptr = (char*)malloc(need + 64);

		char *framebuf, *zbuf;
		int j;
		assert(ptr); // �ռ俪�ٳɹ�

		// �洢ÿ���� framebuffer֡���� ��ָ��
		this->framebuffer = (IUINT32 **)ptr;
		// �洢ÿһ�� Z����zbuffer ��ָ��
		this->zbuffer = (float**)(ptr + sizeof(void*) * height);

		// �洢ÿһ��texture�����ָ��
		ptr += sizeof(void*) * height * 2;
		this->texture = (IUINT32**)ptr;

		ptr += sizeof(void*) * 1024; // Ϊ�˶���

		// ֡����������С width * height * 4(RGBA)
		framebuf = (char*)ptr;  // framebuf ָ��֡�����ڴ�
		// z����ʵ���ڴ��С width * height * 4(float��4���ֽ�)
		zbuf = (char*)ptr + width * height * 4; // zbuf ָ��zbuf�ڴ�
		ptr += width * height * 8;

		if (fb != NULL) framebuf = (char*)fb; // ������ⲿ��֡��������ⲿ��,���紫��һ�������ڴ�

		// ÿһ�зֱ�ָ���ڴ�ռ�
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
		// �����߿�
		this->render_state = RENDER_STATE_WIREFRAME;
	}

	void device_init_mesh(vertex_t *mesh) {
		if (mesh != NULL)
			this->mesh = mesh;
	}

	// D3Dʹ�õ�����������ϵ 
	void device_set_camera(float x, float y, float z) {
		point_t eye = { x, y, z, 1 }, at = { 0, 0, 0, 1 }, up = { 0, 0, 1, 1 };
		// view����ļ���,��������ϵ���������ϵ
		matrix_set_lookat(&this->transform.view, &eye, &at, &up);
		this->transform.transform_update();
	}

	// ��ʼ������
	void device_init_texture() {
		static IUINT32 texture[256][256];
		int i, j;
		for (j = 0; j < 256; j++) {
			for (i = 0; i < 256; i++) {
				int x = i / 32, y = j / 32;
				texture[j][i] = ((x + y) & 1) ? 0xffffff : 0x3fbcef; // �������������
			}
		}
		device_set_texture(texture, 256 * 4, 256, 256);
	}

	// ��������
	void device_set_texture(void *bits, long pitch, int w, int h) {
		char *ptr = (char*)bits;
		int j;
		assert(w <= 1024 && h <= 1024);
		for (j = 0; j < h; ptr += pitch, j++) 	// ���¼���ÿ�������ָ��
			this->texture[j] = (IUINT32*)ptr;
		this->tex_width = w;
		this->tex_height = h;
		this->max_u = (float)(w - 1);
		this->max_v = (float)(h - 1);
	}

	// ��� framebuffer �� zbuffer
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

	// ������ת����
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

	// ���ĺ���������
	void draw_box(float theta) {
		matrix_t m;
		matrix_set_rotate(&m, -1, -0.5, 1, theta); // ������ת����
		this->transform.world = m;
		this->transform.transform_update(); // ����transform����
		
		// ��6��ƽ��
		draw_plane(0, 1, 2, 3);
		draw_plane(4, 5, 6, 7);
		draw_plane(0, 4, 5, 1);
		draw_plane(1, 5, 6, 2);
		draw_plane(2, 6, 7, 3);
		draw_plane(3, 7, 4, 0);
	}

	// ��ƽ��
	void draw_plane(int a, int b, int c, int d) {
		// һ��������ƽ�����ĸ��㹹��
		vertex_t p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];
		// ������������
		p1.tc.u = 0, p1.tc.v = 0, p2.tc.u = 0, p2.tc.v = 1;
		p3.tc.u = 1, p3.tc.v = 1, p4.tc.u = 1, p4.tc.v = 0;
		// ������������
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

	// �������β�� ƽ������
	// https://blog.csdn.net/cppyin/article/details/6232453
	int trapezoid_init_triangle(trapezoid_t *trap, const vertex_t *p1,
		const vertex_t *p2, const vertex_t *p3) {

		const vertex_t *p;
		float k, x;

		// ����һ�� p1.y<p2.y<p3.y
		if (p1->pos.y > p2->pos.y) p = p1, p1 = p2, p2 = p;
		if (p1->pos.y > p3->pos.y) p = p1, p1 = p3, p3 = p;
		if (p2->pos.y > p3->pos.y) p = p2, p2 = p3, p3 = p;

		// ������������,����0
		if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y) return 0;
		if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x) return 0;

		// ƽ��,���µ�������
		if (p1->pos.y == p2->pos.y) {
			// ��֤ p1.pos.x<p2.pos.x
			if (p1->pos.x > p2->pos.x) p = p1, p1 = p2, p2 = p;
			trap[0].top = p1->pos.y; // ����
			trap[0].bottom = p3->pos.y; // �ײ�
			// �����ε���� p1->p3
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p3;
			// �����ε��ұ� p2->p3
			trap[0].right.v1 = *p2;
			trap[0].right.v2 = *p3;
			return (trap[0].top < trap[0].bottom) ? 1 : 0;
		}

		// ƽ������,���ϵ�������
		if (p2->pos.y == p3->pos.y) {
			// ��֤ p2.pos.x<p3.pos.x
			if (p2->pos.x > p3->pos.x) p = p2, p2 = p3, p3 = p;
			trap[0].top = p1->pos.y;
			trap[0].bottom = p2->pos.y;
			// �����ε���� p1->p2
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p2;
			// �����ε��ұ� p1->p3
			trap[0].right.v1 = *p1;
			trap[0].right.v2 = *p3;
			return (trap[0].top < trap[0].bottom) ? 1 : 0;
		}

		// ��Ҫ�������ν��л���,0:ƽ��,1:ƽ��
		trap[0].top = p1->pos.y;
		trap[0].bottom = p2->pos.y;
		trap[1].top = p2->pos.y;
		trap[1].bottom = p3->pos.y;

		k = (p3->pos.y - p1->pos.y) / (p2->pos.y - p1->pos.y);
		x = p1->pos.x + (p2->pos.x - p1->pos.x) * k;

		// �����η�Ϊ�ұ�Ϊ�������Ϊ��
		if (x <= p3->pos.x) {		// �ұ�Ϊ��
			trap[0].left.v1 = *p1;
			trap[0].left.v2 = *p2;
			trap[0].right.v1 = *p1;
			trap[0].right.v2 = *p3;

			trap[1].left.v1 = *p2;
			trap[1].left.v2 = *p3;
			trap[1].right.v1 = *p1;
			trap[1].right.v2 = *p3;
		}
		else {					// ���Ϊ��
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

	// ��Ե�߲�ֵ
	void trapezoid_edge_interp(trapezoid_t *trap, float y) {
		float s1 = trap->left.v2.pos.y - trap->left.v1.pos.y;
		float s2 = trap->right.v2.pos.y - trap->right.v1.pos.y;
		float t1 = (y - trap->left.v1.pos.y) / s1;
		float t2 = (y - trap->right.v1.pos.y) / s2;
		// ��ֵ,��edge�����������ֵ��v��,��ɨ�����ϵĵ�
		vertex_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1);
		vertex_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);
	}


	void trapezoid_init_scan_line(trapezoid_t* trap, scanline_t* scanline, float y) {

		float width = trap->right.v.pos.x - trap->left.v.pos.x;
		// ��ʼx
		scanline->x = (int)(trap->left.v.pos.x + 0.5f);
		// ��һ��
		scanline->y = y;
		// ɨ���߳���
		scanline->w = (int)(trap->right.v.pos.x + 0.5f) - scanline->x;
		// ɨ���ߵ���ʼ��
		scanline->v = trap->left.v;
		if (trap->left.v.pos.x >= trap->right.v.pos.x) scanline->w = 0; // ?
		// ����ÿ�����Ե�ÿһ��ƫ����
		vertex_division(&scanline->step, &trap->left.v, &trap->right.v, width);
	}

	void device_draw_scanline(scanline_t* scanline) {
		IUINT32 *f = framebuffer[scanline->y];
		float *z = zbuffer[scanline->y];
		int x = scanline->x; // ��ʼx
		int w = scanline->w; // ���

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
						f[x] = (R << 16) | (G << 8) | (B);// �����ɫ
					}
					// ��Ⱦ����
					if (render_state& RENDER_STATE_TEXTURE) {
						// u,v�ָ���0,1,���Էֱ���,�����������
						float u = scanline->v.tc.u*_w;  
						float v = scanline->v.tc.v*_w;
						IUINT32 cc = device_texture_read(u, v);
						f[x] = cc;// �������
					}
				}

			}
			vertex_add(&scanline->v, &scanline->step);
			if (x >= width) break;
		}
	}


	// ����Ⱦ����
	void device_render_trap(trapezoid_t *trap) {
		// ɨ���߲�ֵ
		scanline_t scanline;
		int j, top, bottom;
		top = (int)(trap->top + 0.5f);
		bottom = (int)(trap->bottom + 0.5f);

		for (j = top; j < bottom; ++j) {
			// �Ϸ�
			if (j >= 0 && j < height) {
				// 1 ���� j��������left��right���ϵ���������
				trapezoid_edge_interp(trap, (float)j + 0.5f);
				// 2. �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ���
				trapezoid_init_scan_line(trap, &scanline, j);
				// 3. ����ɨ����
				device_draw_scanline(&scanline);
			}
			if (j >= height) break;
		}
	}

	// ���� render_state ����ԭʼ������
	void device_draw_primitive(vertex_t* v1, vertex_t* v2, vertex_t* v3) {
		// c��ʾ��ģ������ϵ->�ü�����ϵ
		// p��ʾ��Ļ����ϵ����
		point_t p1, p2, p3, c1, c2, c3;

		// 1. ��������任  
		transform.transform_apply(&c1, &v1->pos);
		transform.transform_apply(&c2, &v2->pos);
		transform.transform_apply(&c3, &v3->pos);

		// 2. ���вü�
		// ע��˴���������Ϊ�����жϼ������� cvv���Լ�ͬcvv�ཻƽ����������
		// ���н�һ����ϸ�ü�����һ���ֽ�Ϊ������ȫ���� cvv�ڵ�������
		if (transform.transform_check_cvv(&c1) != 0) return;
		if (transform.transform_check_cvv(&c2) != 0) return;
		if (transform.transform_check_cvv(&c3) != 0) return;

		// 3. ��һ��,͸�ӳ���,���ӿڱ仯,�任����Ļ����
		transform.transform_homogenize(&p1, &c1);
		transform.transform_homogenize(&p2, &c2);
		transform.transform_homogenize(&p3, &c3);

		// 4. ���� render_state ����ԭʼ������,��դ��
		/// ����������߸��ݶ���ɫ�ʻ���
		if (render_state & RENDER_STATE_COLOR | RENDER_STATE_TEXTURE) {
			// t1,t2,t3Ϊ����,������:pos,��������,��ɫ,rhw
			vertex_t t1 = *v1, t2 = *v2, t3 = *v3; 
			trapezoid_t traps[2];
			int n;

			t1.pos = p1; // ���Ƕ�ά��������,p�Ǿ���͸�ӳ�����,p��wΪ1.0
			t2.pos = p2;
			t3.pos = p3;
			t1.pos.w = c1.w; // cû�о���͸�ӳ���,w���е�,�����Ȩ��Ҫ
			t2.pos.w = c2.w;
			t3.pos.w = c3.w;

			vertex_rhw_init(&t1);
			vertex_rhw_init(&t2);
			vertex_rhw_init(&t3);

			// ���������Ϊ0-2�����Σ����ҷ��ؿ�����������
			n = trapezoid_init_triangle(traps, &t1, &t2, &t3);
			// ��դ��ƽ��������
			if (n >= 1) device_render_trap(&traps[0]);
			if (n >= 2) device_render_trap(&traps[1]);

		}

		/// �߿����
		if (render_state & RENDER_STATE_WIREFRAME) {
			// ��������ֱ��
			device_draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, foreground);
			device_draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, foreground);
			device_draw_line((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y, foreground);
		}
	}

	// �����߶� Bresenham�㷨(����ɭ�hķֱ��)
	void device_draw_line(int x1, int y1, int x2, int y2, IUINT32 c) {
#define Bresenham 1
#if !Bresenham
		// DDA ����΢�ַ���
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
		// Bresenham �㷨
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
			// �ж�ֱ���ǽ�x�ỹ�ǽ�y��
			int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
			int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
			if (dx >= dy) {
				// ����x��
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
				// ����y��
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

	// ����
	void device_pixel(int x, int y, IUINT32 color) {
		if (((IUINT32)x) < (IUINT32)width && ((IUINT32)y) < (IUINT32)height) {
			framebuffer[y][x] = color;
		}
	}

	// ���������ȡ����
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

	// ===============================  ��Ա���� =============================== //
	transform_t transform;      // ����任��
	int width;                  // ���ڿ��
	int height;                 // ���ڸ߶�
	IUINT32 **framebuffer;      // ���ػ��棺framebuffer[y] ����� y��
	float **zbuffer;            // ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��
	IUINT32 **texture;          // ����ͬ����ÿ������
	int tex_width;              // ������
	int tex_height;             // ����߶�
	float max_u;                // ��������ȣ�tex_width - 1
	float max_v;                // �������߶ȣ�tex_height - 1
	int render_state;           // ��Ⱦ״̬
	IUINT32 background;         // ������ɫ
	IUINT32 foreground;         // �߿���ɫ
	void *fb;					// �ⲿ֡����
	vertex_t *mesh;				// ����ģ��
};


//=====================================================================
// ������
//=====================================================================

// ����
vertex_t mesh[8] = {
	// ģ�Ͷ���,��������,��ɫֵ
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

	TCHAR *title = _T("JayEngine-�ο��� mini3d | ")
		_T("Left/Right: rotation, Up/Down: forward/backward, Space: switch state");

	int width = 800, height = 600;

	// ��ʼ������
	if (screen_init(width, height, title))
		return -1;

	device_t device(width, height, screen_fb); // ��Ⱦ�豸����
	int states[] = { RENDER_STATE_TEXTURE, RENDER_STATE_COLOR, RENDER_STATE_WIREFRAME };
	int indicator = 0; // �л���Ⱦ״̬
	int kbhit = 0;
	float theta = 1; // ��ת
	float pos = 3.5; // xλ��

	device.device_init();
	device.device_set_camera(3, 0, 0); // view����ļ���
	device.device_init_texture();  // ��ʼ������
	device.device_init_mesh(mesh); // ��ʼ��ģ��

	// ��Ⱦ״̬
	device.render_state = RENDER_STATE_WIREFRAME; // �߿�

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		screen_dispatch(); // 
		device.device_clear(1); // �������
		device.device_set_camera(pos, 0, 0); // ����λ��pos
		
		if (screen_keys[VK_UP]) pos -= 0.01f; // pos
		if (screen_keys[VK_DOWN]) pos += 0.01f;
		if (screen_keys[VK_LEFT]) theta += 0.01f; // theta,��ת
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

		device.draw_box(theta);// ���ĺ���
		screen_update();
		Sleep(1);
	}

	return 0;
}

