package threadpool;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.charset.Charset;
import java.util.Collection;
import java.util.Hashtable;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedTransferQueue;
import java.util.concurrent.TimeUnit;

import workingdata.*;

public class MappingWorkPool {
	public final static String delimiters = ";:/?~\\.,><~`[]{}()!@#$%^&-_+'=*\"| \t\n\r";
	private LinkedTransferQueue<MapTask> tasks;
	private final ExecutorService workPool;
	private Vector<MapTaskResult> results;
	
	public MappingWorkPool(int threadCnt) {
		tasks = new LinkedTransferQueue<MapTask>();
		results = new Vector<MapTaskResult>();
		workPool = Executors.newFixedThreadPool(threadCnt);
	}
	
	public Vector<MapTaskResult> getResults() {
		return results;
	}
	
	public void addTask(MapTask mt) {
		tasks.add(mt);
	}
	
	public void AddAll(Collection<? extends MapTask> col) {
		tasks.addAll(col);
	}
	
	public void run() {
		while (false == tasks.isEmpty()) {
			MapWorker mw = new MapWorker(tasks.remove());
			workPool.execute(mw);
		}
		workPool.shutdown();
		
		try {
			// just want to wait all the threads to join
			workPool.awaitTermination(100, TimeUnit.DAYS);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	/*The object that knows how to process a MapTask*/
	class MapWorker implements Runnable {
	private final MapTask mt;
		MapWorker(MapTask myTask) { 
			mt = myTask; 
		}
		
		/*Adjusting the indexes so the interval hasn't margins in the middle of words*/
	
		
		private Integer adjustFirstIndex(RandomAccessFile raf, Integer startIdx) throws IOException {
			raf.seek(startIdx);
			if(startIdx > 0) {
				// the current char is not a delimiter
				if(-1 == delimiters.indexOf(raf.readByte())) {
					raf.seek(startIdx - 1);
					// the previous character is not a delimiter
					if(-1 == delimiters.indexOf(raf.readByte())) {
						//see if the starting position has to be shifted to point to beginning of a word 
						while(-1 == delimiters.indexOf(raf.readByte())) {
							startIdx++;
						}
					}
				}
			}
			return startIdx;
		}
			
	
		private Integer adjustSecondIndex(RandomAccessFile raf, Integer stopIdx) throws IOException {
			//see if the starting position has to be shifted to point to beginning of a word
			if(stopIdx == raf.length()) {
				return stopIdx;
			}
			raf.seek(stopIdx);
			char ch = (char) raf.readByte();
			while(-1 == delimiters.indexOf(ch)) {
				stopIdx = stopIdx + 1;
				ch = (char)raf.readByte();
			}
			
			return stopIdx;
		}
		
		
		public void run() {
			//System.out.println("MapWorke#" + Thread.currentThread().getName() + " se executa");
			RandomAccessFile raf = null;
			try {
				raf = new RandomAccessFile(mt.getFileName(), "r");
				Integer startIdx = mt.getOffset();
				Integer stopIdx = new Integer(startIdx + mt.getDimension());
				
				startIdx = adjustFirstIndex(raf, startIdx);
				stopIdx = adjustSecondIndex(raf, stopIdx);
				int newDim = stopIdx - startIdx;
				
				raf.seek(startIdx);
				byte[] data = new byte[newDim];
				raf.read(data);
				
				Hashtable<Integer, Integer> hash = new Hashtable<Integer, Integer>();
				Vector<String> maximalWords = new Vector<String>();
				int maxLength = 0;
				
				String str = new String(data, Charset.forName("UTF-8"));
				StringTokenizer token = new StringTokenizer(str, delimiters);
				
				while(token.hasMoreTokens()) {
					String word = token.nextToken();
					if(hash.containsKey(word.length())){
						Integer val = hash.get(word.length());
						hash.put(word.length(), val + 1);
					} else {
						hash.put(word.length(), 1);
					}
					
					if(word.length() == maxLength && !maximalWords.contains(word)) {
						maximalWords.addElement(word);
					} else if (word.length() > maxLength) {
						maxLength = word.length();
						maximalWords.clear();
						maximalWords.addElement(word);
					}
				}
				results.addElement(new MapTaskResult(mt.getFileName(), hash, maximalWords));
				raf.close();				
			} catch (IOException e) {
				System.err.println("Thread #" + Thread.currentThread().getName() + " couldn't open || read || close " + mt.getFileName());
				e.printStackTrace();
				return;
			}
		}
	}
}
