import datetime
import os.path
import re
import sys
import libs.header

def codepointToHexadecimalWideUtf16(codepoint, wroteHex = False):
	result = ''
	
	if codepoint <= 0x7F:
		conversion = {
			0x00: "\\0",
			0x07: "\\a",
			0x08: "\\b",
			0x09: "\\t",
			0x0A: "\\n",
			0x0B: "\\v",
			0x0C: "\\f",
			0x0D: "\\r",
			
			# must be escaped
			0x22: "\\\"",
			0x5C: "\\\\",
		}
		if codepoint in conversion:
			result += conversion[codepoint]
			
			return result, False
		elif codepoint < 0x20:
			result += '\\x' + format(codepoint, 'X')
			
			return result, True
		else:
			isHex = (codepoint >= 0x41 and codepoint <= 0x46) or (codepoint >= 0x61 and codepoint <= 0x76) or (codepoint >= 0x30 and codepoint <= 0x39)
			
			if wroteHex and isHex:
				result += "\" L\""
			result += "%c" % codepoint
			
			return result, False
	elif codepoint <= 0xFFFF:
		result += '\\x' + format(codepoint, 'X')
		
		return result, True
	elif codepoint <= 0x10FFFF:
		decoded = codepoint - 0x10000
		surrogate_high = 0xD800 + (decoded >> 10)
		surrogate_low = 0xDC00 + (decoded & 0x03FF)
		
		result += '\\x' + format(surrogate_high, '4X')
		result += '\\x' + format(surrogate_low, '4X')
		
		return result, True
	else:
		result += '\\xFFFD'

		return result, True

def codepointToHexadecimalWideUtf32(codepoint, wroteHex = False):
	result = ''
	
	if codepoint <= 0x7F:
		conversion = {
			0x00: "\\0",
			0x07: "\\a",
			0x08: "\\b",
			0x09: "\\t",
			0x0A: "\\n",
			0x0B: "\\v",
			0x0C: "\\f",
			0x0D: "\\r",
			
			# must be escaped
			0x22: "\\\"",
			0x5C: "\\\\",
		}
		if codepoint in conversion:
			result += conversion[codepoint]
			
			return result, False
		elif codepoint < 0x20:
			result += '\\x' + format(codepoint, 'X')
			
			return result, True
		else:
			isHex = (codepoint >= 0x41 and codepoint <= 0x46) or (codepoint >= 0x61 and codepoint <= 0x76) or (codepoint >= 0x30 and codepoint <= 0x39)
			
			if wroteHex and isHex:
				result += "\" L\""
			result += "%c" % codepoint
			
			return result, False
	elif codepoint <= 0x10FFFF:
		result += '\\x' + format(codepoint, 'X')
		
		return result, True
	else:
		result += '\\xFFFD'

		return result, True

class Test:
	def __init__(self, line, bytes, offset):
		self.line_utf16 = line.encode('utf-16le')
		self.line_utf32 = line.encode('utf-32le')
		self.bytes = bytes
		self.offset = offset
		self.converted_utf16 = ''
		self.converted_utf32 = ''

	def Convert(self):
		hex_utf16 = False
		self.converted_utf16 = ''
		hex_utf32 = False
		self.converted_utf32 = ''

		for c in (self.line_utf16[pos:pos + 2] for pos in range(0, len(self.line_utf16), 2)):
			codepoint = (c[1] << 8) | c[0]

			result_utf16, hex_utf16 = codepointToHexadecimalWideUtf16(codepoint, hex_utf16)
			self.converted_utf16 += result_utf16

		for c in (self.line_utf32[pos:pos + 4] for pos in range(0, len(self.line_utf32), 4)):
			codepoint = (c[3] << 8) | (c[2] << 16) | (c[1] << 8) | c[0]

			result_utf32, hex_utf32 = codepointToHexadecimalWideUtf32(codepoint, hex_utf32)
			self.converted_utf32 += result_utf32

		return self.converted_utf16 == self.converted_utf32

	def Render(self, header, type='utf16'):
		header.writeLine('EXPECT_STREQ(L"' + self.__dict__['converted_' + type] + '", helpers::wide(ReadSection(' + str(self.offset) + ', ' + str(len(self.bytes)) + ')).c_str());')

class Section:
	def __init__(self, name):
		self.name = re.sub('[^A-Za-z0-9_]', '', name.title())
		self.tests = []
		self.differs = False

	def Process(self):
		self.differs = False
		for t in self.tests:
			if not t.Convert():
				self.differs = True
	
	def Render(self, header):
		print('Writing tests for "' + self.name + '"...')
		if self.differs:
			print('\tUTF-16 differs from UTF-32, writing both.')
		
		header.newLine()
		header.newLine()
		header.writeLine('TEST_F(NaughtyStrings, ' + self.name + ')')
		header.writeLine('{')
		
		if self.differs:
			header.writeLine('#if UTF8_WCHAR_UTF16')
			header.indent()

			for t in self.tests:
				t.Render(header, 'utf16')

			header.outdent()
			header.writeLine('#elif UTF8_WCHAR_UTF32')
			header.indent()

			for t in self.tests:
				t.Render(header, 'utf32')

			header.outdent()
			header.writeLine('#endif')
		else:
			header.indent()

			for t in self.tests:
				t.Render(header)

			header.outdent()
		
		header.write("}")

class Processor:
	def __init__(self):
		self.current = None
		self.sections = []
		self.state_map = {
			'section': self.ProcessSection,
			'comments': self.ProcessComments,
			'test': self.ProcessTest,
			'exit': self.ProcessExit
		}
		self.state = 'section'
	
	def Parse(self, filepath):
		print('Parsing "' + os.path.realpath(filepath) + '"...')

		with open(filepath, 'rb') as f:
			self.state = 'section'
			
			bytes_read = bytearray()
			offset = 0
			offset_start = offset

			while True:
				current = f.read(1)
				offset += 1

				if not current:
					break
				
				if current == b'\n':
					line = str(bytes_read, encoding='utf-8')
					self.state = self.state_map[self.state](line, bytes_read, offset_start)
					
					bytes_read = bytearray()
					offset_start = offset
				else:
					bytes_read.append(ord(current))

		print('Processing sections...')

		for s in self.sections:
			s.Process()
	
	def Render(self, filepath):
		print('Rendering tests to "' + os.path.realpath(filepath) + '"...')

		command_line = sys.argv[0]
		arguments = sys.argv[1:]
		for a in arguments:
			command_line += ' ' + a
		
		header = libs.header.Header(filepath)
		header.generatedNotice()
		header.newLine()
		header.writeLine('#include "tests-base.hpp"')
		header.newLine()
		header.writeLine('#include "../helpers/helpers-strings.hpp"')
		header.newLine()
		header.writeLine('#define NAUGHTY_STRINGS_LENGTH 10370')
		header.newLine()
		header.writeLine('class NaughtyStrings')
		header.writeLine('	: public ::testing::Test')
		header.writeLine('{')
		header.newLine()
		header.writeLine('protected:')
		header.newLine()
		header.writeLine('	void SetUp()')
		header.writeLine('	{')
		header.writeLine('		file.open("testdata/big-list-of-naughty-strings-master/blns.txt", std::ios_base::in);')
		header.writeLine('		ASSERT_TRUE(file.is_open());')
		header.writeLine('	}')
		header.newLine()
		header.writeLine('	void TearDown()')
		header.writeLine('	{')
		header.writeLine('		file.close();')
		header.writeLine('	}')
		header.newLine()
		header.writeLine('	std::string ReadSection(size_t position, size_t length)')
		header.writeLine('	{')
		header.writeLine('		std::string result;')
		header.newLine()
		header.writeLine('		file.seekg(position, std::ios::beg);')
		header.writeLine('		if (file.eof())')
		header.writeLine('		{')
		header.writeLine('			return result;')
		header.writeLine('		}')
		header.newLine()
		header.writeLine('		result.resize(length + 1);')
		header.writeLine('		file.read(&result[0], length);')
		header.newLine()
		header.writeLine('		return result;')
		header.writeLine('	}')
		header.newLine()
		header.writeLine('	std::fstream file;')
		header.newLine()
		header.write('};')
		
		for s in self.sections:
			s.Render(header)

	def ProcessSection(self, line, bytes, offset):
		match = re.match('#[\t ]+(.+)', line)
		if not match:
			return 'exit'
		
		self.current = Section(match.group(1))
		self.sections.append(self.current)
		
		return 'comments'

	def ProcessComments(self, line, bytes, offset):
		if len(line) > 0 and not re.match('#.*', line):
			return self.ProcessTest(line, bytes, offset)
		
		return 'comments'

	def ProcessTest(self, line, bytes, offset):
		if len(line) == 0:
			return 'section'
		
		test = Test(line, bytes, offset)
		self.current.tests.append(test)
		
		return 'test'
	
	def ProcessExit(self, line, bytes, offset):
		print('Error parsing file.')
		exit(1)

if __name__ == '__main__':
	current_directory = os.path.dirname(os.path.realpath(sys.argv[0])) + '/'

	processor = Processor()
	processor.Parse(current_directory + '../../testdata/big-list-of-naughty-strings-master/blns.txt')
	processor.Render(current_directory + '../../source/tests/integration-naughty-strings.cpp')

	print('Done.')