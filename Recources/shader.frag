#version 330 core
//in vec3 color;

out vec4 FragColor;
layout(location = 0) out vec3 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  
uniform bool enableSpecular;
uniform Material material;
uniform bool lighting;
void main()
{
// vec2 correctedTexCoord = TexCoords + vec2(FragPos.x * Depth, FragPos.y * Depth);
   
   if(lighting){
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  
    float diff = max(dot(norm, lightDir), 0.0);
    

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
     vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    if(enableSpecular){
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
     vec3 result = ambient + diffuse + specular;
     FragColor = vec4(result, 1.0);
    }
    else {
     vec3 specular = vec3(0.0, 0.0, 0.0);
     vec3 result = ambient + diffuse + specular;
     FragColor = vec4(result, 1.0);
    }
    }
    else{
    FragColor = texture(material.diffuse, TexCoords);
    }
   // vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
  //  vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
   // vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

  //  vec3 result = ambient + diffuse + specular;
    //FragColor = texture(material.diffuse, TexCoords);
   // FragColor = vec4(result, 1.0);
};