#pragma once

#include <chrono>

// pausable timer for measuring time in seconds
class Timer
{
public:
	Timer();

	float GetPassedTime() const noexcept;
	float Delta() const noexcept;
	bool Paused() const noexcept;
	void Reset() noexcept;
	void Pause() noexcept;
	void Continue() noexcept;
	void Tick() noexcept;
private:
	std::chrono::steady_clock::time_point startPoint;
	std::chrono::steady_clock::time_point pausePoint;
	std::chrono::steady_clock::time_point prevTick;
	float currentDelta;
	bool paused;
	static float DeltaSeconds(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point finish);
};

