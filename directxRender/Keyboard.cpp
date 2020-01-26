#include "Keyboard.h"

#include "Util.h"

using namespace Util;

bool Keyboard::KeyPressed(uint8_t keyCode) const noexcept
{
	return keyStates[keyCode];
}

Keyboard::Event Keyboard::ReadKey()
{
	return FirstOrDefault(keyBuffer);
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
	return FirstOrDefault(charBuffer);
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
	Trim(keyBuffer, bufferSize);
}

void Keyboard::OnKeyReleased(uint8_t keyCode)
{
	keyStates[keyCode] = false;
	keyBuffer.emplace(Event::Type::Press, keyCode);
	Trim(keyBuffer, bufferSize);
}

void Keyboard::OnChar(char c)
{
	charBuffer.push(c);
	Trim(charBuffer, bufferSize);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}
