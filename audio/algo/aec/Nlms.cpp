/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/aec/debug.h>
#include <audio/algo/aec/Nlms.h>
#include <audio/algo/aec/updateFilter.h>
#include <audio/algo/aec/convolution.h>
#include <audio/algo/aec/power.h>

audio::algo::aec::Nlms::Nlms(void) :
  m_filter(),
  m_feedBack() {
	setFilterSize(256);
}

audio::algo::aec::Nlms::~Nlms(void) {
	
}

void audio::algo::aec::Nlms::reset(void) {
	// simply reset filters.
	setFilterSize(m_filter.size());
}

bool audio::algo::aec::Nlms::process(int16_t* _output, const int16_t* _feedback, const int16_t* _microphone, int32_t _nbSample) {
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

bool audio::algo::aec::Nlms::process(float* _output, const float* _feedback, const float* _microphone, int32_t _nbSample) {
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

float audio::algo::aec::Nlms::processValue(float* _feedback, float _microphone) {
	// Error calculation.
	float convolutionValue = audio::algo::aec::convolution(_feedback, &m_filter[0], m_filter.size());
	float error = _microphone - convolutionValue;
	float out = std::avg(-1.0f, error, 1.0f);
	// calculate mu:
	float mu = audio::algo::aec::power(_feedback, m_filter.size());
	//mu = *_feedback * *_feedback;
	//AA_AEC_WARNING("Mu =" << mu);
	if (mu <= 1.5f) {
		// Not enought power in output
		mu = 0.0001; // arbitrary
	} else {
		mu = 1.0f/mu;
		//AA_AEC_WARNING("Mu =" << mu);
	}
	audio::algo::aec::updateFilter(&m_filter[0], _feedback, error*mu, m_filter.size());
	return out;
}

void audio::algo::aec::Nlms::setFilterSize(size_t _sampleRate, std11::chrono::microseconds _time) {
	setFilterSize((_sampleRate*_time.count())/1000000LL);
}

void audio::algo::aec::Nlms::setFilterSize(size_t _nbSample) {
	m_filter.clear();
	m_feedBack.clear();
	m_filter.resize(_nbSample, 0.0f);
	m_feedBack.resize(_nbSample, 0.0f);
}
