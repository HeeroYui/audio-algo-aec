#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug

def get_desc():
	return "test for LMS ALGO"


def create(target):
	myModule = module.Module(__file__, 'audio_algo-river-test', 'BINARY')
	myModule.add_src_file([
		'test/main.cpp',
		'test/debug.cpp'
		])
	myModule.add_module_depend(['audio-algo-river'])
	return myModule









