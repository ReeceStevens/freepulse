#ifndef __CircleBuffer_h__
#define __CircleBuffer_h__ 1

template <typename T>

class CircleBuffer {
private:
	int length;
	T* data;
	int next;
	int end;

public:
	// Null case for empty fifo.
    CircleBuffer() {
		length = 2;
		data = new T[length];
		for (int i = 0; i < length; i ++) {
		    data[i] = (T) 0;
		}
		next = 0; 
		end = 1;
    }

	// Requests length of N+1 for circular operation
	CircleBuffer(int N) {
		length = N+1;
		data = new T[length];	
		for (int i = 0; i < length; i ++) {
		    data[i] = (T) 0;
		}
		next = 0;
		end = 1;
	};

	CircleBuffer(const CircleBuffer<T>& that) {
        T* newdata =  new T[that.length];
		this->length = that.length;
		this->data = newdata;
		for (int i = 0; i < this->length; i ++) {
		    this->data[i] = that.data[i];
		}
		this->next = that.next;
		this->end = that.end;
	};

    int size(void) {
        return this->length;
    }

    void resize(int new_len) {
        if (new_len <= this->length) { return; }
        T* new_data = new T[new_len];
		for (int i = 0; i < length; i ++) {
		    new_data[i] = data[i];
		}
        length = new_len;
        delete [] data;
        data = new_data;
    };

	bool add(const T& payload) {
		data[next] = payload;
		next = end;
		end = mod((end + 1), length);
        if (end == 0) { return true; } // True if circling back
        return false;
	};

	T& operator[](const int k) {
		if (length <= k) {
          __attribute__((unused)) int* p = 0;
            p = (int*) 1; // TODO: more graceful way to handle this case?
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

	~CircleBuffer() {
		delete [] data;
	}
};

#endif
