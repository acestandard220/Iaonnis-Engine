#pragma once
#include "log.h"

#define SAFE_DELETE(x) if(x){ delete x; x = nullptr}

#define PI 3.141592.0

#define IAONNIS_ASSERT(expression,msg)                                \
if (!(expression))                                              \
{                                                               \
    IAONNIS_LOG_ERROR("Assertion failed: %s", msg);                 \
    assert(expression && msg);                              \
}