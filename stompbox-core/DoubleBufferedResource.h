#pragma once

#include <atomic>
#include <mutex>

template <typename T>
class DoubleBufferedResource
{
public:
	T& GetRead()
	{
		return *currentResource;
	}

	T& GetWriteLock()
	{
		writeLock.lock();

		return (currentResource == &resource1) ? resource2 : resource1;
	}

	void FinishWrite()
	{
		Swap();

		writeLock.unlock();
	}

private:
	void Swap()
	{
		currentResource.exchange((currentResource == &resource1) ? &resource2 : &resource1);
	}

	T resource1;
	T resource2;
	std::atomic<T*> currentResource = &resource1;
	std::mutex writeLock;
};