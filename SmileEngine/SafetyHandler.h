#pragma once

#ifdef __cplusplus
extern "C++" {
#endif

	


// -----------------------------------------------------------------
// comment a define if you do not want that extension anywhere, 
// or undefine it after including this header if you do not want it 
// to be included in a particular file. 
#define MATH_EXTENSION

// -----------------------------------------------------------------



// -----------------------------------------------------------------
#ifdef  MATH_EXTENSION
	
	namespace sMath
	{
		constexpr unsigned int Infinite = ~0;
		static const char* disionBy0Msg = "Error: division by 0";

		template<typename num>
		inline static num SafeDivisionInf(num dividend, num divisor)
		{
			//static_assert((double)divisor != 0 && disionBy0Msg);

			if ((double)divisor == 0)
			{
#pragma warning (disionBy0Msg); 
				return (num)Infinite;
			}
			else
				return dividend / divisor;

		}

		template<typename num>
		inline static num SafeDivision0(num dividend, num divisor)
		{
			//static_assert((double)divisor != 0 && disionBy0Msg);

			if ((double)divisor == 0)
			{
#pragma warning (disionBy0Msg); 
				return (num)0;
			}
			else
				return dividend / divisor;
		}

#define SaveDivision SafeDivisionInf; 

		template<typename num>
		num Sign(num n)
		{
			return (n > 0) ? 1 : ((n < 0) ? -1 : 0);
		}
	}


#endif 



#ifdef __cplusplus
}
#endif

