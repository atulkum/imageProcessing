import java.io.IOException;
import java.io.RandomAccessFile;

public class JPEGCompressor {	
	static int prevDCL = 0;
	static int prevDCU = 0;
	static int prevDCV = 0;
	static boolean fl = false;
	static boolean fu = false;
	static boolean fv = false;	
	
	static void convertRGB2YUV(ImageFile img ){
		byte[][] r = img.getData()[0];
		byte[][] g = img.getData()[1];
		byte[][] b = img.getData()[2];
		
		for(int y = 0; y < img.getHeight(); ++y){
			for(int x = 0; x < img.getWidth(); ++x){
				double Y =  Const.RGB2YUV[0][0]*(r[y][x]&0xff) 
						+ Const.RGB2YUV[0][1]*(g[y][x]&0xff) 
						+ Const.RGB2YUV[0][2]*(b[y][x]&0xff);
				double U =  Const.RGB2YUV[1][0]*(r[y][x]&0xff) 
    					+ Const.RGB2YUV[1][1]*(g[y][x]&0xff) 
			            + Const.RGB2YUV[1][2]*(b[y][x]&0xff);
				double V =  Const.RGB2YUV[2][0]*(r[y][x]&0xff) 
			            + Const.RGB2YUV[2][1]*(g[y][x]&0xff) 
			            + Const.RGB2YUV[2][2]*(b[y][x]&0xff);						
				U = U + 127.5;
				V = V + 127.5;					
				r[y][x] = Const.truncatetorgb(Y);
				g[y][x] = Const.truncatetorgb(U);
				b[y][x] = Const.truncatetorgb(V);
			}
		}
	}
	static void doDCTFileBuffer(byte[][] src, short[][] dest, int ox, int oy, int yuv, RandomAccessFile fd, byte q) 
	throws IOException{
		double pc;
		int[] zstr = new int[64];		
		for(int v = 0; v < 8; v++){
			for(int u = 0; u < 8; u++){				
				pc = 0;								
				for(int y= 0; y < 8; y++){
					for(int x = 0; x < 8; x++){					
						if((src[y + oy][x + ox]&0xff) == 128) continue;
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];						
						pc += ((src[y + oy][x + ox]&0xff) - 128)*ang/4;					
					}					
				}				
				if((u == 0 && v != 0) || (u != 0 && v == 0)){
					pc*=Const.c;
				}				
				else if(u == 0 && v == 0){
					pc/= 2;					
				}				
				if(yuv == 1){
					pc = Math.round(pc/(Const.LQTable[v][u]*q));
				}
				else{
					pc = Math.round(pc/(Const.UVQTable[v][u]*q));
				}
				dest[v + oy][u + ox] = (short)(pc);				
				zstr[Const.Zigzag[v][u]] = (int) pc;						
			}
		}		
		write2file(fd, zstr, yuv);		
	}	
	
	static void compressFileBuffer(ImageFile img,short[][][] dest, RandomAccessFile fd, byte q) throws IOException{				
		int blocky = img.getHeight()/16;
		int blockx = img.getWidth()/16;
		prevDCL = prevDCU = prevDCV = 0;
		fl = fu = fv = false;
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTFileBuffer(img.getData()[0], dest[0], x*16 + xx*8, y*16 + yy*8, 1, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTFileBuffer(img.getData()[1], dest[1], x*16 + xx*8, y*16 + yy*8, 2, fd, q);
						doDCTFileBuffer(img.getData()[2], dest[2], x*16 + xx*8, y*16 + yy*8, 3, fd, q);
					}
				}
			}
		}		
	}
	static void doDCTErrorFileBuffer(short[][] src, short[][] dest, int ox, int oy, 
			int yuv, RandomAccessFile fd, byte q) throws IOException{
		double pc;			
		int[] zstr = new int[64];	
		for(int v = 0; v < 8; v++){
			for(int u = 0; u < 8; u++){				
				pc = 0;								
				for(int y= 0; y < 8; y++){
					for(int x = 0; x < 8; x++){		
						if(src[y + oy][x + ox] == 0) continue;
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];						
						pc += src[y + oy][x + ox]*ang/4;					
					}					
				}				
				if((u == 0 && v != 0) || (u != 0 && v == 0)){
					pc*=Const.c;
				}				
				else if(u == 0 && v == 0){
					pc/= 2;					
				}				
				if(yuv == 1){
					pc = Math.round(pc /(Const.LQTable[v][u]*4*q));
				}
				else{
					pc = Math.round(pc /(Const.UVQTable[v][u]*4*q));
				}
				dest[v + oy][u + ox] = (short)(pc);
				zstr[Const.Zigzag[v][u]] = (int) (pc);				
			}
		}		
		write2file(fd, zstr, yuv);
	}	
	static void compressErrorFileBuffer(short[][][] src, short[][][] dest, RandomAccessFile fd, byte[][][] mv, byte q) 
		throws IOException{		
		int blocky = src[0].length/16;
		int blockx = src[0][0].length/16;
		prevDCL = prevDCU = prevDCV = 0;
		fl = fu = fv = false;
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				fd.writeByte(mv[y][x][0]);
				fd.writeByte(mv[y][x][1]);
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTErrorFileBuffer(src[0], dest[0], x*16 + xx*8, y*16 + yy*8, 1, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTErrorFileBuffer(src[1], dest[1], x*16 + xx*8, y*16 + yy*8, 2, fd, q);
						doDCTErrorFileBuffer(src[2], dest[2], x*16 + xx*8, y*16 + yy*8, 3, fd, q);			
					}
				}
			}
		}																
	}
	static void doDCTEnhancedFile(short[][] src, int ox, int oy, int yuv, RandomAccessFile fd, byte q) throws IOException{
		double pc;
		int[] zstr = new int[64];
		for(int v = 0; v < 8; v++){
			for(int u = 0; u < 8; u++){				
				pc = 0;								
				for(int y= 0; y < 8; y++){
					for(int x = 0; x < 8; x++){	
						if(src[y + oy][x + ox] == 0) continue;
						double ang = Const.cosVal[((2*x + 1)*u)%32]*Const.cosVal[((2*y + 1)*v)%32];						
						pc += src[y + oy][x + ox]*ang/4;					
					}					
				}				
				if((u == 0 && v != 0) || (u != 0 && v == 0)){
					pc*=Const.c;
				}				
				else if(u == 0 && v == 0){
					pc/= 2;					
				}				
				if(yuv == 1){
					pc = Math.round(pc /(Const.LQTable[v][u]*q));
				}
				else{
					pc = Math.round(pc /(Const.UVQTable[v][u]*q));
				}							
				zstr[Const.Zigzag[v][u]] = (int) pc;		
			}
		}		
		write2file(fd, zstr, yuv);
	}	
	static void compressEnhancedFile(short[][][] src, RandomAccessFile fd, byte q) 
		throws IOException{		
		int blocky = src[0].length/16;
		int blockx = src[0][0].length/16;
		prevDCL = prevDCU = prevDCV = 0;
		fl = fu = fv = false;
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){			
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTEnhancedFile(src[0], x*16 + xx*8, y*16 + yy*8, 1, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){			
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTEnhancedFile(src[1], x*16 + xx*8, y*16 + yy*8, 2, fd, q);
						doDCTEnhancedFile(src[2], x*16 + xx*8, y*16 + yy*8, 3, fd, q);			
					}
				}
			}
		}																
	}	
	static void write2file(RandomAccessFile fd, int[] zstr, int yuv) throws IOException {
		String eob = "00000000";
		String zero16 = "11110000";
		StringBuffer sb = new StringBuffer();			
		//dc coding
		if(yuv == 1){
			if(!fl){
				fl = true;
				prevDCL = zstr[0];
			}
			else{
				zstr[0] = zstr[0] - prevDCL; 
			}		
		}
		else if(yuv == 2){
			if(!fu){
				fu = true;
				prevDCU = zstr[0];
			}
			else{
				zstr[0] = zstr[0] - prevDCU; 
			}		
		}
		else if(yuv == 3){
			if(!fv){
				fv = true;
				prevDCV = zstr[0];
			}
			else{
				zstr[0] = zstr[0] - prevDCV; 
			}		
		}
		String temp = vliCoding(zstr[0]);
		String tl = Integer.toBinaryString(temp.length());
		sb.append(eob.substring(0, 4 - tl.length()));
		sb.append(tl);
		sb.append(temp);
		int zeros = 0;	
		for(int i = 1; i < 64; ++i){			
			if(zstr[i] == 0) zeros++;
			else{
				while(zeros > 15){
					sb.append(zero16);
					zeros-= 15;
				}				
				tl = Integer.toBinaryString(zeros);
				sb.append(eob.substring(0, 4 - tl.length()));
				sb.append(tl);
				temp = vliCoding(zstr[i]);
				tl = Integer.toBinaryString(temp.length());
				sb.append(eob.substring(0, 4 - tl.length()));
				sb.append(tl);
				sb.append(temp);
			}
		}
		sb.append(eob);
		byte bi = (byte) Math.ceil(sb.length()/8.0);
		byte[] store = new byte[bi + 1];
		store[0] = (byte) sb.length();
		sb.append(eob.substring(0, (bi*8 - sb.length())));
		for(int i = 0; i < bi; ++i){	
			store[i+1] = Const.int2byte(Integer.parseInt(sb.substring(i*8, (i+1)*8), 2));
		}
		fd.write(store);
	}
	private static String vliCoding(int i) {
		if(i < 0){
			i = -i;
			char[] temp = (Integer.toBinaryString(i)).toCharArray();
			for(int k = 0; k < temp.length; ++k){
				if(temp[k] == '0'){
					temp[k] = '1';
				}
				else{
					temp[k] = '0';
				}
			}
			return new String(temp); 
		}
		else{
			return Integer.toBinaryString(i);
		}	
	}
}
