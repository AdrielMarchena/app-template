#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <list>
namespace midi
{

	struct MidiEvent
	{
		enum class Type
		{
			NoteOff,
			NoteOn,
			Other
		} Event;

		uint8_t Key = 0;
		uint8_t Velocity = 0;
		uint32_t WallTick = 0;
		uint32_t DeltaTick = 0;

	};

	struct MidiNote
	{
		uint8_t Key = 0;
		uint8_t Velocity = 0;
		uint32_t StartTime = 0;
		uint32_t Duration = 0;
	};

	struct MidiTrack
	{
		std::string Name;
		std::string Instrument;
		std::vector<MidiEvent> Events;
		std::vector<MidiNote> Notes;
		uint8_t MaxNote = 64;
		uint8_t MinNote = 64;
	};

	class MidiFile
	{
	public:

		std::vector<MidiTrack> Tracks;

		enum EventName : uint8_t
		{
			VoiceNoteOff = 0x80,
			VoiceNoteOn = 0x90,
			VoiceAftertouch = 0xA0,
			VoiceControlChange = 0xB0,
			VoiceProgramChange = 0xC0,
			VoiceChannelPressure = 0xD0,
			VoicePitchBend = 0xE0,
			SystemExclusive = 0xF0,
		};

		enum MetaEventName : uint8_t
		{
			MetaSequence = 0x00,
			MetaText = 0x01,
			MetaCopyright = 0x02,
			MetaTrackName = 0x03,
			MetaInstrumentName = 0x04,
			MetaLyrics = 0x05,
			MetaMarker = 0x06,
			MetaCuePoint = 0x07,
			MetaChannelPrefix = 0x20,
			MetaEndOfTrack = 0x2F,
			MetaSetTempo = 0x51,
			MetaSMPTEOffset = 0x54,
			MetaTimeSignature = 0x58,
			MetaKeySignature = 0x59,
			MetaSequencerSpecific = 0x7F,
		};

	public:
		MidiFile() = default;
		MidiFile(const std::string& fileName)
		{
			ParseFile(fileName);
		}

		bool ParseFile(const std::string& fileName)
		{

			std::ifstream ifs;
			ifs.open(fileName, std::fstream::in | std::ios::binary);
			if (!ifs.is_open())
				return false;

			//Swap the bits is required, MIDI format store they the other way around
			auto Swap32 = [](uint32_t n)
			{
				return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | ((n << 24) & 0xff000000));
			};

			auto Swap16 = [](uint16_t n)
			{
				return ((n >> 8) | (n << 8));
			};

			auto ReadString = [&ifs](uint32_t lenght)
			{
				std::string s;
				for (uint32_t i = 0; i < lenght; i++)
					s += ifs.get();
				return s;
			};

			//For the 7 bits thing where the 8 says you must read the next bit the same way to form the number
			auto ReadValue = [&ifs]()
			{
				uint32_t value = 0;
				uint8_t byte = 0;

				value = ifs.get();
				// Check MSB, if set, more bytes need reading
				if (value & 0x80)
				{
					//Extract bottom 7 bits of read byte
					value &= 0x7F;

					do
					{
						byte = ifs.get();

						value = (value << 7) | (byte & 0x7F);

					} while (byte & 0x80); //loop while MSb is set to 1

				}
				return value;
			};

			//Parsing
			uint32_t n32 = 0;
			uint16_t n16 = 0;

			//Read MIDI Header (fixed size)
			ifs.read((char*)&n32, sizeof(uint32_t));
			uint32_t fileId = Swap32(n32);
			ifs.read((char*)&n32, sizeof(uint32_t));
			uint32_t headerLength = Swap32(n32);
			ifs.read((char*)&n16, sizeof(uint16_t));
			uint16_t format = Swap16(n16);
			ifs.read((char*)&n16, sizeof(uint16_t));
			uint16_t trackChunks = Swap16(n16);
			ifs.read((char*)&n16, sizeof(uint16_t));
			uint16_t division = Swap16(n16);


			for (uint16_t chunk = 0; chunk < trackChunks; chunk++)
			{

				std::cout << "===== NEW TRACK" << std::endl;

				//Read track header
				ifs.read((char*)&n32, sizeof(uint32_t));
				uint32_t trackId = Swap32(n32);
				ifs.read((char*)&n32, sizeof(uint32_t));
				uint32_t trackLength = Swap32(n32);

				bool endOfTrack = false;
				int8_t previousStatus = 0;

				Tracks.push_back(MidiTrack());

				while (!ifs.eof() && !endOfTrack)
				{
					//Fundamentally all MIDI events contain a timecode, and a status byte (but not all of them)
					uint32_t statusTimeDelta = 0;
					uint8_t status = 0;

					//Read Timecode
					statusTimeDelta = ReadValue();
					status = ifs.get();

					if ((status < 0x80))
					{
						status = previousStatus;
						ifs.seekg(-1, std::ios_base::cur);
					}

					switch ((status & 0xF0))
					{
					case EventName::VoiceNoteOff:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t noteId = ifs.get();
						uint8_t noteVelocity = ifs.get();
						Tracks[chunk].Events.push_back({ MidiEvent::Type::NoteOff, noteId, noteVelocity, statusTimeDelta });
						break;
					}
					case EventName::VoiceNoteOn:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t noteId = ifs.get();
						uint8_t noteVelocity = ifs.get();
						if(noteVelocity == 0)
							Tracks[chunk].Events.push_back({ MidiEvent::Type::NoteOff, noteId, noteVelocity, statusTimeDelta });
						else
							Tracks[chunk].Events.push_back({ MidiEvent::Type::NoteOn, noteId, noteVelocity, statusTimeDelta });

						break;
					}
					case EventName::VoiceAftertouch:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t noteId = ifs.get();
						uint8_t noteVelocity = ifs.get();
						Tracks[chunk].Events.push_back({ MidiEvent::Type::Other });
						break;
					}
					case EventName::VoiceControlChange:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t noteId = ifs.get();
						uint8_t noteVelocity = ifs.get();
						Tracks[chunk].Events.push_back({ MidiEvent::Type::Other });
						break;
					}
					case EventName::VoiceProgramChange:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t programId = ifs.get();
						Tracks[chunk].Events.push_back({ MidiEvent::Type::Other });
						break;
					}
					case EventName::VoiceChannelPressure:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t channelPressure = ifs.get();
						Tracks[chunk].Events.push_back({ MidiEvent::Type::Other });
						break;
					}
					case EventName::VoicePitchBend:
					{
						previousStatus = status;
						uint8_t channel = status & 0x0F;
						uint8_t LS7B = ifs.get();
						uint8_t MS7B = ifs.get();
						Tracks[chunk].Events.push_back({ MidiEvent::Type::Other });
						break;
					}
					case EventName::SystemExclusive:
					{

						if (status == 0xF0)
						{
							std::cout << "System Exclusive Begin: " << ReadString(ReadValue()) << std::endl;
						}

						if (status == 0xF7)
						{
							std::cout << "System Exclusive End: " << ReadString(ReadValue()) << std::endl;
						}

						if (status == 0xFF)
						{
							uint8_t type = ifs.get();
							uint8_t length = ReadValue();

							switch (type)
							{
							case MetaSequence:
								std::cout << "Sequence Number: " << ifs.get() << ifs.get() << std::endl;
								break;
							case MetaText:
								std::cout << "Text: " << ReadString(length) << std::endl;
								break;
							case MetaCopyright:
								std::cout << "Copyright: " << ReadString(length) << std::endl;
								break;
							case MetaTrackName:
								Tracks[chunk].Name = ReadString(length);
								std::cout << "Track Name: " << Tracks[chunk].Name << std::endl;
								break;
							case MetaInstrumentName:
								Tracks[chunk].Instrument = ReadString(length);
								std::cout << "Instrument Name: " << Tracks[chunk].Instrument << std::endl;
								break;
							case MetaLyrics:
								std::cout << "Lyrics: " << ReadString(length) << std::endl;
								break;
							case MetaMarker:
								std::cout << "Marker: " << ReadString(length) << std::endl;
								break;
							case MetaCuePoint:
								std::cout << "Cue: " << ReadString(length) << std::endl;
								break;
							case MetaChannelPrefix:
								std::cout << "Prefix: " << ifs.get() << std::endl;
								break;
							case MetaEndOfTrack:
								endOfTrack = true;
								break;
							case MetaSetTempo:
								// Tempo is in microseconds per quarter note	
								if (m_nTempo == 0)
								{
									(m_nTempo |= (ifs.get() << 16));
									(m_nTempo |= (ifs.get() << 8));
									(m_nTempo |= (ifs.get() << 0));
									m_nBPM = (60000000 / m_nTempo);
									std::cout << "Tempo: " << m_nTempo << " (" << m_nBPM << "bpm)" << std::endl;
								}
								break;
							case MetaSMPTEOffset:
								std::cout << "SMPTE: H:" << ifs.get() << " M:" << ifs.get() << " S:" << ifs.get() << " FR:" << ifs.get() << " FF:" << ifs.get() << std::endl;
								break;
							case MetaTimeSignature:
								std::cout << "Time Signature: " << ifs.get() << "/" << (2 << ifs.get()) << std::endl;
								std::cout << "ClocksPerTick: " << ifs.get() << std::endl;

								// A MIDI "Beat" is 24 ticks, so specify how many 32nd notes constitute a beat
								std::cout << "32per24Clocks: " << ifs.get() << std::endl;
								break;
							case MetaKeySignature:
								std::cout << "Key Signature: " << ifs.get() << std::endl;
								std::cout << "Minor Key: " << ifs.get() << std::endl;
								break;
							case MetaSequencerSpecific:
								std::cout << "Sequencer Specific: " << ReadString(length) << std::endl;
								break;
							default:
								std::cout << "Unrecognised MetaEvent: " << type << std::endl;
							}

						}

						Tracks[chunk].Events.push_back({ MidiEvent::Type::Other });
						break;
					}
					default:
						std::cout << "Unreconised Status Byte: " << status << std::endl;
						break;
					}//End Switch
				}
			}

			//COnvert Time Events to notes
			for (auto& track : Tracks)
			{
				uint32_t wallTime = 0;

				std::list<MidiNote> listNotesBeingProcessed;

				for (auto& event : track.Events)
				{
					wallTime += event.DeltaTick;

					if (event.Event == MidiEvent::Type::NoteOn)
					{
						listNotesBeingProcessed.push_back({ event.Key, event.Velocity, wallTime, 0 });
					}

					if (event.Event == MidiEvent::Type::NoteOff)
					{
						auto note = std::find_if(listNotesBeingProcessed.begin(), listNotesBeingProcessed.end(), [&](const MidiNote& n) { return n.Key == event.Key; });
					
						if (note != listNotesBeingProcessed.end())
						{
							note->Duration = wallTime - note->StartTime;
							track.Notes.push_back(*note);
							track.MinNote = std::min(track.MinNote, note->Key);
							track.MaxNote = std::max(track.MaxNote, note->Key);
							listNotesBeingProcessed.erase(note);
						}
					}
				}
			}
		}
		public:
			uint32_t m_nTempo = 0;
			uint32_t m_nBPM = 0;
	};

}