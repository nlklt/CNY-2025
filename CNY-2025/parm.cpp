#include "parm.h"
#include "error.h"

#include <cwchar>

namespace Parm {
	PARM getparm(int argc, wchar_t* argv[]) {
		PARM parm;

		parm.in [0] = L'\0';
		parm.log[0] = L'\0';
		parm.out[0] = L'\0';

		for (int i = 1; i < argc; i++) {
			if (wcslen(argv[i]) >= PARM_MAX_SIZE) { ERROR_THROW(105); }

			if		(wcscmp(argv[i], PARM_IN ) == 0) {
				if (i + 1 >= argc) { ERROR_THROW(100); }
				wcscpy_s(parm.in , argv[++i]);
			}
			else if (wcscmp(argv[i], PARM_LOG) == 0) {
				if (i + 1 >= argc) { ERROR_THROW(101); }
				wcscpy_s(parm.log, argv[++i]);
			}
			else if (wcscmp(argv[i], PARM_OUT) == 0) {
				if (i + 1 >= argc) { ERROR_THROW(102); }
				wcscpy_s(parm.out, argv[++i]);
			}
			else if (wcsstr(argv[i], PARM_IN) || wcsstr(argv[i], PARM_LOG) || wcsstr(argv[i], PARM_OUT)) {
				ERROR_THROW(103);
			}
			else
			{
				ERROR_THROW(106);
			}
		}

		if (parm.in [0] == L'\0') { ERROR_THROW(104); }

		if (parm.log[0] == L'\0') {
			wcscpy_s(parm.log, parm.in);
			wcsncat_s(parm.log, PARM_MAX_SIZE, L".log", 4);
		}
		if (parm.out[0] == L'\0') {
			wcscpy_s(parm.out, parm.in);
			wcsncat_s(parm.out, PARM_MAX_SIZE, L".out.asm", 8);
		}
		return parm;
	}
}