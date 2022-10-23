#pragma once
#include "Generic.h"
#include <future>
#include <functional>
#include <any>
#include "Scene/UUID.h"
namespace Game
{
template<class _Fty, typename... _Args>
using Ret = std::invoke_result_t<std::decay_t<_Fty>, std::decay_t<_Args>...>;

template<typename T>
using Future = std::future<T>;

template<typename T>
using RefFuture = std::shared_future<T>;

template<typename Def>
using Job = std::function<Def>;

template<class _Fty, typename... _Args>
_NODISCARD inline Future<Ret<_Fty, _Args...>> ExecuteAsync(_Fty&& job, _Args&&... args)
{
	return std::async(std::launch::async, job, std::forward<_Args>(args)...);
}

template<class _Fty, typename... _Args>
_NODISCARD inline Future<Ret<_Fty, _Args...>> ExecuteLazy(_Fty&& job, _Args&&... args)
{
	return std::async(std::launch::deferred, job, std::forward<_Args>(args)...);
}

template<class _Fty, typename... _Args>
_NODISCARD inline RefFuture<Ret<_Fty, _Args...>> ExecuteAsyncRef(_Fty&& job, _Args&&... args)
{
	return std::async(std::launch::async, job, std::forward<_Args>(args)...).share();
}

template<class _Fty, typename... _Args>
_NODISCARD inline RefFuture<Ret<_Fty, _Args...>> ExecuteLazyRef(_Fty&& job, _Args&&... args)
{
	return std::async(std::launch::deferred, job, std::forward<_Args>(args)...).share();
}

template<typename Key,typename T>
struct AwaiterIt
{
	std::unordered_map<Key,T>& vec_;
	int pointer_;

	AwaiterIt(std::unordered_map<Key, T>& vec) : vec_{ vec }, pointer_{ 0 } {}

	AwaiterIt(std::unordered_map<Key, T>& vec, int size) : vec_{ vec }, pointer_{ size } {}

	bool operator!=(const AwaiterIt<Key, T>& other) const
	{
		return !(*this == other);
	}

	bool operator==(const AwaiterIt<Key, T>& other) const
	{
		return pointer_ == other.pointer_;
	}

	AwaiterIt& operator++()
	{
		++pointer_;
		return *this;
	}

	T& operator*() const
	{
		return vec_.at(pointer_);
	}
};

template<typename T>
class Awaiter
{
public:
	using WhenIsReadyCallback = std::function<void(T)>;
private:
	std::unordered_map<UUID, RefFuture<T>> m_Futures;
	WhenIsReadyCallback m_ReadyCallback;
public:
	Awaiter(std::vector<RefFuture<T>>& futures)
	{
		for (auto& f : futures)
			m_Futures[UUID()] = f;
	}
	~Awaiter() = default;

	std::vector<RefFuture<T>> GetAll()
	{
		return GetValues(m_Futures);
	}

	RefFuture<T> GetFirstNotReady()
	{
		for (auto& f : m_Futures)
			if (!f.second.valid() && !f.second._Is_ready())
				return f.second;
	}

	_NODISCARD RefFuture<T> PopFirstReady()
	{
		for (auto& f : m_Futures)
			if (f.second.valid() && f.second._Is_ready())
				return Pop(f.first);
	}

	bool IsAllDone() const
	{
		for (auto& f : m_Futures)
			if (!f.second.valid() || !f.second._Is_ready())
				return false;
		return true;
	}

	_NODISCARD std::vector<RefFuture<T>> PopAllReady()
	{
std::vector<RefFuture<T>> dones;
LABEL_BEGIN_FOREACH_AGAIN:
		for (auto& f : m_Futures)
			if (f.second.valid() && f.second._Is_ready())
			{
				dones.push_back(Pop(f.first));
				goto LABEL_BEGIN_FOREACH_AGAIN;
			}
		return dones;
	}

	void AwaitAll() const
	{
LABEL_IS_NOT_ALL_READY:
		for (auto& f : m_Futures)
		{
			if (f.second.valid() && f.second._Is_ready()) continue;
			f.second.wait();
		}
		if (!IsAllDone()) goto LABEL_IS_NOT_ALL_READY;
	}

	RefFuture<T> Pop(const UUID& f)
	{
		auto it = m_Futures.find(f);
		if (it != m_Futures.end())
		{
			auto copy = (*it).second;
			m_Futures.erase(f);
			return copy;
		}
	}

	AwaiterIt<UUID, RefFuture<T>> begin()
	{
		return AwaiterIt<UUID, RefFuture<T>>(m_Futures);
	}

	AwaiterIt<UUID, RefFuture<T>> end()
	{
		return AwaiterIt<UUID, RefFuture<T>>(m_Futures, m_Futures.size());
	}

	constexpr bool empty() const noexcept
	{
		return m_Futures.empty();
	}

	constexpr size_t size() const noexcept
	{
		return m_Futures.size();
	}
};
}
