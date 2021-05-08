#include <iostream>
#include <queue>
#include <chrono>

#include "Queue.h"
#include "CircularBuffer.h"

int main() {
	std::queue<float> q1;
	std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < 6; ++i) {
		q1.push(abs(rand()) / 1000.0F);
	}
	for (size_t i = 0; i < 200; ++i) {
		q1.push(abs(rand()) / 1000.0F);
		q1.push(abs(rand()) / 1000.0F);
		q1.pop();
		q1.pop();
	}
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::cout << "STL queue time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " us\n";

	Queue<float> q2;

	begin = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < 6; ++i) {
		q2.push(abs(rand()) / 1000.0F);
	}
	for (size_t i = 0; i < 200; ++i) {
		q2.push(abs(rand()) / 1000.0F);
		q2.push(abs(rand()) / 1000.0F);
		q2.pop();
		q2.pop();
	}
	end = std::chrono::high_resolution_clock::now();
	std::cout << "circular-buffer-Queue queue time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " us\n";

	CircularBuffer<float> cb1 {10, 15, 20, 25};
	std::cout << cb1;
	
	return 0;
}
