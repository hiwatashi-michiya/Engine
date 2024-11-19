
struct Material
{
	
    float32_t4 color;
    
    int32_t enableLighting;
    float32_t shininess;
    
    //dissolve
    float32_t Threshold;
    float32_t edgeValue;
    float32_t3 edgeColor;
    
    //ノイズ
    int32_t isActiveNoise;
    
    float32_t4x4 uvTransform;
    
};