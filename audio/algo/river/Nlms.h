/** @file
 * @author Edouard DUPIN
 * @author Fatima MARFOUQ
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_ALGO_RIVER_NLMS_H__
#define __AUDIO_ALGO_RIVER_NLMS_H__

#include <etk/types.h>
#include <chrono>
#include <audio/format.h>
#include <memory>

namespace audio {
	namespace algo {
		namespace river {
			class NlmsPrivate;
			/**
			 * @brief Least Mean Square (LMS) algorithm "echo canceller"
			 * base on publication: http://www.arpapress.com/Volumes/Vol7Issue1/IJRRAS_7_1_05.pdf
			    Electronic description:
			                                            /                                                                 
			                                     o---o /|                                                                 
			                      _feedback      |   |/ |                                                                 
			                   >---------------->|   |  | >~~~~~~~~o                                                      
			                         x(n)        |   |\ |          |                                                      
			                                     o---o \|          |                                                      
			                                            \      o--------0                                                 
			                                                   |        |    Environement                                 
			                                                   |   u(n) |  transfert fonction                             
			                                                   |        |                                                 
			                                                   o--------o                                                 
			                                                       |                                                      
			                                                       |                                                      
			                                     o---o ___         |                                                      
			                      _microphone    |   |/   \ <~~~~~~o                                                      
			                   <----------------<|   |    | <~~~~~~~~~~~~< Noise                                          
			                         d(n)        |   |\___/ <~~~~~~0                                                      
			                                     o---o             |                                                      
			                                                       |                                                      
			                                                       o~~~~~< Usefull signal                                 
			                                                                    s(n)                                      
			                                                                                                              
			    LMS Algorithm:
			                                                                                                              
			       _microphone -----------------------------o                                                             
			           d(n)                                 |                                                             
			                                                |                                                             
			                           o--------o           |    o-------------o                                          
			 o---> filter    --------->|        |           o--->|             |                      _output             
			 |        û(n)             | convol-|                | d(n) - y(n) |----> e(n) -------> echo-cancelled        
			 |                         | -ution |----> y(n) ---->|             |        |                                 
			 |     _feedback -----o--->|        |                o-------------o        |                                 
			 |        x(n)        |    o--------o                                       |                                 
			 |                    |                                                     |                                 
			 |                    |         o----------------------------------o        |                                 
			 |                    |         |                                  |        |                                 
			 |                    o-------->|                                  |<-------o                                 
			 |                              |  û(n+1) =   û(n)                 |                                          
			 |                              |           + mu * e(n) * x(n)     |                                          
			 |                              |                                  |                                          
			 |                              o----------------------------------o                                          
			 |                                            |                                                               
			 |                                            |                                                               
			 o--------------------------------------------o                                                               
			                                                                                                              
			                                                                                                              
			                                                                                                              
			*/
			class Nlms {
				public:
					/**
					 * @brief Constructor
					 */
					Nlms();
					/**
					 * @brief Destructor
					 */
					virtual ~Nlms();
				public:
					/**
					 * @brief Reset algorithm
					 */
					virtual void reset();
					/**
					 * @brief Initialize the Algorithm
					 * @param[in] _nbChannel Number of channel in the stream.
					 * @param[in] _sampleRate Sample rate.
					 * @param[in] _format Input/output data format.
					 */
					virtual void init(int8_t _nbChannel, float _sampleRate, enum audio::format _format = audio::format_float);
					/**
					 * @brief Get list of format suported in input.
					 * @return list of supported format
					 */
					virtual std::vector<enum audio::format> getSupportedFormat();
					/**
					 * @brief Get list of algorithm format suported. No format convertion.
					 * @return list of supported format
					 */
					virtual std::vector<enum audio::format> getNativeSupportedFormat();
					/**
					 * @brief Main input algo process.
					 * @param[in,out] _output Output data.
					 * @param[in] _input Input data.
					 * @param[in] _inputFeedback Input feedback data (all time MONO).
					 * @param[in] _nbChunk Number of chunk in the input buffer.
					 * @param[in] _nbChannel Number of channel in the stream.
					 */
					virtual void process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk);
				protected:
					std11::shared_ptr<NlmsPrivate> m_private; // private data.
				public: // parameter area:
					/**
					 * @brief Set filter size with specifing the filter temporal size and his samplerate
					 * @param[in] _time Time of the filter size
					 */
					virtual void setFilterTime(std11::chrono::microseconds _time);
					/**
					 * @brief Set filter size in number of sample
					 * @param[in] _nbSample Sample size of the filter
					 */
					virtual void setFilterSize(size_t _nbSample);
					/**
					 * @brief Set Mu value for basic LMS value
					 * @param[in] _val new mu value
					 */
					virtual void setMu(double _val);
			};
		}
	}
}

#endif
