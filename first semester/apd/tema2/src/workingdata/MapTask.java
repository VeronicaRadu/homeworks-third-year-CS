package workingdata;

public class MapTask extends WorkTask {
	private int offset;
	private int dimension;
	
	public MapTask(String fileName, int offset, int dimension) {
		setFileName(fileName);
		setOffset(offset);
		setDimension(dimension);
	}
	
	public int getOffset() {
		return offset;
	}
	public void setOffset(int offset) {
		this.offset = offset;
	}
	public int getDimension() {
		return dimension;
	}
	public void setDimension(int dimension) {
		this.dimension = dimension;
	}
}
