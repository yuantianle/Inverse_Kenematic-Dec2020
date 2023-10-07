#version 450 compatibility

uniform float   angle;		// specular exponent
uniform vec3	startPos;

out  vec3  vN;		// normal vector
out	 vec2  vST;		// texture coords
out  vec3  vE;		// vector from point to eye
out  float alpha;
out  vec3  Color;

void
main( )
{ 
	vec3 vert = gl_Vertex.xyz;

	vN = normalize( gl_NormalMatrix * gl_Normal );	// normal vector

	vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;	// vector from the point to eye

	//Calculate the wave position

	float PI = 3.1415926535897932384626433832795;

	vec3 Origin = normalize(startPos);
	vec3 Current = normalize(vert);
	float d = acos(dot(Origin, Current)) * 5.0;

	float wavePos = sin(angle) * 3.15 * 5.0;
	float inner = wavePos - .6;
	float outer = wavePos + .6;

	float delta = d - inner;

	Color = vec3(0.0, 10.0, 10.0);
	if(d >= inner && d <= outer)
	{
		vert.x -= 0.015 * vert.x * sin(delta * PI * 2) / (d);
		vert.y -= 0.015 * vert.y * sin(delta * PI * 2) / (d);
		vert.z -= 0.015 * vert.z * sin(delta * PI * 2) / (d);
		alpha = 0.1 * delta;
		//Color = vec3(abs(sin(delta * PI * 2)), 0.0, 1 - abs(sin(delta * PI * 2)));
	}
	else
		alpha = 0.03;

	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );

	vST = gl_MultiTexCoord0.st;
}
