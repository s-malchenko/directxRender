#include "Timer.h"

using namespace std::chrono;

Timer::Timer()
{
	Reset();
}

void Timer::Reset() noexcept
{
	startPoint = steady_clock::now();
	paused = false;
	prevTick = startPoint;
	currentDelta = 0;
}

float Timer::GetPassedTime() const noexcept
{
	if (paused)
	{
		return DeltaSeconds(startPoint, pausePoint);
	}

	return DeltaSeconds(startPoint, steady_clock::now());
}

float Timer::Delta() const noexcept
{
	return currentDelta;
}

bool Timer::Paused() const noexcept
{
	return paused;
}

void Timer::Pause() noexcept
{
	if (!paused)
	{
		paused = true;
		pausePoint = steady_clock::now();
	}
}

void Timer::Continue() noexcept
{
	if (!paused)
	{
		return;
	}

	paused = false;
	const auto pauseTime = steady_clock::now() - pausePoint;
	startPoint += pauseTime;
	prevTick += pauseTime;
}

void Timer::Tick() noexcept
{
	const auto tpNow = paused ? pausePoint : steady_clock::now();
	currentDelta = DeltaSeconds(prevTick, tpNow);
	prevTick = tpNow;
}

float Timer::DeltaSeconds(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point finish)
{
	return duration<float>(finish - start).count();
}
