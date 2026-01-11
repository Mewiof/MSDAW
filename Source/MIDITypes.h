#pragma once
#include <cstdint>

// MIDI message block data
struct MIDIMessage {
	uint8_t status; // 0x90 (on), 0x80 (off)
	uint8_t data1;	// note number
	uint8_t data2;	// velocity
	int frameIndex; // sample offset
};
