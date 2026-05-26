#ifndef CURVES_H
#define CURVES_H
#include <vector>

namespace crv {

	inline float linear(float x, float m, float b = 0) {
		return m * x + b;
	}

	inline float quadratic(float x, float m, float k) {
		return powf(x/m, k);
	}

	inline float logistic(float x, float k) {
		return 1.f / (1.f + powf(k,-1*x));
	}


}

#endif