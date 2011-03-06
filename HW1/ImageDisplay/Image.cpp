// Image.cpp : Defines the entry point for the application.
//

#include "Image.h"
#include <math.h>


MyImage myImage;
int sample;
int theta;
int antiAliasing;

#define MAX_LOADSTRING 100
#define PI 3.14159265

void subSampleImage(MyImage* originalImage,int n);
void rotateImage(MyImage* originalImage, int theta);
void doAntiAliasing(MyImage* originalImage, int q);


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	
	int w, h;	
	char ImagePath[_MAX_PATH];
	sscanf(lpCmdLine, "%s %d %d %d", &ImagePath, &sample, &theta, &antiAliasing);
	w=h=512; //assuming all images are of size 512*512
	//////////////////////////////////////////////////
	
	char toprint[50];
	
	if(sample > h){
		sample = 1;
	}
	if(sample < 0){
		sprintf(toprint,"Sample should be greater than 0.");
		MessageBox(NULL, toprint, NULL, NULL);
		return FALSE;
	}
	
	theta = (theta + 360)%360;
	
	if(antiAliasing != 0 && antiAliasing != 1){
		sprintf(toprint,"Anti Aliasing can be 0 or 1.");
		MessageBox(NULL, toprint, NULL, NULL);
		return FALSE;
	}

	//strcpy(ImagePath, "C:\\uscProjects\\576\\HW1\\image\\grid_16.rgb"); 
	//sample = 1;
	//theta = 20;
	//antiAliasing = 0;
	//////////////////////////////////////////////////

	myImage.setWidth(w);
	myImage.setHeight(h);
	myImage.setImagePath(ImagePath);
	myImage.ReadImage();
	/*char toprint[50];
	sprintf(toprint," %d %d %d",antiAliasing, sample, theta);
	MessageBox(NULL, toprint, NULL, NULL);*/

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				RECT rt;
				GetClientRect(hWnd, &rt);
				//char text[1000];
				//strcpy(text, "Display original image and Image after Compression/Decompression here\n");
				//DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				//strcpy(text, "\nFor now both display the original image");
				//DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = myImage.getWidth();
				bmi.bmiHeader.biHeight = -myImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = myImage.getWidth()*myImage.getHeight();
				
				SetDIBitsToDevice(hdc,
					0,0,myImage.getWidth(),myImage.getHeight(),
					0,0,0,myImage.getHeight(),
					myImage.getImageData(),&bmi,DIB_RGB_COLORS);				
				int nextStart = myImage.getWidth()+50;
				int height = myImage.getHeight();
				int width = myImage.getWidth();
				
				MyImage img;
				img.setHeight(height);
				img.setWidth(width);
				Byte* imgd = new Byte[height*width*3];
				memset(imgd, 0, sizeof(Byte)*height*width*3);
				memcpy(imgd, myImage.getImageData(), sizeof(Byte)*height*width*3);
				img.setImageData(imgd);	
				imgd = NULL;
				
				if(sample > 1){
					subSampleImage(&img, sample);
				}
				if(theta > 0){										
					rotateImage(&img, theta);					
				}
				/*if(sample > 1){
					subSampleImage(&img, sample);
				}*/

				if(antiAliasing == 1){		
					doAntiAliasing(&img, 3);
				}
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = img.getWidth();
				bmi.bmiHeader.biHeight = -img.getHeight();   // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = img.getWidth()*img.getHeight();
				

				SetDIBitsToDevice(hdc,
					nextStart,0,img.getWidth(),img.getHeight(),
					0,0,0,img.getHeight(),
					img.getImageData(),&bmi,DIB_RGB_COLORS);					
				
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}



// MyImage functions defined here

void MyImage::ReadImage()
{

	FILE *IN_FILE;
	int i;
	Byte *Rbuf = new Byte[Height*Width]; 
	Byte *Gbuf = new Byte[Height*Width]; 
	Byte *Bbuf = new Byte[Height*Width]; 

	IN_FILE = fopen(ImagePath, "rb");


	if (IN_FILE == NULL) 
	{
		fprintf(stderr, "Error");
		exit(0);
	}

	for (i = 0; i < Width*Height; i ++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width*Height; i ++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}
	
	Data = new Byte[Width*Height*3];

	for (i = 0; i < Height*Width; i++)
	{
		Data[3*i]	= Bbuf[i];
		Data[3*i+1]	= Gbuf[i];
		Data[3*i+2]	= Rbuf[i];
	}

	delete [] Rbuf;
	delete [] Gbuf;
	delete [] Bbuf;
	fclose(IN_FILE);

}

void subSampleImage(MyImage* originalImage,int n){
	int h = originalImage->getHeight();
	int w = originalImage->getWidth();
	int sh = originalImage->getHeight()/n;
	int sw = originalImage->getWidth()/n;
	Byte* imgData = originalImage->getImageData();

	int padding = sw%4;
	if( padding != 0){
		padding = 4 - padding;
	}
	sw += padding;			
	Byte* sampleData = new Byte[sh*sw*3];
	memset(sampleData, 0xFF, sizeof(Byte)*sh*sw*3);	
	int j = 0;
	for(int i = n-1; i < h; i += n){
		for(int k = n-1; k < w; k += n){
			unsigned long index = (i*w + k)*3;						
			sampleData[j++] = imgData[index];
			sampleData[j++] = imgData[index + 1];
			sampleData[j++] = imgData[index + 2];
		}			
		j += padding*3;					
	}
	originalImage->setImageData(sampleData);
	originalImage->setHeight(sh);
	originalImage->setWidth(sw);
	delete imgData;	
}
void rotatePoint(int xorig, int yorig, double angle, int* point){
	int xt = point[0] - xorig;
	int yt = point[1] - yorig;			
		
	double xr = xt*cos(angle) - yt*sin(angle);
	double yr = xt*sin(angle) + yt*cos(angle);
						
	point[0] = (int)xr + xorig;
	point[1] = (int)yr + yorig;
}

void rotateImage(MyImage* originalImage,int theta){
	int h = originalImage->getHeight();
	int w = originalImage->getWidth();
	
	Byte* imgData = originalImage->getImageData();
	
	double ang = theta*PI/180.0;
	int* pt = new int[2];

	int w1, w2, h1, h2;
	
	pt[0] = w;
	pt[1] = 0;
	rotatePoint(w/2, h/2, ang, pt);
	w1=w2=pt[0];
	h1=h2=pt[1];

	pt[0] = 0;
	pt[1] = h;
	rotatePoint(w/2, h/2, ang, pt);
	if(w1 < pt[0]) w1 = pt[0];
	if(w2 > pt[0]) w2 = pt[0];
	if(h1 < pt[1]) h1 = pt[1];
	if(h2 > pt[1]) h2 = pt[1];

	pt[0] = 0;
	pt[1] = 0;
	rotatePoint(w/2, h/2, ang, pt);
	if(w1 < pt[0]) w1 = pt[0];
	if(w2 > pt[0]) w2 = pt[0];
	if(h1 < pt[1]) h1 = pt[1];
	if(h2 > pt[1]) h2 = pt[1];

	pt[0] = w;
	pt[1] = h;
	rotatePoint(w/2, h/2, ang, pt);
	if(w1 < pt[0]) w1 = pt[0];
	if(w2 > pt[0]) w2 = pt[0];
	if(h1 < pt[1]) h1 = pt[1];
	if(h2 > pt[1]) h2 = pt[1];

	int height = abs(h1-h2);
	int width = abs(w1-w2);

	int padding = width%4;
	if( padding != 0){
		padding = 4 - padding;
	}
	width += padding;			
	
	padding = height%4;
	if( padding != 0){
		padding = 4 - padding;
	}
	height += padding;			
	//char toprint[50];
	//sprintf(toprint,"h %d w %d ",height, width);
	//MessageBox(NULL, toprint, NULL, NULL);

	int arrLen = height*width*3;
	int xleft = (height - h)/2;
	int yleft = (width - w)/2;
	
	Byte* rotatedData = new Byte[arrLen];
	memset(rotatedData, 0xFF, sizeof(Byte)*arrLen);
	
			
	unsigned long oldIndex, newIndex;
	
	for(int i = 0; i < h; ++i){
		for(int j = 0; j < w; ++j){			 
			oldIndex = (i*w + j)*3;
			newIndex = ((i+yleft)*width + (j + xleft))*3;
			rotatedData[newIndex] = imgData[oldIndex];
			rotatedData[newIndex + 1] = imgData[oldIndex + 1];
			rotatedData[newIndex + 2] = imgData[oldIndex + 2];
		}
	}
	delete[] imgData;

	Byte* rotatedData1 = new Byte[arrLen];
	memset(rotatedData1, 0xFF, sizeof(Byte)*arrLen);
	for(int y = 0; y < height; ++y){
		for(int x = 0; x < width; ++x){			 			
			pt[0] = x;
			pt[1] = y;
			rotatePoint(width/2, height/2, -ang, pt);
			newIndex = (y*width + x)*3;
			if(pt[1] >= 0 && pt[1] < height && pt[0] >= 0 && pt[0] < width){								
				oldIndex = (pt[1]*width + pt[0])*3;
				rotatedData1[newIndex] = rotatedData[oldIndex];
				rotatedData1[newIndex + 1] = rotatedData[oldIndex + 1];
				rotatedData1[newIndex + 2] = rotatedData[oldIndex + 2];								
			}
		}							
	}
	
	originalImage->setImageData(rotatedData1);
	originalImage->setHeight(height);
	originalImage->setWidth(width);

	delete[] rotatedData;
	delete[] pt;
}
void doAntiAliasing(MyImage* originalImage, int q){	
	Byte* imgData = originalImage->getImageData();	

	int h = originalImage->getHeight();
	int w = originalImage->getWidth();
	
	int arrayLen = h*w*3;

	Byte* imgd = new Byte[arrayLen];
	unsigned long r, g, b;
	unsigned long index;
	for(int y = -1; y < h; y++){
		for(int x = -1; x < w; x++){	
			int count = 0;
			r=g=b=0;
			for(int yy = 0; yy < q; ++yy){
				for(int xx = 0; xx < q; ++xx){	
					if((yy+y) >= 0 && (yy+y) < h && (xx+x) >= 0 && (xx+x) < w){
						index = ((yy+y)*w + xx+x)*3;
						r += imgData[index];
						g += imgData[index+1];
						b += imgData[index+2];										
						count++;
					}
				}
			}
			r = r/count;
			g = g/count;
			b = b/count;
			
			if((1+y) >= 0 && (1+y) < h && (1+x) >= 0 && (1+x) < w){
				index = ((y+1)*w + x+1)*3;
				imgd[index] = (int)r;
				imgd[index+1] = (int)g;
				imgd[index+2] = (int)b;					
			}			
		}
	}
	originalImage->setImageData(imgd);
	delete[] imgData;
}