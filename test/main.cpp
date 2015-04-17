/** @file
 * @author Edouard DUPIN 
 * @copyright 2015, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <test/debug.h>
#include <etk/etk.h>
#include <audio/algo/aec/Lms.h>
#include <audio/algo/aec/Nlms.h>
#include <etk/os/FSNode.h>
#include <etk/chrono.h>
#include <etk/thread/tools.h>

#include <unistd.h>
#undef __class__
#define __class__ "test"


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
			APPL_PRINT("Help : ");
			APPL_PRINT("    ./xxx --fb=file.raw --mic=file.raw");
			APPL_PRINT("        --fb=YYY.raw        Feedback file");
			APPL_PRINT("        --mic=XXX.raw       Microphone file");
			APPL_PRINT("        --filter-size=xxx   Size of the filter");
			APPL_PRINT("        --mu=0.xx           Mu value -1.0< mu < -1.0");
			APPL_PRINT("        --nlms              NLMS version");
			APPL_PRINT("        --perf              Enable performence test (little slower but real performence test)");
			APPL_PRINT("        --sample-rate=XXXX  Signal sample rate (default 48000)");
			exit(0);
		}
	}
	if (    fbName == ""
	     || micName == "") {
		APPL_ERROR("Can not Process missing parameters...");
		exit(-1);
	}
	APPL_INFO("Read FeedBack:");
	std::vector<int16_t> fbData = etk::FSNodeReadAllDataType<int16_t>(fbName);
	APPL_INFO("    " << fbData.size() << " samples");
	APPL_INFO("Read Microphone:");
	std::vector<int16_t> micData = etk::FSNodeReadAllDataType<int16_t>(micName);
	APPL_INFO("    " << micData.size() << " samples");
	// resize output :
	std::vector<int16_t> output;
	output.resize(std::min(fbData.size(), micData.size()), 0);
	// process in chunk of 256 samples
	int32_t blockSize = 256;
	// end filter :
	std::vector<float> filter;
	std11::chrono::nanoseconds totalTimeProcessing(0);
	std11::chrono::nanoseconds minProcessing(99999999999999LL);
	std11::chrono::nanoseconds maxProcessing(0);
	int32_t totalIteration = 0;
	if (nlms == false) {
		APPL_PRINT("***********************");
		APPL_PRINT("**         LMS       **");
		APPL_PRINT("***********************");
		audio::algo::aec::Lms algo;
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
				APPL_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
			} else {
				APPL_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
			}
			std11::chrono::steady_clock::time_point timeStart = std11::chrono::steady_clock::now();
			algo.process(&output[iii*blockSize], &fbData[iii*blockSize], &micData[iii*blockSize], blockSize);
			if (perf == true) {
				std11::chrono::steady_clock::time_point timeEnd = std11::chrono::steady_clock::now();
				std11::chrono::nanoseconds time = timeEnd - timeStart;
				minProcessing = std::min(minProcessing, time);
				maxProcessing = std::max(maxProcessing, time);
				totalTimeProcessing += time;
				totalIteration++;
				usleep(10000);
			}
		}
		filter = algo.getFilter();
	} else {
		APPL_PRINT("***********************");
		APPL_PRINT("**    NLMS (power)   **");
		APPL_PRINT("***********************");
		audio::algo::aec::Nlms algo;
		if (filterSize != 0) {
			algo.setFilterSize(filterSize);
		}
		int32_t lastPourcent = -1;
		for (int32_t iii=0; iii<output.size()/blockSize; ++iii) {
			if (lastPourcent != 100*iii / (output.size()/blockSize)) {
				lastPourcent = 100*iii / (output.size()/blockSize);
				APPL_INFO("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
			} else {
				APPL_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(output.size()/blockSize)*blockSize);
			}
			algo.process(&output[iii*blockSize], &fbData[iii*blockSize], &micData[iii*blockSize], blockSize);
		}
		filter = algo.getFilter();
	}
	APPL_PRINT("Process done");
	if (perf == true) {
		APPL_PRINT("Performance Result: ");
		APPL_PRINT("    blockSize=" << blockSize << " sample");
		APPL_PRINT("    total data time=" << int64_t(totalIteration)*int64_t(blockSize)*1000000000LL/int64_t(sampleRate) << " ns");
		APPL_PRINT("    min=" << minProcessing.count() << " ns");
		APPL_PRINT("    max=" << maxProcessing.count() << " ns");
		APPL_PRINT("    avg=" << totalTimeProcessing.count()/totalIteration << " ns");
		
		APPL_PRINT("    " << sampleRate << " Hz");
		APPL_PRINT("        min=" << (float((minProcessing.count()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_PRINT("        max=" << (float((maxProcessing.count()*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_PRINT("        avg=" << (float(((totalTimeProcessing.count()/totalIteration)*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_PRINT("    48000 Hz");
		sampleRate = 48000;
		APPL_PRINT("        avg=" << (float(((totalTimeProcessing.count()/totalIteration)*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_PRINT("    32000 Hz");
		sampleRate = 32000;
		APPL_PRINT("        avg=" << (float(((totalTimeProcessing.count()/totalIteration)*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_PRINT("    16000 Hz");
		sampleRate = 16000;
		APPL_PRINT("        avg=" << (float(((totalTimeProcessing.count()/totalIteration)*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
		APPL_PRINT("    8000 Hz");
		sampleRate = 8000;
		APPL_PRINT("        avg=" << (float(((totalTimeProcessing.count()/totalIteration)*sampleRate)/blockSize)/1000000000.0)*100.0 << " %");
	}
	etk::FSNodeWriteAllDataType<int16_t>("output.raw", output);
	etk::FSNodeWriteAllDataType<float>("filter.raw", filter);
	
}

