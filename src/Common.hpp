/*
 EassyStream EIP, 2021
 easystream_main_plugin
 File description:
 common
*/

#ifndef COMMONHPP
#define COMMONHPP

// Common C includes
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <cstddef>

// Common C++ includes
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>
#include <atomic>
#include <numeric>
#include <condition_variable>
#include <list>
#include <thread>
#include <fstream>
#include <sstream>

// QT includes
#include <QStringList>
#include <QRegularExpression>
#include <QLibrary>

// Common OBS includes
#include <obs-module.h>
#include <obs.h>
#include <obs-frontend-api.h>
#include <util/platform.h>
#include <util/config-file.h>
#include <util/util_uint64.h>

#ifndef __FUNCTION_NAME__
#ifdef WIN32 // WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif

// Common Files Includes
#include "obs/Memory.hpp"
#include "plugin-macros.generated.h"
#include "Macros.hpp"

#endif /* !COMMONHPP */