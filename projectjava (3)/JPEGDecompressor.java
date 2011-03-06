import java.io.IOException;
import java.io.RandomAccessFile;

public class JPEGDecompressor {	
	static int prevDCL = 0;
	static int prevDCU = 0;
	static int prevDCV = 0;
	static boolean fl = false;
	static boolean fu = false;
	static boolean fv = false;	
	
	static void convertYUV2RGB(ImageFile img){	
		byte[][] Y = img.getData()[0]; 
		byte[][] U = img.getData()[1];
		byte[][] V = img.getData()[2];
		
		for(int y = 0; y < img.getHeight(); ++y){
			for(int x = 0; x < img.getWidth(); ++x){
				double r =  Const.YUV2RGB[0][0]*(Y[y][x]&0xff) 
						+ Const.YUV2RGB[0][1]*((U[y][x]&0xff) - 127.5) 
						+ Const.YUV2RGB[0][2]*((V[y][x]&0xff) - 127.5);
				double g =  Const.YUV2RGB[1][0]*(Y[y][x]&0xff) 
    					+ Const.YUV2RGB[1][1]*((U[y][x]&0xff) - 127.5) 
			            + Const.YUV2RGB[1][2]*((V[y][x]&0xff) - 127.5);
				double b =  Const.YUV2RGB[2][0]*(Y[y][x]&0xff) 
			            + Const.YUV2RGB[2][1]*((U[y][x]&0xff) - 127.5) 
			            + Const.YUV2RGB[2][2]*((V[y][x]&0xff) - 127.5);			
				Y[y][x] = Const.truncatetorgb(r);
				U[y][x] = Const.truncatetorgb(g);
				V[y][x] = Const.truncatetorgb(b);
			}
		}
	}		
	static void doIDCTFile(byte[][] dest, int ox, int oy, int isY, RandomAccessFile fd, byte q) throws IOException{
		double pc;		
		int[]  val = readfile(fd,isY);
		for(int y = 0; y < 8; y++){			
			for(int x = 0; x < 8; x++){				
				pc = 0;							
				for(int v= 0; v < 8; v++){
					for(int u = 0; u < 8; u++){																				
						double tpc = 0;
						int temp = val[Const.Zigzag[v][u]];		
						if(temp == 0) continue;
						if(isY == 1){
							tpc = temp*Const.LQTable[v][u]*q;
						}
						else{
							tpc = temp*Const.UVQTable[v][u]*q;
						}						
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];
						tpc *= ang;
						tpc /= 4;
						if((u == 0 && v != 0) || (u != 0 && v == 0)){
							tpc*=Const.c;
						}
						else if(u == 0 && v == 0){
							tpc/= 2;
						}						
						pc += tpc;
					}
				}								
				dest[y + oy][x + ox] = Const.truncatetorgb(pc + 128);				
			}
		}
	}
	static void decompressFile(ImageFile img, RandomAccessFile fd, byte q) throws IOException{		
		int blocky = img.getHeight()/16;
		int blockx = img.getWidth()/16;		
		prevDCL = prevDCU = prevDCV = 0;
		fl = fu = fv = false;
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){				
						doIDCTFile(img.getData()[0], x*16 + xx*8, y*16 + yy*8, 1, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){										
						doIDCTFile(img.getData()[1], x*16 + xx*8, y*16 + yy*8, 2, fd, q);
						doIDCTFile(img.getData()[2], x*16 + xx*8, y*16 + yy*8, 3, fd, q);
					}
				}
			}
		}						
	}
	static void doIDCTErrorFile(short[][] dest, int ox, int oy, int isY, RandomAccessFile fd, byte q) throws IOException{		
		double pc;
		int[]  val = readfile(fd, isY);
		for(int y = 0; y < 8; y++){			
			for(int x = 0; x < 8; x++){				
				pc = 0;							
				for(int v= 0; v < 8; v++){
					for(int u = 0; u < 8; u++){																									
						double tpc = 0;
						int temp =  val[Const.Zigzag[v][u]];		
						if(temp == 0) continue;
						if(isY == 1){
							tpc = temp*Const.LQTable[v][u]*4*q;
						}
						else{
							tpc = temp*Const.UVQTable[v][u]*4*q;
						}
						
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];
						tpc *= ang;
						tpc /= 4;
						if((u == 0 && v != 0) || (u != 0 && v == 0)){
							tpc*=Const.c;
						}
						else if(u == 0 && v == 0){
							tpc/= 2;
						}
						pc += tpc;
					}
				}								
				dest[y + oy][x + ox] = (short)(pc);				
			}
		}		
	}
	static void decompressErrorFile(short[][][] dest, RandomAccessFile fd, byte[][][] mv, byte q) throws IOException{		
		int blocky = dest[0].length/16;
		int blockx = dest[0][0].length/16;			
		prevDCL = prevDCU = prevDCV = 0;
		fl = fu = fv = false;
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){				
				mv[y][x][0] = fd.readByte();
				mv[y][x][1] = fd.readByte();				
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){				
						doIDCTErrorFile(dest[0], x*16 + xx*8, y*16 + yy*8, 1, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){				
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){				
						doIDCTErrorFile(dest[1], x*16 + xx*8, y*16 + yy*8, 2, fd, q);
						doIDCTErrorFile(dest[2], x*16 + xx*8, y*16 + yy*8, 3, fd, q);
					}
				}
			}
		}				
	}
	static void doIDCTEnhanceFile(short[][] dest, int ox, int oy, int isY, RandomAccessFile fd, byte q) throws IOException{		
		double pc;
		int[]  val = readfile(fd, isY);
		
		for(int y = 0; y < 8; y++){			
			for(int x = 0; x < 8; x++){				
				pc = 0;							
				for(int v= 0; v < 8; v++){
					for(int u = 0; u < 8; u++){																										
						double tpc = 0;
						int temp =  val[Const.Zigzag[v][u]];
						if(temp == 0) continue;
						if(isY == 1){
							tpc = temp*Const.LQTable[v][u]*q;
						}
						else{
							tpc = temp*Const.UVQTable[v][u]*q;
						}
						
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];
						tpc *= ang;
						tpc /= 4;
						if((u == 0 && v != 0) || (u != 0 && v == 0)){
							tpc*=Const.c;
						}
						else if(u == 0 && v == 0){
							tpc/= 2;
						}
						pc += tpc;
					}
				}		
				dest[y + oy][x + ox] = (short)(pc);				
			}
		}		
	}
	static void decompressEnhanceFile(short[][][] dest, RandomAccessFile fd, byte q) throws IOException{		
		int blocky = dest[0].length/16;
		int blockx = dest[0][0].length/16;				
		prevDCL = prevDCU = prevDCV = 0;
		fl = fu = fv = false;
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){								
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){				
						doIDCTEnhanceFile(dest[0], x*16 + xx*8, y*16 + yy*8, 1, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){								
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){										
						doIDCTEnhanceFile(dest[1], x*16 + xx*8, y*16 + yy*8, 2, fd, q);
						doIDCTEnhanceFile(dest[2], x*16 + xx*8, y*16 + yy*8, 3, fd, q);
					}
				}
			}
		}				
	}

	static void doIDCTBuffer(short[][] src, byte[][] dest, int ox, int oy, boolean isY, byte q){
		double pc;
		for(int y = 0; y < 8; y++){			
			for(int x = 0; x < 8; x++){				
				pc = 0;							
				for(int v= 0; v < 8; v++){
					for(int u = 0; u < 8; u++){																				
						double tpc = 0;						
						if(isY){
							tpc = src[v + oy][u + ox]*Const.LQTable[v][u]*q;
						}
						else{
							tpc = src[v + oy][u + ox]*Const.UVQTable[v][u]*q;
						}
						if(tpc == 0) continue;
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];
						tpc *= ang;
						tpc /= 4;
						if((u == 0 && v != 0) || (u != 0 && v == 0)){
							tpc*=Const.c;
						}
						else if(u == 0 && v == 0){
							tpc/= 2;
						}
						pc += tpc;
					}
				}								
				dest[y + oy][x + ox] = Const.truncatetorgb(pc + 128);				
			}
		}
	}
	static void decompressBuffer(ImageFile img, short src[][][], byte q){		
		int blocky = img.getHeight()/16;
		int blockx = img.getWidth()/16;			
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){				
						doIDCTBuffer(src[0], img.getData()[0], x*16 + xx*8, y*16 + yy*8, true, q);
						doIDCTBuffer(src[1], img.getData()[1], x*16 + xx*8, y*16 + yy*8, false, q);
						doIDCTBuffer(src[2], img.getData()[2], x*16 + xx*8, y*16 + yy*8, false, q);				
					}
				}
			}
		}						
	}
	static void doIDCTErrorBuffer(short[][] src, short[][] dest, int ox, int oy, boolean isY, byte q){		
		double pc;
		for(int y = 0; y < 8; y++){			
			for(int x = 0; x < 8; x++){				
				pc = 0;							
				for(int v= 0; v < 8; v++){
					for(int u = 0; u < 8; u++){																									
						double tpc = 0;						
						if(isY){
							tpc = src[v + oy][u + ox]*Const.LQTable[v][u]*4*q;
						}
						else{
							tpc = src[v + oy][u + ox]*Const.UVQTable[v][u]*4*q;
						}
						if(tpc == 0) continue;
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];
						tpc *= ang;
						tpc /= 4;
						if((u == 0 && v != 0) || (u != 0 && v == 0)){
							tpc*=Const.c;
						}
						else if(u == 0 && v == 0){
							tpc/= 2;
						}
						pc += tpc;
					}
				}								
				dest[y + oy][x + ox] = (short)(pc);				
			}
		}		
	}
	static void decompressErrorBuffer(short[][][] src,short[][][] dest, byte q) {		
		int blocky = src[0].length/16;
		int blockx = src[0][0].length/16;			
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){				
						doIDCTErrorBuffer(src[0], dest[0], x*16 + xx*8, y*16 + yy*8, true, q);
						doIDCTErrorBuffer(src[1], dest[1], x*16 + xx*8, y*16 + yy*8, false, q);
						doIDCTErrorBuffer(src[2], dest[2], x*16 + xx*8, y*16 + yy*8, false, q);
					}
				}
			}
		}				
	}
	static int[] readfile(RandomAccessFile fd, int yuv) throws IOException {
		String eob = "00000000";			
		byte n = fd.readByte();
		byte bi = (byte) Math.ceil(n/8.0);
		byte[] store = new byte[bi];
		fd.readFully(store);
		StringBuffer sb = new StringBuffer();
		for(int i = 0; i < bi; ++i){	
			int t = store[i]&0xff;
			String ts = Integer.toBinaryString(t);			
			sb.append(eob.substring(0, 8 - ts.length()));			
			sb.append(ts);
		}
		int[] zstr = new int[64];
		int ctr = 0;
		int len = Integer.parseInt(sb.substring(ctr, 4), 2);ctr+= 4;
		zstr[0] = Integer.parseInt(sb.substring(ctr, len), 2);ctr+=len;
		//dc coding
		if(yuv == 1){
			if(!fl){
				fl = true;
				prevDCL = zstr[0];
			}
			else{
				zstr[0] = zstr[0] + prevDCL; 
			}		
		}
		else if(yuv == 2){
			if(!fu){
				fu = true;
				prevDCU = zstr[0];
			}
			else{
				zstr[0] = zstr[0] + prevDCU; 
			}		
		}
		else if(yuv == 3){
			if(!fv){
				fv = true;
				prevDCV = zstr[0];
			}
			else{
				zstr[0] = zstr[0] + prevDCV; 
			}		
		}			
		int zeros = 0;	
		int i = 1;
		while(sb.substring(ctr, 8).equals(eob)){
			zeros = Integer.parseInt(sb.substring(ctr, 4), 2);ctr+= 4;
			for(int j = 0; j < zeros; ++j){
				zstr[i++] = 0;
			}
			len = Integer.parseInt(sb.substring(ctr, 4), 2);ctr+= 4;
			if(len == 0) continue;
			zstr[i] = vliDecoding(sb.substring(ctr, len));ctr+=len;
		}
		return zstr;
	}
	private static int vliDecoding(String str) {		
		if(str.charAt(0) == 0){			
			char[] temp = str.toCharArray();
			for(int k = 0; k < temp.length; ++k){
				if(temp[k] == '0'){
					temp[k] = '1';
				}
				else{
					temp[k] = '0';
				}
			}
			str = new String(temp); 
		}
		return Integer.parseInt(str, 2);
	}
}
