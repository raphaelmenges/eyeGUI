import argparse
import datetime
import os
import re
import sys
import unicodedata
import libs.header
import libs.unicode
import libs.utf8

def codepointToUnicode(codepoint):
	return "U+%04X" % codepoint

class IntegrationSuite:
	def __init__(self, db):
		self.db = db
	
	def open(self, filepath):
		script_path = os.path.dirname(os.path.realpath(sys.argv[0]))
		
		self.header = libs.header.Header(script_path + filepath)
		self.header.generatedNotice()
		self.header.newLine()
		self.header.writeLine("#include \"tests-base.hpp\"")
		self.header.newLine()
	
	def close(self):
		self.header.close()
	
	def execute(self):
		pass

class CaseMappingIntegrationSuite(IntegrationSuite):
	def execute(self):
		print('Executing case mapping tests...')
		
		valid_blocks = []
		
		print('Checking for valid blocks...')
		
		for b in db.blocks:
			for u in range(b.start, b.end + 1):
				if u in db.records:
					r = db.records[u]
					if r.uppercase or r.lowercase or r.titlecase:
						valid_blocks.append(b)
						break
		
		print('Writing case mapping tests...')
		
		self.open('/../../source/tests/integration-casemapping.cpp')
		
		self.header.writeLine("#include \"../helpers/helpers-casemapping.hpp\"")
		self.header.writeLine("#include \"../helpers/helpers-locale.hpp\"")
		self.header.newLine()
		self.header.writeLine("class CaseMapping")
		self.header.writeLine("\t: public ::testing::Test")
		self.header.writeLine("{")
		self.header.newLine()
		self.header.writeLine("protected:")
		self.header.newLine()
		self.header.writeLine("\tvoid SetUp()")
		self.header.writeLine("\t{")
		self.header.writeLine("\t\tSET_LOCALE_ENGLISH();")
		self.header.writeLine("\t}")
		self.header.newLine()
		self.header.writeLine("\tvoid TearDown()")
		self.header.writeLine("\t{")
		self.header.writeLine("\t\tRESET_LOCALE();")
		self.header.writeLine("\t}")
		self.header.newLine()
		self.header.write("};")
		
		for b in valid_blocks:
			self.writeTest(range(b.start, b.end + 1), b.name)
		
		self.close()
	
	def writeTest(self, codepointRange, name):
		name = re.sub('[ \-]', '', name)
		
		if len(codepointRange) > 4000:
			for i in range(0, len(codepointRange), 4000):
				chunk = codepointRange[i:i + 4000]
				self.writeTest(chunk, name + "Part" + str((i / 4000) + 1))
			return
		
		records = []
		
		for i in codepointRange:
			if i not in self.db.records:
				continue
			
			records.append(self.db.records[i])
		
		if len(records) == 0:
			return
		
		print("Writing tests " + codepointToUnicode(codepointRange[0]) + " - " + codepointToUnicode(codepointRange[len(codepointRange) - 1]) + " \"" + name + "\"")
		
		self.header.newLine()
		
		self.header.newLine()
		self.header.writeLine("TEST_F(CaseMapping, " + name + ")")
		self.header.writeLine("{")
		self.header.indent()
		
		for r in records:
			converted_codepoint = "0x%08X" % r.codepoint
			
			if r.codepoint == 0:
				self.header.writeLine("EXPECT_CASEMAPPING_CODEPOINT_NUL_EQ(" + converted_codepoint + ", \"" + r.name + "\");")
			else:
				if r.lowercase:
					converted_lowercase = libs.utf8.unicodeToUtf8(r.lowercase)
				else:
					converted_lowercase = libs.utf8.codepointToUtf8(r.codepoint)[0]
				
				if r.uppercase:
					converted_uppercase = libs.utf8.unicodeToUtf8(r.uppercase)
				else:
					converted_uppercase = libs.utf8.codepointToUtf8(r.codepoint)[0]
				
				if r.titlecase:
					converted_titlecase = libs.utf8.unicodeToUtf8(r.titlecase)
				else:
					converted_titlecase = libs.utf8.codepointToUtf8(r.codepoint)[0]
				
				self.header.writeLine("EXPECT_CASEMAPPING_CODEPOINT_EQ(" + converted_codepoint + ", \"" + converted_lowercase + "\", \"" + converted_uppercase + "\", \"" + converted_titlecase + "\", \"" + r.name + "\");")
		
		self.header.outdent()
		self.header.write("}")

class NormalizationEntry:
	def __init__(self):
		self.codepoint = 0
		self.source = ""
		self.nfc = ""
		self.nfd = ""
		self.nfkc = ""
		self.nfkd = ""
	
	def __str__(self):
		return "{ codepoint " + hex(self.codepoint) + " source " + self.source + " nfc " + self.nfc + " nfd " + self.nfd + " nfkc " + self.nfkc + " nfkd " + self.nfkd + " }";
	
	def parse(self, entry):
		if len(entry.matches[0]) == 1:
			self.codepoint = int(entry.matches[0][0], 16)
		self.source = self.matchToString(entry.matches[0])
		self.nfc = self.matchToString(entry.matches[1])
		self.nfd = self.matchToString(entry.matches[2])
		self.nfkc = self.matchToString(entry.matches[3])
		self.nfkd = self.matchToString(entry.matches[4])
	
	def matchToString(self, match):
		codepoints = []
		
		for m in match:
			codepoints.append(int(m, 16))
		
		return libs.utf8.unicodeToUtf8(codepoints)

class NormalizationGroup:
	def __init__(self, block):
		self.block = block
		self.entries = []

class NormalizationSection:
	def __init__(self, identifier, title):
		self.identifier = identifier
		self.title = title
		self.entries = []

class NormalizationIntegrationSuite(IntegrationSuite):
	def __init__(self, db):
		self.db = db
		self.current = None
		self.sections = []
		self.blockGroups = dict()
		self.exclusions = CompositionExclusionIntegrationSuite(db)
	
	def execute(self):
		print('Executing normalization tests...')
		
		script_path = os.path.dirname(os.path.realpath(sys.argv[0]))
		
		document_normalization = libs.unicode.UnicodeDocument()
		document_normalization.parse(script_path + '/data/NormalizationTest.txt')
		document_normalization.accept(self)
		
		self.exclusions.execute()
		
		print('Writing normalization tests...')
		
		self.open('/../../source/tests/integration-normalization.cpp')
		
		self.header.writeLine("#include \"../helpers/helpers-normalization.hpp\"")
		self.header.write("#include \"../helpers/helpers-strings.hpp\"")
		
		section_mapping = {
			'Part0': self.writeSpecificCasesSection,
			'Part1': self.writeBlockGroupsSection,
			'Part2': self.writeDefaultSection,
			'Part3': self.writeDefaultSection,
		}
		for s in self.sections:
			print(s.title + " (" + s.identifier + "):")
			section_mapping[s.identifier](s)
		
		self.writeNormalizationTest(self.exclusions.entries, "Composition exclusions", 100)
		
		self.close()
	
	def writeDefaultSection(self, section):
		if len(section.entries) == 0:
			return
		
		self.writeNormalizationTest(section.entries, section.title, 100)
	
	def writeSpecificCasesSection(self, section):
		if len(section.entries) == 0:
			return
		
		normalization = []
		ordering = []
		
		for e in section.entries:
			if e.codepoint == 0:
				ordering.append(e)
			else:
				normalization.append(e)
		
		self.writeNormalizationTest(normalization, section.title + " Codepoints")
		self.writeNormalizationTest(ordering, section.title + " Ordering")
	
	def writeBlockGroupsSection(self, section):
		groups = dict()
		
		for u in section.entries:
			e = self.db.records[u.codepoint]
			if e.block.name in groups:
				group = groups[e.block.name]
			else:
				group = NormalizationGroup(e.block)
				groups[e.block.name] = group
			group.entries.append(u)
		
		block_groups = sorted(groups.items(), key = lambda item: item[1].block.start)
		
		for g in block_groups:
			if g[1].block.start == 0xAC00 and g[1].block.end == 0xD7AF:
				# ignore hangul syllables
				continue
			
			self.writeNormalizationTest(g[1].entries, "Characters " + g[0])
	
	def writeNormalizationTest(self, entries, title, limit = 2000):
		if len(entries) > limit:
			for i in range(0, len(entries), limit):
				chunk = entries[i:i + limit]
				self.writeNormalizationTest(chunk, title + " Part" + str(int(i / limit) + 1), limit)
			return
		
		title = re.sub('[^\w ]', '', title.title()).replace(' ', '')
		
		print("Writing tests \"" + title + "\"")
		
		self.header.newLine()
		
		self.header.newLine()
		self.header.writeLine("TEST(Normalization, " + title + ")")
		self.header.writeLine("{")
		self.header.indent()
		
		for e in entries:
			self.header.writeIndentation()
			
			if e.codepoint == 0:
				self.header.write("CHECK_NORMALIZE_SEQUENCE")
				self.header.write("(\"" + e.source + "\", \"" + e.nfd + "\", \"" + e.nfc + "\", \"" + e.nfkd + "\", \"" + e.nfkc + "\");")
			else:
				self.header.write("CHECK_NORMALIZE_CODEPOINT")
				self.header.write("(0x" + format(e.codepoint, '08X') + ", \"" + e.nfd + "\", \"" + e.nfc + "\", \"" + e.nfkd + "\", \"" + e.nfkc + "\", \"" + self.db.records[e.codepoint].name + "\");")
			
			self.header.newLine()
		
		self.header.outdent()
		self.header.write("}")
	
	def visitDocument(self, document):
		print('Parsing normalization tests...')
		return True
	
	def visitSection(self, section):
		self.current = NormalizationSection(section.identifier, section.title)
		self.sections.append(self.current)
		
		return True
	
	def visitEntry(self, entry):
		normalization = NormalizationEntry()
		normalization.parse(entry)
		self.current.entries.append(normalization)
		
		return True

class CompositionExclusionIntegrationSuite(IntegrationSuite):
	def __init__(self, db):
		self.db = db
		self.entries = []
	
	def execute(self):
		print('Executing composition exclusion tests...')
		
		script_path = os.path.dirname(os.path.realpath(sys.argv[0]))
		
		document_exclusions = libs.unicode.UnicodeDocument()
		document_exclusions.parse(script_path + '/data/CompositionExclusions.txt')
		document_exclusions.accept(self)
	
	def visitDocument(self, document):
		return True
	
	def visitSection(self, section):
		return True
	
	def visitEntry(self, entry):
		codepoint = int(entry.matches[0][0], 16)
		if codepoint in self.db.records:
			record = db.records[codepoint]
			
			entry = NormalizationEntry()
			entry.codepoint = record.codepoint
			entry.source = libs.utf8.codepointToUtf8Hex(record.codepoint)
			entry.nfd = libs.utf8.unicodeToUtf8(record.decomposedNFD)
			entry.nfc = entry.nfd
			entry.nfkd = libs.utf8.unicodeToUtf8(record.decomposedNFD)
			entry.nfkc = entry.nfkd
			
			self.entries.append(entry)
		return True

class QuickCheckEntry:
	def __init__(self, codepoint):
		self.codepoint = codepoint
		self.nfc = "YES"
		self.nfd = "YES"
		self.nfkc = "YES"
		self.nfkd = "YES"

class QuickCheckGroup:
	def __init__(self, block):
		self.block = block
		self.entries = []

class IsNormalizedIntegrationSuite(IntegrationSuite):
	def __init__(self, db):
		self.db = db
		self.entries = dict()
		self.groups = dict()
	
	def execute(self):
		print('Parsing quickcheck records...')
		
		records_list = [
			{
				"record": "qcNFCRecords",
				"target": "nfc"
			},
			{
				"record": "qcNFDRecords",
				"target": "nfd"
			},
			{
				"record": "qcNFKCRecords",
				"target": "nfkc"
			},
			{
				"record": "qcNFKDRecords",
				"target": "nfkd"
			},
		]
		
		value_map = {
			1: "MAYBE",
			2: "NO"
		}
		
		for i in records_list:
			for r in db.__dict__[i["record"]]:
				str_value = value_map[r.value]
				
				for c in range(r.start, r.start + r.count + 1):
					if c in self.entries:
						e = self.entries[c]
					else:
						e = QuickCheckEntry(c)
						self.entries[c] = e
					e.__dict__[i["target"]] = str_value
		
		for e in self.entries.values():
			block = self.db.getBlockByCodepoint(e.codepoint)
			if block in self.groups:
				group = self.groups[block]
			else:
				group = QuickCheckGroup(block)
				self.groups[block] = group
			
			group.entries.append(e)
		
		print('Writing is-normalized tests...')
		
		self.open('/../../source/tests/integration-isnormalized.cpp')
		
		self.header.write("#include \"../helpers/helpers-normalization.hpp\"")
		
		for key, value in sorted(self.groups.items(), key = lambda block: block[0].start):
			if key.start == 0xAC00 and key.end == 0xD7AF:
				# ignore hangul syllables
				continue
			
			self.writeBlockSection(sorted(value.entries, key = lambda entry: entry.codepoint), key.name)
	
	def writeBlockSection(self, entries, title, limit = 2000):
		if len(entries) > limit:
			for i in range(0, len(entries), limit):
				chunk = entries[i:i + limit]
				self.writeBlockSection(chunk, title + " Part" + str((i / limit) + 1), limit)
			return
		
		title = re.sub('[^\w ]', '', title.title()).replace(' ', '')
		
		print("Writing tests \"" + title + "\"")
		
		self.header.newLine()
		
		self.header.newLine()
		self.header.writeLine("TEST(IsNormalized, " + title + ")")
		self.header.writeLine("{")
		self.header.indent()
		
		for e in entries:
			self.header.writeLine("CHECK_IS_NORMALIZED(0x" + format(e.codepoint, '08X') + ", " + e.nfd + ", " + e.nfc + ", " + e.nfkd + ", " + e.nfkc + ", \"" + self.db.records[e.codepoint].name + "\");")
		
		self.header.outdent()
		self.header.write("}")

class CaseFoldingRecord():
	def __init__(self):
		self.codePoint = 0
		self.type = ''
		self.folded = []
	
	def __str__(self):
		return '{ codePoint ' + hex(self.codePoint) + ' type ' + self.type + ' folded ' + str(self.folded) + ' }'
	
	def parse(self, entry):
		self.codePoint = int(entry.matches[0][0], 16)
		
		types = {
			'C': 'Common',
			'F': 'Full',
			'S': 'Simple',
			'T': 'Turkish'
		}
		self.type = types[entry.matches[1][0]]
		
		for m in entry.matches[2]:
			self.folded.append(int(m, 16))

class CaseFoldingIntegrationSuite(IntegrationSuite):
	def __init__(self, db):
		self.db = db
		self.records = []
	
	def execute(self):
		print('Parsing case folding records...')
		
		script_path = os.path.dirname(os.path.realpath(sys.argv[0]))
		
		document_exclusions = libs.unicode.UnicodeDocument()
		document_exclusions.parse(script_path + '/data/CaseFolding.txt')
		document_exclusions.accept(self)
		
		self.open('/../../source/tests/integration-casefolding.cpp')
		
		self.header.writeLine("#include \"../helpers/helpers-casemapping.hpp\"")
		self.header.writeLine("#include \"../helpers/helpers-locale.hpp\"")
		self.header.newLine()
		self.header.writeLine("class CaseFolding")
		self.header.writeLine("\t: public ::testing::Test")
		self.header.writeLine("{")
		self.header.newLine()
		self.header.writeLine("protected:")
		self.header.newLine()
		self.header.writeLine("\tvoid SetUp()")
		self.header.writeLine("\t{")
		self.header.writeLine("\t\tSET_LOCALE_ENGLISH();")
		self.header.writeLine("\t}")
		self.header.newLine()
		self.header.writeLine("\tvoid TearDown()")
		self.header.writeLine("\t{")
		self.header.writeLine("\t\tRESET_LOCALE();")
		self.header.writeLine("\t}")
		self.header.newLine()
		self.header.write("};")
		
		tests_turkish = []
		
		for b in db.blocks:
			tests = []
			
			for r in self.records:
				if r.codePoint >= b.start and r.codePoint <= b.end:
					if r.type in ['Common', 'Full']:
						tests.append(r)
					elif r.type == 'Turkish':
						tests_turkish.append(r)
			
			if len(tests) > 0:
				self.writeTest(tests, b.name)
		
		if len(tests_turkish) > 0:
			print("Writing tests to \"TurkishLocale\"")
			
			self.header.newLine()
			
			self.header.newLine()
			self.header.writeLine("TEST_F(CaseFolding, TurkishLocale)")
			self.header.writeLine("{")
			self.header.indent()
			self.header.writeLine("SET_LOCALE_TURKISH();")
			self.header.newLine()
			
			for r in tests_turkish:
				self.header.writeLine("EXPECT_CASEFOLDING_EQ(0x" + format(r.codePoint, '08X') + ", \"" + libs.utf8.unicodeToUtf8(r.folded) + "\", \"" + self.db.records[r.codePoint].name + "\");")
			
			self.header.outdent()
			self.header.write("}")
	
	def writeTest(self, records, name):
		name = re.sub('[ \-]', '', name)
		
		if len(records) > 4000:
			for i in range(0, len(records), 4000):
				chunk = records[i:i + 4000]
				self.writeTest(chunk, name + "Part" + str((i / 4000) + 1))
			return
		
		print("Writing tests to \"" + name + "\"")
		
		self.header.newLine()
		
		self.header.newLine()
		self.header.writeLine("TEST_F(CaseFolding, " + name + ")")
		self.header.writeLine("{")
		self.header.indent()
		
		for r in records:
			self.header.writeLine("EXPECT_CASEFOLDING_EQ(0x" + format(r.codePoint, '08X') + ", \"" + libs.utf8.unicodeToUtf8(r.folded) + "\", \"" + self.db.records[r.codePoint].name + "\");")
		
		self.header.outdent()
		self.header.write("}")
	
	def visitDocument(self, document):
		return True
	
	def visitSection(self, section):
		return True
	
	def visitEntry(self, entry):
		record = CaseFoldingRecord()
		record.parse(entry)
		
		self.records.append(record)
		
		return True

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Parse Unicode codepoint database and write integration tests.')
	parser.add_argument(
		'-v', '--verbose',
		dest = 'verbose',
		action = 'store_true',
		help = 'verbose output')
	parser.add_argument(
		'--casemapping',
		dest = 'casemapping',
		action = 'store_true',
		help = 'write case mapping tests')
	parser.add_argument(
		'--normalization',
		dest = 'normalization',
		action = 'store_true',
		help = 'write normalization tests')
	parser.add_argument(
		'--is-normalized',
		dest = 'isnormalized',
		action = 'store_true',
		help = 'write is-normalized tests')
	parser.add_argument(
		'--casefolding',
		dest = 'casefolding',
		action = 'store_true',
		help = 'write casefolding tests')
	args = parser.parse_args()
	
	if not args.casemapping and not args.normalization and not args.isnormalized and not args.casefolding:
		all = True
	else:
		all = False
	
	db = unicodedata.Database()
	db.loadFromFiles(None)
	
	if all or args.casemapping:
		suite = CaseMappingIntegrationSuite(db)
		suite.execute()
	
	if all or args.normalization:
		suite = NormalizationIntegrationSuite(db)
		suite.execute()
	
	if all or args.isnormalized:
		suite = IsNormalizedIntegrationSuite(db)
		suite.execute()
	
	if all or args.casefolding:
		suite = CaseFoldingIntegrationSuite(db)
		suite.execute()