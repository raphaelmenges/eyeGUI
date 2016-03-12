import argparse
import os.path
import re
import subprocess

executable_path = None

def Run(filters, outputPath, repeatCount):
	print('Running executable at \"' + executable_path + '\".')

	process = subprocess.Popen(
		'"' + executable_path + '" --gtest_filter=' + filters + ' --gtest_repeat_count=' + str(repeatCount) + ' --gtest_display_individual --gtest_output_results=' + outputPath,
		shell=True
	)

	output, errors = process.communicate()
	errcode = process.returncode

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Runs combinations of performance tests.')
	parser.add_argument(
		'--config',
		dest = 'config',
		default = '',
		help = 'Override configuration to <Platform>_<Configuration>, i.e. x64_Debug.'
	)
	parser.add_argument(
		'--out',
		dest = 'out',
		help = 'Change destination for output CSV file.'
	)
	parser.add_argument(
		'--repeat_count',
		dest = 'repeat_count',
		default = 10,
		help = 'Number of times each test suite should be run.'
	)
	parser.add_argument(
		'--casefolding',
		dest = 'casefolding',
		action = 'store_true',
		help = 'Compare casefolding performance against lowercasing.'
	)
	parser.add_argument(
		'--regression-140',
		dest = 'regression_140',
		action = 'store_true',
		help = 'Regression testing for 1.4.0.'
	)
	args = parser.parse_args()

	path = None

	if os.path.exists('output/windows'):
		path = 'output/windows'
	elif os.path.exists('output/linux'):
		path = 'output/linux'

	if not path:
		print('Failed to find executable path.')
		exit(-1)

	if len(args.config) > 0:
		matches = re.match('(\w+)_(\w+)', args.config)
		path = path + '/' + matches.group(1) + '/' + matches.group(2)
	else:
		if os.path.exists(path + '/x64'):
			path += '/x64'
		elif os.path.exists(path + '/Win32'):
			path += '/Win32'
		else:
			print('Failed to find configuration path at ' + path + '.')
			exit(-1)

		if os.path.exists(path + '/Release'):
			path += '/Release'
		elif os.path.exists(path + '/Debug'):
			path += '/Debug'
		else:
			print('Failed to find release configuration path at ' + path + '.')
			exit(-1)

	executable_path = path + '/performance-rewind.exe'

	if not os.path.exists(executable_path):
		print('Failed to find path at ' + path + '.')
		exit(-1)

	print('Found executable at \"' + executable_path + '\".')

	if args.regression_140:
		Run('-Category*.*:*Casefold*', 'regression_140_to_130.csv', args.repeat_count)
	elif args.casefolding:
		Run('*Casefold:CaseMapping*.Casefold*', 'casefolding.csv', args.repeat_count)
		Run('*Lowercase:CaseMapping*.Lowercase*', 'lowercasing.csv', args.repeat_count)
	else:
		if args.out:
			Run('*.*', args.out, args.repeat_count)
		else:
			Run('*.*', 'all.csv', args.repeat_count)