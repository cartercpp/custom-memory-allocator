#include <iostream>
#include "allocator.hpp"

int main()
{
	allocator alloc;
	int* numbers = alloc.allocate<int>(3);
	numbers[0] = 67;
	numbers[1] = 67;
	numbers[2] = 67;

	std::cout << numbers[0] << ' ' << numbers[1] << ' ' << numbers[2] << '\n';
	alloc.deallocate(numbers);

	return 0;
}
