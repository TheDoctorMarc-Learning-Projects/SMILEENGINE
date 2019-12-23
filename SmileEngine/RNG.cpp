#include "RNG.h"
#include <random>
#include "SmileSetup.h"


pcg32 RNG::rng;  // <---- define static here

bool RNG::Initialize()
{
	LOG("Initializing PCG");

	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine 
	rng.seed(seed_source);

	return true; 
}

// ----------------------------------------------------------------- [Get a random int or float number]  
std::variant<int, float> RNG::GetRandomValue(std::variant<int, float> start,
	std::variant<int, float> end)
{
	if (start.index() != end.index())
		return NAN;

	std::variant<int, float> number;
	std::variant<int, float> test = 1;

	if (start.index() == test.index())  // the index of a variant variable translates to a type (int, float, etc) 
	{
		std::uniform_int_distribution <int> uniform_dist(std::get<int>(start), std::get<int>(end));
		number = uniform_dist(rng);
	}

	else
	{
		std::uniform_real_distribution <float> uniform_dist(std::get<float>(start), std::get<float>(end));
		number = uniform_dist(rng);
	}

	return number;
}

SmileUUID RNG::GetRandomUUID()
{
	SmileUUID ret = 0;
	long double min = 0, max = ULLONG_MAX; 
	std::uniform_real_distribution <long double> uniform_dist(min, max);
	return ret = uniform_dist(rng);
}