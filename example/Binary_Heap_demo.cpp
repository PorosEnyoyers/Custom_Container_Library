#include "Binary_Heap.h"
#include <iostream>
#include <vector>
#include <array>
#include <initializer_list>

int main()
{
	const std::vector<int> v{ 0,1,2,3,4,5,6,7,8,9,10 };
	custom::binary_heap<int, custom::heap_type::MAX, 11> h{ v };
	h.push(5);
	return 0;
}