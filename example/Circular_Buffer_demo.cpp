#include "Circular_Buffer.h"
#include <iostream>
#include <vector>
#include <array>
#include <initializer_list>

int main()
{
	const std::vector<int> v{ 1,2,3,4,5,6,7,8,9,10 };
	custom::circular_buffer<int, 15> c{ v };
	const std::array<int, 10> a {11, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	for (const auto& i : a)
	{
		c.push_back(i);
	}
	std::cout << c;
	for (std::size_t i{ 5 }; i != 0; --i)
	{
		c.pop_front();
	}
	std::cout << "\npop front called 5 times: " << c;
	return 0;
}