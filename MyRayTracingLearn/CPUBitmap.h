#ifndef __CPU_BITMAP_H__
#define __CPU_BITMAP_H__

#include <stdio.h>
#include <stdlib.h>
#include <gl\glut.h>

//创建了一个叫做 CPUBitmap的结构体

struct CPUBitmap
{
	unsigned char *pixels;	//像素点的总个数
	int x, y;	//图像的长宽
	void *dataBlock;              
	void(*bitmapExit)(void*); 

	CPUBitmap( int width, int height, void *d = NULL ) 
	{
		pixels = new unsigned char[width * height * 4];	//计算总的像素点数，并分配新的空间
		x = width;	//图像的宽
		y = height;	//图像的高
		dataBlock = d;                                                                                                           
	}

	//析构函数
	~CPUBitmap() 
	{
		//删除像素点
		delete [] pixels;        
	}
	//取得所有像素点        
	unsigned char* get_ptr( void ) const   { return pixels; }
	
	//取得图片总大小
	long image_size( void ) const { return x * y * 4; }
	void display_and_exit( void(*e)(void*) = NULL )
	{
		CPUBitmap**   bitmap = get_bitmap_ptr(); 
		*bitmap = this;
		bitmapExit = e;
		// a bug in the Windows GLUT implementation prevents us from
		// passing zero arguments to glutInit()
		int c=1;
		char* dummy = "";
		
		glutInit( &c, &dummy );         
		
		//glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA ); 
		glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA ); 
		
		glutInitWindowSize( x, y );

		glutCreateWindow( "bitmap" );
		
		// 当有普通按键被按，调用Key函数 
		glutKeyboardFunc(Key);
		//设置一个函数,当需要进行画图时,这个函数就会被调用。
		glutDisplayFunc(Draw);
		//显示窗口 
		glutMainLoop();
	}

	// static method used for glut callbacks
	static CPUBitmap** get_bitmap_ptr( void )
	{
		static CPUBitmap   *gBitmap;
		return &gBitmap;
	}
	
	// static method used for glut callbacks
	static void Key(unsigned char key, int x, int y)
	{
		// 如果按键按的是Esc按键，则退出程序。
		switch (key)
		{
		case 27:
			CPUBitmap*   bitmap = *(get_bitmap_ptr());
			if (bitmap->dataBlock != NULL && bitmap->bitmapExit != NULL)
				bitmap->bitmapExit( bitmap->dataBlock );
			exit(0);
		}
	}
	// static method used for glut callbacks
	// 画图 
	static void Draw( void )
	{
		CPUBitmap*   bitmap = *(get_bitmap_ptr());
		//设置背景颜色
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		//清除
		glClear( GL_COLOR_BUFFER_BIT );
		//绘制像素点
		glDrawPixels( bitmap->x, bitmap->y, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels );
		
		//保证前面的 OpenGL 命令立即执行(而不是让它们在缓冲区中等待)。其作用跟 fflush(stdout)类似。
		glFlush();
	}
};

#endif  // __CPU_BITMAP_H__
