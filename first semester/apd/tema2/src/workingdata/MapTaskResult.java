package workingdata;

import java.util.Hashtable;
import java.util.Vector;

public class MapTaskResult extends TaskResult {
	private Hashtable<Integer, Integer> lengthFrequency;
	private Vector<String> 	maximalWords;
		
	public MapTaskResult(String fileName, Hashtable<Integer, Integer> h, Vector<String> mw) {
		setFileName(fileName);
		setLengthFrequency(h);
		setMaximalWords(mw);
	}

	public Hashtable<Integer, Integer> getLengthFrequency() {
		return lengthFrequency;
	}

	public void setLengthFrequency(Hashtable<Integer, Integer> lengthFrequency) {
		this.lengthFrequency = lengthFrequency;
	}

	public Vector<String> getMaximalWords() {
		return maximalWords;
	}

	public void setMaximalWords(Vector<String> maximalWords) {
		this.maximalWords = maximalWords;
	}
	
	@Override
	public String toString() {
		return "MapTaskResult:{Hash:" + lengthFrequency + " Vector:" + maximalWords + "}";
	}
}
