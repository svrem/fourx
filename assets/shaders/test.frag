#version 330 core
out vec4 FragColor;

uniform vec2 aspectRatio;
uniform vec2 position;

in vec2 pos;

void main()
{
    vec2 normalizedPos = pos / aspectRatio;
    vec2 offsettedPos = (pos + position / 5) / aspectRatio;
    

    float distance_to_center = distance(normalizedPos, vec2(0, 0));

    float a = 0.1;
    //float a = 0.1 - frameCount / 10000; 

//    if (a < 0.04)
  //  {
    //    a = 0.04;
    //}

    float c = offsettedPos.x / a;
    float d = offsettedPos.y / a;

    float max_dist = 0.08;

    float dist = min(abs(round(c) - c), abs(round(d) - d));

    if (dist < max_dist)
    {
        float rounding = -pow(dist * 40, 2) + 1;
        float fading = 1 - pow(distance_to_center, 0.7);
        FragColor = vec4(1.0, 1.0, 1.0, min(rounding, fading));
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }

//    if (abs(round(c) - c) < dis || abs(round(d) - d) < dis)
//    {
//        FragColor = vec4(1.0, 1.0, 1.0, 1-pow(distance_to_center,0.7));
//    } else {
//        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
//    }

    

}