//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMStructs.h
/// @brief		Galaxy-Music Engine -Kernel
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.09
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <osg/Group>
#include <osgViewer/Viewer>
#include "GMViewWidget.h"

namespace GM
{ 
	/*************************************************************************
	 Macro Defines
	*************************************************************************/
	#define GM_Root					m_pKernelData->vRoot
    #define GM_ViewWidget			m_pKernelData->pViewWidget
	#define GM_Viewer				GM_ViewWidget->getOsgViewer()

	/*************************************************************************
	 Struct
	*************************************************************************/
	/*!
	 *  @struct SGMKernelData
	 *  @brief �ں�����
	 */
	struct SGMKernelData
	{
		bool									bInited = false;		//!< �Ƿ��ʼ��
		osg::ref_ptr<osg::Group>				vRoot;					//!< ���ڵ�
		CGMViewWidget*							pViewWidget = nullptr;	//!< �̳���osgQOpenGLWidget
		osg::ref_ptr<osg::Camera>				pBackgroundCam;			//!< ����RTT���
		osg::ref_ptr<osg::Camera>				pForegroundCam;			//!< ǰ��RTT���
	};

	/*!
	*  @brief ��Ⱦ˳��
	*/


}	// GM
