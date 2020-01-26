#pragma once

#include <queue>
#include <cstdint>

class Mouse
{
	friend class Window;
public:
	struct Position
	{
		uint16_t x;
		uint16_t y;
	};

	class Event
	{
	public:
		enum Type
		{
			None,
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
		};

		Event() = default;
		Event(Type type, const Mouse& parent)
			: type(type),
			pos(parent.pos),
			leftPressed(parent.leftPressed),
			rightPressed(parent.rightPressed)
		{}
		operator bool() const noexcept
		{
			return type != Type::None;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		Position GetPosition() const noexcept
		{
			return pos;
		}
		bool LeftPressed() const noexcept
		{
			return leftPressed;
		}
		bool RightPressed() const noexcept
		{
			return rightPressed;
		}
	private:
		Type type = Type::None;
		bool leftPressed = false;
		bool rightPressed = false;
		Position pos = { 0, 0 };
	};

	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	Position GetPosition() const noexcept;
	bool LeftPressed() const noexcept;
	bool RightPressed() const noexcept;
	Event Read();
	bool Empty() const noexcept;
	void Clear() noexcept;
private:
	void PostEvent(Event::Type type, Position pos);
	void OnWheelDelta(int delta, Position pos);
	bool leftPressed = false;
	bool rightPressed = false;
	int wheelDelta = 0;
	Position pos = { 0, 0 };
	std::queue<Event> events;

	static constexpr size_t bufferSize = 16;
	static constexpr uint16_t wheelDeltaThreshold = 120;
};
