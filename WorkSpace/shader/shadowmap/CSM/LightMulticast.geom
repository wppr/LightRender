

#extension GL_NV_viewport_array2 : require

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
layout(viewport_relative) out int gl_Layer;

uniform int frustumSegmentCount;

uniform vec4 viewports[4]; // max 4 frustum segments, viewport bounds in pixels, x, y, w, h
uniform vec2 shadowMapSize;

// converts clip coordinates into window coordinates for a given viewport
vec2 getWindowPos(vec4 clip_Pos, uint viewport) {
    vec2 ndc_Pos = (clip_Pos.xy / clip_Pos.w); // -1 to 1
    vec2 blend_factor = (ndc_Pos + 1.0) * 0.5; // 0 to 1
    vec2 view_Pos = (viewports[viewport].zw * blend_factor) + viewports[viewport].xy; // pixel position in the shadow map of the vertex
    return view_Pos;
}

// checks if two 2d bounding boxes intersect
bool checkIntersection(vec4 bbox0, vec4 bbox1) {
    bool xmiss = bbox0.x > bbox1.z || bbox0.z < bbox1.x;
    bool ymiss = bbox0.y > bbox1.w || bbox0.w < bbox1.y;
    return !xmiss && !ymiss;
}

void main(void) {
    const vec4 mapBounds = vec4(0, 0, shadowMapSize);
    int viewportMask = 0;
    for (int segment = 0; segment < frustumSegmentCount; ++segment) {
        vec2 start_Pos = getWindowPos(gl_in[0].gl_Position, segment);
        vec4 primBounds = vec4(start_Pos, start_Pos); // minx, miny, maxx, maxy
        for (int i = 1; i < gl_in.length(); ++i) {
            vec2 window_Pos = getWindowPos(gl_in[i].gl_Position, segment);
            primBounds.x = min(primBounds.x, window_Pos.x);
            primBounds.y = min(primBounds.y, window_Pos.y);
            primBounds.z = max(primBounds.x, window_Pos.x);
            primBounds.w = max(primBounds.y, window_Pos.y);
        }
        // we should only emit the primitive if its bounding box intersects the current viewport
        if (checkIntersection(primBounds, mapBounds)) {
            viewportMask |= (1 << segment);
        }
    }

    // send the whole primitive with the viewport mask we calculated
    for (int i = 0; i < gl_in.length(); ++i) {
        gl_Position = gl_in[i].gl_Position;
        gl_ViewportMask[0] = viewportMask;
        gl_Layer = 0;
        EmitVertex();
    }
    EndPrimitive();
}
