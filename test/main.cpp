/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <test-debug/debug.h>
#include <etk/etk.h>
#include <audio/algo/river/Lms.h>
#include <audio/algo/river/Nlms.h>
#include <etk/os/FSNode.h>
#include <chrono>
#include <etk/thread/tools.h>

#include <unistd.h>
#undef __class__
#define __class__ "test"


class Performance {
	private:
		std11::chrono::steady_clock::time_point m_timeStart;
		std11::chrono::steady_clock::time_point m_timeStop;
		std11::chrono::nanoseconds m_totalTimeProcessing;
		std11::chrono::nanoseconds m_minProcessing;
		std11::chrono::nanoseconds m_maxProcessing;
		int32_t m_totalIteration;
	public:
		Performance() :
		  m_totalTimeProcessing(0),
		  m_minProcessing(99999999999999LL),
		  m_maxProcessing(0),
		  m_totalIteration(0) {
			
		}
		void tic() {
			m_timeStart = std11::chrono::steady_clock::now();
		}
		void toc() {
			m_timeStop = std11::chrono::steady_clock::now();
			std11::chrono::nanoseconds time = m_timeStop - m_timeStart;
			m_minProcessing = std::min(m_minProcessing, time);
			m_maxProcessing = std::max(m_maxProcessing, time);
			m_totalTimeProcessing += time;
			m_totalIteration++;
			
		}
		
		std11::chrono::nanoseconds getTotalTimeProcessing() {
			return m_totalTimeProcessing;
		}
		std11::chrono::nanoseconds getMinProcessing() {
			return m_minProcessing;
		}
		std11::chrono::nanoseconds getMaxProcessing() {
			return m_maxProcessing;
		}
		int32_t getTotalIteration() {
			return m_totalIteration;
		}
};



int main(int _argc, const char** _argv) {
	// the only one init for etk:
	etk::init(_argc, _argv);
	etk::thread::setName("test thread");
	std::string fbName = "";
	std::string micName = "";
	int32_t filterSize = 0;
	float mu = 0.0f;
	bool nlms = false;
	bool perf = false;
	int64_t sampleRate = 48000;
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (etk::start_with(data,"--fb=")) {
			fbName = &data[5];
		} else if (etk::start_with(data,"--mic=")) {
			micName = &data[6];
		} else if (etk::start_with(data,"--filter-size=")) {
			data = &data[14];
			filterSize = etk::string_to_int32_t(data);
		} else if (etk::start_with(data,"--mu=")) {
			data = &data[5];
			mu = etk::string_to_float(data);
		} else if (data == "--nlms") {
			nlms = true;
		} else if (data == "--perf") {
			etk::thread::setPriority(-2);
			perf = true;
		} else if (etk::start_with(data,"--sample-rate=")) {
			data = &data[14];
			sampleRate = etk::string_to_int32_t(data);
		} else if (    data == "-h"
		            || data == "--help") {
			TEST_PRINT("Help : ");
			TEST_PRINT("    ./xxx --fb=file.raw --mic=file.raw");
			TEST_PRINT("        --fb=YYY.raw        Feedback file");
			TEST_PRINT("        --mic=XXX.raw       Microphone file");
			TEST_PRINT("        --filter-size=xxx   Size of the filter");
			TEST_PRINT("        --mu=0.xx           Mu value -1.0< mu < -1.0");
			TEST_PRINT("        --nlms              NLMS version");
			TEST_PRINT("        --perf              Enable performence test (little slower but real performence test)");
			TEST_PRINT("        --sample-rate=XXXX  Signal sample rate (default 48000)");
			exit(0);
		}
	}
	if (    fbName == ""
	     || micName == "") {
		TEST_ERROR("Can not Process missing parameters...");
		exit(-1);
	}
	
	Performance perfo;
	
	TEST_INFO("Read FeedBack:");
	std::vector<int16_t> fbData = etk::FSNodeReadAllDataType<int16_t>(fbName);
	TEST_INFO("    " << fbData.size() << " samples");
	TEST_INFO("Read Microphone:");
	std::vector<int16_t> micData = etk::FSNodeReadAllDataType<int16_t>(micName);
	TEST_INFO("    " << micData.size() << " samples");
	// resize output :
	std::vector<int16_t> output;
	output.resize(std::min(fbData.size(), micData.size()), 0);
	// process in chunk of 256 samples
	int32_t blockSize = 256;
	if (nlms == false) {
		TEST_PRINT("***********************");
		TEST_PRINT("**         LMS       **");
		TEST_PRINT("***********************");
		audio::algo::river::Lms algo;
		algo.init(1, sampleRate, audio::format_float);
		if (filterSize != 0) {
			algo.setFilterSize(filterSize);
		}
		if (mu != 0.0f) {
			algo.setMu(mu);
		}
		int32_t lastPourcent = -1;
		for (int32_t iii=0; iii<output.size()/blockSize; ++iii) {
			if (lastPourcent != 100*iii / (output.size()/blockSize)) {
				lastPourcent = 100*iii / (output.size()/blockSize);
				TEST_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
			} else {
				TEST_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
			}
			perfo.tic();
			algo.process(&output[iii*blockSize], &fbData[iii*blockSize], &micData[iii*blockSize], blockSize);
			if (perf == true) {
				perfo.toc();
				usleep(1000);
			}
		}
	} else {
		TEST_PRINT("***********************");
		TEST_PRINT("**    NLMS (power)   **");
		TEST_PRINT("***********************");
		audio::algo::river::Nlms algo;
		algo.init(1, sampleRate, audio::format_float);
		if (filterSize != 0) {
			algo.setFilterSize(filterSize);
		}
		int32_t lastPourcent = -1;
		for (int32_t iii=0; iii<output.size()/blockSize; ++iii) {
			if (lastPourcent != 100*iii / (output.size()/blockSize)) {
				lastPourcent = 100*iii / (output.size()/blockSize);
				TEST_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
			} else {
				TEST_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
			}
			perfo.tic();
			algo.process(&output[iii*blockSize], &fbData[iii*blockSize], &micData[iii*blockSize], blockSize);
			if (perf == true) {
				perfo.toc();
				usleep(1000);
			}
		}
	}
	TEST_PRINT("Process done");
	if (perf == true) {
		TEST_PRINT("Performance Result: ");
	TEST_INFO("    blockSize=" << blockSize << " sample");
	TEST_INFO("    min < avg < max =" << perfo.getMinProcessing().count() << "ns < "
	                                  << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << "ns < "
	                                  << perfo.getMaxProcessing().count() << "ns ");
	float avg = (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/double(blockSize))/1000000000.0)*100.0;
	TEST_INFO("    min < avg < max= " << (float((perfo.getMinProcessing().count()*sampleRate)/double(blockSize))/1000000000.0)*100.0 << "% < "
	                                  << avg << "% < "
	                                  << (float((perfo.getMaxProcessing().count()*sampleRate)/double(blockSize))/1000000000.0)*100.0 << "%");
	TEST_PRINT("float : " << sampleRate << " : " << avg << "%");
	}
	etk::FSNodeWriteAllDataType<int16_t>("output.raw", output);
}

