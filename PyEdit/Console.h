#pragma once

#include "stdafx.h"

class Console
{
public:
	Console();
	~Console();

	int Run(INT argc, LPWSTR* argv);
private:
	BOOL CreateConsole();
	void ExitMessage();
	void Log(std::string, bool sign);
	bool debug = false;
	bool nocwd = false;
};

