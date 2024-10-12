//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMVolumeBasic.h
/// @brief		Galaxy-Music Engine - GMEngine.h
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.10
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "GMCommon.h"
#include "GMKernel.h"
#include <random>

/*************************************************************************
Class
*************************************************************************/
class QWidget;

namespace GM
{
	/*************************************************************************
	Macro Defines
	*************************************************************************/
	#define GM_ENGINE					CGMEngine::getSingleton()
	#define GM_ENGINE_PTR				CGMEngine::getSingletonPtr()

	/*************************************************************************
	Enums
	*************************************************************************/

	/*************************************************************************
	Structs
	*************************************************************************/

	/*************************************************************************
	Class
	*************************************************************************/
	class CGMCommonUniform;
	class CGMPost;

	/*!
	*  @class CGMEngine
	*  @brief Galaxy-Music GMEngine
	*/
	class CGMEngine : public CGMSingleton<CGMEngine>
	{
		// ����
	protected:
		/** @brief ���� */
		CGMEngine();
		/** @brief ���� */
		~CGMEngine();

	public:
		/** @brief ��ȡ���� */
		static CGMEngine& getSingleton(void);
		static CGMEngine* getSingletonPtr(void);

		/** @brief ��ʼ�� */
		bool Init();
		/** @brief �ͷ� */
		void Release();
		/** @brief ���� */
		bool Update();
		/** @brief ���� */
		bool Load();
		/** @brief ���� */
		bool Save();
		/**
		* �޸���Ļ�ߴ�ʱ���ô˺���
		* @param iW: ��Ļ���
		* @param iH: ��Ļ�߶�
		*/
		void ResizeScreen(const int iW, const int iH);
		/**
		* @brief ����/��ͣ��Ⱦ����С��������������ö�ʱ���Թر���Ⱦ
		*  �ر���Ϊ����С��ʱ���˷��Կ���Դ
		* @param bEnable: �Ƿ�����Ⱦ
		*/
		inline void SetRendering(const bool bEnable)
		{
			m_bRendering = bEnable;
		}
		/**
		* @brief �Ƿ�����Ⱦ
		* @return bool: �Ƿ�����Ⱦ
		*/
		inline bool GetRendering() const
		{
			return m_bRendering;
		}
		
		/** @brief �����ӿ�(QT:QWidget) */
		CGMViewWidget* CreateViewWidget(QWidget* parent);

	private:
		/**
		* @brief ��������
		*/
		bool _LoadConfig();
		/**
		* @brief ��ʼ��������ؽڵ�
		*/
		void _InitBackground();
		/**
		* @brief ��ʼ��ǰ����ؽڵ�
		*/
		void _InitForeground();
		
		/**
		* @brief ������£�һ���Ӹ���10��
		* @param updateStep ���μ�����µ�ʱ����λs
		*/
		void _InnerUpdate(const float updateStep);
		/** @brief ����(�������������̬֮��) */
		bool _UpdateLater(const double dDeltaTime);
		
		/**
		* @brief SGMVector3 ת osg::Vec3d
		* @param vGM:				�����GM����
		* @return osg::Vec3d:		�����osg���� */
		inline SGMVector3 _OSG2GM(const osg::Vec3d& vOSG) const
		{
			return SGMVector3(vOSG.x(), vOSG.y(), vOSG.z());
		}

		// ����
	private:

		SGMKernelData*						m_pKernelData;				//!< �ں�����
		SGMConfigData*						m_pConfigData;				//!< ��������
		CGMCommonUniform*					m_pCommonUniform;			//!< ����Uniform

		bool								m_bInit;					//!< ��ʼ����־
		bool								m_bRendering;				//!< �Ƿ���Ⱦ
		double								m_dTimeLastFrame;			//!< ��һ֡ʱ��
		float								m_fDeltaStep;				//!< ��λs
		float								m_fConstantStep;			//!< �ȼ�����µ�ʱ��,��λs

		CGMPost*							m_pPost;					//!< ����ģ��

		std::default_random_engine			m_iRandom;

		osg::ref_ptr<osg::Texture2D>		m_pSceneTex;				//!< ��������ɫͼ
		osg::ref_ptr<osg::Texture2D>		m_pBackgroundTex;			//!< ������ɫͼ
		osg::ref_ptr<osg::Texture2D>		m_pForegroundTex;			//!< ǰ����ɫͼ
	};
}	// GM