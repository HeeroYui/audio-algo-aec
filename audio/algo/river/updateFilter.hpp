/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>

namespace audio {
	namespace algo {
		namespace river {
			void updateFilter(float* _filter, float* _data, float _value, int32_t _count);
		}
	}
}
