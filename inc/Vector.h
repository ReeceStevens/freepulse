// Vector.h interface description for the Vector abstract data type (ADT)

#ifndef _Vector_h
#define _Vector_h 1

//#include <assert.h>

template<typename T>

struct Vector {
private:
	uint32_t length; // number of elements stored in the vector
	uint32_t capacity; // size of the array (amount of available storage)
	T* data; // pointer to the array on the heap (never null)

        void copy(const Vector<T>& that) {
          T* newdata = new T[that.length];
          for (uint32_t k = 0; k < that.length; k += 1) {
             newdata[k] = that.data[k];
          } 
          this->data = newdata;
          this->length = that.length; 
          this->capacity = this->length;
        }


        void copy(const Vector<T>& that) volatile {
          T* newdata = new T[that.length];
          for (uint32_t k = 0; k < that.length; k += 1) {
             newdata[k] = that.data[k];
          } 
          this->data = newdata;
          this->length = that.length; 
          this->capacity = this->length;
        }

public:
    // Default constructor just gives us an empty vector.
    Vector(void) {
        length = 0;
        capacity = 2;
        data = new T[2];
    }

    // Copy constructor
    Vector(const Vector<T>& that) {
          T* newdata = new T[that.length];
          for (uint32_t k = 0; k < that.length; k += 1) {
             newdata[k] = that.data[k];
          } 
          this->data = newdata;
          this->length = that.length; 
          this->capacity = this->length;
    }

    // Copy constructor
    Vector(const volatile Vector<T>& that) {
          T* newdata = new T[that.length];
          for (uint32_t k = 0; k < that.length; k += 1) {
             newdata[k] = that.data[k];
          } 
          this->data = newdata;
          this->length = that.length; 
          this->capacity = this->length;
    }
	/* initialize and return a vector for my client, the vector will contain
	sz elements all initialized to the default constructor for that given type */
	Vector(uint32_t sz) {
        length = sz;
        capacity = sz;
        data = new T[sz];
        for (uint32_t k = 0; k < length; k += 1) {
            data[k] = 0;
        }
    }
	~Vector(void) {
        delete [] data;
    }

    void resize(int new_len) {
        if (new_len <= this->capacity) { return; }
        T* new_data = new T[new_len];
		for (int i = 0; i < this->length; i ++) {
		    new_data[i] = data[i];
		}
        this->capacity = new_len;
        delete [] this->data;
        this->data = new_data;
    }

	uint32_t size(void) {
        return length;
    }

	T& operator[](uint32_t k) {
        if (length <= k) {
          __attribute__((unused)) int* p = 0;
          p = (int*) 1; // Intentionally crash.
        }
        //assert(length > k);
        return data[k]; 
    }

	T& operator[](uint32_t k) volatile {
        if (length <= k) {
          __attribute__((unused)) int* p = 0;
          p = (int*) 1; // Intentionally crash.
        }
        //assert(length > k);
        return data[k]; 
    }

    bool operator!=(const Vector<T>& that) {
        if (this->length != that.length) {
            return false;
        }
        for (uint32_t k = 0; k < this->length; k +=1 ){
            if (this->data[k] != that.data[k]) {
                return false;
            }
        }
        return true;
    }
    
    bool operator==(const Vector<T>& that) {
        return !(*this != that);
    }

	int set(uint32_t k, const T& v){
        //assert(length > k);
        if (length <= k) {
          return 0;
        }
        data[k] = v;
        return 1;
    }

	int set(uint32_t k, const T& v) volatile {
        //assert(length > k);
        if (length <= k) {
          return 0;
        }
        data[k] = v;
        return 1;
    } 

    Vector<T>& operator=(volatile Vector<T>& that) volatile {
       if (this->data == that.data) {
           return *this;
        }
        delete [] this->data;
        this->copy(that);
        return *this;
    }

    Vector<T>& operator=(const Vector<T>& that) {
       if (this->data == that.data) {
           return *this;
        }
        delete [] this->data;
        this->copy(that);
        return *this;
    }

    volatile Vector<T>& operator=(const Vector<T>& that) volatile {
       if (this->data == that.data) {
           return *this;
        }
        delete [] this->data;
        this->copy(that);
        return *this;
    }

	void push_back(const T& x) {
        // Amortized doubling of vector capacity
        if (length == capacity) {
            T* old_data = data;
            capacity *= 2;
            T* new_data = new T[capacity];
            
		    for (uint32_t k = 0; k < this->length; k += 1) {
		    	new_data[k] = old_data[k];
		    }
            data = new_data;
            delete [] old_data;
        } 
        data[length] = x;
        length += 1;
    }

};

#endif /* _Vector_h */
