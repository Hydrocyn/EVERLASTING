//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMModel.h
/// @brief		GMEngine - Model Module
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.12
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "GMCommon.h"
#include "GMCommonUniform.h"
#include "GMKernel.h"

#include <osg/Texture2D>

namespace GM
{
	/*************************************************************************
	 Enums
	*************************************************************************/

	/*************************************************************************
	 Structs
	*************************************************************************/

	/*************************************************************************
	 Class
	*************************************************************************/

	class CGMAnimation;
	class CGMMaterial;

	/*!
	 *  @class CGMModel��ϵͳ��λ������
	 *  @brief GM Model Module
	 */
	class CGMModel
	{
	// ����
	public:
		/** @brief ���� */
		CGMModel();
		/** @brief ���� */
		~CGMModel();

		/** @brief ��ʼ�� */
		bool Init(SGMKernelData* pKernelData, SGMConfigData* pConfigData, CGMCommonUniform* pCommonUniform);
		/** @brief ���� */
		bool Load();
		/** @brief ���� */
		bool Save();
		/** @brief ���� */
		bool Reset();
		/** @brief ���� */
		bool Update(double dDeltaTime);
		/** @brief ����(�������������̬֮��) */
		bool UpdatePost(double dDeltaTime);

		/**
		* @brief ������߽���ģ�͵Ķ������ܣ��������������ζ�����
		* @param strName: ģ���ڳ����е�����
		* @param bEnable: �Ƿ����ö�������
		* @return bool �ɹ�OK��ʧ��Fail��ģ�Ͳ����ڷ���NotExist
		*/
		bool SetAnimationEnable(const std::string& strName, const bool bEnable);

		/**
		* @brief ���ö�������ʱ��
		* @param strModelName ģ������
		* @param fDuration ����ʱ������λ����
		* @param strAnimationName �������ƣ������붯�����ƾ�ֻ�޸ĵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_Fail
		*/
		bool SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ��������ʱ��
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return float ����ʱ������λ����
		*/
		float GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���ö�������ģʽ
		* @param strModelName ģ������
		* @param ePlayMode ����ģʽ
		* @param strAnimationName �������ƣ������붯�����ƾ�ֻ�޸ĵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_Fail
		*/
		bool SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ��������ģʽ
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return EGMPlayMode ����ģʽ
		*/
		EGMPlayMode GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���ö������ȼ�
		* @param strModelName ģ������
		* @param iPriority �������ȼ���[0,100]����ֵԽ�����ȼ�Խ��
		* @param strAnimationName �������ƣ������붯�����ƾ�ֻ�޸ĵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_Fail
		*/
		bool SetAnimationPriority(const std::string& strModelName, const int iPriority, const std::string& strAnimationName = "");
		/**
		* @brief ��ȡ�������ȼ�
		* @param strModelName ģ������
		* @param strAnimationName ��������
		* @return int �������ȼ�
		*/
		int GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName);
		/**
		* @brief ���Ŷ�������������붯������������Ȩ��Ϊ0.0����ֹͣ���ж���
		* @param strModelName ģ������
		* @param fWeight �������Ȩ�أ�[0.0,1.0]��0.0��ʾֹͣ��1.0��ʾ���ţ������ɸ���Ȩ�ػ��
		* @param strAnimationName �������ƣ������붯�����ƾͲ��ŵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_Fail
		*/
		bool SetAnimationPlay(const std::string& strModelName, const float fWeight, const std::string& strAnimationName = "");
		/**
		* @brief ��ͣ����
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ������붯�����ƾ���ͣ���ж���
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_Fail
		*/
		bool SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName = "");
		/**
		* @brief �������Ŷ���
		* @param strModelName ģ������
		* @param strAnimationName �������ƣ������붯�����ƾͲ��ŵ�һ��������һ�����ƵĶ���
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_Fail
		*/
		bool SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName = "");

	private:
		/**
		* @brief ���ز��ʣ��ڲ�ʹ�ã�
		* @param pNode ��Ҫ�޸Ĳ��ʵĽڵ�ָ��
		* @param sData ��Ҫ�޸ĵ�ģ����Ϣ
		* @return bool �ɹ����� EGM_EC_OK��ʧ�ܷ��� EGM_EC_OK
		*/
		bool _SetMaterial(osg::Node* pNode, const SGMModelData& sData);
	
		void _InnerUpdate(const double dDeltaTime);

	// ����
	private:
		SGMKernelData* m_pKernelData;					//!< �ں�����
		SGMConfigData* m_pConfigData;					//!< ��������
		CGMCommonUniform* m_pCommonUniform;				//!< ����Uniform

		osg::ref_ptr<osg::Group>			m_pRootNode;
		osg::ref_ptr<osg::Node>				m_pAvatarNode;

		// �����ͼ��Ĭ��·��
		std::string							m_strDefTexPath = "Textures/";
		// ģ���ļ�Ĭ��·��
		std::string							m_strDefModelPath = "Models/";

		//!< ����������
		CGMAnimation*						m_pAnimationManager;
		//!< ���ʹ�����
		CGMMaterial*						m_pMaterial;
	};
}	// GM
