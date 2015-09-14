#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "river algos"


def create(target):
	myModule = module.Module(__file__, 'audio-algo-river', 'LIBRARY')
	myModule.add_src_file([
		'audio/algo/river/debug.cpp',
		'audio/algo/river/convolution.cpp',
		'audio/algo/river/power.cpp',
		'audio/algo/river/updateFilter.cpp',
		'audio/algo/river/Lms.cpp',
		'audio/algo/river/Nlms.cpp',
		'audio/algo/river/Suppressor.cpp'
		])
	myModule.add_header_file([
		'audio/algo/river/convolution.h',
		'audio/algo/river/power.h',
		'audio/algo/river/updateFilter.h',
		'audio/algo/river/Lms.h',
		'audio/algo/river/Nlms.h',
		'audio/algo/river/Suppressor.h'
		])
	myModule.add_module_depend(['etk', 'audio'])
	myModule.add_path(tools.get_current_path(__file__))
	# return module
	return myModule









