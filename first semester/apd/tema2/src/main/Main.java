package main;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.Vector;
import java.util.concurrent.LinkedTransferQueue;
import inputoutput.InputParser;
import threadpool.MappingWorkPool;
import threadpool.ReduceWorkPool;
import workingdata.MapTask;
import workingdata.MapTaskResult;
import workingdata.ReduceTask;
import workingdata.ReduceTaskResult;

public class Main {

	public static void main(String[] args) {
		if(args.length != 3) {
			System.err.println("Wrong usage! app.exe threadCount inputFile outputFile.");
		}
		int threadsCnt = Integer.parseInt(args[0]);
		String inputFileName = args[1];
		String outputFileName = args[2];
		
		final InputParser ip = new InputParser(inputFileName);
		Vector<MapTask> tasks = ip.parseTasks();
		
		MappingWorkPool mwp = new MappingWorkPool(threadsCnt);
		mwp.AddAll(tasks);
		mwp.run();
		
		Vector<MapTaskResult> partialResults = mwp.getResults();
		HashMap<String, Vector<MapTaskResult> > rawHash = new HashMap<String, Vector<MapTaskResult>>();
		for(MapTaskResult mtr : partialResults) {
			Vector<MapTaskResult> temp = null;
			if(rawHash.containsKey(mtr.getFileName())) {
				temp = rawHash.get(mtr.getFileName());
			} else {
				temp = new Vector<MapTaskResult>();
				
			}
			temp.add(mtr);
			rawHash.put(mtr.getFileName(), temp);
		}
		
		LinkedTransferQueue<ReduceTask> queue = new LinkedTransferQueue<ReduceTask>();
		for(Entry<String, Vector<MapTaskResult> > e : rawHash.entrySet()) {
			queue.add(new ReduceTask(e.getKey(), e.getValue()));
		}		
		ReduceWorkPool rwp = new ReduceWorkPool(threadsCnt, queue);
		rwp.run();
		
		Vector<ReduceTaskResult> results = rwp.getRTResults();
		
		Collections.sort(results, new Comparator<ReduceTaskResult>() {

			@Override
			public int compare(ReduceTaskResult o1, ReduceTaskResult o2) {
				int r = 0;
				if(o1.getRank() < o2.getRank()) {
					r = 1;
				} else if(o1.getRank() > o2.getRank()){
					r = -1;
 				} else {
 					if(ip.FileIndex(o1.getFileName()) > ip.FileIndex(o2.getFileName())) {
 						r = 1;
 					} else {
 						r = -1;
 					}
 				}
				return r;
			}
		});
		
		
		
		try {
			BufferedWriter bw = new BufferedWriter(new FileWriter(outputFileName));
			for(ReduceTaskResult rtr : results) {
				bw.write(rtr.toString());
				bw.newLine();
			}
			bw.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}

}
