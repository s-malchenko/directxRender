#pragma once

#include <chrono>

//simple class for measuring time in seconds
class Timer
{
public:
	Timer();

	//get passed time and restart timer
	float Mark();

	//get passed time without restart
	float Peek() const;
private:
	std::chrono::steady_clock::time_point start;
};

