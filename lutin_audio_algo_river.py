#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "audio_algo_river : AEC basic algo"


def create(target):
	myModule = module.Module(__file__, 'audio_algo_river', 'LIBRARY')
	myModule.add_src_file([
		'audio/algo/river/debug.cpp',
		'audio/algo/river/convolution.cpp',
		'audio/algo/river/power.cpp',
		'audio/algo/river/updateFilter.cpp',
		'audio/algo/river/Lms.cpp',
		'audio/algo/river/Nlms.cpp',
		'audio/algo/river/Suppressor.cpp'
		])
	myModule.add_module_depend(['etk', 'audio'])
	myModule.add_export_path(tools.get_current_path(__file__))
	# return module
	return myModule









