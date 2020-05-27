#include "Util.h"

#include <chrono>
#include <thread>

void Util::SleepMs(size_t ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
