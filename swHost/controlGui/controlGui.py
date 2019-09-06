import os
import socket
import chalkbot

#import ui
cb = chalkbot.chalkBot()

def hoch(sender):
	print("Hoch!")
	cb.move(100)
	
def runter(sender):
	print("Runter")
	cb.move(100, "b")

def links(sender):
	print("Links drehen")
	cb.turn(10,"ccw")
	
def rechts(sender):
	print("Rechts drehen")
	cb.turn(10)

def stop(sender):
	print("Stop")
	cb.stop()
	
def motor(sender):
	print("Motor ist jetzt ")
	if sender.value:
		print("an!")
		cb.stepper("en")
	else:
		print("aus!")
		cb.stepper("dis")
	


#v = ui.load_view()
#v.present('sheet')
