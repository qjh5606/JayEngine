#include "JayPipeline.h"

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



//==============================================================//
// 主函数
//==============================================================//

// 屏幕分辨率
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

const int REAL_WIDTH = 600;
const int REAL_HEIGHT = 600;

// 默认模型
extern Vertex ground_mesh[6];
extern Vertex box_mesh[36];
extern Vertex triangle_mesh[3];

int main() {
	TCHAR *title = _T("JayEngine");

	std::cout << "========= Render Mode =========" << std::endl;
	std::cout << "\t" << "线框模式" << std::endl;
	std::cout << "\t" << "颜色模式" << std::endl;
	std::cout << "\t" << "纹理模式" << std::endl;
	std::cout << "\t" << "SPACE: " << "切换" << std::endl;
	std::cout << "==================================" << std::endl;

	std::cout << "========= Camera Control =========" << std::endl;
	std::cout << "\t" << "Ctrl + Q: " << "向上" << std::endl;
	std::cout << "\t" << "Ctrl + E: " << "向下" << std::endl;
	std::cout << "\t" << "Ctrl + W: " << "向前" << std::endl;
	std::cout << "\t" << "Ctrl + S: " << "向后" << std::endl;
	std::cout << "\t" << "Ctrl + A: " << "向左" << std::endl;
	std::cout << "\t" << "Ctrl + W: " << "向右" << std::endl;
	std::cout << "==================================" << std::endl;

	std::cout << "========= 其他功能控制键 ============" << std::endl;
	std::cout << "\t" << "Ctrl + B: " << "背面剔除" << std::endl;
	std::cout << "\t" << "Ctrl + C: " << "2D裁剪(目前存在问题对于ground)" << std::endl;
	std::cout << "=====================================" << std::endl;

	/// 初始化窗口
	if (screen_init(SCREEN_WIDTH, SCREEN_HEIGHT, title))
		return -1;
	int states[] = { RENDER_STATE_TEXTURE, RENDER_STATE_COLOR, RENDER_STATE_WIREFRAME };
	int indicator = 0; // 切换渲染状态
	int kbhit = 0;
	int kbhit1 = 0;
	int kbhit2 = 0;

	// control
	float c_yaw = 0.0f;
	float c_pitch = 0.0f;
	float c_movementspeed = 2.0f;

	// ===================== 创建管线 ======================// 
	IUINT32* framebuffer = new IUINT32[SCREEN_WIDTH*SCREEN_HEIGHT];
	float* zbuffer = new float[SCREEN_WIDTH*SCREEN_HEIGHT];
	float* shadowbuffer = new float[SCREEN_WIDTH*SCREEN_HEIGHT];
	
	// 初始化shadowbuffer
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			shadowbuffer[y*SCREEN_WIDTH + x] = INT_MAX;
		}
	}

	//Pipeline  pipeline(REAL_WIDTH, REAL_HEIGHT, screen_fb);
	Pipeline  pipeline(REAL_WIDTH, REAL_HEIGHT, framebuffer);

	// ===================== 加载光源 ======================// 
	DirLight *dirlight = new DirLight({ 1.0f, -1.0f, 1.0f, 0.0f }, { 0.3f, 0.3f, 0.3f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }, { 0.3f, 0.3f, 0.3f, 1.0f }, true);
	PointLight *pointlight = new PointLight({ { 0.0f, 6.0f, -1.0f, 1.0f }, 1.0f, 0.09f, 0.032f,{ 0.6f, 0.6f, 0.6f, 1.0f },{ 0.8f, 0.8f, 0.8f, 1.0f },{ 0.7f, 0.7f, 0.7f, 1.0f }, false });
	PointLight *pointlight1 = new PointLight({ { 0.0f, 6.0f, 2.0f, 1.0f }, 1.0f, 0.09f, 0.032f, { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f }, false });
	PointLight *pointlight2 = new PointLight({{ 0.0f, 6.0f, -1.0f, 1.0f }, 1.0f, 0.09f, 0.032f, { 0.6f, 0.6f, 0.6f, 1.0f }, { 0.8f, 0.8f, 0.8f, 1.0f }, { 0.3f, 0.3f, 0.3f, 1.0f }, false});
	PointLight *pointlight3 = new PointLight({ { 0.0f, 6.0f, -1.0f, 1.0f }, 1.0f, 0.09f, 0.032f,{ 0.6f, 0.6f, 0.6f, 1.0f },{ 0.8f, 0.6f, 0.8f, 1.0f },{ 0.3f, 0.3f, 0.3f, 1.0f }, false });
	
	pipeline.dirLights.push_back(dirlight);
	pipeline.pointLights.push_back(pointlight);
	//pipeline.pointLights.push_back(pointlight1);
	//pipeline.pointLights.push_back(pointlight2);
	//pipeline.pointLights.push_back(pointlight3);

	/// check shadows 增加阴影摄像机
	pipeline.checkLights();

	// ===================== 加载主摄像机 ======================// 
	Camera *mainCamera = new Camera(
		perspective,
		true,
		{ 0.0f, 2.0f, -2.5f, 1.0f },// pos    at = pos+front
		{ 0.0f, 0.0f, 1.0f, 0.0f }, // front,   
		{ 0.0f, 1.0f, 0.0f, 0.0f }, // worldup
		3.1415926 * 0.5f,
		0.1f,
		500.0f,
		REAL_WIDTH,
		REAL_WIDTH,
		-1.0f,
		1.0f,
		-1.0f,
		1.0f,
		true
	);

	// 设置主摄像机
	pipeline.cameras.push_back(mainCamera);

	// ===================== 加载物体模型 ======================// 
	/// 共享资源创建 纹理,材质
	//Texture *defaultTexure = new Texture();// 初始化纹理
	Material *dimian = new Material(
		"dimian", 
		{ 0.2f, 0.2f, 0.2f }, 
		{ 0.5f, 0.5f, 0.5f }, 
		{ 0.2f, 0.2f, 0.2f }, 
		{ 0.5f, 0.5f, 0.5f }, 
		{ 0.5f, 0.5f, 0.5f }, 
		32.0f, 1.0f, 1.0f, 1, 1, "dimian.png", -1, "", -1, "", -1, "", -1, "", -1, "", -1, "", -1
	);

	Material *mabu = new Material(
		 "mabu",
		{ 0.2f, 0.2f, 0.2f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.2f, 0.2f, 0.2f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		32.0f, 1.0f, 1.0f, 1, 1, "mabu.png", -1, "", -1, "", -1, "", -1, "", -1, "", -1, "", -1
	);

	/// 加载人型模型和材质
	Object *nanosuit = new Object();
	nanosuit->make_mesh_and_material_by_obj("nanosuit.obj");
	nanosuit->pos = { 0, 0, 0, 1 };
	nanosuit->scale = { 0.12f,0.12f, 0.12f, 0 };
	nanosuit->axis = { 0, 1, 0, 1 };
	nanosuit->theta = 60.0f;
	nanosuit->shadow = false;
	nanosuit->dirty = true;

	/// 模型一
	Model *groundModel = new Model(ground_mesh, 6);
	Object *ground = new Object(groundModel, dimian);
	ground->pos = { 0, 0, 0, 1 };
	ground->scale = { 20, 1, 20, 0 };
	ground->axis = { 0, 0, 0, 1 };
	ground->theta = 0.0f;
	ground->shadow = false;
	ground->dirty = true;

	/// 模型二
	Model *boxModel = new Model(box_mesh, 36);
	Object *box = new Object(boxModel, mabu);
	box->pos = { -0.5, 2, 0, 1 };
	box->scale = { 0.3f, 0.3f, 0.3f, 0 };
	box->axis = { 0, 1, 0, 1 };
	box->theta = 0.0f;
	box->shadow = false;
	box->dirty = true;

	Object *box1 = new Object(boxModel, mabu);
	box1->pos = { 0.5, 2, -1, 1 };
	box1->scale = { 0.5, 0.5, 0.5, 0 };
	box1->axis = { 1, 0, 1, 1 };
	box1->theta = 0.0f;
	box1->shadow = false;
	box1->dirty = true;

	Model *planeModel = new Model(triangle_mesh, 3);
	Object *debugTrianle = new Object(planeModel, mabu);
	debugTrianle->pos = { 0.5, 3, -1, 1 };
	debugTrianle->scale = { 1, 1, 2, 0 };
	debugTrianle->axis = { 1, 0, 1, 1 };
	debugTrianle->theta = 0.0f;
	debugTrianle->shadow = false;
	debugTrianle->dirty = true;
	//pipeline.objects.push_back(debugTrianle);

	// 加载物体 
	pipeline.objects.push_back(nanosuit);
	pipeline.objects.push_back(ground);
	pipeline.objects.push_back(box);
	pipeline.objects.push_back(box1);

	pipeline.render_state = RENDER_STATE_TEXTURE;

	// 渲染主循环
	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		if (screen_keys[VK_SPACE]) {
			if (kbhit == 0) {
				kbhit = 1;
				if (++indicator >= 3) indicator = 0;
				pipeline.render_state = states[indicator];
			}
		} else {
			kbhit = 0;
		}
		float deltaTime = 4.0f / 1000;

		// 控制主相机的运动 Ctrl+
		// w 相机向前, s 相机向后 a 相机向左 d 相机向右 q相机向上 e相机向下
		if (screen_keys[VK_W]) {
			float velocity = c_movementspeed * deltaTime;
			Vector temp = pipeline.mainCamera->front;
			Vector_scale(&temp, velocity);
			Vector_add(&pipeline.mainCamera->pos, &pipeline.mainCamera->pos, &temp);
			pipeline.mainCamera->dirty = true;
		}
		if (screen_keys[VK_S]) {
			float velocity = -c_movementspeed * deltaTime;
			Vector temp = pipeline.mainCamera->front;
			Vector_scale(&temp, velocity);
			Vector_add(&pipeline.mainCamera->pos, &pipeline.mainCamera->pos, &temp);
			pipeline.mainCamera->dirty = true;
		}
		if (screen_keys[VK_A]) {
			float velocity = c_movementspeed * deltaTime;
			Vector temp;
			Vector_crossproduct(&temp, &pipeline.mainCamera->front, &pipeline.mainCamera->worldup);
			Vector_normalize(&temp);
			Vector_scale(&temp, velocity);
			Vector_add(&pipeline.mainCamera->pos, &pipeline.mainCamera->pos, &temp);
			pipeline.mainCamera->dirty = true;
		}
		if (screen_keys[VK_D]) {
			float velocity = -c_movementspeed * deltaTime;
			Vector temp;
			Vector_crossproduct(&temp, &pipeline.mainCamera->front, &pipeline.mainCamera->worldup);
			Vector_normalize(&temp);
			Vector_scale(&temp, velocity);
			Vector_add(&pipeline.mainCamera->pos, &pipeline.mainCamera->pos, &temp);
			pipeline.mainCamera->dirty = true;
		}
		if (screen_keys[VK_Q]) { // 向上
			float velocity = c_movementspeed * deltaTime;
			Vector temp;
			Vector_crossproduct(&temp, &pipeline.mainCamera->front, &pipeline.mainCamera->worldup);
			Vector_normalize(&temp);
			Vector_crossproduct(&temp, &temp, &pipeline.mainCamera->front);
			Vector_normalize(&temp);
			Vector_scale(&temp, velocity);
			Vector_add(&pipeline.mainCamera->pos, &pipeline.mainCamera->pos, &temp);
			pipeline.mainCamera->dirty = true;
		}
		if (screen_keys[VK_E]) {
			float velocity = -c_movementspeed * deltaTime;
			Vector temp;
			Vector_crossproduct(&temp, &pipeline.mainCamera->front, &pipeline.mainCamera->worldup);
			Vector_normalize(&temp);
			Vector_crossproduct(&temp, &temp, &pipeline.mainCamera->front);
			Vector_normalize(&temp);
			Vector_scale(&temp, velocity);
			Vector_add(&pipeline.mainCamera->pos, &pipeline.mainCamera->pos, &temp);
			pipeline.mainCamera->dirty = true;
		}

		if (screen_keys[VK_C]) {
			if (kbhit1 == 0) {
				kbhit1 = 1;
				pipeline.CLIP2D = !pipeline.CLIP2D;
			}
		} else {
			kbhit1 = 0;
		}
		if (screen_keys[VK_B]) {
			if (kbhit2 == 0) {
				kbhit2 = 1;
				pipeline.CULLBACK = !pipeline.CULLBACK;
			}
		} else {
			kbhit2 = 0;
		}

		// 控制某个物体
		if (screen_keys[VK_UP]) {}
		if (screen_keys[VK_DOWN]) {}
		if (screen_keys[VK_LEFT]) {}
		if (screen_keys[VK_RIGHT]) {}

		// auto rotate
		box->theta -= 0.003f;
		box->dirty = true;
		box1->theta += 0.003f;
		box1->dirty = true;

		/// 清空数据
		screen_dispatch();
		pipeline.clear();

		for (int i = 0; i < pipeline.cameras.size(); i++) {
			Camera* camera = pipeline.cameras[i];
			if (camera->main == true) {
				pipeline.cull = 1; // 背面剔除
				//pipeline.setFrameBuffer((IUINT32*)screen_fb);
				pipeline.setFrameBuffer(framebuffer);
				pipeline.setZBuffer(zbuffer);
				pipeline.setShadowBuffer(NULL); // 用于更新shadowmap
				pipeline.useShadowBuffer(shadowbuffer);
			} else {
				// 渲染阴影
				pipeline.cull = 2; // 正面剔除 pite pinning
				pipeline.setFrameBuffer(NULL);
				pipeline.setZBuffer(NULL);
				pipeline.setShadowBuffer(shadowbuffer);
			}

			// 重置空间变换工具
			pipeline.transform.reset();
			// 设置摄像机
			pipeline.setCurCamera(camera);
			// 绘制物体
			pipeline.Draw();
		}

		IUINT32* pFB = (IUINT32*)framebuffer;
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			for (int x = 0; x < SCREEN_WIDTH; x++) {
				IUINT32 color = framebuffer[y * REAL_WIDTH + x];
				screen_fb[y * REAL_WIDTH * 4 + x * 4 + 0] = 0xff & color;
				screen_fb[y * REAL_WIDTH * 4 + x * 4 + 1] = (0xff << 8 & color) >> 8;
				screen_fb[y * REAL_WIDTH * 4 + x * 4 + 2] = (0xff << 16 & color) >> 16;
				screen_fb[y * REAL_WIDTH * 4 + x * 4 + 3] = (0xff << 24 & color) >> 24;
			}
		}
		// 刷新
		screen_update();
		Sleep(1);
	}

	return 0;
}