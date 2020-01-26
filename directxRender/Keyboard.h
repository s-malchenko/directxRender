#pragma once

#include <queue>
#include <bitset>
#include <cstdint>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum Type
		{
			None,
			Press,
			Release,
		};

		Event() = default;
		Event(Type type, uint8_t code) : type(type), code(code)
		{}
		operator bool() const noexcept
		{
			return type != Type::None;
		}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		uint8_t Code() const noexcept
		{
			return code;
		}

	private:
		Type type = Type::None;
		uint8_t code = 0;
	};

	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	//key event
	bool KeyPressed(uint8_t keyCode) const noexcept;
	Event ReadKey();
	bool KeyEmpty() const noexcept;
	void ClearKey() noexcept;
	//char event
	char ReadChar() noexcept;
	bool CharEmpty() const noexcept;
	void ClearChars() noexcept;
	//clear chars and key info
	void ClearAll() noexcept;
	//autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatEnabled() const noexcept;
private:
	void OnKeyPressed(uint8_t keyCode);
	void OnKeyReleased(uint8_t keyCode);
	void OnChar(char c);
	void ClearState() noexcept;

	static constexpr auto keyCount = 256u;
	static constexpr auto bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<keyCount> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};
