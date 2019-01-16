#ifndef _yi_h_
#define _yi_h_
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif // !__cplusplus

#include "lua.h"
#include "lauxlib.h"
#include "lapi.h"

#include "ultil.h"
#include "api.h"
#include "types.h"

#ifdef __cplusplus
}
#endif

#endif // !_yi_h_