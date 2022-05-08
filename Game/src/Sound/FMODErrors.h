#pragma once
#include "Log/Log.h"
#include "Core/Assert.h"

#include "Core/Core.h"

#include "fmod_errors.h"

static void InternalFMODErrorCheck(FMOD_RESULT res, bool only_warn, const char* function, const char* file, int line, const std::string& soundpath)
{
    if (only_warn)
    {
        GAME_WARN_IF(res != FMOD_OK, "[FMOD error]: {0}, {1}): {2} {3}: {4}: {0}", res, FMOD_ErrorString(res), function, file, line);
    }
    else
    {
        GAME_CORE_ASSERT(res == FMOD_OK, "[FMOD error]: {0}, {1}): {2} {3}: {4}: {0}", res, FMOD_ErrorString(res), function, file, line);
    }
}

#define FMODErrorCheck(x) InternalFMODErrorCheck(x,false,#x,__FILE__,__LINE__, "Unknown")
#define FMODErrorCheckPath(x,soundpath) InternalFMODErrorCheck(x,false,#x,__FILE__,__LINE__,soundpath)
#define FMODErrorCheckPathWarn(x,soundpath,only_warn) InternalFMODErrorCheck(x,only_warn,#x,__FILE__,__LINE__, soundpath)
