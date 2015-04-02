/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/aec/power.h>


float audio::algo::aec::power(float* _data, int32_t _count) {
	float out = 0;
	for (size_t iii = 0; iii < _count; ++iii) {
		out += *_data * *_data;
		_data--;
	}
	return out;
}

