
#if !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
#define AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "stdafx.h"
#include "resource.h"
#include <stdio.h>
#include <math.h>
#include <process.h>

#define PI 3.14159265

typedef unsigned char Byte;
typedef short TwoByte;
// Class structure of Image 

class MyStorage
{
private:
	TwoByte *str;
public:
	~MyStorage() { if(str) delete[] str; };
	void setStorage(TwoByte* s) { str = s; };
};


class MyImage 
{

private:
	int		Width;
	int		Height;
	char	ImagePath[_MAX_PATH];
	Byte*	Data;

public:
	MyImage() {};
	~MyImage() { if(Data) delete[] Data; };
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
void encode(TwoByte* s);
void doDCT(Byte* src, double* dest);

void sendDecodeSequentialMode(void* storage);
void sendSpectralSelection(void* storage);
void decodeProgressiveModeSpectralSelection(TwoByte* st);
void sendSuccessiveBits(void* storage);
void decodeProgressiveModeBitApproximation(TwoByte* st, TwoByte offset);

void doIDCT(TwoByte* src, Byte* dest);

void drawImage(MyImage* i, int n);


#endif // !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
