/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/log.h>

namespace audio {
	namespace algo {
		namespace river {
			int32_t getLogId();
		}
	}
}

#define AA_RIVER_BASE(info,data) TK_LOG_BASE(audio::algo::river::getLogId(),info,data)

#define AA_RIVER_CRITICAL(data)      AA_RIVER_BASE(1, data)
#define AA_RIVER_ERROR(data)         AA_RIVER_BASE(2, data)
#define AA_RIVER_WARNING(data)       AA_RIVER_BASE(3, data)
#ifdef DEBUG
	#define AA_RIVER_INFO(data)          AA_RIVER_BASE(4, data)
	#define AA_RIVER_DEBUG(data)         AA_RIVER_BASE(5, data)
	#define AA_RIVER_VERBOSE(data)       AA_RIVER_BASE(6, data)
	#define AA_RIVER_TODO(data)          AA_RIVER_BASE(4, "TODO : " << data)
#else
	#define AA_RIVER_INFO(data)          do { } while(false)
	#define AA_RIVER_DEBUG(data)         do { } while(false)
	#define AA_RIVER_VERBOSE(data)       do { } while(false)
	#define AA_RIVER_TODO(data)          do { } while(false)
#endif

#define AA_RIVER_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			APPL_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

