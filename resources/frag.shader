#version 300 es

// fragment shaders don't have a default precision so we need
// to pick one. highp is a good default. It means "high precision"
precision highp float;

uniform vec3 iResolution;
uniform vec3 iChannelResolution;

// our texture
uniform sampler2D iChannel0;

// the texCoords passed in from the vertex shader.
in vec2 fragCoord;

// we need to declare an output for the fragment shader
out vec4 fragColor;

//float warp = 0.35; // simulate curvature of CRT monitor
float scan = 0.55; // simulate darkness between scanlines


uniform float iTime;



//Phosphor decay
float decay(in float d) {
    return mix(exp2(-d*d*2.5-.3),0.05/(d*d*d*0.45+0.055),.65)*0.99;
}

//Phosphor shape
float sqd(in vec2 a, in vec2 b) {
    a -= b;
    a *= vec2(1.25,1.8)*.905;
    float d = max(abs(a.x), abs(a.y));
    d = mix(d, length(a*vec2(1.05, 1.))*0.85, .3);
    return d;
}

vec3 phosphors(in vec2 p, sampler2D tex) {   
    p /= 4.0;
    vec2 p2 = p;
    vec3 col = vec3(0);
    p -= 0.25;
    p.y += mod(p2.x,2.)<1.?.03:-0.03;
    p.y += mod(p2.x,4.)<2.?.02:-0.02;
    
	//5x5 kernel (this means a given fragment can be affected by a pixel 4 game pixels away)
    for(int i=-2;i<=2;i++)
    for(int j=-2;j<=2;j++)
    {
        vec2 tap = floor(p) + 0.5 + vec2(i,j);
		vec3 rez = texture(tex, tap/iResolution.xy).rgb; //nearest neighbor
        
		//center points
        float rd = sqd(tap, p + vec2(0.0,0.2));//distance to red dot
		const float xoff = .25;
        float gd = sqd(tap, p + vec2(xoff,.0));//distance to green dot
        float bd = sqd(tap, p + vec2(-xoff,.0));//distance to blue dot
		
        rez = pow(rez,vec3(1.18))*1.08;
        rez.r *= decay(rd);
        rez.g *= decay(gd);
        rez.b *= decay(bd);
		
        col += rez;
    }
    return col;
}






void main() {

    if (int(iTime) % 2000 < 1000) {
        fragColor = texture(iChannel0,fragCoord/iResolution.xy);
    }
    else {
        vec3 col = phosphors(fragCoord.xy, iChannel0);
	    fragColor = vec4(col, 1.0);
    }




    /*

    vec2 uv = fragCoord;
    vec2 dc = abs(0.5-uv);
    dc *= dc;

    // warp the fragment coordinates
    uv.x -= 0.5; uv.x *= 1.0+(dc.y*(0.3*warp)); uv.x += 0.5;
    uv.y -= 0.5; uv.y *= 1.0+(dc.x*(0.4*warp)); uv.y += 0.5;

    if (uv.y > 1.0 || uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0)
        //fragColor = vec4(0.3,0.0,0.0,1.0);
        discard;
    else {
        float apply = abs(sin(fragCoord.y*180.0)*0.5*scan);
       fragColor = vec4(mix(texture(iChannel0,uv).rgb,vec3(0.0),apply),1.0);
    }
    //*/


}
