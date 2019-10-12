#pragma once
#include <typeinfo>

namespace quickMath
{
	template<typename num>
	num Sign(num n)
	{
		int ret = (n > 0) ? 1 : ((n < 0) ? -1 : 0);
		return ret; 
		//return (n > 0) ? 1 : ((n < 0) ? -1 : 0); 
	}
}
