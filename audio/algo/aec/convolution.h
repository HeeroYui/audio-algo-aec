/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_ALGO_AEC_CONVOLUTION_H__
#define __AUDIO_ALGO_AEC_CONVOLUTION_H__

#include <etk/types.h>

namespace audio {
	namespace algo {
		namespace aec {
			float convolution(float* _dataMinus, float* _dataPlus, size_t _count);
		}
	}
}

#endif
