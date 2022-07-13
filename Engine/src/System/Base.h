#pragma once

#include <memory>

#define	SAFE_RELEASE(x)	if(x){ x->Release(); x = NULL;	}

namespace Engine
{
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
