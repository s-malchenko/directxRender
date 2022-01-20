#pragma once

#include <atomic>
#include <memory>

// Class for swapping the data between two threads.
// Writer thread can modify the data while reader thread is only allowed to read.
// Data holders automatically notify swapper in destructor.
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
	class DataHolder
	{
	public:
		DataHolder() : mSwapper(nullptr), mData(nullptr) {}
		DataHolder(DataSwapper<T>* swapper, T* data) : mSwapper(swapper), mData(data) {}
		virtual ~DataHolder() = default;
		operator bool() { return mSwapper; } // no swapper means that data was not acquired
	protected:
		T* mData;
		DataSwapper<T>* mSwapper;
	};

	class ReaderHolder : public DataHolder
	{
	public:
		ReaderHolder() = default;
		ReaderHolder(DataSwapper<T>* swapper, T* data) : DataHolder(swapper, data) {}
		virtual ~ReaderHolder()
		{
			if (*this)
			{
				DataHolder::mSwapper->ReaderDone();
			}
		}

		const T* GetData() const { return DataHolder::mData; }
	};

	class WriterHolder : public DataHolder
	{
	public:
		WriterHolder() = default;
		WriterHolder(DataSwapper<T>* swapper, T* data) : DataHolder(swapper, data) {}
		virtual ~WriterHolder()
		{
			if (*this)
			{
				DataHolder::mSwapper->WriterDone();
			}
		}

		T* GetData() { return DataHolder::mData; }
	};

	DataSwapper() : mWriterState(State::READY), mReaderState(State::READY)
	{
		mWriterData = std::make_unique<T>();
		mReaderData = std::make_unique<T>();
	}

	// Returns actual data once per cycle. If the data was not swapped since last successfull call, returns invalid holder.
	WriterHolder TryGetDataForWrite()
	{
		if (mWriterState == State::READY)
		{
			mWriterState = State::PROCESSING;
			return WriterHolder(this, mWriterData.get());
		}

		return {};
	}

	// Returns actual data once per cycle. If the data was not swapped since last successfull call, returns invalid holder.
	ReaderHolder TryGetDataForRead()
	{
		if (mReaderState == State::READY)
		{
			mReaderState = State::PROCESSING;
			return ReaderHolder(this, mReaderData.get());
		}

		return {};
	}

private:
	std::unique_ptr<T> mWriterData;
	std::unique_ptr<T> mReaderData;
	std::atomic<State> mWriterState;
	std::atomic<State> mReaderState;


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
