#ifndef __CircleFifo_h__
#define __CircleFifo_h__ 1

template <typename T>

class CircleFifo {
private:
	int length;
	T* data;
	int next;
	int end;

public:
	// Null case for empty fifo.
    CircleFifo() {
		length = 2;
		data = new T[length];
		for (int i = 0; i < length; i ++) {
		    data[i] = (T) 0;
		}
		next = 0; 
		end = 1;
    }

	// Requests length of N+1 for circular operation
	CircleFifo(int N) {
		length = N+1;
		data = new T[length];	
		for (int i = 0; i < length; i ++) {
		    data[i] = (T) 0;
		}
		next = 0;
		end = 1;
	};

	void add(const T& payload) {
		data[next] = payload;
		next = end;
		end = mod((end + 1), length);
	};
	
	T& operator[](const int k) {
		if (length <= k) {
          __attribute__((unused)) int* p = 0;
          p = (int*) 1; // Intentionally crash.
		}	
		return data[mod(next-k-1, length)];
	};

	T& newest(void) {
		return data[mod(next-1, length)]; 
	}

	static int mod(int a, int b) {
		if (b > 0){
		    if (a < 0) {
				int inv_a = -a;
				do {
					inv_a = inv_a - b;
				} while (inv_a > 0);
				return -inv_a; 
		    }
			else if (a < b) { return a; }
			else {
				do {
					a = a - b;
				} while (a > b);
			return a;
		    }
		}
		else { return -1; } // Error state. Mod will not be called in this context.
	}

	~CircleFifo() {
		delete [] data;
	}
};

#endif
