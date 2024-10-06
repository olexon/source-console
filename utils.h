#pragma once
#include "includes.h"

#if _WIN32 || _WIN64
#if _WIN64
#define ENV64
#else
#define ENV32
#endif
#endif

using namespace std;
namespace fs = std::filesystem;

namespace utils 
{
	extern unsigned int Instances;
	extern vector<string> proc_names;

	void Config();
	void Initialize();
	void SendCommand(const char* m_pTestCommand);
}