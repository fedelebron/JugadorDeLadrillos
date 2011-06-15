# coding: utf-8

import gtk
from gtk import gdk

BRD_MAX=500

PLAYER_NAME = ["Rojo", "Verde"]
PLAYER_COLOR = [['#ff0000', '#00c000'], ['#ff8080', '#80e080'], ['#ffd0d0', '#d0f0d0']]

class Ladrillos:
	def __init__(self, n, drw):
		self.n = n
		self.m = 0
		self.brd = [[-1 for j in xrange(n)] for i in xrange(n)]
		self.bxsz = min(max(15, 2*(BRD_MAX/(2*n+1))+1),75)
		self.imgsz = (self.bxsz+1)*n+1+2*10
		self.colormap = drw.get_colormap()
		self.colors = map(lambda v: map(lambda x: self.colormap.alloc_color(x), v), PLAYER_COLOR)
		self.c_white = self.colormap.alloc_color('#fff')
		self.c_black = self.colormap.alloc_color('#000')

	def valid(self, mv):
		i,j,d = mv
		return d in (0,1) and 0 <= i and i < self.n-d and 0<=j and j<self.n-1+d

	def libre(self, mv):
		i,j,d = mv
		return self.brd[i][j] == -1 and self.brd[i+d][j+1-d] == -1

	def setmove(self, mv):
		n = self.n
		if not self.valid(mv) or not self.libre(mv):
			raise "Invalid move"
		i,j,d = mv
		self.brd[i][j] = self.m,d
		self.brd[i+d][j+1-d] = self.m,(d+2)
		self.m += 1

	def redraw(self, pm):
		gc = gdk.GC(pm, self.c_white, self.c_black)
		pm.draw_rectangle(gc, True, 0, 0, self.imgsz, self.imgsz)
		gc.foreground = self.c_black
		for i in xrange(self.n+1):
			p = 10+(self.bxsz+1)*i
			pm.draw_line(gc, p, 10, p, self.imgsz-10-1)
			pm.draw_line(gc, 10, p, self.imgsz-10-1, p)

	def drawpiece(self, pm, mv, mode):
		gc = gdk.GC(pm, self.c_white, self.c_black, None, 0, 0)
		sz = self.bxsz
		msz = sz/6
		i,j,d = mv
		x = 10+j*(sz+1)+1
		y = 10+i*(sz+1)+1
		ox = x+(1+sz)*(1-d)
		oy = y+(1+sz)*d
		if mode == 0:
			gc.set_foreground(self.c_white)
			pm.draw_rectangle(gc, True, x, y, ox-x+sz, oy-y+sz)
			gc.set_foreground(self.c_black)
			pm.draw_line(gc, ox-1, oy-1, ox-1+d*sz, oy-1+(1-d)*sz)
		else:
			gc.set_foreground(self.colors[mode][self.m%2])
			pm.draw_rectangle(gc, True, x, y, sz, sz)
			pm.draw_rectangle(gc, True, ox, oy, sz, sz)
			gc.set_foreground(self.colors[mode-1][self.m%2])
			pm.draw_rectangle(gc, True, x+msz, y+msz, sz-2*msz+(1-d)*sz, sz-2*msz+d*sz)
		return x,y,(1+sz)*(1-d)+sz,(1+sz)*d+sz

	def getmove(self, px, py):
		px = int(px) - 10
		py = int(py) - 10
		pi = (-px+py)/(self.bxsz+1)
		pj = (px+py)/(self.bxsz+1)
		i = (pi+pj)/2
		j = (-pi+pj-1)/2
		d = (pi+pj) % 2
		mv = (i,j,d)
		if not self.valid(mv) or not self.libre(mv):
			return None
		#print (pi,pj), (i,j,d)
		return mv
