#include "Mouse.h"

#include "utility/Util.h"

using namespace Util;

Mouse::Position Mouse::GetPosition() const noexcept
{
	return pos;
}

bool Mouse::Inside() const noexcept
{
	return inside;
}

bool Mouse::LeftPressed() const noexcept
{
	return leftPressed;
}

bool Mouse::RightPressed() const noexcept
{
	return rightPressed;
}

void Mouse::PostEvent(Event::Type type, Position newPos)
{
	switch (type)
	{
	case Event::Type::EnterRegion:
		inside = true;
		break;
	case Event::Type::LeaveRegion:
		inside = false;
		break;
	case Event::Type::LPress:
		leftPressed = true;
		break;
	case Event::Type::LRelease:
		leftPressed = false;
		break;
	case Event::Type::RPress:
		rightPressed = true;
		break;
	case Event::Type::RRelease:
		rightPressed = false;
		break;
	case Event::Type::Move:
		pos = newPos;
		break;
	}

	events.emplace(type, *this);
	Trim(events, bufferSize);
}

void Mouse::OnWheelDelta(int delta, Position pos)
{
	wheelDelta += delta;

	while (wheelDelta >= wheelDeltaThreshold)
	{
		this->PostEvent(Mouse::Event::WheelUp, pos);
		wheelDelta -= wheelDeltaThreshold;
	}

	while (wheelDelta <= -wheelDeltaThreshold)
	{
		this->PostEvent(Mouse::Event::WheelDown, pos);
		wheelDelta += wheelDeltaThreshold;
	}
}

Mouse::Event Mouse::Read()
{
	return PopOrDefault(events);
}

bool Mouse::Empty() const noexcept
{
	return events.empty();
}

void Mouse::Clear() noexcept
{
	events = std::queue<Mouse::Event>();
}
