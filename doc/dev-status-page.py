#!/usr/bin/env python
from __future__ import with_statement
import urllib, HTMLParser, os

class PageParser(HTMLParser.HTMLParser):
	def __init__(self, output_file):
		HTMLParser.HTMLParser.__init__(self, )
		self.out_file = open(output_file, "w")
		self.ignore = 0
	
	def __del__(self):
		self.out_file.close()

	def handle_starttag(self, tag, attrs):
		d = dict(attrs)
		if tag == 'head':
			self.ignore += 1
		elif tag == 'div':
			if 'id' in d and (d['id'] == 'banner' or d['id'] == 'mainnav' or d['id'] == 'ctxtnav' \
					or d['id'] == 'footer' or d['id'] == 'altlinks'):
				self.ignore += 1
			elif self.ignore:
				self.ignore += 1
		elif tag == 'form':
			self.ignore += 1
		elif tag == 'script':
			self.ignore += 1
		if not self.ignore:
			self.out_file.write(self.get_starttag_text())

	def handle_endtag(self, tag):
		if not self.ignore:
			#self.out_file.write(self.get_starttag_text() + '\n')
			self.out_file.write(r'</' + tag + r'>')

		if tag == 'head':
			self.ignore -= 1
		elif tag == 'div' and self.ignore:
			self.ignore -= 1
		elif tag == 'form':
			self.ignore -= 1
		elif tag == 'script':
			self.ignore -= 1

	def handle_startendtag(self, tag, attrs):
		if not self.ignore:
			self.out_file.write(self.get_starttag_text())
	
	def handle_data(self, data):
		if not self.ignore:
			self.out_file.write(data)

local_file = "dev_status.html"
stripped_file = "stripped.html"
urllib.urlretrieve("http://dev.bertos.org/wiki/DevelopmentStatus", local_file)
development_status_parser = PageParser(stripped_file)
with open(local_file, "r") as f:
	for line in f:
		if line.find('body>') != -1 or line.find('</html') != -1 or line.find('<html') != -1:
			continue
		development_status_parser.feed(line)
	development_status_parser.close()

# TODO: append tabs header
# TODO: make internal links really internal
# TODO: rename stripped file?
os.unlink(local_file)
