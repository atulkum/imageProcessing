
#if !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
#define AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
typedef unsigned char Byte;
// Class structure of Image 
class MyImage 
{

private:
	int		Width;
	int		Height;
	char	ImagePath[_MAX_PATH];
	Byte*	Data;

public:
	MyImage() {};
	~MyImage() { if(Data) delete Data; };
	void	setWidth(int w)  { Width = w; }; 
	void	setHeight(int h) { Height = h; }; 
	void	setImageData(Byte *img ) { Data = img; };
	void	setImagePath(char *path) { strcpy(ImagePath, path); }
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	Byte*	getImageData() { return Data; };
	char*	getImagePath() { return ImagePath; }
	void	ReadImage();	
};

#endif // !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
