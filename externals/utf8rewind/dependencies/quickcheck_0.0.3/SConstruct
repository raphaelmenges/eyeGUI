import os
import SCons.Util

env = Environment()

# Use C++ flags if user provided them
if os.environ.has_key('CXXFLAGS'):
   env['CXXFLAGS'] += SCons.Util.CLVar(os.environ['CXXFLAGS'])

# Set more quiet compile/link messages if VERBOSE is not set
if ARGUMENTS.get('VERBOSE') != "1":
   env['CXXCOMSTR'] = "Compiling $TARGET"
   env['LINKCOMSTR'] = "Linking $TARGET"

# Targets
env.Program('tests.cc')
env.Program('examples.cc')

# vim:ft=python
