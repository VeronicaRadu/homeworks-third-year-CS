package workingdata;

import java.util.Vector;

public class ReduceTask extends WorkTask {
	private Vector<MapTaskResult> results;
	
	public ReduceTask(String fileName, Vector<MapTaskResult> results) {
		setFileName(fileName);
		setResults(results);
	}

	public Vector<MapTaskResult> getResults() {
		return results;
	}

	public void setResults(Vector<MapTaskResult> results) {
		this.results = results;
	}
}
