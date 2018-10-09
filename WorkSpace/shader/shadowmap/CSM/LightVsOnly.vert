

#extension GL_NV_viewport_array2 : require

layout(location = 0) in vec4 position;

uniform mat4 mvpMatrix;
uniform int viewportMask;

layout (viewport_relative) out int gl_Layer;

void main(void) {
    gl_Position = mvpMatrix * position;
    gl_ViewportMask[0] = viewportMask;
    gl_Layer = 0;
}
