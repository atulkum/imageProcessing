import java.io.IOException;
import java.io.RandomAccessFile;

public class JPEGCompressor {	
	
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
	static void doDCTFileBuffer(byte[][] src, short[][] dest, 
			int ox, int oy, boolean isY, RandomAccessFile fd, byte q) 
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
				pc =  pc + (1<<(Const.bits - 1));
				if(isY){
					pc = Math.round(pc/(Const.LQTable[v][u]*q));
				}
				else{
					pc = Math.round(pc/(Const.UVQTable[v][u]*q));
				}
				dest[v + oy][u + ox] = (short)(pc);
				
				zstr[Const.Zigzag[v][u]] = (int) pc;						
			}
		}		
		write2file(fd, zstr, false, isY);		
	}	
	
	static void compressFileBuffer(ImageFile img,short[][][] dest, RandomAccessFile fd, byte q) throws IOException{				
		int blocky = img.getHeight()/16;
		int blockx = img.getWidth()/16;				
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTFileBuffer(img.getData()[0], dest[0], x*16 + xx*8, y*16 + yy*8, true, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTFileBuffer(img.getData()[1], dest[1], x*16 + xx*8, y*16 + yy*8, false, fd, q);
						doDCTFileBuffer(img.getData()[2], dest[2], x*16 + xx*8, y*16 + yy*8, false, fd, q);
					}
				}
			}
		}		
	}
	static void doDCTErrorFileBuffer(short[][] src, short[][] dest, int ox, int oy, 
			boolean isY, RandomAccessFile fd, byte q) throws IOException{
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
				pc = pc + (1<<(Const.bits - 1));
				if(isY){
					pc = Math.round(pc /(Const.LQTable[v][u]*4*q));
				}
				else{
					pc = Math.round(pc /(Const.UVQTable[v][u]*4*q));
				}
				dest[v + oy][u + ox] = (short)(pc);
				zstr[Const.Zigzag[v][u]] = (int) (pc);				
			}
		}		
		write2file(fd, zstr, true, isY);
	}	
	static void compressErrorFileBuffer(short[][][] src, short[][][] dest, RandomAccessFile fd, byte[][][] mv, byte q) 
		throws IOException{		
		int blocky = src[0].length/16;
		int blockx = src[0][0].length/16;				
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				fd.writeByte(mv[y][x][0]);
				fd.writeByte(mv[y][x][1]);
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTErrorFileBuffer(src[0], dest[0], x*16 + xx*8, y*16 + yy*8, true, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTErrorFileBuffer(src[1], dest[1], x*16 + xx*8, y*16 + yy*8, false, fd, q);
						doDCTErrorFileBuffer(src[2], dest[2], x*16 + xx*8, y*16 + yy*8, false, fd, q);			
					}
				}
			}
		}																
	}
	static void doDCTEnhancedFile(short[][] src, int ox, int oy, boolean isY, RandomAccessFile fd, byte q) throws IOException{
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
				pc = pc + (1<<(Const.bits - 1));
				if(isY){
					pc = Math.round(pc /(Const.LQTable[v][u]*q));
				}
				else{
					pc = Math.round(pc /(Const.UVQTable[v][u]*q));
				}							
				zstr[Const.Zigzag[v][u]] = (int) pc;		
			}
		}		
		write2file(fd, zstr, false, isY);
	}	
	static void compressEnhancedFile(short[][][] src, RandomAccessFile fd, byte q) 
		throws IOException{		
		int blocky = src[0].length/16;
		int blockx = src[0][0].length/16;				
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){			
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTEnhancedFile(src[0], x*16 + xx*8, y*16 + yy*8, true, fd, q);
					}
				}
			}
		}
		for(int y = 0; y < blocky; ++y){
			for(int x = 0; x < blockx; ++x){			
				for(int yy = 0; yy < 2; ++yy){
					for(int xx = 0; xx < 2; ++xx){
						doDCTEnhancedFile(src[1], x*16 + xx*8, y*16 + yy*8, false, fd, q);
						doDCTEnhancedFile(src[2], x*16 + xx*8, y*16 + yy*8, false, fd, q);			
					}
				}
			}
		}																
	}	
	static void write2file(RandomAccessFile fd, int[] zstr, boolean isP, boolean isY) throws IOException {
		int n = 0;
		String z = "00000000000";
		if(isP) n = 2;
		n += Const.qf;
		StringBuffer sb = new StringBuffer();
		int bi = 0;
		for(int i = 0; i < 64; ++i){			
			int rb = Const.bits - n; 
			if(isY){
				rb -= Const.LBits[Const.Natural[i]];							
			}
			else{
				rb -= Const.UVBits[Const.Natural[i]];
			}	
			String temp = Integer.toBinaryString(zstr[i]);
			if(rb <  temp.length()) System.out.println(rb + " " + temp.length()+ " "+ zstr[i]+" "+ temp +" "+ Const.Natural[i]);
			//if(rb >  temp.length()){
				sb.append(z.substring(0, (rb - temp.length())));
			//}
			sb.append(temp);
			bi += rb;
		}
		//System.out.print(bi + " ");
		bi = (int) Math.ceil(bi/8.0);
		sb.append(z.substring(0, (bi*8 - sb.length())));
		//System.out.println(sb + " "+ sb.length());
		byte[] store = new byte[bi];
		for(int i = 0; i < bi; ++i){	
			store[i] = Const.int2byte(Integer.parseInt(sb.substring(i*8, (i+1)*8), 2));
		}			
		fd.write(store);
	}
}
