# metin2.dev@Ken

class Grid:
	def __init__(self, width, height):
		self.grid = [False] * (width * height)
		self.width = width
		self.height = height

	def __str__(self):
		output = "Grid {}x{} Information\n".format(self.width, self.height)
		for row in range(self.height):
			for col in range(self.width):
				output += "Status of %d: " % (row * self.width + col)
				output += "NotEmpty, " if self.grid[row *
												   self.width + col] else "Empty, "
			output += "\n"

		return output

	def find_blank(self, width, height):
		if width > self.width or height > self.height:
			return -1

		for row in range(self.height):
			for col in range(self.width):
				index = row * self.width + col
				if self.is_empty(index, width, height):
					return index

		return -1

	def put(self, pos, width, height):
		if not self.is_empty(pos, width, height):
			return False

		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = True
				col += 1

		return True

	def clear(self, pos, width, height):
		if pos < 0 or pos >= (self.width * self.height):
			return

		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = True
			col = 1
			while col < width:
				self.grid[start + col] = False
				col += 1

	def is_empty(self, pos, width, height):
		if pos < 0:
			return False

		row = pos // self.width
		if (row + height) > self.height:
			return False

		if (pos + width) > ((row * self.width) + self.width):
			return False

		for row in range(height):
			start = pos + (row * self.width)			
			if self.grid[start]:				
				return False

			col = 1
			while col < width:
				if self.grid[start + col]:					
					return False
				col += 1

		return True

	def set_empty(self, pos, width, height):
		if pos < 0:
			return False

		row = pos // self.width
		if (row + height) > self.height:
			return False

		if (pos + width) > ((row * self.width) + self.width):
			return False

		for row in range(height):
			start = pos + (row * self.width)
			self.grid[start] = False
			col = 1
			while col < width:
				self.grid[start + col] = False
				col += 1

	def get_size(self):
		return self.width * self.height

	def reset(self):
		self.grid = [False] * (self.width * self.height)