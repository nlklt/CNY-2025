#pragma once

#include <tchar.h>

#define PARM_IN				 L"-i"
#define PARM_LOG			 L"-l"
#define PARM_OUT			 L"-o"
#define PARM_MAX_SIZE		 300
#define PARM_OUT_DEFAULT_EXT L".out.asm"
#define PARM_LOG_DEFAULT_EXT L".log"

namespace Parm {
	struct PARM {
		wchar_t in[PARM_MAX_SIZE];
		wchar_t log[PARM_MAX_SIZE];
		wchar_t out[PARM_MAX_SIZE];
	};

	PARM getparm(int argc, wchar_t* argv[]);
}