// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include <Physics/StateRecorder.h>

namespace JPH {

/// Implementation of the StateRecorder class that uses a stringstream as underlying store and that implements checking if the state doesn't change upon reading
class StateRecorderImpl final : public StateRecorder
{
public:
	/// Constructor
						StateRecorderImpl() = default;
						StateRecorderImpl(StateRecorderImpl &&inRHS)				: StateRecorder(inRHS), mStream(move(inRHS.mStream)) { }

	/// Write a string of bytes to the binary stream
	virtual void		WriteBytes(const void *inData, size_t inNumBytes) override;

	/// Rewind the stream for reading
	void				Rewind();

	/// Read a string of bytes from the binary stream
	virtual void		ReadBytes(void *outData, size_t inNumBytes) override;

	// See StreamIn
	virtual bool		IsEOF() const override										{ return mStream.eof(); }

	// See StreamIn / StreamOut
	virtual bool		IsFailed() const override									{ return mStream.fail(); }

	/// Compare this state with a reference state and ensure they are the same
	bool				IsEqual(StateRecorderImpl &inReference);

private:
	stringstream		mStream;
};

} // JPH