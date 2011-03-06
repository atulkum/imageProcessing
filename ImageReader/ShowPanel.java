import java.io.IOException;
import java.io.RandomAccessFile;

import javax.swing.JFrame;
import javax.swing.JPanel;

public class ShowPanel {			
	static OutputPanel original = null;	
	static ImageFile img = null;	

	public static void main(String[] args){			
		String filename = "C:/uscProjects/576/project/video/sample1_352x288.576v"; 
			//"C:/uscProjects/576/ImageReader/prison_960_540.rgb";	
		int h = 288;
		int w = 352;	
		int frate = 1;
		img = new ImageFile(h, w);
		original = new OutputPanel(h, w);
	
		JFrame frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		
		frame.setSize(750, 500);
		final JPanel pane = new JPanel();
		pane.setSize(720, 325);
		pane.setLayout(null);		
		frame.getContentPane().add(pane);		
		original.setSize(360, 325);
		original.setLocation(10, 10);
		pane.add(original);									
		
		RandomAccessFile videoFile = null;
		try {	
			videoFile = new RandomAccessFile(filename,"r");
			for(int i = 0; i < 10; ++i){															
				int buffid = 0;
				byte[] buff = new byte[3*h*w];				
				videoFile.readFully(buff);
				for(int m = 0; m < 3; ++m){
					for(int y = 0; y < h; ++y){					
						System.arraycopy(buff, buffid, img.getData()[m][y], 0, w);
						buffid += w;
					}
				}
				original.setImgData(img);
				pane.repaint();				
				Thread.sleep(1000/frate);
				frame.setVisible(true);
			}			
		} catch (Exception e) {
			e.printStackTrace();
		} 
		finally{
			try {
				if(videoFile != null)
					videoFile.close();
			} catch (IOException e) {				
				e.printStackTrace();
			}
		}				
	}
}
