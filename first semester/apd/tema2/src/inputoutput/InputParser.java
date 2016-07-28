package inputoutput;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Vector;

import workingdata.*;

public class InputParser {
	private String filePath = "";
	private int chunkSize = -1;
	public HashMap<String, Integer> fileOrder;
	
	public InputParser(String file) {
		filePath = file;
		fileOrder = new HashMap<String, Integer>();
	}
	
	public int FileIndex(String file) {
		return fileOrder.get(file);
	}

	public Vector<MapTask> parseTasks() {
		Vector<MapTask> ret = new Vector<MapTask>();
		int noDoc = 0;
		try {
			BufferedReader br = new BufferedReader(new FileReader(filePath));
			chunkSize = Integer.parseInt(br.readLine());
			noDoc = Integer.parseInt(br.readLine());
			for(int i = 0; i < noDoc; i++) {
				String currFile = br.readLine();
				ret.addAll(_getTasksFromFile(currFile));
				fileOrder.put(currFile, i);
			}
			br.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (NumberFormatException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return ret;		
	}
	
	
	private Vector<MapTask> _getTasksFromFile(String file) {
		Vector<MapTask> ret = new Vector<MapTask>();
		File f = new File(file);
		int byteCnt = (int) f.length();
		int offset = 0;
		while(offset + chunkSize <= byteCnt) {
			ret.addElement(new MapTask(file, offset, chunkSize));
			offset += chunkSize;
		}
		if(offset != byteCnt) {
			ret.addElement(new MapTask(file, offset, byteCnt - offset));
		}
		
		return ret;
	}
}
