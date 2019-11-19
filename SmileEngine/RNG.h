#pragma once
#include "Utility.h"
#include <variant>
#include <vector>
#include "pcg/include/pcg_random.hpp"
#include "SmileSetup.h"

class RNG : public Utility
{
public:
	bool Initialize(); 

public:
	 std::variant<int, float> GetRandomValue(std::variant<int, float> start = 0,
		std::variant<int, float> end = INT_MAX);

	 SmileUUID GetRandomUUID();
private: 
	 pcg32 rng; 
}; 
