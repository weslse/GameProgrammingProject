float4x4 gWorldMatrix;
float4x4 gViewMatrix;
float4x4 gProjectionMatrix;

float4 gWorldLightPosition;
float4 gWorldCameraPosition;

struct VS_INPUT
{
   float4 mPosition : POSITION;
   float3 mNormal: NORMAL;
};


struct VS_OUTPUT
{
   float4 mPosition : POSITION;
   float3 mDiffuse : TEXCOORD1;
   float3 mViewDir: TEXCOORD2;
   float3 mReflection: TEXCOORD3;
};

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.mPosition = mul( Input.mPosition, gWorldMatrix );

   float3 lightDir = Output.mPosition.xyz - gWorldLightPosition.xyz;
   lightDir = normalize(lightDir);

   float3 viewDir = normalize(Output.mPosition.xyz - gWorldCameraPosition.xyz);
   Output.mViewDir = viewDir;

   Output.mPosition = mul( Output.mPosition, gViewMatrix );
   Output.mPosition = mul( Output.mPosition, gProjectionMatrix );

   float3 worldNormal = mul( Input.mNormal, (float3x3)gWorldMatrix );
   worldNormal = normalize(worldNormal);

   Output.mDiffuse = dot(-lightDir, worldNormal);
   Output.mReflection = reflect(lightDir, worldNormal);

   return Output;
}

float3 gLightColor;

float4 ps_main(VS_OUTPUT Input) : COLOR
{
   float3 diffuse = gLightColor * saturate(Input.mDiffuse);

   float3 reflection = normalize(Input.mReflection);
   float3 viewDir = normalize(Input.mViewDir);
   float3 specular = 0;
   if ( diffuse.x > 0 )
   {
      specular = saturate(dot(reflection, -viewDir ));
      specular = pow(specular, 20.0f);
   }

   float3 ambient = float3(0.1f, 0.1f, 0.1f);

   return float4(ambient + diffuse + specular, 1);
}


technique Transform
{
    pass P0
    {
        VertexShader = compile vs_2_0 vs_main();
        PixelShader = compile ps_2_0 ps_main();
    }
}

