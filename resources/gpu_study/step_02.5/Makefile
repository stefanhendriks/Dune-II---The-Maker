# La cible principale qui sera exécutée par défaut (quand tu tapes juste 'make')
all: test triangle_ps.spirv triangle_vs.spirv

# Compilation du programme C++
test: test.cpp
	g++ test.cpp -lSDL3 -o test

# Compilation du shader de fragment
triangle_ps.spirv: Fragment.hlsl
	shadercross Fragment.hlsl -e PS_Main -t fragment -d SPIRV -o triangle_ps.spirv

# Compilation du shader de vertex
triangle_vs.spirv: Vertex.hlsl
	shadercross Vertex.hlsl -e VS_Main -t vertex -d SPIRV -o triangle_vs.spirv

# Nettoyage des fichiers générés
clean:
	rm -f test triangle_ps.spirv triangle_vs.spirv
