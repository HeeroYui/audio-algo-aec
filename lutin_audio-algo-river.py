#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug


def get_type():
	return "LIBRARY"

def get_desc():
	return "river algos"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def get_version():
	return "version.txt"

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
	my_module.add_src_file([
		'audio/algo/river/debug.cpp',
		'audio/algo/river/convolution.cpp',
		'audio/algo/river/power.cpp',
		'audio/algo/river/updateFilter.cpp',
		'audio/algo/river/Lms.cpp',
		'audio/algo/river/Nlms.cpp',
		'audio/algo/river/Suppressor.cpp'
		])
	my_module.add_header_file([
		'audio/algo/river/convolution.hpp',
		'audio/algo/river/power.hpp',
		'audio/algo/river/updateFilter.hpp',
		'audio/algo/river/Lms.hpp',
		'audio/algo/river/Nlms.hpp',
		'audio/algo/river/Suppressor.hpp'
		])
	my_module.add_depend(['etk', 'audio'])
	my_module.add_path(tools.get_current_path(__file__))
	return my_module









