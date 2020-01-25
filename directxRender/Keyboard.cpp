#include "Keyboard.h"

template <class T>
T firstOrDefault(std::queue<T>& q)
{
	if (!q.empty())
	{
		auto result = q.front();
		q.pop();
		return result;
	}

	return T();
}

bool Keyboard::KeyPressed(uint8_t keyCode) const noexcept
{
	return keyStates[keyCode];
}

Keyboard::Event Keyboard::ReadKey()
{
	return firstOrDefault(keyBuffer);
}

bool Keyboard::KeyEmpty() const noexcept
{
	return keyBuffer.empty();
}

void Keyboard::ClearKey() noexcept
{
	keyBuffer = std::queue<Event>();
}

char Keyboard::ReadChar() noexcept
{
	return firstOrDefault(charBuffer);
}

bool Keyboard::CharEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::ClearChars() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::ClearAll() noexcept
{
	ClearKey();
	ClearChars();
}

void Keyboard::EnableAutorepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatEnabled() const noexcept
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(uint8_t keyCode)
{
	keyStates[keyCode] = true;
	keyBuffer.emplace(Event::Type::Press, keyCode);
	Trim(keyBuffer);
}

void Keyboard::OnKeyReleased(uint8_t keyCode)
{
	keyStates[keyCode] = false;
	keyBuffer.emplace(Event::Type::Press, keyCode);
	Trim(keyBuffer);
}

void Keyboard::OnChar(char c)
{
	charBuffer.push(c);
	Trim(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}
