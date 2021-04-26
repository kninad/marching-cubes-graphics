#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 object_color;
uniform vec3 view_position;

uniform bool on_Dlight;
uniform bool on_Plight;

uniform vec3 dlight_dir;
uniform vec3 dlight_amb;
uniform vec3 dlight_dif;
uniform vec3 dlight_spc;

uniform vec3 plight_pos;
uniform vec3 plight_amb;
uniform vec3 plight_dif;
uniform vec3 plight_spc;

vec3 CalcLight(vec3 pos_dir, vec3 ambient, vec3 diffuse, vec3 specular, vec3 normal,
                vec3 viewDir, bool is_dir);

void main()
{
    vec3 normHat = normalize(Normal);
    vec3 viewDir = normalize(view_position - FragPos);
    vec3 final = vec3(0.0, 0.0, 0.0);
    if (on_Dlight)
    {
        final += CalcLight(dlight_dir, dlight_amb, dlight_dif, dlight_spc, normHat, viewDir, true);
    }
    if(on_Plight)
    {
        final += CalcLight(plight_pos, plight_amb, plight_dif, plight_spc, normHat, viewDir, false);
    }   
    if((!on_Dlight) && (!on_Plight)){
        final = vec3(1.0, 1.0, 1.0);
    }
    final *= (object_color * 0.7f);
    FragColor = vec4(final, 1.0);
    // FragColor = vec4(object_color, 1.0);
}


vec3 CalcLight(vec3 pos_dir, vec3 ambient, vec3 diffuse, vec3 specular, vec3 normal,
                vec3 viewDir, bool is_dir)
{
    vec3 lightDir;
    if(is_dir)
    {
        lightDir = normalize(-1.0f * pos_dir); // position vector is actually a direction!
    }
    else
    {
        lightDir = normalize(pos_dir - FragPos);
    }
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        
    vec3 _ambient = ambient;
    vec3 _diffuse = diffuse * diff;
    vec3 _specular = specular * spec;
    return (_ambient + _diffuse + _specular);
}
