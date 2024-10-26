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
#include <random>

namespace GM
{
	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/
	class CGMDispatchCompute;
	class CopyMipmapCallback;

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
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);

		/** @brief ���� PBR shader
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetPBRShader(osg::Node* pNode);

		/** @brief ���� ���� shader
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		*/
		void SetBackgroundShader(osg::Node* pNode);

	private:
		/** @brief ��ʼ������̽��probe */
		void _InitProbe();
		/**
		* @brief �������������Զ���mipmap��Compute�ڵ�
		* @param iSourceLevel: Դmipmap�㼶��ֻ����0��1��2��3��4
		*/
		CGMDispatchCompute* _CreateMipmapCompute(const int iSourceLevel);

		/** @brief �������ڿ���mipmap�Ľڵ� */
		osg::Geometry* _CreateMipmapCopyNode();
		/**
		* @brief ��ȫ��ͼ���ɷ���̽��Probe
		* @param strInputFilePath		�����ȫ��ͼ·��
		* @param strOutputFilePath		�����probeͼ·��
		* @return bool					�Ƿ�ɹ�
		*/
		bool _CreateProbe( const std::string& strInputFilePath, const std::string& strOutputFilePath);

		/**
		* @brief ���� ת ȫ��ͼ�����е�UV
		* @param vDir			���򣬵�λΪ���㣬��������ϵ��x = ��ƫ���Ƕȣ�y = ������
		* @return osg::Vec2f	ȫ��ͼ�е�UV [0.0,1.0]
		*/
		inline osg::Vec2f _Dir2UV(const osg::Vec2f& vDir) const
		{
			return osg::Vec2f(vDir.x() / 360.0f, vDir.y() / 180.0f + 0.5f);
		}
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
		/**
		* @brief floatתunsigned char
		* @param fIn			float����,��Χ[0.0,1.0]
		* @return unsigned char	�����8λ���ݣ�[0,255]������
		*/
		inline unsigned char _Float2UnsignedChar(const float fIn) const
		{
			float fX = 255.0f * fIn;
			if ((fX - int(fX)) >= 0.5f) { fX += 1.0; }
			return (unsigned char)(fmin(fX, 255.0f));
		}

	// ����
	private:
		SGMKernelData* m_pKernelData = nullptr;					//!< �ں�����
		SGMConfigData* m_pConfigData = nullptr;					//!< ��������
		CGMCommonUniform* m_pCommonUniform = nullptr;			//!< ����Uniform

		std::string								m_strModelShaderPath;					//!< ģ��shader·��
		std::string								m_strDefTexPath;						//!< ģ�������ͼ��Ĭ��·��
		std::default_random_engine				m_iRandom;								//!< ���ֵ
		osg::ref_ptr<osgDB::Options>			m_pDDSOptions;							//!< dds���������
		// Ĭ�ϵĸ������ʵ���ͼ�����ڲ�������Ԫ
		std::vector<osg::ref_ptr<osg::Texture2D>> m_pPBRTexVector;						//!< PBRģ�͵�����ԪĬ����ͼ

		// ������ͼ
		osg::ref_ptr<osg::Texture2D>			m_pRainRippleTex;						//!< ˮ������
		osg::ref_ptr<osg::Texture2D>			m_pWetNormalTex;						//!< ��ʪ���淨����ͼ
		osg::ref_ptr<osg::Texture2D>			m_pNoiseTex;							//!< ������ͼ
		osg::ref_ptr<osg::Texture2D>			m_pSnowTex;								//!< ��ѩ��ͼ
		osg::ref_ptr<osg::Texture2D>			m_pSandTex;								//!< ɳ����ͼ
		osg::ref_ptr<osg::Texture2D>			m_pEnvProbeTex;							//!< ����̽����ͼ

		std::vector<osg::ref_ptr<osg::TextureCubeMap>> m_pCubeMapVector; //!< cubemap���飬6������6��level
		std::vector<osg::ref_ptr<CGMDispatchCompute>> m_pMipmapComputeVec; // �����Զ���mipmap�ļ�����ɫ���ڵ�
		osg::ref_ptr<osg::Geometry>				m_pCopyMipmapGeom;		// ����mipmap�ļ��Ͻڵ�
		CopyMipmapCallback* m_pCopyMipmapCB = nullptr;	// ����mipmap�Ļص�

	};

}	// GM
