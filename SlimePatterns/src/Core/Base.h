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
