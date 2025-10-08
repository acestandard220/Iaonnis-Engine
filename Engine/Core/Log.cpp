#include "log.h"

namespace Iaonnis {

	Log s_log;

	Log& Log::logger()
	{
		return s_log;
	}

}
