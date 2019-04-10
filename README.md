
# JayEngine
a soft render based on mini3d and tinyEngine

> [编写光栅渲染器的参考资料](https://dhblooo.github.io/2017/11/15/SoftRendererRefer/)
> [光栅化Demo1](https://www.cnblogs.com/zhangbaochong/p/5751111.html)
> [mini3d](https://github.com/skywind3000/mini3d)
> [tinyEngine](https://github.com/sdlwlxf1/tinyEngine)

---

> [我的CSDN博客链接](https://blog.csdn.net/qjh5606)
> [我的Github项目链接](https://github.com/qjh5606/JayEngine)

--- 
## 概述
- 光栅化是实时渲染领域的核心
- 之前没有系统地学习过图形学,想借由这次做软件渲染器的学习机会了解一些基础的图形学算法.主要是光栅化的过程.
- [D3D和OpenGL坐标系](https://blog.csdn.net/you_lan_hai/article/details/37992123)

## 相关内容

目前涉及到的:
- 渲染管线
    - 顶点着色器 完成的坐标变换
        - **透视投影** 
       - **CVV裁剪** [只有完全落在CVV中的通过]
      - **透视除法**
    - **光栅化**
        - 绘制线框 [绘制直线]
        - 绘制实体图形[光栅化三角形]
 - zbuffer

后续工作:
- [x] 支持模型导入 [obj文件,tinyobjloader]
- [x] 支持纹理输入 [借助stb实现] 
- [x] 背面剔除 
- [x] 裁剪 [2D裁剪][3D视椎体裁剪]
- [x] Phong光照模型
- [x] 阴影

## 2019年4月5日
- 裁剪算法基本实现
![Demo1](https://img-blog.csdnimg.cn/20190405172157547.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FqaDU2MDY=,size_16,color_FFFFFF,t_70)
## 2019年4月7日
- [实现Obj模型导入](https://blog.csdn.net/qjh5606/article/details/89075014)
- 支持纹理读取 

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190407230501509.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FqaDU2MDY=,size_16,color_FFFFFF,t_70)

## 2019年4月9日
- 增加 phong光照模型

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190409150945239.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FqaDU2MDY=,size_16,color_FFFFFF,t_70)


## 2019年4月10日
- ShadowMap 实现阴影
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190410082242930.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FqaDU2MDY=,size_16,color_FFFFFF,t_70)
