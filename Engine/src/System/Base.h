#pragma once
#pragma warning( disable : 4006 )

#include <memory>

#define	SAFE_RELEASE(x)	if(x){ x->Release(); x = NULL;	}

#define NULL_CHECK(val)  if (val == nullptr);

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
