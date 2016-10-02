/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/river/updateFilter.hpp>


void audio::algo::river::updateFilter(float* _filter, float* _data, float _value, int32_t _count) {
	for (size_t iii = 0; iii < _count; ++iii) {
		*(_filter++) += *_data-- * _value;
	}
}

