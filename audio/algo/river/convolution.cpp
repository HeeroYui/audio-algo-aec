/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/river/convolution.h>

float audio::algo::river::convolution(float* _dataMinus, float* _dataPlus, size_t _count) {
	float out = 0.0f;
	for (size_t iii = 0; iii < _count; ++iii) {
		out += *_dataMinus-- * *_dataPlus++;
	}
	return out;
}

