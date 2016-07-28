package workingdata;

import java.util.Hashtable;

/*Provides fibonacci number up to 45th element*/
public class Fibonacci {
	private static Hashtable<Integer, Integer> hash = new Hashtable<Integer, Integer>();
	private static final int maxWordLength = 44;
	
	static {
		for(int i = 0; i < maxWordLength; i++) {
			int fib = compute_fibonacci(i);
			hash.put(i, fib);
		}
	}
	
	private static int compute_fibonacci(int n) {
		if(0 == n) {
			return 0;
		} else if (1 == n) {
			return 1;
		} else if(hash.containsKey(n)) {
			return hash.get(n);
		} else {
			int rez = compute_fibonacci(n - 1) + compute_fibonacci(n - 2);
			hash.put(n, rez);
			return rez;
		}
	}
	
	public static Integer getNthElement(int n) {
		if(hash.containsKey(n)) {
			return hash.get(n);
		} else {
			return compute_fibonacci(n);
		}
	}
}
