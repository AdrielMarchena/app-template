#include "pch.h"
#include "ParseArgs.h"

static inline std::string ToUpper(const std::string& str)
{
	std::string tmp = "";
	for (auto& c : str)
	{
		if (c >= 'a' && c <= 'z')
			tmp += c - 32;
		else
			tmp += c;
	}
	return tmp;
}
static inline std::string ToLower(const std::string& str)
{
	std::string tmp = "";
	for (auto& c : str)
	{
		if (c >= 'A' && c <= 'Z')
			tmp += c + 32;
		else
			tmp += c;
	}
	return tmp;
}


namespace Generic
{

void ConsoleArgsParser::RegistryFlag(const std::string& flag_name)
{
	const std::string lower = ToLower(flag_name);
	if (m_Flags.find(lower) == m_Flags.end())
		m_Flags[lower];
}
void ConsoleArgsParser::RegistryOption(const std::string& opt_name)
{
	const std::string lower = ToLower(opt_name);
	if (m_Options.find(lower) == m_Options.end())
		m_Options[lower];
}

float ConsoleArgsParser::GetOptionAsFloat(const std::string& opt) const
{
	const std::string o = GetOptionAsString(opt);
	if (!o.empty())
	{
		return std::stof(o);
	}
	return -1.0f;
}

int ConsoleArgsParser::GetOptionAsInt(const std::string& opt) const
{
	const std::string o = GetOptionAsString(opt);
	if (!o.empty())
	{
		return std::stoi(o);
	}
	return -1;
}

const std::string& ConsoleArgsParser::GetOptionAsString(const std::string& opt) const
{
	if (!opt.empty())
	{
		auto optionIt = m_Options.find(ToLower(opt));
		if (optionIt != m_Options.end())
		{
			return optionIt->second;
		}
	}
	static std::string EmptyOption = "";
	return EmptyOption;
}

const std::string& ConsoleArgsParser::GetOption(const std::string& opt) const
{
	return GetOptionAsString(opt);
}

bool ConsoleArgsParser::GetFlag(const std::string& flag) const
{
	if (!flag.empty())
	{
		auto flagIt = m_Flags.find(flag);
		if (flagIt != std::end(m_Flags))
		{
			return flagIt->second;
		}
	}
	return false;
}

void ConsoleArgsParser::P_ParseArgs(int argc, char* argv[])
{
	if (argc > 1)
	{
		for (int i = 0; i < argc; ++i)
		{
			std::string arg = argv[i];
			if (arg.length() >= 3)
			{
				if (arg[0] == '-' && arg[1] == '-')
				{
					arg = arg.substr(2);

					if (arg.find_first_of('=') != std::string::npos)
					{
						//Option
						const std::size_t equalSignPos = arg.find('=');
						if (equalSignPos != std::string::npos)
						{
							std::string optionName = arg.substr(0, equalSignPos);
							std::string optionValue = arg.substr(equalSignPos + 1);

							if (m_RegistryMode)
							{
								auto optionIt = m_Options.find(optionName);
								if (optionIt != m_Options.end())
								{
									optionIt->second = optionValue;
								}
							}
							else
							{
								m_Options[ToLower(optionName)] = optionValue;
							}

						}
					}
					else
					{
						if (m_RegistryMode)
						{
							auto flagIt = m_Flags.find(arg);
							if (flagIt != std::end(m_Flags))
							{
								flagIt->second = true;
							}
						}
						else
						{
							m_Flags[ToLower(arg)] = true;
						}
					}
				}
			}
		}
	}
}
} //End Generic Namespace