// Image.cpp : Defines the entry point for the application.
//

#include "Image.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

////
//MyImage myImage;
//MyImage img;

MyImage originalImage;
MyImage workingImage;

double c = sqrt(2.0)/2;
double cosVal[32];
int quantizationLevel, deliveryMode, latency;
int nextStart;

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
	//prepare cos value table
	cosVal[0] = 1; cosVal[1] = cos(PI/16); cosVal[2] = cos(PI/8); cosVal[3] = cos((3*PI)/16);
	cosVal[4] = c; cosVal[5] = cos((5*PI)/16); cosVal[6] = cos((3*PI)/8); cosVal[7] = cos((7*PI)/16);
	cosVal[8] = 0; cosVal[9] = -sin(PI/16); cosVal[10] = -sin(PI/8); cosVal[11] = -sin((3*PI)/16);
	cosVal[12] = -c; cosVal[13] = -sin((5*PI)/16); cosVal[14] = -sin((3*PI)/8); cosVal[15] = -sin((7*PI)/16);

	for(int i = 0; i < 16; ++i){
		cosVal[i+16] = - cosVal[i];
	}

	int wd, ht;
	char ImagePath[_MAX_PATH];
	sscanf(lpCmdLine, "%s %d %d %d", &ImagePath, &quantizationLevel, &deliveryMode, &latency);
	
	if(quantizationLevel > 7 || quantizationLevel < 0){
		MessageBox(NULL, "Quantization Level can be 0 to 7 only.", NULL, NULL);
		return FALSE;
	}
	if(deliveryMode > 3 || deliveryMode < 1){
		MessageBox(NULL, "Delivery Mode can be either 1 2 or 3 only.", NULL, NULL);
		return FALSE;
	}
	if(latency < 0){
		MessageBox(NULL, "Latency can be greater than 0 only.", NULL, NULL);
		return FALSE;
	}

	wd=352; ht=288;	
	originalImage.setWidth(wd);
	originalImage.setHeight(ht);
	originalImage.setImagePath(ImagePath);
	originalImage.ReadImage();
	
	nextStart = wd + 50;
				
	workingImage.setHeight(ht);
	workingImage.setWidth(wd);
	
	Byte* imgd = new Byte[ht*wd*3];
	memset(imgd, 0xFF, sizeof(Byte)*ht*wd*3);
	
	workingImage.setImageData(imgd);
	imgd = NULL;
		
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}	
	
	TwoByte *s = new TwoByte[ht*wd*3];
	MyStorage strg;
	strg.setStorage(s);
	encode(s);	

	if(deliveryMode == 1){
		_beginthread (sendDecodeSequentialMode, 0, s);	
	}
	else if(deliveryMode == 2){
		_beginthread (sendSpectralSelection, 0, s);
	}
	else if(deliveryMode == 3){
		_beginthread (sendSuccessiveBits, 0, s);
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
	//TCHAR szHello[MAX_LOADSTRING];
	//LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

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
					
				drawImage(&originalImage, 0);																		
				drawImage(&workingImage, nextStart);
				
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

void encode(TwoByte *store){
	int h = originalImage.getHeight();
	int w = originalImage.getWidth();										
	Byte* imgd = originalImage.getImageData();	

	Byte* tblock = new Byte[192];
	double* dctblock = new double[192];
	
	memset(tblock, 0x00, sizeof(Byte)*192);
	memset(dctblock, 0x00, sizeof(double)*192);
	
	unsigned int  q = 1<<quantizationLevel;

	for(int y = 0; y < h; y+=8){
		for(int x = 0; x < w; x+=8){
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int src = ((y+yy)*w + (x + xx))*3;
					int dest = (yy*8 + xx)*3;
					tblock[dest] = imgd[src];
					tblock[dest + 1] = imgd[src + 1];  
					tblock[dest + 2] = imgd[src + 2];  
				}
			}

			doDCT(tblock, dctblock);			

			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int dest = ((y+yy)*w + (x + xx))*3;
					int src = (yy*8 + xx)*3;

					store[dest]  = (TwoByte)(dctblock[src]/q); 
					store[dest + 1] = (TwoByte)(dctblock[src + 1]/q);
					store[dest + 2] = (TwoByte)(dctblock[src + 2]/q);   
				}
			}
		}
	}
	delete[] tblock;
	delete[] dctblock;
}

void doDCT(Byte* src, double* dest){
	double r,g,b;
	for(int v = 0; v < 8; v ++){
		for(int u = 0; u < 8; u++){
			int indexuv = (v*8+u)*3;
			r=g=b=0;
			for(int y= 0; y < 8; y++){
				for(int x = 0; x < 8; x++){
					int indexxy = (y*8+x)*3;								
					double tang = cosVal[((x<<1 | 0x01)*u) % 32]*cosVal[(((y<<1) | 0x01)*v) %32];
					r +=  (src[indexxy] - 128)*tang/4;
					g +=  (src[indexxy + 1] - 128)*tang/4;
					b +=  (src[indexxy + 2] - 128)*tang/4;
				}
			}
			if((u == 0 && v != 0) || (u != 0 && v == 0)){
				r*=c;
				g*=c;
				b*=c;
			}
			else if(u == 0 && v == 0){
				r/= 2;
				g/= 2;
				b/= 2;
			}
			dest[indexuv] = r;
			dest[indexuv + 1] = g;
			dest[indexuv + 2] = b;
		}
	}
}

void sendDecodeSequentialMode(void* storage){
	TwoByte* st = (TwoByte*) storage;

	int h = workingImage.getHeight();
	int w = workingImage.getWidth();
										
	Byte* imgd = workingImage.getImageData();
	
	unsigned int q = 1<<quantizationLevel;

	TwoByte* tblock = new TwoByte[192];
	Byte* idctblock = new Byte[192];

	for(int y = 0; y < h; y+=8){
		for(int x = 0; x < w; x+=8){
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int src = ((y + yy)*w + (x + xx))*3;
					int dest = (yy*8 + xx)*3;
					tblock[dest] = st[src]*q;
					tblock[dest + 1] = st[src + 1]*q;  
					tblock[dest + 2] = st[src + 2]*q;  
				}
			}

			doIDCT(tblock, idctblock);
			
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int dest = ((y + yy)*w + (x + xx))*3;
					int src = (yy*8 + xx)*3;
					imgd[dest]  = idctblock[src];					
					imgd[dest + 1] = idctblock[src + 1];					
					imgd[dest + 2] = idctblock[src + 2];  					
				}
			}
			drawImage(&workingImage, nextStart);
			Sleep(latency);
		}
	}
		
	delete[] tblock;
	delete[] idctblock;
	MessageBox(NULL, "DECODING DONE", "Status", NULL);
}

void sendSpectralSelection(void* storage){
	TwoByte* st = (TwoByte*) storage;
	
	int h = workingImage.getHeight();
	int w = workingImage.getWidth();
	int len = h*w*3;

	TwoByte *newst = new TwoByte[len];
	memset(newst, 0x00, sizeof(TwoByte)*len);

	int xx, yy;
	int t = 0;
	for(int k= 0; k <= 14; ++k){		
		if((k%7)%2 == 0){
			if(k == 7 || k==14){
				xx = 7;
				yy = k-7;
			}
			else{
				yy = k%7;
				xx = k-yy;
			}
		}
		else{
			xx = k%7;
			yy = k-xx;
		}
		if(yy > xx){
			t = yy - xx;
			for(int i = 0; i <= t; ++i){
				for(int y = 0; y < h; y+=8){
					for(int x = 0; x < w; x+=8){
						int index = ((y + yy)*w + (x + xx))*3;
						newst[index] = st[index];
						newst[index + 1] = st[index + 1];  
						newst[index + 2] = st[index + 2];  				
					}
				}
				decodeProgressiveModeSpectralSelection(newst);
				Sleep(latency);
				yy--;xx++;				
			}			
		}
		else{
			t = xx - yy;
			for(int i = 0; i <= t; ++i){
				for(int y = 0; y < h; y+=8){
					for(int x = 0; x < w; x+=8){
						int index = ((y + yy)*w + (x + xx))*3;
						newst[index] = st[index];
						newst[index + 1] = st[index + 1];  
						newst[index + 2] = st[index + 2];  				
					}
				}
				decodeProgressiveModeSpectralSelection(newst);
				Sleep(latency);
				yy++;xx--;
			}
		}		
	}	
	delete[] newst;
	MessageBox(NULL, "DECODING DONE", "Status", NULL);
}

void decodeProgressiveModeSpectralSelection(TwoByte* st){
	int h = workingImage.getHeight();
	int w = workingImage.getWidth();
										
	Byte* imgd = workingImage.getImageData();
	
	unsigned int q = 1<<quantizationLevel;

	TwoByte* tblock = new TwoByte[192];
	Byte* idctblock = new Byte[192];

	memset(tblock, 0x00, sizeof(TwoByte)*192);
	memset(idctblock, 0x00, sizeof(Byte)*192);

	for(int y = 0; y < h; y+=8){
		for(int x = 0; x < w; x+=8){
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int src = ((y + yy)*w + (x + xx))*3;
					int dest = (yy*8 + xx)*3;
					tblock[dest] = st[src]*q;
					tblock[dest + 1] = st[src + 1]*q;  
					tblock[dest + 2] = st[src + 2]*q;  
				}
			}

			doIDCT(tblock, idctblock);
			
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int dest = ((y + yy)*w + (x + xx))*3;
					int src = (yy*8 + xx)*3;
					imgd[dest]  = idctblock[src];					
					imgd[dest + 1] = idctblock[src + 1];					
					imgd[dest + 2] = idctblock[src + 2];  					
				}
			}
		}
	}
		
	delete[] tblock;
	delete[] idctblock;
	drawImage(&workingImage, nextStart);		
}
void sendSuccessiveBits(void* storage){
	TwoByte* st = (TwoByte*) storage;
	
	int h = workingImage.getHeight();
	int w = workingImage.getWidth();
	int len = h*w*3;
	
	TwoByte *newst = new TwoByte[len];
	memset(newst, 0x00, sizeof(TwoByte)*len);
	TwoByte offset = 1<<12;
	for(int k = 12; k >= 0; --k){
		for(int i = 0; i < len; ++i){
			TwoByte temp = st[i] + offset;						
			temp &= (1<<k);				
			newst[i] |= temp;			
		}
		decodeProgressiveModeBitApproximation(newst, offset);	
		Sleep(latency);
	}
	delete[] newst;
	MessageBox(NULL, "DECODING DONE", "Status", NULL);
}
void decodeProgressiveModeBitApproximation(TwoByte* st, TwoByte offset){
	int h = workingImage.getHeight();
	int w = workingImage.getWidth();
										
	Byte* imgd = workingImage.getImageData();
	unsigned int q = 1<<quantizationLevel;

	TwoByte* tblock = new TwoByte[192];
	Byte* idctblock = new Byte[192];

	for(int y = 0; y < h; y+=8){
		for(int x = 0; x < w; x+=8){
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int src = ((y + yy)*w + (x + xx))*3;
					int dest = (yy*8 + xx)*3;
					tblock[dest] = (st[src]-offset)*q;
					tblock[dest + 1] = (st[src + 1]-offset)*q;  
					tblock[dest + 2] = (st[src + 2]-offset)*q;  
				}
			}

			doIDCT(tblock, idctblock);
			
			for(int yy= 0; yy < 8; yy++){
				for(int xx = 0; xx < 8; xx++){
					int dest = ((y + yy)*w + (x + xx))*3;
					int src = (yy*8 + xx)*3;
					imgd[dest]  = idctblock[src];					
					imgd[dest + 1] = idctblock[src + 1];					
					imgd[dest + 2] = idctblock[src + 2];  					
				}
			}
		}
	}
		
	delete[] tblock;
	delete[] idctblock;
	drawImage(&workingImage, nextStart);		
}

void doIDCT(TwoByte* src, Byte* dest){
	double r,g,b;
	for(int y = 0; y < 8; y++){
		for(int x = 0; x < 8; x++){
			int indexxy = (y*8+x)*3;
			r=g=b=0;
			for(int v= 0; v < 8; v++){
				for(int u = 0; u < 8; u++){
					int indexuv = (v*8+u)*3;
					double tang = cosVal[((x<<1 | 0x01)*u) % 32]*cosVal[(((y<<1) | 0x01)*v) %32];
					double tr = src[indexuv]*tang/4; 
					double tg = src[indexuv + 1]*tang/4; 
					double tb = src[indexuv + 2]*tang/4; 
					if((u == 0 && v != 0) || (u != 0 && v == 0)){
						tr*=c;
						tg*=c;
						tb*=c;
					}
					else if(u == 0 && v == 0){
						tr/= 2;
						tg/= 2;
						tb/= 2;
					}
					r += tr;
					g += tg;
					b += tb;
				}
			}
			int t = (TwoByte)(r + 128);
			if(t > 255) t = 255;
			if(t < 0) t = 0;
			dest[indexxy] = (Byte)t;
			
			t = (TwoByte)(g + 128);
			if(t > 255) t = 255;
			if(t < 0) t = 0;
			dest[indexxy + 1] = (Byte)t;
			
			t = (TwoByte)(b + 128);
			if(t > 255) t = 255;
			if(t < 0) t = 0;
			dest[indexxy + 2] = (Byte)t;			
		}
	}
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
void drawImage(MyImage* i, int n){
	int h = i->getHeight();
	int w = i->getWidth();
	
	BITMAPINFO bmi;
	memset(&bmi,0,sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = -h;   // Use negative height.  DIB is top-down.
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = w*h;
	HDC hdc = GetDC (hWnd) ;		
	SetDIBitsToDevice(hdc, n, 0, w, h, 0, 0, 0, h, i->getImageData(),&bmi,DIB_RGB_COLORS);
	ReleaseDC (hWnd, hdc) ;
}