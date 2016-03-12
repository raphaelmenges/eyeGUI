class BlobPage:
	def __init__(self, splitter, pageIndex):
		self.splitter = splitter
		self.pageIndex = pageIndex
		self.startIndex = 0
		self.endIndex = 0
	
	def start(self):
		self.line = ""
		self.read = 0
		self.written = 0
		self.firstLine = True
		self.atEnd = False
		
		self.blobPage = self.splitter.data[self.startIndex * 4:self.endIndex * 4]
		self.blobSliced = self.blobPage
		self.read = self.endIndex - self.startIndex
		self.written = 0
	
	def nextLine(self):
		if self.firstLine:
			character_count = min(self.read, 24)
		else:
			character_count = min(self.read, 25)
		
		character_line = self.blobSliced[:(character_count * 4)]
		
		self.line = "\""
		if self.firstLine:
			self.line += "\\x00"
			self.firstLine = False
		self.line += self.blobSliced[:(character_count * 4)]
		self.line += "\""
		
		self.written += character_count
		self.read -= character_count
		
		if self.read <= 0 and self.pageIndex > 0:
			self.line += ","
		
		if self.read > 0:
			self.blobSliced = self.blobSliced[(character_count * 4):]
		else:
			self.atEnd = True

class BlobSplitter:
	def __init__(self):
		self.pages = []
		self.data = ""
		self.codepointCount = 0
		self.pageSize = 32767
	
	def split(self, data, codepointCount):
		self.data = data
		self.codepointCoint = codepointCount
		
		page_current = BlobPage(self, len(self.pages))
		self.pages.append(page_current)
		
		blob_page = data
		blob_size = codepointCount
		
		total_offset = 0
		
		while 1:
			if blob_size < self.pageSize:
				page_current.endIndex = total_offset + blob_size
				break
			
			page_read = 0
			blob_search = blob_page
			while 1:
				end_index = blob_search.find("\\x00")
				if end_index == -1:
					break
				offset = (end_index / 4) + 1
				if (page_read + offset) >= self.pageSize:
					break
				page_read += offset
				blob_search = blob_search[(end_index + 4):]
			
			total_offset += page_read
			
			page_current.endIndex = total_offset
			
			page_current = BlobPage(self, len(self.pages))
			page_current.startIndex = total_offset
			self.pages.append(page_current)
			
			blob_page = blob_page[(page_read * 4):]
			blob_size -= page_read