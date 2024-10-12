//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2024~2044, LiuTao
/// All rights reserved.
///
/// @file		GMNodeVisitor.h
/// @brief		GMEngine - Node Visitor
/// @version	1.0
/// @author		LiuTao
/// @date		2024.10.12
//////////////////////////////////////////////////////////////////////////
#pragma once
#include <osg/NodeVisitor>

namespace GM
{
	/*************************************************************************
	 Class
	*************************************************************************/
	/*!
	 *  @class CGMNodeVisitor
	 *  @brief VrEarth Node Visitor
	 */
	template<class T>
	class CGMNodeVisitor : public osg::NodeVisitor
	{
	// ����
	public:
		/** @brief ���� */
		CGMNodeVisitor(TraversalMode eMode = TRAVERSE_ALL_CHILDREN)
			: osg::NodeVisitor(eMode), m_pResultNode(nullptr)
		{}

		/** @brief ��¼��� */
		virtual void apply(T& sNode)
		{
			m_pResultNode = &sNode;
			return;
		}

		/** @brief ��ȡ��� */
		inline T* GetResult() { return m_pResultNode; }

	// ����
	private:
		T*					m_pResultNode;			//!< ����ڵ�
	};

	template<class T>
	class CGMMultiNodeVisitor : public osg::NodeVisitor
	{
		// ����
	public:
		/** @brief ���� */
		CGMMultiNodeVisitor(TraversalMode eMode = TRAVERSE_ALL_CHILDREN)
			: osg::NodeVisitor(eMode)
		{}

		/** @brief ��¼��� */
		virtual void apply(T& sNode)
		{
			//m_pResultNode = &sNode;
			m_pResultNode.push_back(&sNode);
			return;
		}

		/** @brief ��ȡ��� */
		inline std::vector<T*> GetResult() { return m_pResultNode; }

		// ����
	private:
		std::vector<T*> m_pResultNode;			//!< ����ڵ�
	};

}	// GM
