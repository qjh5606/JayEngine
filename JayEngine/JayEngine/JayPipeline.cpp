#include "JayPipeline.h"


// ����Դ
void Pipeline::checkLights() {
	for (int i = 0; i < lights.size(); i++) {
		if (lights[i]->shadow == true) {
			// ����һ�������
		}
	}
}

// ���õ�ǰ����ʹ�õ����
void Pipeline::setCurCamera(Camera* camera) {
	curCamera = camera;
	curCamera->camera_update();
	// ����TransformһЩ����
	transform.view = curCamera->view_matrix;
	transform.view_r = curCamera->view_matrix_r;
	transform.projection = curCamera->projection_matrix;
}

// ������ں���
void Pipeline::Draw() {

	// �������е�����
	for (int i = 0; i < objects.size(); ++i) {

		curObeject = objects[i];
		curObeject->updateModelMatrix();

		// ����transform
		transform.model = curObeject->model_matrix;
		transform.update_transform();

		Vertex *mesh = curObeject->model->mesh;
		int mesh_num = curObeject->model->mesh_num;

		for (int j = 0; j < mesh_num; j += 3) {
			clip_polys(&mesh[j], &mesh[j + 1], &mesh[j + 2], false);
		}
	}
}




// Cohen�CSutherland��һ���߶βü��㷨
typedef int OutCode;
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

int xmin = 0;
int xmax = 600;
int ymin = 0;
int ymax = 600;

OutCode ComputeOutCode(Vertex *v) {
	int x = v->pos.x;
	int y = v->pos.y;
	OutCode code;
	code = INSIDE;
	if (x < xmin)           // to the left of clip window
		code |= LEFT;
	else if (x > xmax)      // to the right of clip window
		code |= RIGHT;
	if (y < ymin)           // above the clip window
		code |= TOP;
	else if (y > ymax)      // under the clip window
		code |= BOTTOM;
	return code;
}

// 
bool PointCmp(const Vertex &a, const Vertex &b, double &x, double &y) {
	double det = (a.pos.x - x) * (b.pos.y - y) - (b.pos.x - x) * (a.pos.y - y);
	if (det < 0)
		return true;
	if (det > 0)
		return false;
	double d1 = (a.pos.x - x) * (a.pos.x - x) + (a.pos.y - y) * (a.pos.y - y);
	double d2 = (b.pos.x - x) * (b.pos.x - x) + (b.pos.y - y) * (b.pos.y - y);
	return d1 > d2;
}

void Pipeline::sortVertexs(std::vector<Vertex> &VertexSet, int size) {

	// 1. ��������
	double x = 0, y = 0;
	for (int i = 0; i < size; i++) {
		x += VertexSet[i].pos.x;
		y += VertexSet[i].pos.y;
	}
	x /= size;
	y /= size;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size - i - 1; j++) {
			if (PointCmp(VertexSet[j], VertexSet[j + 1], x, y)) {
				Vertex tmp = VertexSet[j];
				VertexSet[j] = VertexSet[j + 1];
				VertexSet[j + 1] = tmp;
			}
		}
	}
}

void Pipeline::clip_lines(Vertex v1, Vertex v2, std::vector<Vertex> &VertexSet) {

	// �����㷨
	// https://blog.csdn.net/qq_32126633/article/details/78724987
	Vertex *t1 = &v1;
	Vertex *t2 = &v2;

	// ����������.��һ���δ���.����2D�ռ�Ĳü�
	OutCode outcode0 = ComputeOutCode(t1);
	OutCode outcode1 = ComputeOutCode(t2);

	bool accept = false;

	while (true) {
		// ������Ļ��
		if (!(outcode0 | outcode1)) {
			accept = true;
			break;
		}
		// ����ƽ����,�����㶼����Ļ��
		else if (outcode0&outcode1) {
			accept = false;
			break;
		} else {
			// �ҳ�����ĵ�
			OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

			// ���²ü��ĵ�
			Vertex clipV;
			// clipV = (1-alpha)*t1 + alpha*t2
			if (outcodeOut&TOP) {
				// ����alphaֵ
				float alpha = (ymin - t1->pos.y) / (t2->pos.y - t1->pos.y);
				vertex_interp(&clipV, t1, t2, alpha);
				clipV.pos.y = ymin;
			} else if (outcodeOut&BOTTOM) {
				float alpha = (ymax - t1->pos.y) / (t2->pos.y - t1->pos.y);
				vertex_interp(&clipV, t1, t2, alpha);
				clipV.pos.y = ymax;
			} else if (outcodeOut&LEFT) {
				float alpha = (xmin - t1->pos.x) / (t2->pos.x - t1->pos.x);
				vertex_interp(&clipV, t1, t2, alpha);
				clipV.pos.x = xmin;
			} else {
				float alpha = (xmax - t1->pos.x) / (t2->pos.x - t1->pos.x);
				vertex_interp(&clipV, t1, t2, alpha);
				clipV.pos.x = xmax;
			}

			if (outcodeOut == outcode0) {
				*t1 = clipV;
				outcode0 = ComputeOutCode(t1);
			} else {
				*t2 = clipV;
				outcode1 = ComputeOutCode(t2);
			}
		}

	}

	if (accept) {
		auto it = find(VertexSet.begin(), VertexSet.end(), *t1);
		if (it == VertexSet.end()) {
			VertexSet.push_back(*t1);
		}
		it = find(VertexSet.begin(), VertexSet.end(), *t2);
		if (it == VertexSet.end()) {
			VertexSet.push_back(*t2);
		}

	}

}


// ���� �������������t1,t2,t3 ������������ϵ
void Pipeline::device_draw_primitive(Vertex* t1, Vertex* t2, Vertex* t3) {

	Vertex *vertice[3] = { t1,t2,t3 };
	Point points[3];

	// ͶӰ δ����͸�ӳ���
	for (int i = 0; i < 3; i++) {
		auto vertex = vertice[i];
		Matrix_apply(&vertex->pos, &vertex->pos, &transform.vp);
		points[i] = vertex->pos; // ͸�ӿռ��pos,������w����
	}

	// ��cvv�ü�,�жϵ�ȫ������ -w<= x,y <= w ; 0<=z<=w
	// �ɽ�һ����ϸ�ü�����һ���ֽ�Ϊ������ȫ���� cvv�ڵ�������
#define SimpleCVVCut 0
#if SimpleCVVCut 
	if (transform.transform_check_cvv(&points[0]) != 0) return;
	if (transform.transform_check_cvv(&points[1]) != 0) return;
	if (transform.transform_check_cvv(&points[2]) != 0) return;
#endif

	// ͸�ӳ��� + �ӿڱ仯
	// x,y->��Ļ����
	// z-> z/w [0..1]
	// w�����1/w;
	for (int i = 0; i < 3; i++) {
		auto vertex = vertice[i];
		transform.transform_homogenize(&vertex->pos, &vertex->pos);
	}

	// �����޳�
	if (CULLBACK) {
		if (this->cull > 0) {
			Point t21, t32;
			Vector_sub(&t21, &t2->pos, &t1->pos);
			Vector_sub(&t32, &t3->pos, &t2->pos);
			if (this->cull == 1) {
				if (t21.x * t32.y - t32.x * t21.y <= 0)    // ������
					return;
			} else if (this->cull == 2) {
				if (t21.x * t32.y - t32.x * t21.y > 0)     // ������
					return;
			}
		}
	}

	// 2Dͼ��ռ�ü�
	if (CLIP2D) {
		// 2Dͼ��ռ�ü��㷨: ����������.��һ���δ���.����2D�ռ�Ĳü�
		std::vector<Vertex> VertexSet;// �����ظ��ĵ�

		// ����1:��ÿ���߽��вü�		
		clip_lines(*t1, *t2, VertexSet);
		clip_lines(*t1, *t3, VertexSet);
		clip_lines(*t3, *t2, VertexSet);

		auto end = std::unique(VertexSet.begin(), VertexSet.end());
		int size = end - VertexSet.begin();
		if (size <= 2) {
			return;
		}

		// ������Ҫ�Զ���εĵ㼯�Ͻ�������.Ҫô˳��ʱ��,Ҫô��ʱ��
		// http://www.cnblogs.com/dwdxdy/p/3230156.html
		sortVertexs(VertexSet, size);

		Vertex source = VertexSet[0];
		for (int i = 1; i < size - 1; i++) {
			t1 = &source;
			t2 = &VertexSet[i];
			t3 = &VertexSet[i + 1];
			// ������Ļ������в���
			// ���������Ҫ�õ���������ϵ
			//this->render_state = RENDER_STATE_TEXTURE;
			if (this->render_state & (RENDER_STATE_TEXTURE | RENDER_STATE_COLOR)) {
				Trapezoid traps[2];

				int n = trapezoid_init_triangle(traps, t1, t2, t3);
				// 0:ƽ��,1:ƽ��
				if (n >= 1) device_render_trap(&traps[0]);
				if (n >= 2) device_render_trap(&traps[1]);
			}

			if ((this->render_state & RENDER_STATE_WIREFRAME) && this->framebuffer != NULL) {
				draw_line((int)t1->pos.x, (int)t1->pos.y, (int)t2->pos.x, (int)t2->pos.y, foreground);
				draw_line((int)t1->pos.x, (int)t1->pos.y, (int)t3->pos.x, (int)t3->pos.y, foreground);
				draw_line((int)t3->pos.x, (int)t3->pos.y, (int)t2->pos.x, (int)t2->pos.y, foreground);
			}
		}
	} else { // # CLIP2D
		// ������Ļ������в���
		// ���������Ҫ�õ���������ϵ
		if (this->render_state & (RENDER_STATE_TEXTURE | RENDER_STATE_COLOR)) {
			Trapezoid traps[2];

			int n = trapezoid_init_triangle(traps, t1, t2, t3);
			// 0:ƽ��,1:ƽ��
			if (n >= 1) device_render_trap(&traps[0]);
			if (n >= 2) device_render_trap(&traps[1]);
		}

		if ((this->render_state & RENDER_STATE_WIREFRAME) && this->framebuffer != NULL) {
			draw_line((int)t1->pos.x, (int)t1->pos.y, (int)t2->pos.x, (int)t2->pos.y, foreground);
			draw_line((int)t1->pos.x, (int)t1->pos.y, (int)t3->pos.x, (int)t3->pos.y, foreground);
			draw_line((int)t3->pos.x, (int)t3->pos.y, (int)t2->pos.x, (int)t2->pos.y, foreground);
		}
	} // # CLIP2D
}

// �ֽ�������
int Pipeline::trapezoid_init_triangle(Trapezoid *trap, const Vertex *p1, const Vertex *p2, const Vertex *p3) {
	const Vertex *p;
	float k, x;

	if (p1->pos.y > p2->pos.y) p = p1, p1 = p2, p2 = p;
	if (p1->pos.y > p3->pos.y) p = p1, p1 = p3, p3 = p;
	if (p2->pos.y > p3->pos.y) p = p2, p2 = p3, p3 = p;
	if (p1->pos.y == p2->pos.y && p1->pos.y == p3->pos.y) return 0;
	if (p1->pos.x == p2->pos.x && p1->pos.x == p3->pos.x) return 0;

	if (p1->pos.y == p2->pos.y) {	// triangle down
		if (p1->pos.x > p2->pos.x) p = p1, p1 = p2, p2 = p;
		trap[0].top = p1->pos.y;
		trap[0].bottom = p3->pos.y;
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p3;
		trap[0].right.v1 = *p2;
		trap[0].right.v2 = *p3;
		return (trap[0].top < trap[0].bottom) ? 1 : 0;
	}

	if (p2->pos.y == p3->pos.y) {	// triangle up
		if (p2->pos.x > p3->pos.x) p = p2, p2 = p3, p3 = p;
		trap[0].top = p1->pos.y;
		trap[0].bottom = p3->pos.y;
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p2;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p3;
		return (trap[0].top < trap[0].bottom) ? 1 : 0;
	}

	trap[0].top = p1->pos.y;
	trap[0].bottom = p2->pos.y;
	trap[1].top = p2->pos.y;
	trap[1].bottom = p3->pos.y;

	k = (p3->pos.y - p1->pos.y) / (p2->pos.y - p1->pos.y);
	x = p1->pos.x + (p2->pos.x - p1->pos.x) * k;

	if (x <= p3->pos.x) {		// triangle left
		trap[0].left.v1 = *p1;
		trap[0].left.v2 = *p2;
		trap[0].right.v1 = *p1;
		trap[0].right.v2 = *p3;
		trap[1].left.v1 = *p2;
		trap[1].left.v2 = *p3;
		trap[1].right.v1 = *p1;
		trap[1].right.v2 = *p3;
	} else {					// triangle right
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

// �����ֵ
void Pipeline::vertex_interp(Vertex *y, const Vertex *x1, const Vertex *x2, float k) {
	Vector_interp(&y->pos, &x1->pos, &x2->pos, k);
	y->tc.u = interp(x1->tc.u, x2->tc.u, k);
	y->tc.v = interp(x1->tc.v, x2->tc.v, k);
	y->color.r = interp(x1->color.r, x2->color.r, k);
	y->color.g = interp(x1->color.g, x2->color.g, k);
	y->color.b = interp(x1->color.b, x2->color.b, k);
}


// ���� Y ��������������������������� Y �Ķ���
void Pipeline::trapezoid_edge_interp(Trapezoid *trap, float y) {
	float s1 = trap->left.v2.pos.y - trap->left.v1.pos.y;
	float s2 = trap->right.v2.pos.y - trap->right.v1.pos.y;
	float t1 = (y - trap->left.v1.pos.y) / s1;
	float t2 = (y - trap->right.v1.pos.y) / s2;
	vertex_interp(&trap->left.v, &trap->left.v1, &trap->left.v2, t1);
	vertex_interp(&trap->right.v, &trap->right.v1, &trap->right.v2, t2);

	//left:
	//cout << "left " << endl;
	//cout << "v1.y "<< trap->left.v1.pos.y << ends << "v2.y "<< trap->left.v2.pos.y << endl;
	//cout << "v.y " << y << endl;
	//cout << "t1 " << t1<<endl;
	// right
	//cout << "right " << endl;
	//cout << "v1.y " << trap->right.v1.pos.y << ends << "v2.y " << trap->right.v2.pos.y << endl;
	//cout << "v.y " << y << endl;
	//cout << "t2 " << t2 << endl;
}

// �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ���
void  Pipeline::trapezoid_init_scan_line(const Trapezoid *trap, Scanline *scanline, int y) {
	// ������
	float width = trap->right.v.pos.x - trap->left.v.pos.x;
	// ��ʼx
	scanline->x = (int)(trap->left.v.pos.x + 0.5f);
	// ɨ����
	scanline->w = (int)(trap->right.v.pos.x + 0.5f) - scanline->x;
	if (trap->left.v.pos.x >= trap->right.v.pos.x)
		scanline->w = 0;
	// ��һ��
	scanline->y = y;
	// ����ߵĶ���
	scanline->v = trap->left.v;
	// ������step
	vertex_division(&scanline->step, &trap->left.v, &trap->right.v, width);
}

//  �������ݳ�����step
void Pipeline::vertex_division(Vertex *y, const Vertex *x1, const Vertex *x2, float w) {
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
}

// ����
void Pipeline::vertex_add(Vertex *y, const Vertex *x) {
	y->pos.x += x->pos.x;
	y->pos.y += x->pos.y;
	y->pos.z += x->pos.z;
	y->pos.w += x->pos.w;
	y->tc.u += x->tc.u;
	y->tc.v += x->tc.v;
	y->color.r += x->color.r;
	y->color.g += x->color.g;
	y->color.b += x->color.b;
}

// ����ɨ����
void Pipeline::device_draw_scanline(Scanline* scanline) {

	int y = scanline->y;
	int x = scanline->x; // ��ʼx
	int scanline_w = scanline->w; // ���

	// �������������,����Ӱ֡�������Ϊ0
	IUINT32 *f = framebuffer + y*this->width;
	float *z = zbuffer + y*this->width;

	Texture* tex = curObeject->texture;

	for (; scanline_w > 0; x++, scanline_w--) {
		if (x >= 0 && x < width) {
			float rhw = scanline->v.pos.w;
			// 1/w buffer
			if (rhw >= z[x]) {
				z[x] = rhw;
				//float w = 1.0f / rhw; // ͸�ӳ���ǰ��w
				// ������ɫ��ֵ
				if (render_state&RENDER_STATE_COLOR) {
					float r = scanline->v.color.r;
					float g = scanline->v.color.g;
					float b = scanline->v.color.b;
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
					// u,v [0,1],���Էֱ���,�����������
					float u = scanline->v.tc.u;
					float v = scanline->v.tc.v;

					IUINT32 cc = 0x2F4F4F;
					if (tex != NULL) {
						cc = tex->readTexture(u, v);
						//cout << u << ends << v << ends << cc<< endl;
					}
					f[x] = cc;// �������
				}
			}

		}
		// ��������
		vertex_add(&scanline->v, &scanline->step);
		if (x >= width) break;
	}
}

// ����������
void Pipeline::device_render_trap(Trapezoid *trap) {
	// ɨ���߲�ֵ
	Scanline scanline;
	int j, top, bottom;
	top = (int)(trap->top + 0.5f);
	bottom = (int)(trap->bottom + 0.5f);

	for (j = top; j < bottom; ++j) {
		// �Ϸ�
		//j = (top + bottom) / 2;
		if (j >= 0 && j < this->curCamera->height) {
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

// �����߶� Bresenham�㷨(����ɭ�hķֱ��)
void Pipeline::draw_line(int x1, int y1, int x2, int y2, IUINT32 c) {
#define Bresenham 0
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
void Pipeline::device_pixel(int x, int y, IUINT32 color) {
	if (x >= 0 && x < width && y >= 0 && y < height) {
		framebuffer[y*width + x] = color;
	}
}

// �� 3D���ü���ü�. ������ռ����
// v1 v2 v3 ��ģ������
void Pipeline::clip_polys(Vertex* v1, Vertex* v2, Vertex* v3, bool world) {

	Vertex p1 = *v1, p2 = *v2, p3 = *v3;
#define CLIP3D 1
#if CLIP3D // 3D��׵��ü�. ���ü���ü�
#define CLIP_CODE_GZ    0x0001
#define CLIP_CODE_LZ    0x0002
#define CLIP_CODE_IZ    0x0004

#define CLIP_CODE_GX    0x0001
#define CLIP_CODE_LX    0x0002
#define CLIP_CODE_IX    0x0004

#define CLIP_CODE_GY    0x0001
#define CLIP_CODE_LY    0x0002
#define CLIP_CODE_IY    0x0004

#define CLIP_CODE_NULL  0x0000

	int vertex_ccodes[3];
	int num_verts_in = 0;

	float z_factor_x, z_factor_y, z_factor, z_test;

	float xi, yi, x01i, y01i, x02i, y02i, t1, t2, ui, vi, u01i, v01i, u02i, v02i;

	bool cliped = false;

	Vector v;

	if (world == false) {
		Matrix_apply(&p1.pos, &p1.pos, &this->transform.mv);
		Matrix_apply(&p2.pos, &p2.pos, &this->transform.mv);
		Matrix_apply(&p3.pos, &p3.pos, &this->transform.mv);
	} else {
		Matrix_apply(&p1.pos, &p1.pos, &this->transform.view);
		Matrix_apply(&p2.pos, &p2.pos, &this->transform.view);
		Matrix_apply(&p3.pos, &p3.pos, &this->transform.view);
	}

	z_factor_y = tan(this->curCamera->fovy*0.5);
	z_factor_x = z_factor_y / this->curCamera->aspect;
	z_factor = z_factor_x;
	z_test = z_factor * p1.pos.z;

	if (p1.pos.x > z_test)
		vertex_ccodes[0] = CLIP_CODE_GX;
	else if (p1.pos.x < -z_test)
		vertex_ccodes[0] = CLIP_CODE_LX;
	else
		vertex_ccodes[0] = CLIP_CODE_IX;

	z_test = z_factor * p2.pos.z;

	if (p2.pos.x > z_test)
		vertex_ccodes[1] = CLIP_CODE_GX;
	else if (p2.pos.x < -z_test)
		vertex_ccodes[1] = CLIP_CODE_LX;
	else
		vertex_ccodes[1] = CLIP_CODE_IX;

	z_test = z_factor * p3.pos.z;

	if (p3.pos.x > z_test)
		vertex_ccodes[2] = CLIP_CODE_GX;
	else if (p3.pos.x < -z_test)
		vertex_ccodes[2] = CLIP_CODE_LX;
	else
		vertex_ccodes[2] = CLIP_CODE_IX;

	if (((vertex_ccodes[0] == CLIP_CODE_GX) && (vertex_ccodes[1] == CLIP_CODE_GX) && (vertex_ccodes[2] == CLIP_CODE_GX))
		|| ((vertex_ccodes[0] == CLIP_CODE_LX) && (vertex_ccodes[1] == CLIP_CODE_LX) && (vertex_ccodes[2] == CLIP_CODE_LX))) {
		return;
	}

	z_factor = z_factor_y;
	z_test = z_factor * p1.pos.z;

	if (p1.pos.y > z_test)
		vertex_ccodes[0] = CLIP_CODE_GY;
	else if (p1.pos.y < -z_test)
		vertex_ccodes[0] = CLIP_CODE_LY;
	else
		vertex_ccodes[0] = CLIP_CODE_IY;

	z_test = z_factor * p2.pos.z;

	if (p2.pos.y > z_test)
		vertex_ccodes[1] = CLIP_CODE_GY;
	else if (p2.pos.y < -z_test)
		vertex_ccodes[1] = CLIP_CODE_LY;
	else
		vertex_ccodes[1] = CLIP_CODE_IY;

	z_test = z_factor * p3.pos.z;

	if (p3.pos.y > z_test)
		vertex_ccodes[2] = CLIP_CODE_GY;
	else if (p3.pos.y < -z_test)
		vertex_ccodes[2] = CLIP_CODE_LY;
	else
		vertex_ccodes[2] = CLIP_CODE_IY;

	if (((vertex_ccodes[0] == CLIP_CODE_GY) && (vertex_ccodes[1] == CLIP_CODE_GY) && (vertex_ccodes[2] == CLIP_CODE_GY))
		|| ((vertex_ccodes[0] == CLIP_CODE_LY) && (vertex_ccodes[1] == CLIP_CODE_LY) && (vertex_ccodes[2] == CLIP_CODE_LY))) {
		return;
	}

	// �Ƿ���ȫ��Զ�ü������ü������
	if (p1.pos.z > this->curCamera->zf)
		vertex_ccodes[0] = CLIP_CODE_GZ;
	else if (p1.pos.z < this->curCamera->zn)
		vertex_ccodes[0] = CLIP_CODE_LZ;
	else {
		vertex_ccodes[0] = CLIP_CODE_IZ;
		num_verts_in++;
	}

	if (p2.pos.z > this->curCamera->zf)
		vertex_ccodes[1] = CLIP_CODE_GZ;
	else if (p2.pos.z < this->curCamera->zn)
		vertex_ccodes[1] = CLIP_CODE_LZ;
	else {
		vertex_ccodes[1] = CLIP_CODE_IZ;
		num_verts_in++;
	}


	if (p3.pos.z > this->curCamera->zf)
		vertex_ccodes[2] = CLIP_CODE_GZ;
	else if (p3.pos.z < this->curCamera->zn)
		vertex_ccodes[2] = CLIP_CODE_LZ;
	else {
		vertex_ccodes[2] = CLIP_CODE_IZ;
		num_verts_in++;
	}


	if (((vertex_ccodes[0] == CLIP_CODE_GZ) && (vertex_ccodes[1] == CLIP_CODE_GZ) && (vertex_ccodes[2] == CLIP_CODE_GZ))
		|| ((vertex_ccodes[0] == CLIP_CODE_LZ) && (vertex_ccodes[1] == CLIP_CODE_LZ) && (vertex_ccodes[2] == CLIP_CODE_LZ))) {
		return;
	}

	// �ж��Ƿ��ж����ڽ��ü������
	if (((vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ)) {
		Vertex temp;
		//num_verts_in = 0;
		// ��������1�������ڽ��ü����ڲ࣬2�����������
		// ��������2�������ڽ��ü����ڲ࣬1�����������
		if (num_verts_in == 1) {
			if (vertex_ccodes[0] == CLIP_CODE_IZ) {
			} else if (vertex_ccodes[1] == CLIP_CODE_IZ) {
				temp = p1;
				p1 = p2;
				p2 = p3;
				p3 = temp;
			} else {
				temp = p1;
				p1 = p3;
				p3 = p2;
				p2 = temp;
			}
			// ��ÿ���߽��вü�
			// ��������������p = v0 + v01 * t
			Vector_sub(&v, &p2.pos, &p1.pos);
			t1 = (this->curCamera->zn - p1.pos.z) / v.z;

			xi = p1.pos.x + v.x * t1;
			yi = p1.pos.y + v.y * t1;

			// �ý��㸲��ԭ���Ķ���
			p2.pos.x = xi;
			p2.pos.y = yi;
			p2.pos.z = this->curCamera->zn;

			// �������α�v0->v2���вü�
			Vector_sub(&v, &p3.pos, &p1.pos);
			t2 = (this->curCamera->zn - p1.pos.z) / v.z;

			xi = p1.pos.x + v.x * t2;
			yi = p1.pos.y + v.y * t2;

			// �ý��㸲��ԭ���Ķ���
			p3.pos.x = xi;
			p3.pos.y = yi;
			p3.pos.z = this->curCamera->zn;

			// ��������вü�
			ui = p1.tc.u + (p2.tc.u - p1.tc.u) * t1;
			vi = p1.tc.v + (p2.tc.v - p1.tc.v) * t1;

			p2.tc.u = ui;
			p2.tc.v = vi;

			ui = p1.tc.u + (p3.tc.u - p1.tc.u) * t2;
			vi = p1.tc.v + (p3.tc.v - p1.tc.v) * t2;

			p3.tc.u = ui;
			p3.tc.v = vi;

			cliped = true;
		} else if (num_verts_in == 2) {
			// ���ĵ�
			if (vertex_ccodes[0] == CLIP_CODE_LZ) {
			} else if (vertex_ccodes[1] == CLIP_CODE_LZ) {
				temp = p1;
				p1 = p2;
				p2 = p3;
				p3 = temp;
			} else {
				temp = p1;
				p1 = p3;
				p3 = p2;
				p2 = temp;
			}

			Vertex np1 = p1, np2 = p2, np3 = p3;
			// ��ÿ���߽��вü�
			// ��������������p = v0 + v01 * t
			Vector_sub(&v, &p2.pos, &p1.pos);
			t1 = (this->curCamera->zn - p1.pos.z) / v.z;

			x01i = p1.pos.x + v.x * t1;
			y01i = p1.pos.y + v.y * t1;

			// �������α�v0->v2���вü�
			Vector_sub(&v, &p3.pos, &p1.pos);
			t2 = (this->curCamera->zn - p1.pos.z) / v.z;

			x02i = p1.pos.x + v.x * t2;
			y02i = p1.pos.y + v.y * t2;

			// �ý��㸲��ԭ���Ķ���
			p1.pos.x = x01i;
			p1.pos.y = y01i;
			p1.pos.z = this->curCamera->zn;

			np2.pos.x = x01i;
			np2.pos.y = y01i;
			np2.pos.z = this->curCamera->zn;

			np1.pos.x = x02i;
			np1.pos.y = y02i;
			np1.pos.z = this->curCamera->zn;

			// ��������вü�
			u01i = p1.tc.u + (p2.tc.u - p1.tc.u) * t1;
			v01i = p1.tc.v + (p2.tc.v - p1.tc.v) * t1;

			u02i = p1.tc.u + (p3.tc.u - p1.tc.u) * t2;
			v02i = p1.tc.v + (p3.tc.v - p1.tc.v) * t2;

			p1.tc.u = u01i;
			p1.tc.v = v01i;

			np1.tc.u = u02i;
			np1.tc.v = v02i;
			np2.tc.u = u01i;
			np2.tc.v = v01i;

			Matrix_apply(&np1.pos, &np1.pos, &this->transform.view_r);
			Matrix_apply(&np2.pos, &np2.pos, &this->transform.view_r);
			Matrix_apply(&np3.pos, &np3.pos, &this->transform.view_r);
			device_draw_primitive(&np1, &np2, &np3);

			cliped = true;
		}
	}

	// view�������
	Matrix_apply(&p1.pos, &p1.pos, &this->transform.view_r);
	Matrix_apply(&p2.pos, &p2.pos, &this->transform.view_r);
	Matrix_apply(&p3.pos, &p3.pos, &this->transform.view_r);
#else
	Matrix_apply(&p1.pos, &p1.pos, &transform.model);
	Matrix_apply(&p2.pos, &p2.pos, &transform.model);
	Matrix_apply(&p3.pos, &p3.pos, &transform.model);
#endif //#if CLIP
	// ��������
	// p1,p2,p3������ռ�
	device_draw_primitive(&p1, &p2, &p3);
}


// ����
void Pipeline::clear() {
	// TODO:
	//this->framebuffer = (IUINT32*)fb;
	if (this->framebuffer != NULL) {
		for (int y = 0; y < this->height; y++)
			for (int x = 0; x < this->width; x++)
				this->framebuffer[y * width + x] = this->background;
	}
	if (this->zbuffer != NULL)
		memset(this->zbuffer, 0, width * height * sizeof(float));
	if (this->shadowbuffer != NULL) {
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				this->shadowbuffer[y * width + x] = 1.0f;
	}
}


