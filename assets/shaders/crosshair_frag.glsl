#version 430 core


in vec3 normal;
in vec3 fragment_position;

//in vec2 v_tex_coords;

out vec4 color;

uniform vec2 resolution;

  //======//
 // Data //
//======//

// Represents a rectangle.
struct Rect {
    // Represents the x/y coordinates of the rect's origin point.
    vec2 point;
    
    // Represents the width/height of the rect.
    vec2 size;
    
    // Origin point of the rect.
    vec2 pivot;
};
    
  //===========//
 // Constants //
//===========//

const float Scale = 4.0;

const vec3 BackgroundColorRGB = vec3(1, 0.7, 0.4);
const vec4 CrosshairColor = vec4(1, 1, 1, 1);
const vec4 BorderColor = vec4(0, 0, 0, 1);

const float Separation = 2.0;
const float MaxSeparation = 30.0;

const vec2 Size = vec2(10, 2);
const float BorderSize = 2.0;

const vec4 LeftLine = vec4(Size, vec2(1, 0.5));
const vec4 RightLine = vec4(Size, vec2(0, 0.5));
const vec4 TopLine = vec4(Size.yx, vec2(0.5, 0));
const vec4 BottomLine = vec4(Size.yx, vec2(0.5, 1));
    
  //===========//
 // Functions //
//===========//

// Apply the pivot and retrieve the coordinate.
vec2 apply_pivot(Rect r) {
    return r.point - r.size * r.pivot;
}

// Determine if a point p is inside rect r.
float inside(vec2 p, Rect r, float margin) {
    vec2 rp = apply_pivot(r);
    
    return
        step(rp.x - margin, p.x) *
        step(p.x, rp.x + r.size.x + margin) *
        step(rp.y - margin, p.y) *
        step(p.y, rp.y + r.size.y + margin);
}

void draw_line(inout float crosshair, inout float border, vec2 coord, float sep, vec2 center, vec4 line) {
    // Offset.
    vec2 o = vec2(sep);
    o -= o * 2.0 * line.zw;

    Rect r = Rect(center + o, line.xy, line.zw);

    crosshair += inside(coord, r, 0.0);
    border += inside(coord, r, BorderSize);
}

  //========//
 // Shader //
//========//

void main() {
    vec2 center = resolution.xy * 0.5;

    float crosshair = 0.0;
    float border = 0.0;
    float sep = 8.5;
    
    draw_line(crosshair, border, vec2(fragment_position.xy), sep, center, LeftLine);
    draw_line(crosshair, border, vec2(fragment_position.xy), sep, center, RightLine);
    draw_line(crosshair, border, vec2(fragment_position.xy), sep, center, TopLine);
    draw_line(crosshair, border, vec2(fragment_position.xy), sep, center, BottomLine);
    
    crosshair = min(crosshair, 1.0);
    border = min(border, 1.0);
    
    color.rgb = mix(color.rgb, BorderColor.rgb, border * BorderColor.a);
    color.rgb = mix(color.rgb, CrosshairColor.rgb, crosshair * CrosshairColor.a);
}