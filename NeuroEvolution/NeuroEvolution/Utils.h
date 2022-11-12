#pragma once

#include <utility>
#include <string>
#include <vector>
#include <iostream>


//Needed for unordered map
// custom specialization of std::hash can be injected in namespace std
template<>
struct std::hash<std::pair<unsigned int, unsigned int>>
{
	std::size_t operator()(std::pair<unsigned int, unsigned int> const& pair) const noexcept
	{
		return pair.first ^ (pair.second << 1); // or use boost::hash_combine
	}
};

inline float randFloat() { return ((double)rand()) / RAND_MAX; }

inline int randInt(int x, int y) { return rand() % (y - x + 1) + x; };

bool saveVectorToCsv(std::string name, std::vector<float>& data);

inline int randPosNeg() { return (rand() % 2) == 1 ? 1 : -1; };