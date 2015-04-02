/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/aec/debug.h>
#include <audio/algo/aec/Lms.h>

audio::algo::aec::Lms::Lms(void) :
  m_filter(),
  m_feedBack(),
  m_mu(0.03f) {
	setFilterSize(256);
}

audio::algo::aec::Lms::~Lms(void) {
	
}

void audio::algo::aec::Lms::reset(void) {
	// simply reset filters.
	setFilterSize(m_filter.size());
}

bool audio::algo::aec::Lms::process(int16_t* _output, const int16_t* _feedback, const int16_t* _microphone, int32_t _nbSample) {
	float output[_nbSample];
	float feedback[_nbSample];
	float microphone[_nbSample];
	for (size_t iii=0; iii<_nbSample; ++iii) {
		microphone[iii] = float(_microphone[iii])/32767.0f;
		feedback[iii] = float(_feedback[iii])/32767.0f;
	}
	bool ret = process(output, feedback, microphone, _nbSample);
	for (size_t iii=0; iii<_nbSample; ++iii) {
		_output[iii] = int16_t(float(output[iii])*32767.0f);
	}
	return ret;
}

bool audio::algo::aec::Lms::process(float* _output, const float* _feedback, const float* _microphone, int32_t _nbSample) {
	// add sample in the feedback history:
	m_feedBack.resize(m_filter.size()+_nbSample, 0.0f);
	memcpy(&m_feedBack[m_filter.size()], _feedback, _nbSample*sizeof(float));
	for (int32_t iii=0; iii < _nbSample; iii++) {
		_output[iii] = processValue(&m_feedBack[m_filter.size()+iii], _microphone[iii]);
	}
	// remove old value:
	m_feedBack.erase(m_feedBack.begin(), m_feedBack.begin() + (m_feedBack.size()-m_filter.size()) );
	return true;
}


static float convolution(float* _dataMinus, float* _dataPlus, size_t _count) {
	float out = 0.0f;
	for (size_t iii = 0; iii < _count; ++iii) {
		out += *_dataMinus-- * *_dataPlus++;
	}
	return out;
}

static void updateFilter(float* _filter, float* _data, float _value, int32_t _count) {
	for (size_t iii = 0; iii < _count; ++iii) {
		*(_filter++) += *_data-- * _value;
	}
}

float audio::algo::aec::Lms::processValue(float* _feedback, float _microphone) {
	// Error calculation.
	float convolutionValue = convolution(_feedback, &m_filter[0], m_filter.size());
	float error = _microphone - convolutionValue;
	float out = std::avg(-1.0f, error, 1.0f);
	updateFilter(&m_filter[0], _feedback, error*m_mu, m_filter.size());
	return out;
}

void audio::algo::aec::Lms::setFilterSize(size_t _sampleRate, std11::chrono::microseconds _time) {
	setFilterSize((_sampleRate*_time.count())/1000000LL);
}

void audio::algo::aec::Lms::setFilterSize(size_t _nbSample) {
	m_filter.clear();
	m_feedBack.clear();
	m_filter.resize(_nbSample, 0.0f);
	m_feedBack.resize(_nbSample, 0.0f);
}

void audio::algo::aec::Lms::setMu(float _val) {
	m_mu = _val;
}