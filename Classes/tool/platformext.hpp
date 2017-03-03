#ifndef __INCLUDED_PLATFORMEXT_HPP__
#define __INCLUDED_PLATFORMEXT_HPP__

#include <sstream>
#include <string>
#include <stdlib.h>
#include <random>

namespace ext {
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

inline double stod(const std::string& _Str, size_t *_Idx = 0)
{	// convert string to double
	const char *_Ptr = _Str.c_str();
	char *_Eptr;
	double _Ans = strtod(_Ptr, &_Eptr);

	if (_Ptr == _Eptr)
		return 0;
	if (_Idx != 0)
		*_Idx = (size_t)(_Eptr - _Ptr);
	return (_Ans);
}

  /*
	** return a random integer in the interval [a, b]
	*/

	inline int uniform_int(int a, int b) {
		static std::default_random_engine e{ std::random_device{}() }; // avoid "Most vexing parse"  
		static std::uniform_int_distribution<int> u;
	
	    return u(e, decltype(u)::param_type(a, b));
	
	}
 
  /*
	** return a random real in the interval [a, b] (also [a, b))
	*/
   inline double uniform_real(double a, double b) {
		static std::default_random_engine e{ std::random_device{}() };
		static std::uniform_real_distribution<double> u;
	    return u(e, decltype(u)::param_type(a, b));
	
	}
}

#endif
