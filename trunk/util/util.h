#ifndef UTIL_UTIL_H_INCLUDED
#define UTIL_UTIL_H_INCLUDED 1

#define ARRAY_SIZE(x) (int(sizeof(x)/sizeof(x[0])))

namespace {
  template<class T> inline const T& max(const T& a, const T& b) { return a > b ? a : b; }
  template<class T> inline const T& min(const T& a, const T& b) { return a < b ? a : b; }
  template<class T> inline const T& clamp(const T& a, const T& b, const T& c) 
  { 
	  return b < a ? a : (b > c ? c : b); 
  }
};

#endif // _UTIL_H_
