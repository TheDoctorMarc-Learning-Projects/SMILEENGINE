#pragma once

namespace quickMath // Two plus two is four, minus one that's three, quick maths
{
	template<typename num>
	num Sign(num n)
	{
		return (n > 0) ? 1 : ((n < 0) ? -1 : 0); 
	}
}
