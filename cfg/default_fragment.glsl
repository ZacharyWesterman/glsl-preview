uniform float u_time;

void main()
{
	float brightness = (sin(u_time) + 2.0) / 5.0;
	gl_FragColor = vec4(brightness);
}
