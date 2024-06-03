/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007-2023 Adobe Inc.                                  */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Inc. and its suppliers, if                    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Inc. and its                    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Inc.            */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

/*	Skeleton.cpp	

	This is a compiling husk of a project. Fill it in with interesting
	pixel processing code.
	
	Revision History

	Version		Change													Engineer	Date
	=======		======													========	======
	1.0			(seemed like a good idea at the time)					bbb			6/1/2002

	1.0			Okay, I'm leaving the version at 1.0,					bbb			2/15/2006
				for obvious reasons; you're going to 
				copy these files directly! This is the
				first XCode version, though.

	1.0			Let's simplify this barebones sample					zal			11/11/2010

	1.0			Added new entry point									zal			9/18/2017
	1.1			Added 'Support URL' to PiPL and entry point				cjr			3/31/2023

*/

#include "Skeleton.h"

static PF_Err 
About (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	
	suites.ANSICallbacksSuite1()->sprintf(	out_data->return_msg,
											"%s v%d.%d\r%s",
											STR(StrID_Name), 
											MAJOR_VERSION, 
											MINOR_VERSION, 
											STR(StrID_Description));
	return PF_Err_NONE;
}

static PF_Err 
GlobalSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	out_data->my_version = PF_VERSION(	MAJOR_VERSION, 
										MINOR_VERSION,
										BUG_VERSION, 
										STAGE_VERSION, 
										BUILD_VERSION);

	out_data->out_flags =  PF_OutFlag_DEEP_COLOR_AWARE;	// just 16bpc, not 32bpc
	
	return PF_Err_NONE;
}

static PF_Err 
ParamsSetup (	
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err		err		= PF_Err_NONE;
	PF_ParamDef	def;	

	AEFX_CLR_STRUCT(def);

	PF_ADD_FLOAT_SLIDERX(	STR(StrID_Gain_Param_Name), 
							SKELETON_GAIN_MIN, 
							SKELETON_GAIN_MAX, 
							SKELETON_GAIN_MIN, 
							SKELETON_GAIN_MAX, 
							SKELETON_GAIN_DFLT,
							PF_Precision_HUNDREDTHS,
							0,
							0,
							GAIN_DISK_ID);

	AEFX_CLR_STRUCT(def);
    
    PF_ADD_LAYER(   "Layer",
                    PF_LayerDefault_MYSELF,
                    LAYER_DISK_ID);
    

    PF_ADD_FLOAT_SLIDERX(   "Intensity",
                            SKELETON_GAIN_MIN,
                            SKELETON_GAIN_MAX,
                            SKELETON_GAIN_MIN,
                            SKELETON_GAIN_MAX,
                            SKELETON_GAIN_DFLT,
                            PF_Precision_HUNDREDTHS,
                            0,
                            0,
                            INTENSITY_DISK_ID);
	
	out_data->num_params = SKELETON_NUM_PARAMS;

	return err;
}

float sat(float t) {
    return fmax(0.0, fmin(t, 1.0));
}

float mytrunc(float x, float num_levels) {
    return floor(x * num_levels) / num_levels;
}

float rand(float x, float y) {
    float z = sin(x * 12.9898 + y * 78.233) * 43758.5453;
    return z - floor(z);
}

float remap(float t, float a, float b) {
    return sat((t - a) / (b - a));
}

float linterp(float t) {
    return sat(1.0 - abs(2.0 * t - 1.0));
}

/*
float edgeDetection(float x, float y) {
    vec2 texel = 1.0 / iResolution.xy;
    float kernel[9];
    kernel[0] = -1.0; kernel[1] = -2.0; kernel[2] = -1.0;
    kernel[3] = 0.0; kernel[4] = 0.0; kernel[5] = 0.0;
    kernel[6] = 1.0; kernel[7] = 2.0; kernel[8] = 1.0;

    float offsetX[9];
    float offsetY[9];
    offsetX[0] = -x;
    offsetX[1] = 0.f;
    offsetX[2] = x;
    offsetX[3] = -x;
    offsetX[4] = 0.f;
    offsetX[5] = x;
    offsetX[6] = -x;
    offsetX[7] = 0.f;
    offsetX[8] = x;
    
    offsetY[0] = -y;
    offsetY[1] = -y;
    offsetY[2] = -y;
    offsetY[3] = 0.f;
    offsetY[4] = 0.f;
    offsetY[5] = 0.f;
    offsetY[6] = y;
    offsetY[7] = y;
    offsetY[8] = y;

    float edge = 0.0;
    for (int i = 0; i < 9; i++) {
        vec3 texColor = texture(tex, uv + offset[i]).rgb; //TODO get pixel here
        float intensity = dot(texColor, vec3(0.299, 0.587, 0.114));
        edge += intensity * kernel[i];
    }

    return sat(abs(edge));
}
*/
/*
vec2 sat(vec2 t) {
    return clamp(t, 0.0, 1.0);
}

float remap(float t, float a, float b) {
    return sat((t - a) / (b - a));
}

vec3 spectrum_offset(float t) {
    vec3 ret;
    float lo = step(t, 0.5);
    float hi = 1.0 - lo;
    float w = linterp(remap(t, 1.0 / 6.0, 5.0 / 6.0));
    float neg_w = 1.0 - w;
    ret = vec3(lo, 1.0, hi) * vec3(neg_w, w, neg_w);
    return pow(ret, vec3(1.0 / 2.2));
}

float srand(vec2 n) {
    return rand(n) * 2.0 - 1.0;
}


vec2 mytrunc(vec2 x, float num_levels) {
    return floor(x * num_levels) / num_levels;
}

float edgeDetection(vec2 uv, sampler2D tex) {
    vec2 texel = 1.0 / iResolution.xy;
    float kernel[9];
    kernel[0] = -1.0; kernel[1] = -2.0; kernel[2] = -1.0;
    kernel[3] = 0.0; kernel[4] = 0.0; kernel[5] = 0.0;
    kernel[6] = 1.0; kernel[7] = 2.0; kernel[8] = 1.0;

    vec2 offset[9];
    offset[0] = vec2(-texel.x, -texel.y); offset[1] = vec2(0.0, -texel.y); offset[2] = vec2(texel.x, -texel.y);
    offset[3] = vec2(-texel.x, 0.0); offset[4] = vec2(0.0, 0.0); offset[5] = vec2(texel.x, 0.0);
    offset[6] = vec2(-texel.x, texel.y); offset[7] = vec2(0.0, texel.y); offset[8] = vec2(texel.x, texel.y);

    float edge = 0.0;
    for (int i = 0; i < 9; i++) {
        vec3 texColor = texture(tex, uv + offset[i]).rgb;
        float intensity = dot(texColor, vec3(0.299, 0.587, 0.114));
        edge += intensity * kernel[i];
    }

    return sat(abs(edge));
}
void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord.xy / iResolution.xy;
    
    

    fragColor.a = sum.a;
    fragColor.rgb = sum.rgb;
}
 */

static PF_Err
MySimpleGainFunc8 (
	void		*refcon, 
	A_long		xL, 
	A_long		yL, 
	PF_Pixel8	*inP, 
	PF_Pixel8	*outP)
{
	PF_Err		err = PF_Err_NONE;

	GainInfo	*giP	= reinterpret_cast<GainInfo*>(refcon);
    
    // checkout (x,y) pixel from src layer
    PF_LayerDef layer = giP->checkout.u.ld;
    
    float time = giP->gainF * 0.1;
    
    float x = float(xL) / layer.width;
    float y = float(yL) / layer.height;

    float GLITCH = 0.1 * giP->intensityF;
    float gnm = sat(GLITCH);
    float rnd0 = rand(mytrunc(time, time), 6.0);
    float r0 = sat((1.0 - gnm) * 0.7 + rnd0);
    float rnd1 = rand(mytrunc(x, 10.0 * r0), time); // Horizontal distortion
    float r1 = 0.5 - 0.5 * gnm + rnd1;
    r1 = 1.0 - fmax(0.0, ((r1 < 1.0) ? r1 : 0.9999999));
    float rnd2 = rand(mytrunc(x, 40.0 * r1), time); // Vertical distortion
    float r2 = sat(rnd2);

    float rnd3 = rand(mytrunc(y, 10.0 * r0), time);
    float r3 = (1.0 - sat(rnd3 + 0.8)) - 0.1;

    float pxrnd = rand(x + time, y + time);

    float ofs = 0.05 * r2 * GLITCH * (rnd0 > 0.5 ? 1.0 : -1.0);
    ofs += 0.5 * pxrnd * ofs;
    
    // Begin edge detection
    float kernel[9];
    kernel[0] = -1.0; kernel[1] = -2.0; kernel[2] = -1.0;
    kernel[3] = 0.0; kernel[4] = 0.0; kernel[5] = 0.0;
    kernel[6] = 1.0; kernel[7] = 2.0; kernel[8] = 1.0;

    float offsetX[9];
    float offsetY[9];
    offsetX[0] = -x;
    offsetX[1] = 0.f;
    offsetX[2] = x;
    offsetX[3] = -x;
    offsetX[4] = 0.f;
    offsetX[5] = x;
    offsetX[6] = -x;
    offsetX[7] = 0.f;
    offsetX[8] = x;
    
    offsetY[0] = -y;
    offsetY[1] = -y;
    offsetY[2] = -y;
    offsetY[3] = 0.f;
    offsetY[4] = 0.f;
    offsetY[5] = 0.f;
    offsetY[6] = y;
    offsetY[7] = y;
    offsetY[8] = y;

    float edge = 0.0;
    for (int i = 0; i < 9; i++) {
        //vec3 texColor = texture(tex, uv + offset[i]).rgb; //TODO get pixel here
        int xNew = floor((x + offsetX[i]) * layer.width);
        int yNew = floor((y + offsetY[i]) * layer.height);
        
        if(xNew < 0) { xNew = 0; }
        if(xNew >= layer.width) { xNew = layer.width; }
        
        if(yNew < 0) { yNew = 0; }
        if(yNew >= layer.height) { yNew = layer.height; }
        
        PF_Pixel8 *p = (PF_Pixel*)((char*)(layer).data + (yNew * (layer).rowbytes) + (xNew * sizeof(PF_Pixel)));
        float intensity = (float(p->red) / 255.f) * 0.299 + (float(p->green) / 255.f) * 0.587 + (float(p->blue) / 255.f) * 0.114;
        edge += intensity * kernel[i];
    }
    
    edge = sat(abs(edge));
    // End edge detection
    
    //float edge = edgeDetection(uv, iChannel0);
    y += 0.1 * r3 * GLITCH * edge;

    const int NUM_SAMPLES = 15;
    const float RCP_NUM_SAMPLES_F = 1.0 / float(NUM_SAMPLES);

    float sumR = 0.f, sumB = 0.f, sumG = 0.f, sumA = 0.f;
    float wsumR = 0.f, wsumB = 0.f, wsumG = 0.f;
    for (int i = 0; i < NUM_SAMPLES; ++i) {
        float t = float(i) * RCP_NUM_SAMPLES_F;
        x = sat(x + ofs * t);
        
        int xNew = floor(x * layer.width);
        int yNew = floor(y * layer.height);
        
        if(xNew < 0) { xNew = 0; }
        if(xNew >= layer.width) { xNew = layer.width; }
        
        if(yNew < 0) { yNew = 0; }
        if(yNew >= layer.height) { yNew = layer.height; }
        
        PF_Pixel8 *samplecol = (PF_Pixel*)((char*)(layer).data + (yNew * (layer).rowbytes) + (xNew * sizeof(PF_Pixel)));

        //vec3 s = spectrum_offset(t);
        
        float lo = (t < 0.5) ? 0.0 : 1.0; //step(t, 0.5);
        float hi = 1.0 - lo;
        float w = linterp(remap(t, 1.0 / 6.0, 5.0 / 6.0));
        float neg_w = 1.0 - w;
        float sr = lo * neg_w;
        float sg = 1.0 * w;
        float sb = hi * neg_w;
        sr = pow(sr, 1.0 / 2.2);
        sg = pow(sg, 1.0 / 2.2);
        sb = pow(sb, 1.0 / 2.2);
        
        sumR += float(samplecol->red) * sr;
        sumG += float(samplecol->green) * sg;
        sumB += float(samplecol->blue) * sb;
        sumA += float(samplecol->alpha);
        
        wsumR += sr;
        wsumG += sg;
        wsumB += sb;
    }
    sumR /= wsumR;
    sumG /= wsumG;
    sumB /= wsumB;
    sumA *= RCP_NUM_SAMPLES_F;
    
    if(sumA > 255.f) { sumA = 255.f; }
    if(sumR > 255.f) { sumR = 255.f; }
    if(sumB > 255.f) { sumB = 255.f; }
    if(sumG > 255.f) { sumG = 255.f; }
    
    outP->alpha		=	(char)sumA;
    outP->red		=	(char)sumR;
    outP->green		=	(char)sumG;
    outP->blue		=	(char)sumB;

	return err;
}

static PF_Err 
Render (
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output )
{
	PF_Err				err		= PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	/*	Put interesting code here. */
	GainInfo			giP;
	AEFX_CLR_STRUCT(giP);
	A_long				linesL	= 0;
    
    PF_ParamDef checkout;
    AEFX_CLR_STRUCT(checkout);
    
    PF_CHECKOUT_PARAM(in_data,
                      SKELETON_LAYER,
                      in_data->current_time,
                      in_data->time_step,
                      in_data->time_scale,
                      &checkout);

	linesL 		   = output->extent_hint.bottom - output->extent_hint.top;
	giP.gainF 	   = params[SKELETON_GAIN]->u.fs_d.value;
    giP.intensityF = params[SKELETON_INTENSITY]->u.fs_d.value;
    giP.checkout   = checkout;
	
    ERR(suites.Iterate8Suite2()->iterate(	in_data,
                                            0,								// progress base
                                            linesL,							// progress final
                                            &params[SKELETON_INPUT]->u.ld,	// src
                                            NULL,							// area - null for all pixels
                                            (void*)&giP,					// refcon - your custom data pointer
                                            MySimpleGainFunc8,				// pixel function pointer
                                            output));

	return err;
}


extern "C" DllExport
PF_Err PluginDataEntryFunction2(
	PF_PluginDataPtr inPtr,
	PF_PluginDataCB2 inPluginDataCallBackPtr,
	SPBasicSuite* inSPBasicSuitePtr,
	const char* inHostName,
	const char* inHostVersion)
{
	PF_Err result = PF_Err_INVALID_CALLBACK;

	result = PF_REGISTER_EFFECT_EXT2(
		inPtr,
		inPluginDataCallBackPtr,
		"Skeleton", // Name
		"ADBE Skeleton", // Match Name
		"Sample Plug-ins", // Category
		AE_RESERVED_INFO, // Reserved Info
		"EffectMain",	// Entry point
		"https://www.adobe.com");	// support URL

	return result;
}


PF_Err
EffectMain(
	PF_Cmd			cmd,
	PF_InData		*in_data,
	PF_OutData		*out_data,
	PF_ParamDef		*params[],
	PF_LayerDef		*output,
	void			*extra)
{
	PF_Err		err = PF_Err_NONE;
	
	try {
		switch (cmd) {
			case PF_Cmd_ABOUT:

				err = About(in_data,
							out_data,
							params,
							output);
				break;
				
			case PF_Cmd_GLOBAL_SETUP:

				err = GlobalSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_PARAMS_SETUP:

				err = ParamsSetup(	in_data,
									out_data,
									params,
									output);
				break;
				
			case PF_Cmd_RENDER:

				err = Render(	in_data,
								out_data,
								params,
								output);
				break;
		}
	}
	catch(PF_Err &thrown_err){
		err = thrown_err;
	}
	return err;
}

