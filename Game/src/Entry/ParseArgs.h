#pragma once

#include <string>
#include <unordered_map>
	/*	All keys are lower case
	* 
	*	This class have two "modes"
	*	if the Constructor with argc and argv is used
	*	it will start in non registry mode, and it will
	*	registry any flag or option it finds on the args provided
	*	if the normal contructor is used, it will start on registry mode,
	*	in this mode, it is expected that later on the code, you registry
	*	wich flags and options you may accept, and then when you are ready to process the args
	*	call "ParseArgs" method
	*
	*	You can call "DisableRegistryMode" if you want to construct an empty instance and then
	*	process without been on registry mode, additionaly, you can "EnableRegistryMode", both need to be called 
	*	before any processing occur
	* 
	*/

namespace Generic {

	class ConsoleArgsParser
	{
	private:
		std::string m_PathArg;
		std::unordered_map<std::string, bool> m_Flags;
		std::unordered_map<std::string, std::string> m_Options;
		bool m_Parsed = false;
		bool m_RegistryMode;
	public:
		virtual ~ConsoleArgsParser() = default;
		ConsoleArgsParser(int argc, char* argv[])
			:m_RegistryMode(false)
		{
			if (argc)
				m_PathArg = argv[0];
			ParseArgs(argc, argv);
		}

		ConsoleArgsParser()
			:m_RegistryMode(true)
		{
		}

		void ParseArgs(int argc, char* argv[])
		{
			if (m_Parsed)
				return;
			P_ParseArgs(argc, argv);
			m_Parsed = true;
		}

		bool GetFlag(const std::string& flag) const;

		void RegistryFlag(const std::string& flag_name);
		void RegistryOption(const std::string& opt_name);

		void DisableRegistryMode()
		{
			if (m_Parsed) return;
			m_RegistryMode = false;
		}
		void EnableRegistryMode()
		{
			if (m_Parsed) return;
			m_RegistryMode = false;
		}

		float GetOptionAsFloat(const std::string& opt) const;
		int GetOptionAsInt(const std::string& opt) const;
		const std::string& GetOptionAsString(const std::string& opt) const;
		const std::string& GetOption(const std::string& opt) const;

		const std::string& GetArgZero() const { return m_PathArg; }
		const std::unordered_map<std::string, std::string>& GetOptMap() const { return m_Options; }
		const std::unordered_map<std::string, bool>& GetFlagMap() const { return m_Flags; }
	private:
		void P_ParseArgs(int argc, char* argv[]);
	};
}