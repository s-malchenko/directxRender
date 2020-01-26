#include "Timer.h"

using namespace std::chrono;

Timer::Timer()
{
	start = steady_clock::now();
}

float Timer::Mark()
{
	const auto prevStart = start;
	start = steady_clock::now();
	return duration<float>(start - prevStart).count();
}

float Timer::Peek() const
{
	return duration<float>(steady_clock::now() - start).count();
}
