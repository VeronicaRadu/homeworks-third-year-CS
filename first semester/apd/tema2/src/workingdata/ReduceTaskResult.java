package workingdata;

import java.text.DecimalFormat;

public class ReduceTaskResult extends TaskResult {
	private double rank;
	private int maxWordLength;
	private int uniqueMaximalWordsCount;
	
	public ReduceTaskResult(String filename, double r, int mwl, int umwc) {
		setFileName(filename);
		String s = new DecimalFormat("#.000").format(r);
		setRank(new Double(s.substring(0, s.length() - 1)));
		setMaxWordLength(mwl);
		setUniqueMaximalWordsCount(umwc);
	}
	
	public double getRank() {
		return rank;
	}
	public void setRank(double rank) {
		this.rank = rank;
	}
	public int getMaxWordLength() {
		return maxWordLength;
	}
	public void setMaxWordLength(int maxWordLength) {
		this.maxWordLength = maxWordLength;
	}
	public int getUniqueMaximalWordsCount() {
		return uniqueMaximalWordsCount;
	}
	public void setUniqueMaximalWordsCount(int uniqueMaximalWordsCount) {
		this.uniqueMaximalWordsCount = uniqueMaximalWordsCount;
	}
	
	@Override
	public String toString() {
		StringBuilder r = new StringBuilder();
		r.append(getFileName());
		r.append(';');
		//r.append(rank);
		String s = new DecimalFormat("#.000").format(rank);
		r.append(s.substring(0, s.length() - 1));
		r.append(";[");
		r.append(maxWordLength);
		r.append(',');
		r.append(uniqueMaximalWordsCount);
		
		r.append(']');		
		return r.toString();
	}
	
}
