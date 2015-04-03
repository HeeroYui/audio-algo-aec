/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/aec/debug.h>
#include <audio/algo/aec/Lms.h>
#include <audio/algo/aec/updateFilter.h>
#include <audio/algo/aec/convolution.h>



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

#define MAX_PROCESSING_BLOCK_SIZE (256)

bool audio::algo::aec::Lms::process(int16_t* _output, const int16_t* _feedback, const int16_t* _microphone, int32_t _nbSample) {
	bool ret = false;
	// due to the fact we allocate the data in the stack:
	int32_t nbCycle = _nbSample/MAX_PROCESSING_BLOCK_SIZE;
	if (_nbSample - int32_t(_nbSample/MAX_PROCESSING_BLOCK_SIZE)*MAX_PROCESSING_BLOCK_SIZE != 0 ) {
		nbCycle++;
	}
	for (int32_t bbb=0; bbb<nbCycle; ++bbb) {
		float output[MAX_PROCESSING_BLOCK_SIZE];
		float feedback[MAX_PROCESSING_BLOCK_SIZE];
		float microphone[MAX_PROCESSING_BLOCK_SIZE];
		int32_t offset = bbb*MAX_PROCESSING_BLOCK_SIZE;
		int32_t nbData = std::min(MAX_PROCESSING_BLOCK_SIZE,
		                          _nbSample - offset);
		for (size_t iii=0; iii<nbData; ++iii) {
			microphone[iii] = float(_microphone[offset+iii])/32767.0f;
			feedback[iii] = float(_feedback[offset+iii])/32767.0f;
		}
		ret = process(output, feedback, microphone, nbData);
		for (size_t iii=0; iii<nbData; ++iii) {
			_output[offset+iii] = int16_t(float(output[iii])*32767.0f);
		}
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

float audio::algo::aec::Lms::processValue(float* _feedback, float _microphone) {
	// Error calculation.
	float convolutionValue = audio::algo::aec::convolution(_feedback, &m_filter[0], m_filter.size());
	float error = _microphone - convolutionValue;
	float out = std::avg(-1.0f, error, 1.0f);
	audio::algo::aec::updateFilter(&m_filter[0], _feedback, error*m_mu, m_filter.size());
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