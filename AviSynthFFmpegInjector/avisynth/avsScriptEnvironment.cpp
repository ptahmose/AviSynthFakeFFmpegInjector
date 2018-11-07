#include "stdafx.h"
#include "avsScriptEnvironment.h"

/*static*/GUID AvsScriptEnvironment::ScriptEnvGuid = { 0x6e2da142, 0x7180, 0x438c, { 0xbd, 0x94, 0xa, 0x85, 0xfa, 0xbb, 0xbc, 0x55 } };

AvsScriptEnvironment::AvsScriptEnvironment()
{
	this->guid = AvsScriptEnvironment::ScriptEnvGuid;
}

bool AvsScriptEnvironment::CheckSaneState()
{
	bool isSane = this->CheckGuid();

	return isSane;
}

bool AvsScriptEnvironment::CheckGuid()
{
	return this->guid == AvsScriptEnvironment::ScriptEnvGuid;
}