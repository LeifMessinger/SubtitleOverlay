#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
//uniform sampler2D texture0;	//Not needed
//uniform vec4 colDiffuse;	//Not needed

// Output fragment color
out vec4 finalColor;

vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0,4.0,2.0),6.0)-3.0)-1.0,0.0,1.0);
    rgb = rgb * rgb * (3.0 -2.0 * rgb);
    return c.z * mix(vec3(1.0),rgb,c.y);
}

void main(){
	vec2 uv = fragTexCoord;
	vec3 hsv = (uv.y < .5)? vec3(uv.x,1.0,(uv.y * 2.0)) : vec3(uv.x,2.0 - (uv.y * 2.0),1.0);
	
	vec3 color = hsb2rgb(hsv);
	const float alpha = 1.0;   
	finalColor = vec4(color,alpha);
}