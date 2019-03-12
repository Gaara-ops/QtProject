#ifndef __CPU_BITMAP_H__
#define __CPU_BITMAP_H__

#include <stdio.h>
#include <stdlib.h>
#include <gl\glut.h>

//������һ������ CPUBitmap�Ľṹ��

struct CPUBitmap
{
	unsigned char *pixels;	//���ص���ܸ���
	int x, y;	//ͼ��ĳ���
	void *dataBlock;              
	void(*bitmapExit)(void*); 

	CPUBitmap( int width, int height, void *d = NULL ) 
	{
		pixels = new unsigned char[width * height * 4];	//�����ܵ����ص������������µĿռ�
		x = width;	//ͼ��Ŀ�
		y = height;	//ͼ��ĸ�
		dataBlock = d;                                                                                                           
	}

	//��������
	~CPUBitmap() 
	{
		//ɾ�����ص�
		delete [] pixels;        
	}
	//ȡ���������ص�        
	unsigned char* get_ptr( void ) const   { return pixels; }
	
	//ȡ��ͼƬ�ܴ�С
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
		
		// ������ͨ��������������Key���� 
		glutKeyboardFunc(Key);
		//����һ������,����Ҫ���л�ͼʱ,��������ͻᱻ���á�
		glutDisplayFunc(Draw);
		//��ʾ���� 
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
		// �������������Esc���������˳�����
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
	// ��ͼ 
	static void Draw( void )
	{
		CPUBitmap*   bitmap = *(get_bitmap_ptr());
		//���ñ�����ɫ
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		//���
		glClear( GL_COLOR_BUFFER_BIT );
		//�������ص�
		glDrawPixels( bitmap->x, bitmap->y, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels );
		
		//��֤ǰ��� OpenGL ��������ִ��(�������������ڻ������еȴ�)�������ø� fflush(stdout)���ơ�
		glFlush();
	}
};

#endif  // __CPU_BITMAP_H__
