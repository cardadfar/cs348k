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
#include <chrono>
#include <fstream>
#include <filesystem>
#include <iostream>

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

    PF_ADD_FLOAT_SLIDERX(   STR(StrID_Gain_Param_Name),
                            SKELETON_GAIN_MIN,
                            SKELETON_GAIN_MAX,
                            SKELETON_GAIN_MIN,
                            SKELETON_GAIN_MAX,
                            SKELETON_GAIN_DFLT,
                            PF_Precision_INTEGER,
                            0,
                            0,
                            GAIN_DISK_ID);
    
    PF_ADD_FLOAT_SLIDERX(   STR(StrID_Scale_Param_Name),
                            SKELETON_SCALE_MIN,
                            SKELETON_SCALE_MAX,
                            SKELETON_SCALE_MIN,
                            SKELETON_SCALE_MAX,
                            SKELETON_SCALE_DFLT,
                            PF_Precision_TEN_THOUSANDTHS,
                            0,
                            0,
                            SCALE_DISK_ID);
    
    PF_ADD_FLOAT_SLIDERX(   STR(StrID_Magnitude_Param_Name),
                            SKELETON_MAGNITUDE_MIN,
                            SKELETON_MAGNITUDE_MAX,
                            SKELETON_MAGNITUDE_MIN,
                            SKELETON_MAGNITUDE_MAX,
                            SKELETON_MAGNITUDE_DFLT,
                            PF_Precision_TEN_THOUSANDTHS,
                            0,
                            0,
                            MAGNITUDE_DISK_ID);
    
    PF_ADD_FLOAT_SLIDERX(   STR(StrID_Direction_Param_Name),
                            SKELETON_DIRECTION_MIN,
                            SKELETON_DIRECTION_MAX,
                            SKELETON_DIRECTION_MIN,
                            SKELETON_DIRECTION_MAX,
                            SKELETON_DIRECTION_DFLT,
                            PF_Precision_TEN_THOUSANDTHS,
                            0,
                            0,
                            DIRECTION_DISK_ID);
    
    PF_ADD_FLOAT_SLIDERX(   STR(StrID_NumLines_Param_Name),
                            SKELETON_NUM_LINES_MIN,
                            SKELETON_NUM_LINES_MAX,
                            SKELETON_NUM_LINES_MIN,
                            SKELETON_NUM_LINES_MAX,
                            SKELETON_NUM_LINES_DFLT,
                            PF_Precision_INTEGER,
                            0,
                            0,
                            NUM_LINES_DISK_ID);
    
    PF_ADD_FLOAT_SLIDERX(   STR(StrID_LineLength_Param_Name),
                            SKELETON_LINE_LENGTH_MIN,
                            SKELETON_LINE_LENGTH_MAX,
                            SKELETON_LINE_LENGTH_MIN,
                            SKELETON_LINE_LENGTH_MAX,
                            SKELETON_LINE_LENGTH_DFLT,
                            PF_Precision_TEN_THOUSANDTHS,
                            0,
                            0,
                            LINE_LENGTH_DISK_ID);

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
    // Second Shader Pass Timing
    auto start_pass2 = std::chrono::high_resolution_clock::now();
    // Second Shader Pass TIming
    
	PF_Err		err = PF_Err_NONE;
    
	GainInfo	*giP	= reinterpret_cast<GainInfo*>(refcon);
    
    // checkout (x,y) pixel from src layer
    PF_LayerDef layer = giP->checkout.u.ld;
    
    PF_FpLong gainValue = giP->gainF;
    PF_FpLong scaleValue = giP->scaleF;
    
    PF_FpLong magnitudeValue = giP->magnitudeF;
    PF_FpLong directionValue = giP->directionF;
    
    PF_FpLong numLinesValue = giP->numLinesF;
    PF_FpLong lineLengthValue = giP->lineLengthF;
    
    const float blurScale = static_cast<float>(scaleValue);
    const float samples = static_cast<float>(gainValue);
    
    const float mag = static_cast<float>(magnitudeValue);
    const float dir = static_cast<float>(directionValue);

    float x_velocity = mag * sin(dir * 180.0f * PI);
    float y_velocity = mag * cos(dir * 180.0f * PI);
    
    float red = 0;
    float green = 0;
    float blue = 0;
    float alpha = 0;
    

    float numLines = static_cast<float>(numLinesValue);
    float lineLength = static_cast<float>(lineLengthValue);
    float lineThickness = lineLength / 50.0f;

    float cos_dir = cos(dir / 180.0f * PI);
    float sin_dir = sin(dir / 180.0f * PI);
    
    float xCur = static_cast<float>(xL) / layer.width;
    float yCur = static_cast<float>(yL) / layer.height;
    
    std::mt19937 gen(static_cast<unsigned>(giP->frameSeed));
    std::uniform_int_distribution<> dis(1, 50);
    
    
    float n = 0;
    
    // sample scene texture along direction of motion
    for(float i=0; i<numLines; i++) {
            
        if (n < samples)
        {
            float t = i / (samples-1);
                    
            A_long x_offset = static_cast<A_long>(floor(x_velocity*blurScale*t*layer.width));
            A_long y_offset = static_cast<A_long>(floor(y_velocity*blurScale*t*layer.height));
            
            A_long coord_x = std::min(std::max(0, xL + x_offset),layer.width-1);
            A_long coord_y = std::min(std::max(0, yL + y_offset),layer.height-1);
            
            PF_Pixel8 *samp = (PF_Pixel*)((char*)(layer).data + (coord_y * (layer).rowbytes) + (coord_x * sizeof(PF_Pixel)));
            
            red += (static_cast<float>(samp->red) / 255.0f) * (1.0f/samples);
            blue += (static_cast<float>(samp->blue) / 255.0f) * (1.0f/samples);
            green += (static_cast<float>(samp->green) / 255.0f) * (1.0f/samples);
            alpha += (static_cast<float>(samp->alpha) / 255.0f) * (1.0f/samples);
            n++;
        }
        
        float xRand = static_cast<float>(dis(gen)) / 50.0f;
        float yRand = static_cast<float>(dis(gen)) / 50.0f;
        float colorRand = static_cast<float>(dis(gen)) / 50.0f;
        
        float vec1_x = xCur - xRand;
        float vec1_y = yCur - yRand;
        
        float proj = std::min(std::max(0.0f, vec1_x * cos_dir + vec1_y * sin_dir), lineLength);
        
        float dist_squared = (xCur - (xRand + proj * cos_dir)) * (xCur - (xRand + proj * cos_dir)) +
        (yCur - (yRand + proj * sin_dir)) * (yCur - (yRand + proj * sin_dir));
        
        if (dist_squared < lineThickness * lineThickness) {
            alpha = 1.0f;
            red = 0.0f;
            blue = 0.0f;
            green = 0.0f;
            
            if (colorRand > 0.5)
                red = 1.0f;
            
            break;
        }
        
    }
    /*red/= samples;
    blue/= samples;
    green/= samples;
    alpha/=samples;*/
    
    outP->alpha = static_cast<A_u_char>(std::min((alpha * 255.0), 255.0)); //p->alpha;
    outP->red = static_cast<A_u_char>(std::min((red * 255.0), 255.0));
    outP->green = static_cast<A_u_char>(std::min((green * 255.0), 255.0));
    outP->blue = static_cast<A_u_char>(std::min((blue * 255.0), 255.0));
    
    // Second shader pass timing
    auto end_pass2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_pass1 = end_pass2 - start_pass2;
    std::cout << "..." << std::endl;
    
    std::ofstream logFile("plugin_benchmark_2.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << elapsed_pass1.count() << std::endl;
        logFile.close();
    }
    // Second shader pass timing
    return err;
    
}

static PF_Err MySecondPassFunc8(
    void        *refcon,
    A_long      xL,
    A_long      yL,
    PF_Pixel8   *inP,
    PF_Pixel8   *outP)
{
    // First Shader Pass Timing
    auto start_pass1 = std::chrono::high_resolution_clock::now();
    // First Shader Pass TIming
    
    PF_Err err = PF_Err_NONE;

    GainInfo *giP = reinterpret_cast<GainInfo*>(refcon);

    PF_FpLong directionValue = giP->directionF;
    PF_FpLong numLinesValue = giP->numLinesF;
    PF_FpLong lineLengthValue = giP->lineLengthF;
    
    PF_LayerDef layer = giP->checkout.u.ld;

    float numLines = static_cast<float>(numLinesValue);
    float lineLength = static_cast<float>(lineLengthValue);
    float lineThickness = lineLength / 50.0f;

    float dir = static_cast<float>(directionValue);
    float cos_dir = cos(dir / 180.0f * PI);
    float sin_dir = sin(dir / 180.0f * PI);
    
    float xCur = static_cast<float>(xL) / layer.width;
    float yCur = static_cast<float>(yL) / layer.height;
    
    std::mt19937 gen(static_cast<unsigned>(giP->frameSeed));
    std::uniform_int_distribution<> dis(1, 50);
    
    outP->alpha = inP->alpha;
    outP->red = inP->red;
    outP->blue = inP->blue;
    outP->green = inP->green;
    
    for (float n = 0; n < numLines; n++) {
        float xRand = static_cast<float>(dis(gen)) / 50.0f;
        float yRand = static_cast<float>(dis(gen)) / 50.0f;
        float colorRand = static_cast<float>(dis(gen)) / 50.0f;
        
        float vec1_x = xCur - xRand;
        float vec1_y = yCur - yRand;

        float proj = std::min(std::max(0.0f, vec1_x * cos_dir + vec1_y * sin_dir), lineLength);

        float dist_squared = (xCur - (xRand + proj * cos_dir)) * (xCur - (xRand + proj * cos_dir)) +
                             (yCur - (yRand + proj * sin_dir)) * (yCur - (yRand + proj * sin_dir));

        if (dist_squared < lineThickness * lineThickness) {
            outP->alpha = 255;
            outP->red = 0;
            outP->blue = 0;
            outP->green = 0;
            
            if (colorRand > 0.5)
                outP->red = 255;
        }
    }
    
    // First shader pass timing
    auto end_pass1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_pass1 = end_pass1 - start_pass1;
    std::cout << "..." << std::endl;
    
    std::ofstream logFile("plugin_benchmark_1.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << elapsed_pass1.count() << std::endl;
        logFile.close();
    }
    // First shader pass timing
    
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
    giP.scaleF     = params[SKELETON_SCALE]->u.fs_d.value;
    giP.magnitudeF = params[SKELETON_MAGNITUDE]->u.fs_d.value;
    giP.directionF = params[SKELETON_DIRECTION]->u.fs_d.value;
    giP.numLinesF = params[SKELETON_NUM_LINES]->u.fs_d.value;
    giP.lineLengthF = params[SKELETON_LINE_LENGTH]->u.fs_d.value;
    giP.frameSeed = static_cast<A_long>(in_data->current_time / in_data->time_step);
    
    giP.checkout   = checkout;
	
    /*PF_EffectWorld first_pass_world;
    AEFX_CLR_STRUCT(first_pass_world);
    
    PF_NewWorldFlags flags = PF_NewWorldFlag_CLEAR_PIXELS;
    ERR(suites.WorldSuite1()->new_world(in_data->effect_ref, output->width, output->height, flags, &first_pass_world));
    
    if (!err)
    {*/
        ERR(suites.Iterate8Suite2()->iterate(	in_data,
                                             0,								// progress base
                                             linesL,							// progress final
                                             &params[SKELETON_INPUT]->u.ld,	// src
                                             NULL,							// area - null for all pixels
                                             (void*)&giP,					// refcon - your custom data pointer
                                             MySimpleGainFunc8,				// pixel function pointer
                                             output));
        

        /*ERR(suites.Iterate8Suite2()->iterate(    in_data,
                                             0,                                // progress base
                                             linesL,                            // progress final
                                             &first_pass_world,    // src
                                             NULL,                            // area - null for all pixels
                                             (void*)&giP,                    // refcon - your custom data pointer
                                             MySecondPassFunc8,                // pixel function pointer
                                             output));
        ERR(suites.WorldSuite1()->dispose_world(in_data->effect_ref, &first_pass_world));
    }*/
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
		"Directional Motion Blur", // Name
		"ADBE Directional Motion Blur", // Match Name
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

