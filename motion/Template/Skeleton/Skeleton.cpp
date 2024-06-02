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
	
	out_data->num_params = SKELETON_NUM_PARAMS;

	return err;
}

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
    PF_Pixel8 *p = (PF_Pixel*)((char*)(layer).data + (yL * (layer).rowbytes) + (xL * sizeof(PF_Pixel)));
    
    /*if (p->red > 100)
    {
        p = inP;
    }*/
    
    float in_red = static_cast<float>(inP->red) / 255.0f;
    float in_blue = static_cast<float>(inP->blue) / 255.0f;
    float in_green = static_cast<float>(inP->green) / 255.0f;
    
    float cmax = std::max(in_red, std::max(in_green, in_blue)); // maximum of r, g, b
    float cmin = std::min(in_red, std::min(in_green, in_blue)); // minimum of r, g, b
    float diff = cmax - cmin; // diff of cmax and cmin.
    float hue = -1;
      
    // if cmax and cmax are equal then h = 0
    if (cmax == cmin)
        hue = 0;
  
    // if cmax equal r then compute h
    else if (cmax == in_red)
        hue = fmod(60 * ((in_green - in_blue) / diff) + 360, 360);
  
    // if cmax equal g then compute h
    else if (cmax == in_green)
        hue = fmod(60 * ((in_blue - in_red) / diff) + 120, 360);
  
    // if cmax equal b then compute h
    else if (cmax == in_blue)
        hue = fmod(60 * ((in_red - in_green) / diff) + 240, 360);
  
    // compute v
    float value = cmax;
    
    float blurScale = 0.2;
    
    float x_velocity = value * sin(hue*PI/180);
    float y_velocity = value * cos(hue*PI/180);
    

    // sample scene texture along direction of motion
    const float samples = 5;
    
    float red = 0;
    float green = 0;
    float blue = 0;
    float alpha = 0;
    
    if (value > 0)
    {
        for(float i=0; i<samples; i++) {
            float t = i / (samples-1);
            
            A_long x_offset = static_cast<A_long>(floor(x_velocity*blurScale*t*layer.width));
            A_long y_offset = static_cast<A_long>(floor(y_velocity*blurScale*t*layer.height));
            
            A_long coord_x = std::min(std::max(0, xL + x_offset),layer.width-1);
            A_long coord_y = std::min(std::max(0, yL + y_offset),layer.height-1);
            
            PF_Pixel8 *samp = (PF_Pixel*)((char*)(layer).data + (coord_y * (layer).rowbytes) + (coord_x * sizeof(PF_Pixel)));
            
            red += (static_cast<float>(samp->red) / 255.0f);
            blue += (static_cast<float>(samp->blue) / 255.0f);
            green += (static_cast<float>(samp->green) / 255.0f);
            alpha += (static_cast<float>(samp->alpha) / 255.0f);
            
        }
        red/= samples;
        blue/= samples;
        green/= samples;
        alpha/=samples;
        
        outP->alpha = static_cast<A_u_char>(std::min((alpha * 255.0), 255.0)); //p->alpha;
        outP->red = static_cast<A_u_char>(std::min((red * 255.0), 255.0));
        outP->green = static_cast<A_u_char>(std::min((green * 255.0), 255.0));
        outP->blue = static_cast<A_u_char>(std::min((blue * 255.0), 255.0));
    }
    else
    {
        outP->alpha = p->alpha;
        outP->red = p->red;
        outP->green = p->green;
        outP->blue = p->blue;
    }
    
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

