import re

class UnicodeEntry:
	def __init__(self):
		self.lineNumber = 0
		self.matches = []
		self.comment = ""
	
	def accept(self, visitor):
		return visitor.visitEntry(self)

class UnicodeSection:
	def __init__(self):
		self.identifier = ""
		self.title = ""
		self.entries = []
	
	def accept(self, visitor):
		return visitor.visitSection(self)

class UnicodeDocument:
	def __init__(self):
		self.lineLimit = None
		self.entryLimit = None
		self.entrySkip = None
		self.filename = ""
		self.sections = []
	
	def parse(self, filename):
		print("Parsing \"" + filename + "\"...")
		
		self.filename = filename
		
		section_search = re.compile('@(\w+) ?# ?(.+)')
		comment_search = re.compile('.*# (.+)')
		
		section_current = UnicodeSection()
		self.sections = []
		self.sections.append(section_current)
		
		sections_found = 0
		
		line_count = 0
		
		with open(filename, 'r', encoding='utf-8') as f:
			lines = f.readlines()
			lines_total = float(len(lines))
			for line in lines:
				line_count += 1
					
				if len(line) > 0 and not line.startswith('\n') and not line.startswith('#'):
					stripped = line.rstrip('\n')
					
					# section
					
					if line.startswith('@'):
						section_match = re.match(section_search, stripped)
						if section_match:
							if sections_found > 0:
								section_current = UnicodeSection()
								self.sections.append(section_current)
							
							section_current.identifier = section_match.group(1)
							section_current.title = section_match.group(2)
							sections_found += 1
							
							continue
					
					# entry
					
					comment_start = stripped.find('#')
					if comment_start != -1:
						entry_sliced = stripped[:comment_start]
					else:
						entry_sliced = stripped
					
					entry = UnicodeEntry()
					entry.lineNumber = line_count
					while (1):
						section_end = entry_sliced.find(';')
						if section_end == -1:
							if len(entry_sliced) > 1:
								if entry_sliced.find('#') <= len(entry_sliced):
									matches = re.findall('([^ ]+)', entry_sliced)
									entry.matches.append(matches)
							break
						
						sliced = entry_sliced[:section_end]
						matches = re.findall('([^ ]+)', sliced)
						entry.matches.append(matches)
						
						entry_sliced = entry_sliced[section_end + 1:]
					
					# comment
					
					comment_match = re.match(comment_search, stripped)
					if comment_match:
						entry.comment = comment_match.group(1)
					
					section_current.entries.append(entry)
					
					if self.lineLimit != None:
						self.lineLimit -= 1
						if self.lineLimit == 0:
							break
		
		return True
	
	def accept(self, visitor):
		if not visitor.visitDocument(self):
			return False
		for s in self.sections:
			if not s.accept(visitor):
				return False
			for e in s.entries:
				if self.entrySkip and self.entrySkip > 0:
					self.entrySkip -= 1
					continue
				if not e.accept(visitor):
					return False
				if self.entryLimit:
					self.entryLimit -= 1
					if self.entryLimit == 0:
						return True
		return True

class UnicodeVisitor:
	def visitDocument(self, document):
		return True
	
	def visitSection(self, section):
		return True
	
	def visitEntry(self, entry):
		return True