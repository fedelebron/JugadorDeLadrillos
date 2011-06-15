#!/usr/bin/env python
# coding: utf-8


import pygtk
pygtk.require('2.0')
import gtk, sys, gobject
from ladrillos import *

class Player:
	pixmap = None
	
	def __init__(self):
		n,kl = map(int, sys.stdin.readline().strip().split())
		self.n = n
		self.kl = kl
		self.mv = None

		window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		window.set_name("Ladrillos")
		window.set_title("Ladrillos - Jugador " + PLAYER_NAME[self.kl])
		window.set_resizable(False)
		#window.set_alallow-grow = False

		window.connect("destroy", lambda w: gtk.main_quit())

		# Create the drawing area
		drw = gtk.DrawingArea()
		window.add(drw)
		self.lad = Ladrillos(n, drw)
		drw.set_size_request(self.lad.imgsz, self.lad.imgsz)
		drw.show()

		# Signals used to handle backing pixmap
		drw.connect("expose_event", self.expose_event)
		drw.connect("configure_event", self.configure_event)

		# Event signals
		drw.connect("motion_notify_event", self.motion_notify_event)
		drw.connect("button_press_event", self.button_press_event)

		drw.set_events(gtk.gdk.EXPOSURE_MASK
		             | gtk.gdk.LEAVE_NOTIFY_MASK
		             | gtk.gdk.BUTTON_PRESS_MASK
		             | gtk.gdk.POINTER_MOTION_MASK
		             | gtk.gdk.POINTER_MOTION_HINT_MASK)
		self.drw = drw

		window.show()

		if not self.myturn():
			self.handle_data(sys.stdin, None)

		gobject.io_add_watch(sys.stdin, gobject.IO_IN, self.handle_data)

	def handle_data(self, source, condition):
		mv = self.readmove()
		sys.stderr.write('READ: %d %d %d\n' % mv)
		# Update board
		rect = self.lad.drawpiece(self.pixmap, mv, 1)
		self.drw.queue_draw_area(*rect)
		self.lad.setmove(mv)
		return True # stop looping

	def readmove(self):
		i,j,d = map(int, sys.stdin.readline().strip().split())
		return (i-1,j-1,d)

	def myturn(self):
		return self.kl == self.lad.m%2

	### Events
	def configure_event(self, widget, event):
		x, y, width, height = widget.get_allocation()
		self.pixmap = gtk.gdk.Pixmap(widget.window, width, height)
		self.lad.redraw(self.pixmap)
		return True

	def expose_event(self, widget, event):
		x, y, width, height = event.area
		widget.window.draw_drawable(widget.get_style().fg_gc[gtk.STATE_NORMAL], self.pixmap, x, y, x, y, width, height)
		return False

	# Draw a rectangle on the screen
	def draw_brush(self, widget, x, y):
		rect = (int(x-5), int(y-5), 10, 10)
		self.pixmap.draw_rectangle(widget.get_style().black_gc, True,
								rect[0], rect[1], rect[2], rect[3])
		widget.queue_draw_area(rect[0], rect[1], rect[2], rect[3])

	def button_press_event(self, widget, event):
		if event.button == 1 and self.myturn(): # left click
			mv = self.lad.getmove(event.x, event.y)
			if not mv:
				return True
			self.mv = None
			rect = self.lad.drawpiece(self.pixmap, mv, 1)
			widget.queue_draw_area(*rect)
			self.lad.setmove(mv)
			# Send move to judge
			sys.stdout.write("%d %d %d\n" % (mv[0]+1, mv[1]+1, mv[2]))
			sys.stdout.flush()
		return True

	def motion_notify_event(self, widget, event):
		if not self.myturn(): return True
		if event.is_hint:
			x, y, state = event.window.get_pointer()
		else:
			x = event.x
			y = event.y
			state = event.state

		curmv = self.lad.getmove(x,y)
		if curmv and not self.lad.libre(curmv): curmv = None
		if self.mv != curmv:
			#print curmv
			if self.mv:
				rect = self.lad.drawpiece(self.pixmap, self.mv, 0)
				widget.queue_draw_area(*rect)
			if curmv:
				rect = self.lad.drawpiece(self.pixmap, curmv, 2)
				widget.queue_draw_area(*rect)
			self.mv = curmv
		#if state & gtk.gdk.BUTTON1_MASK and self.pixmap != None:
		
		return True

	def main(self):
		gtk.main()

		return 0

if __name__ == "__main__":
	pl = Player()
	pl.main()


class Judge:
	def hello(self, widget, data=None):
		print "Hello World"

	def delete_event(self, widget, event, data=None):
		print "delete event occurred"
		# Change FALSE to TRUE and the main window will not be destroyed
		# with a "delete_event".
		return False

	def destroy(self, widget, data=None):
		print "destroy signal occurred"
		gtk.main_quit()

	def __init__(self):
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.connect("delete_event", self.delete_event)
		self.window.connect("destroy", self.destroy)
		self.window.set_title("Ladrillos")
	
		# Sets the border width of the window.
		self.window.set_border_width(5)
	
		# Primer parte: selector de archivos, tiempo y tamaño
		box = gtk.HBox(False, 0)
		self.window.add(box)

	
		# Creates a new button with the label "Hello World".
		btn = gtk.Button("Hello World")
		btn.connect("clicked", self.hello, None)
		#btn.connect_object("clicked", gtk.Widget.destroy, self.window)
		btn.show()
		box.pack_start(btn, True, True, 0)

		# Creates a new button with the label "Hello World".
		btn = gtk.Button("Hello World2")
		btn.connect("clicked", self.hello, None)
		#btn.connect_object("clicked", gtk.Widget.destroy, self.window)
		btn.show()
		box.pack_start(btn, True, True, 0)

		darea = gtk.DrawingArea()
		darea.set_size_request(200, 200)
		darea.show()
		box.pack_start(darea, True, True, 0)
		
		#self.window.add(darea)

		# and the window
		box.show()
		self.window.show()

	def main(self):
		# All PyGTK applications must have a gtk.main(). Control ends here
		# and waits for an event to occur (like a key press or mouse event).
		pass #gtk.main()

#if __name__ == "__main__":
#	hello = Judge()
#	hello.main()


