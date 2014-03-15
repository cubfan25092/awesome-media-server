#pragma once


namespace globals{

	enum AUDIO_TYPES: unsigned long {
		MP3 = 0x0000,
		M4A = 0x0001,
		AAC = 0x0002,
		FLAC = 0x0004,
		OPUS = 0x0008,
		VORBIS = 0x000f,
		ALAC = 0x0010
	  };
}