/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <audio/algo/river/debug.hpp>
#include <audio/algo/river/Suppressor.hpp>

namespace audio {
	namespace algo {
		namespace river {
			class SupressorPrivate {
				private:
					int8_t m_nbChannel;
					float m_sampleRate;
					enum audio::format m_format;
					float m_attaqueStep;
					float m_releaseStep;
					size_t m_nbSampleDelay;
					float m_minGain;
					float m_threashold;
					int32_t m_sampleCount;
					float m_currentGain;
				public:
					SupressorPrivate() :
					  m_nbChannel(1),
					  m_sampleRate(48000),
					  m_format(audio::format_float),
					  m_attaqueStep(0.01),
					  m_releaseStep(0.01),
					  m_nbSampleDelay(0),
					  m_minGain(0.0),
					  m_threashold(0.0),
					  m_sampleCount(0),
					  m_currentGain(1.0) {
						
					}
					
					~SupressorPrivate(void) {
						
					}
					
					void reset(void) {
						// simply reset filters.
						
					}
					
					void init(int8_t _nbChannel, float _sampleRate, enum audio::format _format) {
						m_nbChannel = _nbChannel;
						m_sampleRate = _sampleRate;
						m_format = _format;
						
					}
					
					void process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk) {
						float* output = reinterpret_cast<float*>(_output);
						const float* input = reinterpret_cast<const float*>(_input);
						const float* feedback = reinterpret_cast<const float*>(_inputFeedback);
						for (size_t iii=0; iii<_nbChunk; ++iii) {
							if (feedback[iii] > m_threashold) {
								m_sampleCount = 0;
							} else {
								m_sampleCount++;
							}
							if (m_sampleCount > m_nbSampleDelay) {
								m_currentGain += m_releaseStep;
								if (m_currentGain >= 1.0) {
									m_currentGain = 1.0;
								}
							} else {
								m_currentGain -= m_attaqueStep;
								if (m_currentGain <= 0.0) {
									m_currentGain = 0.0;
								}
							}
							if (m_currentGain < m_minGain) {
								m_currentGain = m_minGain;
							}
							for (int8_t kkk=0; kkk<m_nbChannel; ++kkk) {
								output[iii*m_nbChannel + kkk] = output[iii*m_nbChannel + kkk] * m_currentGain;
							}
						}
					}
					
					/**
					 * @brief Set the algo attaque time.
					 * @param[in] _time Time of the attaque gain.
					 */
					virtual void setAttaqueTime(echrono::microseconds _time) {
						m_attaqueStep = 1.0 / float(int64_t(m_sampleRate) * 1000000000LL / _time.get());
					}
					/**
					 * @brief Set the algo release time.
					 * @param[in] _time Time of the release gain.
					 */
					virtual void setReleaseTime(echrono::microseconds _time) {
						m_releaseStep = 1.0 / float(int64_t(m_sampleRate) * 1000000000LL / _time.get());
					}
					/**
					 * @brief Set the minimum Gain.
					 * @param[in] _gain dB gain to apply for minimum.
					 */
					virtual void setMinimumGain(float _gain) {
						m_minGain = etk::pow(10.0f, _gain/20.0f);
					}
					/**
					 * @brief Set the threashold Gain.
					 * @param[in] _gain dB gain to start removing data.
					 */
					virtual void setThreshold(float _gain) {
						m_threashold = etk::pow(10.0f, _gain/20.0f);
					}
					/**
					 * @brief Set the delay befor releasing time.
					 * @param[in] _time Time of the dalay release.
					 */
					virtual void setReleaseDelay(echrono::microseconds _time) {
						m_nbSampleDelay = int64_t(m_sampleRate) * 1000000000LL / _time.get();
					}
			};
		}
	}
}

audio::algo::river::Supressor::Supressor(void) {
	
}

audio::algo::river::Supressor::~Supressor(void) {
	
}

void audio::algo::river::Supressor::reset(void) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->reset();
}

void audio::algo::river::Supressor::init(int8_t _nbChannel, float _sampleRate, enum audio::format _format) {
	if (m_private == nullptr) {
		m_private = ememory::makeShared<audio::algo::river::SupressorPrivate>();
	}
	m_private->init(_nbChannel, _sampleRate, _format);
}

etk::Vector<enum audio::format> audio::algo::river::Supressor::getSupportedFormat() {
	etk::Vector<enum audio::format> out = getNativeSupportedFormat();
	return out;
}

etk::Vector<enum audio::format> audio::algo::river::Supressor::getNativeSupportedFormat() {
	etk::Vector<enum audio::format> out;
	out.pushBack(audio::format_float);
	return out;
}

void audio::algo::river::Supressor::process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->process(_output, _input, _inputFeedback, _nbChunk);
}


void audio::algo::river::Supressor::setAttaqueTime(echrono::microseconds _time) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setAttaqueTime(_time);
}

void audio::algo::river::Supressor::setReleaseTime(echrono::microseconds _time) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setReleaseTime(_time);
}

void audio::algo::river::Supressor::setMinimumGain(float _gain) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setMinimumGain(_gain);
}

void audio::algo::river::Supressor::setThreshold(float _gain) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setThreshold(_gain);
}

void audio::algo::river::Supressor::setReleaseDelay(echrono::microseconds _time) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setReleaseDelay(_time);
}