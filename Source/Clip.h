#pragma once
#include <string>
#include <iostream>

class Clip {
public:
	virtual ~Clip() = default;

	void SetStartBeat(double beat) { mStartBeat = beat; }
	double GetStartBeat() const { return mStartBeat; }

	void SetDuration(double beats) { mDuration = beats; }
	double GetDuration() const { return mDuration; }

	// clip source offset in beats
	void SetOffset(double offset) { mOffset = offset; }
	double GetOffset() const { return mOffset; }

	double GetEndBeat() const { return mStartBeat + mDuration; }

	void SetName(const std::string& name) { mName = name; }
	const std::string& GetName() const { return mName; }

	virtual void Save(std::ostream& out) {
		out << "CLIP_NAME \"" << mName << "\"\n";
		out << "START " << mStartBeat << "\n";
		out << "DUR " << mDuration << "\n";
		out << "OFFSET " << mOffset << "\n";
	}

	virtual void Load(std::istream& in) {
		// parsing handled in subclasses
	}
protected:
	double mStartBeat = 0.0;
	double mDuration = 4.0; // 1 bar
	double mOffset = 0.0;	// content offset
	std::string mName = "Clip";
};
