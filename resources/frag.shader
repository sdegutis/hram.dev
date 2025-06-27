#version 300 es

// fragment shaders don't have a default precision so we need
// to pick one. highp is a good default. It means "high precision"
precision highp float;

uniform vec2 u_resolution;

// our texture
uniform sampler2D u_image;

// the texCoords passed in from the vertex shader.
in vec2 v_texCoord;

// we need to declare an output for the fragment shader
out vec4 outColor;

float warp = 0.35; // simulate curvature of CRT monitor
float scan = 0.05; // simulate darkness between scanlines

void main() {

    outColor = texture(u_image,v_texCoord);
    /*

    vec2 uv = v_texCoord;
    vec2 dc = abs(0.5-uv);
    dc *= dc;
    
    // warp the fragment coordinates
    uv.x -= 0.5; uv.x *= 1.0+(dc.y*(0.3*warp)); uv.x += 0.5;
    uv.y -= 0.5; uv.y *= 1.0+(dc.x*(0.4*warp)); uv.y += 0.5;

    if (uv.y > 1.0 || uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0)
        //outColor = vec4(0.3,0.0,0.0,1.0);
        discard;
    else {
        float apply = abs(sin(v_texCoord.y*180.0)*0.5*scan);
    	outColor = vec4(mix(texture(u_image,uv).rgb,vec3(0.0),apply),1.0);
    }
    //*/


}
