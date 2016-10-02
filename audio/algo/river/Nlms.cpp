/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <audio/algo/river/debug.hpp>
#include <audio/algo/river/Nlms.hpp>
#include <audio/algo/river/updateFilter.hpp>
#include <audio/algo/river/convolution.hpp>
#include <audio/algo/river/power.hpp>
#define MAX_PROCESSING_BLOCK_SIZE (256)

namespace audio {
	namespace algo {
		namespace river {
			class NlmsPrivate {
				private:
					int8_t m_nbChannel;
					float m_sampleRate;
					enum audio::format m_format;
					std::vector<std::vector<float> > m_filter; //!< Current filter
					std::vector<std::vector<float> > m_feedBack; //!< Feedback history
					float m_mu; //!< mu step size
				public:
					NlmsPrivate() :
					  m_nbChannel(1),
					  m_sampleRate(48000),
					  m_format(audio::format_float),
					  m_filter(),
					  m_feedBack(),
					  m_mu(0.03f) {
						setFilterSize(256);
					}
					
					~NlmsPrivate(void) {
						
					}
					
					void reset(void) {
						// simply reset filters.
						setFilterSize(m_filter.size());
					}
					
					void init(int8_t _nbChannel, float _sampleRate, enum audio::format _format) {
						m_nbChannel = _nbChannel;
						m_sampleRate = _sampleRate;
						m_format = _format;
						setFilterSize(m_filter.size());
						switch (m_format) {
							case audio::format_int16:
							case audio::format_float:
								break;
							default:
								AA_RIVER_ERROR("Can not INIT LMS with unsupported format : " << m_format);
								break;
						}
					}
					
					void process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk) {
						switch (m_format) {
							case audio::format_int16:
								processI16(reinterpret_cast<int16_t*>(_output),
								           reinterpret_cast<const int16_t*>(_inputFeedback),
								           reinterpret_cast<const int16_t*>(_input),
								           _nbChunk);
								break;
							case audio::format_float:
								processFloat(reinterpret_cast<float*>(_output),
								             reinterpret_cast<const float*>(_inputFeedback),
								             reinterpret_cast<const float*>(_input),
								             _nbChunk);
								break;
							default:
								AA_RIVER_ERROR("Can not Echo remove with unsupported format : " << m_format);
								break;
						}
					}
					
					bool processI16(int16_t* _output, const int16_t* _microphone, const int16_t* _feedback, int32_t _nbSample) {
						bool ret = false;
						int32_t processingSize = MAX_PROCESSING_BLOCK_SIZE*m_nbChannel;
						// due to the fact we allocate the data in the stack:
						int32_t nbCycle = _nbSample/processingSize;
						if (_nbSample - int32_t(_nbSample/processingSize)*processingSize != 0 ) {
							nbCycle++;
						}
						for (int32_t bbb=0; bbb<nbCycle; ++bbb) {
							float output[processingSize];
							float feedback[processingSize];
							float microphone[processingSize];
							int32_t offset = bbb*processingSize;
							int32_t nbData = std::min(processingSize,
							                          _nbSample - offset);
							for (size_t iii=0; iii<nbData; ++iii) {
								microphone[iii] = float(_microphone[offset+iii])/32767.0f;
								feedback[iii] = float(_feedback[offset+iii])/32767.0f;
							}
							ret = processFloat(output, feedback, microphone, nbData);
							for (size_t iii=0; iii<nbData; ++iii) {
								_output[offset+iii] = int16_t(float(output[iii])*32767.0f);
							}
						}
						return ret;
					}
					
					bool processFloat(float* _output, const float* _microphone, const float* _feedback, int32_t _nbSample) {
						for (int8_t kkk=0; kkk<m_nbChannel; ++kkk) {
							// add sample in the feedback history:
							m_feedBack[kkk].resize(m_filter[kkk].size()+_nbSample, 0.0f);
							memcpy(&m_feedBack[kkk][m_filter[kkk].size()], _feedback, _nbSample*sizeof(float));
							for (int32_t iii=0; iii < _nbSample; iii++) {
								_output[iii] = processValue(&m_feedBack[kkk][m_filter[kkk].size()+iii], _microphone[iii], m_filter[kkk]);
							}
							// remove old value:
							m_feedBack[kkk].erase(m_feedBack[kkk].begin(), m_feedBack[kkk].begin() + (m_feedBack[kkk].size()-m_filter[kkk].size()) );
						}
						return true;
					}
					
					float processValue(float* _feedback, float _microphone, std::vector<float>& _filter) {
						// Error calculation.
						float convolutionValue = audio::algo::river::convolution(_feedback, &_filter[0], _filter.size());
						float error = _microphone - convolutionValue;
						float out = std::avg(-1.0f, error, 1.0f);
						// calculate mu:
						float mu = audio::algo::river::power(_feedback, _filter.size());
						//mu = *_feedback * *_feedback;
						//AA_RIVER_WARNING("Mu =" << mu);
						if (mu <= 1.5f) {
							// Not enought power in output
							mu = 0.0001; // arbitrary
						} else {
							mu = 1.0f/mu;
							//AA_RIVER_WARNING("Mu =" << mu);
						}
						audio::algo::river::updateFilter(&_filter[0], _feedback, error*m_mu, _filter.size());
						return out;
					}
					
					void setFilterTime(std::chrono::microseconds _time) {
						setFilterSize((m_sampleRate*_time.count())/1000000LL);
					}
					
					void setFilterSize(size_t _nbSample) {
						m_filter.clear();
						m_feedBack.clear();
						m_filter.resize(m_nbChannel);
						m_feedBack.resize(m_nbChannel);
						for (int8_t kkk=0; kkk<m_nbChannel; ++kkk) {
							m_filter[kkk].resize(_nbSample, 0.0f);
							m_feedBack[kkk].resize(_nbSample, 0.0f);
						}
					}
					
					void setMu(float _val) {
						m_mu = _val;
					}
			};
		}
	}
}

audio::algo::river::Nlms::Nlms(void) {
	
}

audio::algo::river::Nlms::~Nlms(void) {
	
}

void audio::algo::river::Nlms::reset(void) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->reset();
}

void audio::algo::river::Nlms::init(int8_t _nbChannel, float _sampleRate, enum audio::format _format) {
	if (m_private == nullptr) {
		m_private = ememory::makeShared<audio::algo::river::NlmsPrivate>();
	}
	m_private->init(_nbChannel, _sampleRate, _format);
}

std::vector<enum audio::format> audio::algo::river::Nlms::getSupportedFormat() {
	std::vector<enum audio::format> out = getNativeSupportedFormat();
	out.push_back(audio::format_int16);
	return out;
}

std::vector<enum audio::format> audio::algo::river::Nlms::getNativeSupportedFormat() {
	std::vector<enum audio::format> out;
	out.push_back(audio::format_float);
	return out;
}

void audio::algo::river::Nlms::process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->process(_output, _input, _inputFeedback, _nbChunk);
}


void audio::algo::river::Nlms::setFilterTime(std::chrono::microseconds _time) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setFilterTime(_time);
}

void audio::algo::river::Nlms::setFilterSize(size_t _nbSample) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setFilterSize(_nbSample);
}

void audio::algo::river::Nlms::setMu(double _val) {
	if (m_private == nullptr) {
		AA_RIVER_ERROR("Algo is not initialized...");
	}
	m_private->setMu(_val);
}

