/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_ALGO_RIVER_CONVOLUTION_H__
#define __AUDIO_ALGO_RIVER_CONVOLUTION_H__

#include <etk/types.h>

namespace audio {
	namespace algo {
		namespace river {
			float convolution(float* _dataMinus, float* _dataPlus, size_t _count);
		}
	}
}

#endif
