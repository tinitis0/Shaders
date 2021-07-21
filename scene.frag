#version 330

in VS_OUT
{
	in vec3 FragPos; // fragment position
	in vec2 TextCoord; //texture coordinates
	in vec3 FragNormal; // fragment normal
	
	vec3 TangentLightPos; // tengent light position
    vec3 TangentViewPos; // tangent view position
    vec3 TangentFragPos; // tangent fragment position
}fs_in;

// Light attributes
struct LightAttr
{
	vec3 position; // light position
	vec3 ambient; // ambient light
	vec3 diffuse; // diffuse light
	vec3 specular; // spacular light
};
uniform LightAttr light; // Light attributes defiigned as Light
uniform bool normalMapping; // Normal Map uniform set as bool

uniform sampler2D texture_diffuse0;  // 2D texture diffuse uniform
uniform sampler2D texture_specular0; //2D texture specular uniform
uniform sampler2D texture_normal0;   // 2D texture normal uniform

out vec4 color; // Colour

void main()
{   	
	//calculate ambient  light. takes in ambient  light and texture with coardinates
	vec3	ambient = light.ambient * vec3(texture(texture_diffuse0, fs_in.TextCoord));	

	// light direction is normalized position, by taking away tangent frag position from tangent light position
	vec3    lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);

	// normal is normalized fragment normal
	vec3	normal = normalize(fs_in.FragNormal);

	//If statement for normal mapping. gets normals for normal mapping
	if(normalMapping)  
	{
		normal = texture(texture_normal0, fs_in.TextCoord).rgb; // sets normal to texture normal, texture coordinates and RGB colour
		normal = normalize(normal * 2.0 - 1.0);  // sets normal to normalized normal
	}

	// diffuse factor calculation 
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	// calculates diffuse light. takes in diffuse factor, diffuse light and texture with coardinates
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(texture_diffuse0, fs_in.TextCoord));

	// Blinn - Phong Works out half vector between light vector and camera lense(view vector)

	//Normalise view direction
	vec3    viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	// sets specular factor 
	float	specFactor = 0.0;

	// creates half direction from normalised light direction + view direction
	vec3	halfDir = normalize(lightDir + viewDir);
	
	//Specular factor calculation
	specFactor = pow(max(dot(halfDir, normal), 0.0), 64.0); 
	// calculates specular light. takes in specular factor, specular light and texture with coardinates
	vec3	specular = specFactor * light.specular * vec3(texture(texture_specular0, fs_in.TextCoord));;

	// reult is  ambient light + diffuse light + specular light
	vec3	result = (ambient + diffuse + specular);

	// colour is result plus alpha
	color	= vec4(result , 1.0f); 
}