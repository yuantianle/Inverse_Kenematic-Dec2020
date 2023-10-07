#version 450 compatibility

uniform float   fangle;		// specular exponent

in  vec2  vST;			// texture coords
in  vec3  vN;			// normal vector
in  vec3  vE;			// vector from point to eye
in  float alpha;
in  vec3  Color;


void
main( )
{

	vec3 Normal		= normalize(vN);
	vec3 Eye        = normalize(vE);


	//Calculate the wave position

	float PI = 3.1415926535897932384626433832795;

	float d = vST.t;

	float wavePos = sin(fangle);
	float inner = wavePos - .05;
	float outer = wavePos + .05;

	float delta = d - inner;

	vec3 fColor = Color;
	//vec3 fColor = vec3(0.0, vST.s, vST.t);

	float va;

	va = alpha / dot(Normal,Eye);

	gl_FragColor = vec4( fColor,  va );
}
