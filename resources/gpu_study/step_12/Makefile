all: test triangle_ps.spirv triangle_vs.spirv

test: test.cpp ../common/GPURenderer.cpp ../common/GPURenderer.hpp
	g++ test.cpp ../common/GPURenderer.cpp -lSDL3 -lSDL3_image -o test

triangle_ps.spirv: Fragment.hlsl
	shadercross Fragment.hlsl -e PS_Main -t fragment -d SPIRV -o triangle_ps.spirv

triangle_vs.spirv: Vertex.hlsl
	shadercross Vertex.hlsl -e VS_Main -t vertex -d SPIRV -o triangle_vs.spirv

clean:
	rm -f test triangle_ps.spirv triangle_vs.spirv
