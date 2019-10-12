#pragma once

namespace quickMath
{
	template<typename num>
	num Sign(num n)
	{
		return (n > 0) ? 1 : ((n < 0) ? -1 : 0); 
	}
}
