#pragma once

#include <cstring>
#include <ostream>
#include <stdexcept>
#include <iterator>

template <typename T>
class CircularBuffer {
	T *items, *itemse, *pick, *place;
	unsigned int capacitance, stored_count;

public:
	CircularBuffer(unsigned int capacity):capacitance(capacity) {
		if (!capacity) throw std::invalid_argument("Capacity must be at least 1!");

		pick = place = items = reinterpret_cast<T *>(malloc(capacity * sizeof(T)));
		if (!items) throw std::runtime_error("You don't have enough memory!");

		stored_count = 0;
		itemse = items + capacity;
	}

	CircularBuffer(const std::initializer_list<T> &list): capacitance(list.size()) {
		if (!capacitance) throw std::invalid_argument("Capacity must be at least 1!");
		stored_count = capacitance;

		pick = place = items = reinterpret_cast<T *>(malloc(capacitance * sizeof(T)));
		if (!items) throw std::runtime_error("You don't have enough memory!");
		itemse = items + capacitance;

		{
			const T *citem = list.begin();
			for (T *cwpos = items; cwpos != itemse; ++cwpos, ++citem)
				*cwpos = *citem;
		}
	}

	CircularBuffer(const CircularBuffer &arg): capacitance(arg.capacitance), stored_count(arg.stored_count) {
		items = reinterpret_cast<T *>(malloc(capacitance * sizeof(T)));
		if (!items) throw std::runtime_error("You don't have enough memory!");
		itemse = items + capacitance;

		place = items + (arg.place - arg.items);
		pick = items + (arg.pick - arg.items);

		memcpy(items, arg.items, capacitance * sizeof(T));
	}

	CircularBuffer &operator=(const CircularBuffer &arg) {
		if (this != &arg) {
			capacitance = arg.capacitance;
			stored_count = arg.stored_count;

			items = std::unique_ptr<T[]>(new T[capacitance]);
			itemse = items.get() + capacitance;

			place = items.get() + (arg.place - arg.items.get());
			pick = items.get() + (arg.pick - arg.items.get());

			memcpy(items.get(), arg.items.get(), capacitance * sizeof(T));
		}
		
		return *this;
	}

	~CircularBuffer() {
		free(items);
	}

	void emplace(const T &item) {
		if (place == pick) { // Common case
			if (stored_count) { //Ring is full, rewriting
				new (place) T(item); //Assigning an item to a memory address.
				if (++pick == itemse) {
					pick = items;
				}
				place = pick;
			} else { // Ring is empty, all is ok
				new (place) T(item); //Assigning an item to a memory address.
				++stored_count;
				if (++place == itemse) {
					place = items;
				}
			}
		} else {
			new (place) T(item); //Assigning an item to a memory address.
			++stored_count;
			if (++place == itemse) {
				place = items;
			}
		}
	}

	void emplace(unsigned int index, const T &item) {
		if (index >= stored_count) throw std::out_of_range("Index out of range!");

		unsigned int must_steps = stored_count - index;

		T *back = place, *front;

		//Getting empty place
		if (place == pick) {
			if (++pick == itemse) {
				pick = items;
			}
			--must_steps; //It is necessary to account for the change in index from the deletion of the oldest element
		} else
			++stored_count;

		if (++place == itemse) {
			place = items;
		}

		//Shifting items
		for (unsigned int cstep = 0; cstep != must_steps; ++cstep) {
			front = back;
			if (--back < items) {
				back = itemse - 1;
			}
			*front = *back;
		}
		
		//Emplacing
		new (back) T(item);
	}

	void erase(unsigned int index) {
		if (index >= stored_count) throw std::out_of_range("Index out of range!");

		T *back, *front = pick;

		--stored_count;

		//Moving to needed item
		for (unsigned int cstep = 0; cstep != index; ++cstep) {
			if (++front == itemse) {
				front = items;
			}
		}

		if (--place < items) {
			place = itemse - 1;
		}

		//Shifting items
		while (front != place) 	{
			back = front;
			if (++front == itemse) {
				front = items;
			}
			*back = *front;
		}
	}

	unsigned int capacity() const {
		return capacitance;
	}
	
	void capacity(unsigned int new_capacity) {
		if (!new_capacity) throw std::out_of_range("Capacity must be at least 1!");
		if (stored_count > new_capacity) throw std::out_of_range("The capacity of the structure cannot be less than the number of elements in it!");

		T *new_pointer = reinterpret_cast<T *>(malloc(new_capacity * sizeof(T)));
		if (!new_pointer) throw std::runtime_error("You don't have enough memory!");

		if (stored_count) {
			if (place > pick || place == items) {
				memcpy(new_pointer, pick, stored_count * sizeof(T));
			} else {
				memcpy(new_pointer, pick, (itemse - pick) * sizeof(T));
				memcpy(new_pointer + (itemse - pick), items, (place - items) * sizeof(T));
			}
		}

		pick = new_pointer;
		if (new_capacity == stored_count)
			place = new_pointer;
		else
			place = new_pointer + stored_count;

		capacitance = new_capacity;

		free(items); items = new_pointer;
		itemse = items + new_capacity;
	}

	void insert(unsigned int index, T item) {
		const T &ref = item;
		emplace(index, item);
	}

	void push(T item) {
		const T &ref = item;
		emplace(item);
	}

	T pop() {
		if (stored_count) {
			T *item = pick;
			--stored_count;
			if (++pick == itemse) {
				pick = items;
			}
			return *item;
		} else
			throw std::underflow_error("Nothing to Pop!");
	}

	unsigned int count() const {
		return stored_count;
	}


	bool operator==(const CircularBuffer &arg) const {
		if (arg.stored_count != stored_count)
			return false;

		if (stored_count) {
			T *pk = pick, *em = place;
			T *pk2 = arg.pick;

			do {
				if (*pk != *pk2)
					return false;

				if (++pk == itemse)
					pk = items;
				if (++pk2 == arg.itemse)
					pk2 = arg.items;
			} while (pk != em);
		}

		return true;
	}

	bool operator>(const CircularBuffer &arg) const {
		if (stored_count) {
			T *pk = pick, *em = place;
			T *pk2 = arg.pick, *em2 = arg.place;

			while (true) {
				if (*pk > *pk2)
					return true;
				else if (*pk < *pk2)
					return false;

				if (++pk == itemse)
					pk = items;
				if (pk == em) return false;
				if (++pk2 == arg.itemse)
					pk2 = arg.items;
				if (pk2 == em2) return true;
			}
		}

		return false;
	}

	//Just to be short. If necessary, you can make a separate implementation for each operator (like for == and >), which will speed them up.
	bool operator!=(const CircularBuffer &arg) const {
		return !(*this == arg);
	}

	bool operator>=(const CircularBuffer &arg) const {
		return *this > arg || *this == arg;
	}

	bool operator<=(const CircularBuffer &arg) const {
		return !(*this > arg);
	}

	bool operator<(const CircularBuffer &arg) const {
		return *this <= arg && *this != arg;
	}

	class iterator {
		friend class CircularBuffer;

		unsigned int index, err_index;
		T *ptr, *base, *end;
		iterator(T *current, T *base, T *end, unsigned int index, unsigned int err_index): ptr(current), base(base), end(end), index(index), err_index(err_index){ }
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		//iterator(const iterator &it); //Not need
		//iterator &operator=(iterator const &other);
		
		unsigned int get_index() const {
			return index;
		}

		bool operator!=(iterator const &other) const {
			return index != other.index;
		}

		bool operator==(iterator const &other) const {
			return index == other.index;
		}

		T &operator*() const {
			if (index == err_index) throw std::logic_error("Can not dereference end iterator!");
			return *ptr;
		}

		T *operator->() const {
			if (index == err_index) throw std::logic_error("Can not dereference end iterator!");
			return ptr;
		}

		iterator &operator++() {
			if (index == err_index) throw std::out_of_range("Iterator out of range!");
			++index;
			if (++ptr == end) {
				ptr = base;
			}
			return *this;
		}

		iterator operator++(int) {
			if (index == err_index) throw std::out_of_range("Iterator out of range!");
			iterator temp(*this);
			++index;
			if (++ptr == end) {
				ptr = base;
			}
			return temp;
		}

		iterator &operator--() {
			if (index == 0) throw std::out_of_range("Iterator out of range!");
			--index;
			if (--ptr < base) {
				ptr = end - 1;
			}
			return *this;
		}

		iterator operator--(int) {
			if (index == 0) throw std::out_of_range("Iterator out of range!");
			iterator temp(*this);
			--index;
			if (--ptr < base) {
				ptr = end - 1;
			}
			return temp;
		}
	};

	iterator begin() const {
		return iterator(pick, items, itemse, 0, stored_count);
	}

	iterator end() const {
		return iterator(place, items, itemse, stored_count, stored_count);
	}
};

template<typename T>
std::ostream &operator<<(std::ostream &out, const CircularBuffer<T> &buffer) {
	for (auto item : buffer) {
		out << item << std::endl;
	}
	return out;
}