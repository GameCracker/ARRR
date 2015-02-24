#from pymouse import PyMouse 
import time
import pyautogui

'''
def mouse_control():
	m = PyMouse()
	m.position()
	m.move(200, 200)
	#m.click(2560, 0, 1)
	for i in xrange(1000):
		print i
'''

def mouse_move():
	pyautogui.moveTo(1000, 700, duration=5)
	i = 2
	while 1:
		pyautogui.moveRel(0, -2, duration=1)
		print pyautogui.position()
	#for i in xrange(20):
	#	pyautogui.moveRel(10, 10, duration=1)
# m.press(x, y)
# m.release(x, y)

mouse_move()
