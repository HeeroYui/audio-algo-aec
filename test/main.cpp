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


#undef __class__
#define __class__ "test"


static std::vector<int16_t> read(const std::string& _path) {
	std::vector<int16_t> out;
	etk::FSNode node(_path);
	if (node.fileOpenRead() == false) {
		APPL_ERROR("can not open file : '" << node << "'");
		return out;
	}
	uint64_t nbByte = node.fileSize();
	out.resize(nbByte/2);
	node.fileRead(&out[0], 2, nbByte/2);
	node.fileClose();
	return out;
}

static void write(const std::string& _path, const std::vector<int16_t>& _data) {
	etk::FSNode node(_path);
	if (node.fileOpenWrite() == false) {
		APPL_ERROR("can not open file : '" << node << "'");
		return;
	}
	node.fileWrite(&_data[0], 2, _data.size());
	node.fileClose();
}

static void write(const std::string& _path, const std::vector<float>& _data) {
	etk::FSNode node(_path);
	if (node.fileOpenWrite() == false) {
		APPL_ERROR("can not open file : '" << node << "'");
		return;
	}
	node.fileWrite(&_data[0], 4, _data.size());
	node.fileClose();
}


int main(int _argc, const char** _argv) {
	// the only one init for etk:
	etk::init(_argc, _argv);
	std::string fbName = "";
	std::string micName = "";
	int32_t filterSize = 0;
	float mu = 0.0f;
	bool nlms = false;
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
		} else if (    data == "-h"
		            || data == "--help") {
			APPL_INFO("Help : ");
			APPL_INFO("    ./xxx --fb=file.raw --mic=file.raw");
			APPL_INFO("        --fb=YYY.raw        Feedback file");
			APPL_INFO("        --mic=XXX.raw       Microphone file");
			APPL_INFO("        --filter-size=xxx   Size of the filter");
			APPL_INFO("        --mu=0.xx           Mu value -1.0< mu < -1.0");
			APPL_INFO("        --nlms              NLMS version");
			exit(0);
		}
	}
	if (    fbName == ""
	     || micName == "") {
		APPL_ERROR("Can not Process missing parameters...");
		exit(-1);
	}
	APPL_INFO("Read FeedBack:");
	std::vector<int16_t> fbData = read(fbName);
	APPL_INFO("    " << fbData.size() << " samples");
	APPL_INFO("Read Microphone:");
	std::vector<int16_t> micData = read(micName);
	APPL_INFO("    " << micData.size() << " samples");
	// resize output :
	std::vector<int16_t> output;
	output.resize(std::min(fbData.size(), micData.size()), 0);
	// process in chunk of 256 samples
	int32_t blockSize = 256;
	// end filter :
	std::vector<float> filter;
	if (nlms == false) {
		APPL_INFO("***********************");
		APPL_INFO("**         LMS       **");
		APPL_INFO("***********************");
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
			algo.process(&output[iii*blockSize], &fbData[iii*blockSize], &micData[iii*blockSize], blockSize);
		}
		filter = algo.getFilter();
	} else {
		APPL_INFO("***********************");
		APPL_INFO("**    NLMS (power)   **");
		APPL_INFO("***********************");
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
	write("output.raw", output);
	write("filter.raw", filter);
	
}

