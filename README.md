# JayEngine
a soft render based on mini3d

> [编写光栅渲染器的参考资料](https://dhblooo.github.io/2017/11/15/SoftRendererRefer/)
> [光栅化Demo1](https://www.cnblogs.com/zhangbaochong/p/5751111.html)
> [tinyEngine](https://github.com/sdlwlxf1/tinyEngine)

## 概述
- 光栅化是实时渲染领域的核心
- 之前没有系统地学习过图形学,想借由这次做软件渲染器的学习机会了解一些基础的图形学算法.主要是光栅化的过程.
- [D3D和OpenGL坐标系](https://blog.csdn.net/you_lan_hai/article/details/37992123)

## 相关内容

目前涉及到的:
- 各种矩阵的推导
    - 旋转矩阵 如 **绕任意轴旋转的旋转矩阵**  
- 渲染管线
    - 顶点着色器 完成的坐标变换
        - **透视投影**
       - **CVV裁剪** [只有完全落在CVV中的才能通过,可以精细化]
      - **透视除法**
    - **光栅化**
        - 绘制线框 [绘制直线]
        - 绘制实体图形[光栅化三角形]
 - zbuffer
 

后续工作:
- 支持模型导入 [如obj文件]
- 支持纹理输入 
- 隐藏物体 [包围球算法]
- 背面剔除 [法向量]
- **裁剪**
- 图元装配
- 光照模型
- 阴影
- 简单着色器

## 基础图形学算法 
- 画直线的算法
    - DDA
    - [Bresenham算法](https://www.jianshu.com/p/22a3c0ef09ca)
- [光栅化三角形](https://blog.csdn.net/cppyin/article/details/6232453)
