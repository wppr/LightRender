

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

uniform int frustumSegmentCount;

void main(void) {
    for (int segment = 0; segment < frustumSegmentCount; ++segment) {
        for (int i = 0; i < gl_in.length(); ++i) {
            gl_Position = gl_in[i].gl_Position;
            gl_ViewportIndex = segment;
            gl_Layer = segment;
            EmitVertex();
        }

        EndPrimitive();
    }
}
