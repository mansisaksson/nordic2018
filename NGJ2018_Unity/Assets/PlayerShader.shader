﻿Shader "Custom/PlayerShader" {
	Properties {
		_Color ("Color", Color) = (1,1,1,1)
		_MainTex ("Albedo (RGB)", 2D) = "white" {}
		_Glossiness ("Smoothness", Range(0,1)) = 0.5
		_Metallic ("Metallic", Range(0,1)) = 0.0
			_SinValue("SinValue", Range(0,20)) = 1.0
			_TimeValue("TimeValue", Range(0,10)) = 1.0
	}
	SubShader {
		Tags { "RenderType"="Opaque" }
		LOD 200

		CGPROGRAM
		// Physically based Standard lighting model, and enable shadows on all light types
		#pragma surface surf Standard fullforwardshadows

		// Use shader model 3.0 target, to get nicer looking lighting
		#pragma target 3.0

		sampler2D _MainTex;

		struct Input {
			float2 uv_MainTex;
		};

		half _Glossiness;
		half _Metallic;
		fixed4 _Color;
		fixed _SinValue;
		fixed _TimeValue;

		// Add instancing support for this shader. You need to check 'Enable Instancing' on materials that use the shader.
		// See https://docs.unity3d.com/Manual/GPUInstancing.html for more information about instancing.
		// #pragma instancing_options assumeuniformscaling
		UNITY_INSTANCING_BUFFER_START(Props)
			// put more per-instance properties here
		UNITY_INSTANCING_BUFFER_END(Props)

		void surf (Input IN, inout SurfaceOutputStandard o) {
			// Albedo comes from a texture tinted by color
			fixed4 c = tex2D (_MainTex, IN.uv_MainTex) * _Color;
			o.Albedo = c.rgb;
			// Metallic and smoothness come from slider variables
			o.Metallic = _Metallic;
			o.Smoothness = _Glossiness;
			o.Alpha = c.a
				;
			fixed lerpVal;

			
			lerpVal = (sin(_Time.y*3)+1)/2;
			fixed3 color = lerp(fixed3(0, 1, 0), fixed3(1, 0, 1), lerpVal);


			lerpVal = (sin(IN.uv_MainTex.y*_SinValue + _Time.y*_TimeValue) + 1) / 2;
			lerpVal = smoothstep(lerpVal, 0.3, 1);
			o.Emission = lerp(fixed3(0, 0, 0), color, lerpVal);
		}
		ENDCG
	}
	FallBack "Diffuse"
}
