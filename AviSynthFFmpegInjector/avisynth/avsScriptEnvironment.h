#pragma once

class AvsScriptEnvironment
{
private:
	GUID guid;

	static GUID ScriptEnvGuid;
public:
	AvsScriptEnvironment();

	bool CheckSaneState();
private:
	bool CheckGuid();
};
