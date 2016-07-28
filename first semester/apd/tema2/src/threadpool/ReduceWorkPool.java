package threadpool;

import java.util.Collection;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedTransferQueue;
import java.util.concurrent.TimeUnit;

import workingdata.Fibonacci;
import workingdata.MapTaskResult;
import workingdata.ReduceTask;
import workingdata.ReduceTaskResult;

public class ReduceWorkPool{
	private LinkedTransferQueue<ReduceTask> tasks;
	private final ExecutorService workPool;
	private Vector<ReduceTaskResult> results;
	
	
	public ReduceWorkPool(int threadCnt, Collection<? extends ReduceTask> tks) {
		tasks = new LinkedTransferQueue<ReduceTask>();
		tasks.addAll(tks);
		results = new Vector<ReduceTaskResult>();
		workPool = Executors.newFixedThreadPool(threadCnt);
	}
	public void run() {
		while(!tasks.isEmpty()) {
			workPool.execute(new ReduceWorker(tasks.remove()));
		}
		workPool.shutdown();
		
		try {
			workPool.awaitTermination(100, TimeUnit.DAYS);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
	public LinkedTransferQueue<ReduceTask> getTasks() {
		return tasks;
	}
	public void setTasks(LinkedTransferQueue<ReduceTask> tasks) {
		this.tasks = tasks;
	}
	
	public Vector<ReduceTaskResult> getRTResults() {
		return results;
	}

	class ReduceWorker implements Runnable {
		private final ReduceTask myTask;
		private Hashtable<Integer, Integer> docHash;
		private HashSet<String> overAllMaxWords;
		private int overAllMaxLength = 0;
		
		public ReduceWorker(ReduceTask task) {
			myTask = task;
			docHash = new Hashtable<Integer, Integer>();
			overAllMaxWords = new HashSet<String>();
		}
		
		@Override
		public void run() {
			_combineResults();
			double r = _processResults();
			ReduceTaskResult rtr = new ReduceTaskResult(myTask.getFileName(), r, overAllMaxLength, overAllMaxWords.size());
			results.addElement(rtr);
		}
		
		private void _combineResults() {
			if(myTask.getResults().isEmpty()) {
				return;
			}
			
			for(MapTaskResult mt : myTask.getResults()) {
				for(Integer key : mt.getLengthFrequency().keySet()) {
					Integer value = null;
					if(docHash.containsKey(key)){
						value = docHash.get(key) + mt.getLengthFrequency().get(key);
					} else {
						value = mt.getLengthFrequency().get(key);
					}
					docHash.put(key, value);
				}
				
				if(!mt.getMaximalWords().isEmpty()) {
					if(overAllMaxLength < mt.getMaximalWords().firstElement().length()) {
						overAllMaxLength = mt.getMaximalWords().firstElement().length();
					}
				}
			}
			overAllMaxWords.clear();
			for(MapTaskResult mt : myTask.getResults()) {
				if(!mt.getMaximalWords().isEmpty() && mt.getMaximalWords().firstElement().length() == overAllMaxLength) {
					overAllMaxWords.addAll(mt.getMaximalWords());
				}
			}
		}
		
		private double _processResults() {
			/*What if the sum exceeds int?*/
			double rank = 0.0d;
			double sum = 0.0d;
			double wordsCnt = 0.0d;
			for(java.util.Map.Entry<Integer, Integer> e : docHash.entrySet()) {
				sum += Fibonacci.getNthElement(e.getKey() + 1) * e.getValue();
				wordsCnt += e.getValue();
			}
			
			rank = sum / wordsCnt;
			return rank;
		}
	}
}
