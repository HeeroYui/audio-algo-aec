#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "audio_algo_aec : AEC basic algo"


def create(target):
	myModule = module.Module(__file__, 'audio_algo_aec', 'LIBRARY')
	myModule.add_src_file([
		'audio/algo/aec/debug.cpp',
		'audio/algo/aec/convolution.cpp',
		'audio/algo/aec/power.cpp',
		'audio/algo/aec/updateFilter.cpp',
		'audio/algo/aec/Lms.cpp',
		'audio/algo/aec/Nlms.cpp'
		])
	myModule.add_module_depend(['etk'])
	myModule.add_export_path(tools.get_current_path(__file__))
	# return module
	return myModule









