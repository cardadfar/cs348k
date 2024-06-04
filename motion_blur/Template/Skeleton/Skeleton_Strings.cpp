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

#include "Skeleton.h"

typedef struct {
	A_u_long	index;
	A_char		str[256];
} TableString;



TableString		g_strs[StrID_NUMTYPES] = {
	StrID_NONE,						"",
	StrID_Name,						"Skeleton",
	StrID_Description,				"An empty (skeletal, if you will) effect sample,\r for your modifying pleasure.\rCopyright 2007-2023 Adobe Inc.",
	StrID_Gain_Param_Name,			"Samples",
    StrID_Scale_Param_Name,         "Scale",
    StrID_Magnitude_Param_Name,     "Magnitude",
    StrID_Direction_Param_Name,     "Direction",
    StrID_NumLines_Param_Name,      "Number of Lines",
    StrID_LineLength_Param_Name,      "Line Length"
};


char	*GetStringPtr(int strNum)
{
	return g_strs[strNum].str;
}
	
