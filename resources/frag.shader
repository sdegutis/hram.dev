#version 410 core

precision highp float;

uniform vec3 iResolution;
uniform vec3 iChannelResolution;
uniform sampler2D iChannel0;
uniform float iTime;

in vec2 fragCoord;
out vec4 fragColor;

void mainImage(out vec4 fragColor,in vec2 fragCoord);

void main() {
	fragColor = texture(iChannel0, fragCoord/iResolution.xy);
	//mainImage(fragColor, fragCoord);
}
