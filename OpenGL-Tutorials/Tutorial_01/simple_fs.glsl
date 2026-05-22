#version 330

in float colo;

out vec4 frag_colour;

void main () 
{
    if (colo > 0.5){
        frag_colour = vec4(0.0, 0.5, 0.5, 1.0); // cyan
    }
    else{
        frag_colour = vec4(0.5, 0.0, 0.5, 1.0); // magenta
    }
}
