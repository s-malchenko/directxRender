#pragma once

#include <atomic>
#include <memory>

// Class for swapping the data between two threads.
// Writer thread can modify the data while reader thread is only allowed to read.
template <class T>
class DataSwapper
{
private:
	enum class State
	{
		READY,
		PROCESSING,
		DONE,
	};

public:
	DataSwapper() : mWriterState(State::READY), mReaderState(State::READY)
	{
		mWriterData = std::make_unique<T>();
		mReaderData = std::make_unique<T>();
	}

	// Returns actual data pointer once per cycle. If the data was not swapped since last successfull call, returns nullptr.
	T* TryGetDataForWrite()
	{
		if (mWriterState == State::READY)
		{
			mWriterState = State::PROCESSING;
			return mWriterData.get();
		}

		return nullptr;
	}

	// Returns actual data pointer once per cycle. If the data was not swapped since last successfull call, returns nullptr.
	const T* TryGetDataForRead()
	{
		if (mReaderState == State::READY)
		{
			mReaderState = State::PROCESSING;
			return mReaderData.get();
		}

		return nullptr;
	}

	void WriterDone()
	{
		if (mWriterState == State::PROCESSING)
		{
			mWriterState = State::DONE;
			TrySwap();
		}
	}

	void ReaderDone()
	{
		if (mReaderState == State::PROCESSING)
		{
			mReaderState = State::DONE;
			TrySwap();
		}
	}

private:
	std::unique_ptr<T> mWriterData;
	std::unique_ptr<T> mReaderData;
	std::atomic<State> mWriterState;
	std::atomic<State> mReaderState;

	void TrySwap()
	{
		if (mWriterState == State::DONE && mReaderState == State::DONE)
		{
			std::swap(mWriterData, mReaderData);
			mWriterState = State::READY;
			mReaderState = State::READY;
		}
	}
};
