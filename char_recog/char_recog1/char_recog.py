import matplotlib.pylot as plt
from scipy import ndimage
from scipy import misc

class CharRecog(object):
	def __init__(self):
		self.letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'
		self.img_dir = '/Users/ziqipeng/Dropbox/bci/x/ar/char_recog/data/Templates/'
		self.thresthold = 0.8


	def binarize(self, dir):
		self.templates = [0] * len(self.letters)
		print self.templates
		plt.figure()
		for i in range(0, len(self.letters))
			img = misc.imread(self.img_dir + i + ".png"

cr = CharRecog()
cr.binarize()