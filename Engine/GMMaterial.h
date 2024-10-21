//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMMaterial.h
/// @brief		GMEngine - Material manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.02.04
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMCommonUniform.h"
#include <osg/Texture2D>
#include <osg/Texture2DArray>
#include <osg/BufferIndexBinding>
#include <osg/Vec2>

namespace GM
{
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/

	/*!
	*  @Class CGMMaterial
	*  @brief DOF manager for GM
	*/
	class CGMMaterial
	{
	// ����
	public:
		/** @brief ���� */
		CGMMaterial();
		/** @brief ���� */
		~CGMMaterial();

		/** @brief ��ʼ�� */
		bool Init(SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);

		/** @brief ʵ��ģ�ͼ���shader
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetModelShader(osg::Node* pNode);

	private:
		
		/**
		* @brief ���ص�ַ ת �������ĵ��UV
		* @param iAddress		���ص�ַ�����������У������ڴ棩
		* @param iW				ͼ���
		* @param iH				ͼ�߶�
		* @return osg::Vec2f	�������ĵ��UV��(0.0, 1.0)
		*/
		inline osg::Vec2f _PixelAddress2UV(const unsigned int iPixelAddress, const int iW, const int iH) const
		{
			return osg::Vec2f(((iPixelAddress % iW) + 0.5f) / float(iW), ((iPixelAddress / iW) + 0.5f) / float(iH));
		}

	// ����
	private:
		SGMConfigData*							m_pConfigData;							//!< ��������
		CGMCommonUniform*						m_pCommonUniform;						//!< ����Uniform

		std::string								m_strModelShaderPath;					//!< ģ��shader·��
		std::string								m_strDefTexPath;						//!< ģ�������ͼ��Ĭ��·��

		osg::ref_ptr<osgDB::Options>			m_pDDSOptions;							//!< dds���������
		// Ĭ�ϵĸ������ʵ���ͼ�����ڲ�������Ԫ
		std::vector<osg::ref_ptr<osg::Texture2D>> m_pPBRTexVector;						//!< PBRģ�͵�����ԪĬ����ͼ

		// ������ͼ
		osg::ref_ptr<osg::Texture2D>			m_pRainRippleTex;						//!< ˮ������
		osg::ref_ptr<osg::Texture2D>			m_pWetNormalTex;						//!< ��ʪ���淨����ͼ
		osg::ref_ptr<osg::Texture2D>			m_pNoiseTex;							//!< ������ͼ
		osg::ref_ptr<osg::Texture2D>			m_pSnowTex;								//!< ��ѩ��ͼ
		osg::ref_ptr<osg::Texture2D>			m_pSandTex;								//!< ɳ����ͼ
	};

}	// GM
