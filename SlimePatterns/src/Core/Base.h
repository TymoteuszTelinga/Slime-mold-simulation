#pragma once

#include <memory>

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

struct vec3
{
	float r, g, b;

	vec3(float R, float G, float B)
		: r(R), g(G), b(B) {};

	vec3(float v) 
		: r(v), g(v), b(v) {};

	vec3() 
		: vec3(1.f) {};
};

struct vec4
{
	float v[4];

	vec4(float R, float G, float B, float A)
	{
		v[0] = R;
		v[1] = G;
		v[2] = B;
		v[3] = A;
	};

	vec4(float V)
	{
		v[0] = V;
		v[1] = V;
		v[2] = V;
		v[3] = V;
	};

	vec4()
		: vec4(1.f) {};
};
