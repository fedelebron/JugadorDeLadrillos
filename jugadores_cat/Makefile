all: fixed rand heur heurb labi wabi
	
fixed: jugs_varios.cpp
	g++ jugs_varios.cpp -DJUGADOR=JUGADOR_FIJO -O2 -o fixed

rand: jugs_varios.cpp
	g++ jugs_varios.cpp -DJUGADOR=JUGADOR_RANDOM -O2 -o rand

heur: jugs_varios.cpp
	g++ jugs_varios.cpp -DJUGADOR=JUGADOR_HEURISTICO -O2 -o heur

heurb: jugs_varios.cpp
	g++ jugs_varios.cpp -DJUGADOR=JUGADOR_HEURISTICO_MEJOR -O2 -o heurb

labi: jug_labi.cpp
	g++ jug_labi.cpp -O2 -o labi

wabi: jug_wabi.cpp
	g++ jug_wabi.cpp -O2 -o wabi

clean:
	rm -f fixed rand heur heurb labi wabi
