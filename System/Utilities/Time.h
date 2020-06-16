#pragma once
#include <chrono>
#include <string>

class Time
{
public:
	friend class Application;

	static Time& Get()
	{
		static Time time;
		return time;
	}

	float GetDeltaTimeMs() const noexcept
	{
		return dTime * 1000.f;
	}

	float GetDeltaTimeSec() const noexcept
	{
		return dTime;
	}

	float GetTimeAfterLaunchMs()
	{
		return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count() * 1000.f;
	}
	float GetTimeAfterLaunchSec()
	{
		return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
	}
protected:
	void OnUpdate();
private:
	Time();
	std::chrono::steady_clock::time_point last;
	std::chrono::time_point<std::chrono::steady_clock> start;
	float dTime = 1.f;
};

struct Benchmark
{

	Benchmark(std::string&& title);
	~Benchmark();

	std::string title{};
	std::chrono::time_point<std::chrono::steady_clock> start;

};
