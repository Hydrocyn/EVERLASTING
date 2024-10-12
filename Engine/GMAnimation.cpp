//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2040, LT
/// All rights reserved.
///
/// @file		GMAnimation.cpp
/// @brief		GMEngine - Animation manager
/// @version	1.0
/// @author		LiuTao
/// @date		2024.1.11
//////////////////////////////////////////////////////////////////////////

#include "GMAnimation.h"
#include "GMNodeVisitor.h"
#include "GMCommon.h"

using namespace GM;

namespace GM
{
	/*
	*  @brief �������������̳���osgAnimation::BasicAnimationManager
	*/
	class CGMBasicAnimationManager : public osgAnimation::BasicAnimationManager
	{
	public:

		CGMBasicAnimationManager() {}

		CGMBasicAnimationManager(const osgAnimation::AnimationManagerBase& b, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
			: osgAnimation::BasicAnimationManager(b, copyop) {}

		virtual ~CGMBasicAnimationManager() {}

		// add pauseTime to resume animation
		void resumeAnimation(osgAnimation::Animation* pAnimation, int priority = 0, float weight = 1.0, double pauseTime = 0.0)
		{
			if (!findAnimation(pAnimation))
				return;

			if (isPlaying(pAnimation))
				stopAnimation(pAnimation);

			_animationsPlaying[priority].push_back(pAnimation);
			// for debug
			//std::cout << "player Animation " << pAnimation->getName() << " at " << _lastUpdate << std::endl;
			// GM changed by LiuTao: to resume animation
			pAnimation->setStartTime(_lastUpdate - pauseTime);
			pAnimation->setWeight(weight);
		}

		// add pauseTime to resume animation
		bool pauseAnimation(osgAnimation::Animation* pAnimation, double& pauseTime)
		{
			// search though the layer and remove animation
			for (AnimationLayers::iterator iterAnim = _animationsPlaying.begin(); iterAnim != _animationsPlaying.end(); ++iterAnim)
			{
				osgAnimation::AnimationList& list = iterAnim->second;
				for (osgAnimation::AnimationList::iterator it = list.begin(); it != list.end(); ++it)
					if ((*it) == pAnimation)
					{
						(*it)->resetTargets();
						list.erase(it);
						// GM changed by LiuTao: to resume animation, the unit of pauseTime is "ms"
						pauseTime = fmod(_lastUpdate - pAnimation->getStartTime(), pAnimation->getDuration());
						return true;
					}
			}
			return false;
		}
	};

	/*
	* @brief ����������������
	*/
	struct AnimationManagerFinder : public osg::NodeVisitor
	{
		osg::ref_ptr<CGMBasicAnimationManager> _am;
		AnimationManagerFinder() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
		void apply(osg::Node& node) {
			if (_am.valid())
				return;
			if (node.getUpdateCallback()) {
				osgAnimation::AnimationManagerBase* b = dynamic_cast<osgAnimation::AnimationManagerBase*>(node.getUpdateCallback());
				if (b) {
					_am = new CGMBasicAnimationManager(*b);
					return;
				}
			}
			traverse(node);
		}
	};

	/*
	*  @brief ������������ͬ��ģ�͹���һ��������������������ͬʱ���Ŷ������
	*/
	class CAnimationPlayer : public osg::Referenced
	{
	public:
		CAnimationPlayer() : _manager(nullptr), _focus(0) {}

		typedef std::vector<double> AnimationPauseTimeVector;
		typedef std::vector<int> AnimationPriorityVector;
		typedef std::vector<std::string> AnimationMapVector;
		typedef std::vector<std::string> ModelNameVector;

		// ���ģ������
		bool addModel(const std::string& strModelName)
		{
			auto itr = find(_modelNameVec.begin(), _modelNameVec.end(), strModelName);

			if (itr != _modelNameVec.end())
				return false;
			else
				_modelNameVec.push_back(strModelName);
			return true;
		}

		// �ж�_modelNameVec���Ƿ��Ѿ�����˸�ģ������
		bool hasModel(const std::string& strModelName)
		{
			auto itr = find(_modelNameVec.begin(), _modelNameVec.end(), strModelName);

			if (itr == _modelNameVec.end())
				return false;
			else
				return true;
		}

		// ��Ӷ���������
		bool addManager(CGMBasicAnimationManager* manager)
		{
			_manager = manager;
			for (osgAnimation::AnimationList::const_iterator it = _manager->getAnimationList().begin(); it != _manager->getAnimationList().end(); it++)
				_map[(*it)->getName()] = *it;

			for (osgAnimation::AnimationMap::iterator it = _map.begin(); it != _map.end(); it++)
			{
				_animNameVec.push_back(it->first);
				_animPauseTimeVec.push_back(0.0);
				_animPriorityVec.push_back(0);
			}
			return true;
		}

		// ���Ŷ���
		bool play()
		{
			if (_focus < _animNameVec.size())
			{
				_manager->playAnimation(_map[_animNameVec[_focus]].get());

				return true;
			}
			return false;
		}
		bool play(const std::string& name, float weight)
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++) if (_animNameVec[i] == name) _focus = i;

			_manager->playAnimation(_map[name].get(), _animPriorityVec[_focus], weight);

			return true;
		}

		// ֹͣ����
		void stop()
		{
			_manager->stopAll();
		}
		bool stop(const std::string& name)
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++) if (_animNameVec[i] == name) _focus = i;

			_manager->stopAnimation(_map[name].get());

			return true;
		}

		// ��ͣ����
		bool pause()
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++)
			{
				// _animPauseTimeVec�����ڼ�������ʱ�Ŀ�ʼʱ��
				_manager->pauseAnimation(_map[_animNameVec[i]].get(), _animPauseTimeVec[i]);
				return true;
			}
			return false;
		}
		bool pause(const std::string& name)
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++) if (_animNameVec[i] == name) _focus = i;

			// ���ڼ�������ʱ�Ŀ�ʼʱ��
			_manager->pauseAnimation(_map[name].get(), _animPauseTimeVec[_focus]);

			return true;
		}

		// �������Ŷ���
		bool resume()
		{
			if (_focus < _animNameVec.size())
			{
				_manager->resumeAnimation(_map[_animNameVec[_focus]].get(), _animPriorityVec[_focus], 1.0, _animPauseTimeVec[_focus]);
				return true;
			}
			return false;
		}
		bool resume(const std::string& name)
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++) if (_animNameVec[i] == name) _focus = i;

			_manager->resumeAnimation(_map[name].get(), _animPriorityVec[_focus], 1.0, _animPauseTimeVec[_focus]);

			return true;
		}

		bool setPriority(const int iPriority)
		{
			if (_focus < _animNameVec.size())
			{
				_animPriorityVec[_focus] = iPriority;
				return true;
			}
			return false;
		}
		bool setPriority(const std::string& name, const int iPriority)
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++)
			{
				if (_animNameVec[i] == name)
				{
					_animPriorityVec[i] = iPriority;
					return true;
				}
			}
			return false;
		}
		bool getPriority(const std::string& name, int& iPriority)
		{
			for (unsigned int i = 0; i < _animNameVec.size(); i++)
			{
				if (_animNameVec[i] == name)
				{
					iPriority = _animPriorityVec[i];
					return true;
				}
			}
			return false;
		}

		bool setDuration(const double fDuration)
		{
			if (_focus < _animNameVec.size())
			{
				_map[_animNameVec[_focus]].get()->setDuration(fDuration);
				return true;
			}
			return false;
		}
		bool setDuration(const std::string& name, const double fDuration)
		{
			if (!(_map[name].get()))
				return false;

			_map[name].get()->setDuration(fDuration);
			return true;
		}
		bool getDuration(const std::string& name, double& fDuration)
		{
			if (!(_map[name].get()))
				return false;
			fDuration = _map[name].get()->getDuration();
			if (fDuration == 0)
			{
				_map[name].get()->computeDuration();
				fDuration = _map[name].get()->getDuration();
			}
			return true;
		}

		bool setPlayMode(osgAnimation::Animation::PlayMode ePlayMode)
		{
			if (_focus < _animNameVec.size())
			{
				_map[_animNameVec[_focus]].get()->setPlayMode(ePlayMode);
				return true;
			}
			return false;
		}
		bool setPlayMode(const std::string& name, osgAnimation::Animation::PlayMode ePlayMode)
		{
			if (!(_map[name].get()))
				return false;

			_map[name].get()->setPlayMode(ePlayMode);
			return true;
		}
		bool getPlayMode(const std::string& name, osgAnimation::Animation::PlayMode& ePlayMode)
		{
			if (!(_map[name].get()))
				return false;

			ePlayMode = _map[name].get()->getPlayMode();
			return true;
		}

		const std::string& getCurrentAnimationName() const
		{
			return _animNameVec[_focus];
		}

		const AnimationMapVector& getAnimationList() const
		{
			return _animNameVec;
		}

	private:
		osg::ref_ptr<CGMBasicAnimationManager>	_manager; // ����������
		osgAnimation::AnimationMap				_map; // ����ӳ��
		AnimationMapVector						_animNameVec; // ��������
		AnimationPauseTimeVector				_animPauseTimeVec; // ������ͣ������ʱ��
		AnimationPriorityVector					_animPriorityVec; // �������ȼ�
		ModelNameVector							_modelNameVec; // ģ������
		unsigned int							_focus; // ��ǰ��������
	};
}

/*************************************************************************
CGMAnimation Methods
*************************************************************************/

CGMAnimation::CGMAnimation()
{
}

CGMAnimation::~CGMAnimation()
{
}

bool CGMAnimation::Reset()
{
	_ClearPlayer();
	return true;
}

bool CGMAnimation::AddAnimation(const std::string& strName, osg::Node* pNode)
{
	AnimationManagerFinder finder;
	pNode->accept(finder);
	if (finder._am.valid())
	{
		pNode->setUpdateCallback(finder._am.get());
		CAnimationPlayer* pPlayer = new CAnimationPlayer;
		pPlayer->addManager(finder._am.get());
		pPlayer->addModel(strName);

		const std::string strPlayerName = strName + "_Player";
		_AddPlayer(strPlayerName, pPlayer);

		return true;
	}
	return false;
}

bool CGMAnimation::RemoveAnimation(const std::string& strName)
{
	const std::string strPlayerName = strName + "_Player";
	_RemovePlayer(strPlayerName);

	return true;
}

bool CGMAnimation::SetAnimationDuration(const std::string& strModelName, const float fDuration, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			pAniPlayer->setDuration(fDuration);
		}
		else
		{
			pAniPlayer->setDuration(strAnimationName, fDuration);
		}

		return true;
	}
	else
	{
		return false;
	}
}

float CGMAnimation::GetAnimationDuration(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return 0.0;
	double duration = 0.0;
	pAniPlayer->getDuration(strAnimationName, duration);
	return duration;
}

bool CGMAnimation::SetAnimationMode(const std::string& strModelName, EGMPlayMode ePlayMode, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			pAniPlayer->setPlayMode((osgAnimation::Animation::PlayMode)ePlayMode);
		}
		else
		{
			pAniPlayer->setPlayMode(strAnimationName, (osgAnimation::Animation::PlayMode)ePlayMode);
		}

		return true;
	}
	else
	{
		return false;
	}
}

EGMPlayMode CGMAnimation::GetAnimationMode(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return EGMPlayMode::EGM_PLAY_ONCE;
	osgAnimation::Animation::PlayMode playermode;
	pAniPlayer->getPlayMode(strAnimationName, playermode);
	return (EGMPlayMode)playermode;
}

bool CGMAnimation::SetAnimationPriority(const std::string& strModelName, const int iPriority, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			pAniPlayer->setPriority(iPriority);
		}
		else
		{
			pAniPlayer->setPriority(strAnimationName, iPriority);
		}

		return true;
	}
	else
	{
		return false;
	}
}

int CGMAnimation::GetAnimationPriority(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer) return 0;
	int priority = 0;
	pAniPlayer->getPriority(strAnimationName, priority);
	return priority;
}

bool CGMAnimation::SetAnimationPlay(const std::string& strModelName, const float fWeight, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName)
		{
			if (fWeight > 0)
				pAniPlayer->play();
			else
				pAniPlayer->stop();
		}
		else
		{
			if (fWeight > 0)
				pAniPlayer->play(strAnimationName, fWeight);
			else
				pAniPlayer->stop(strAnimationName);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CGMAnimation::SetAnimationPause(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" == strAnimationName) // ��ͣ���ж���
		{
			pAniPlayer->pause();
		}
		else // ��ͣĳһ������
		{
			pAniPlayer->pause(strAnimationName);
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CGMAnimation::SetAnimationResume(const std::string& strModelName, const std::string& strAnimationName)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (pAniPlayer)
	{
		if ("" != strAnimationName) // �������Ʋ���Ϊ��
		{
			pAniPlayer->resume(strAnimationName);
			return true;
		}
	}

	return false;
}

bool CGMAnimation::GetAnimationList(const std::string& strModelName, std::vector<std::string>& vAnimationList)
{
	CAnimationPlayer* pAniPlayer = _GetPlayerByModelName(strModelName);
	if (!pAniPlayer)
		return false;
	for (auto& str : pAniPlayer->getAnimationList())
		vAnimationList.push_back(str);
	return true;
}

CAnimationPlayer* CGMAnimation::_GetPlayerByModelName(const std::string& strModelName)
{
	std::string strPlayerName = _GetPlayerName(strModelName);
	if (strPlayerName == "") return nullptr;

	return _GetPlayer(strPlayerName);
}

bool CGMAnimation::_AddPlayer(const std::string& strPlayerName, CAnimationPlayer* pPlayer)
{
	// ���������Ʋ���Ϊ��
	if (strPlayerName == "") return false;

	if (m_playerMap.end() == m_playerMap.find(strPlayerName))
	{
		m_playerMap[strPlayerName] = pPlayer;
		return true;
	}
	return false;
}

bool CGMAnimation::_RemovePlayer(const std::string& strPlayerName)
{
	// ���������Ʋ���Ϊ��
	if (strPlayerName == "") return false;

	auto iter = m_playerMap.find(strPlayerName);
	if (iter != m_playerMap.end())
	{
		m_playerMap.erase(iter);
		return true;
	}
	return false;
}

bool CGMAnimation::_ClearPlayer()
{
	auto iter = m_playerMap.begin();
	while (iter != m_playerMap.end())
	{
		m_playerMap.erase(iter++);
	}
	return true;
}

/* ���ݲ��������ƻ�ȡ���������� */
CAnimationPlayer* CGMAnimation::_GetPlayer(const std::string& strPlayerName)
{
	auto iter = m_playerMap.find(strPlayerName);

	if (iter == m_playerMap.end())
		return nullptr;
	else
		return m_playerMap[strPlayerName];
}

/* ����ģ�����ƻ�ȡ�������������� */
std::string CGMAnimation::_GetPlayerName(const std::string& strModelName)
{
	for (auto it = m_playerMap.begin(); it != m_playerMap.end(); it++)
	{
		if (it->second->hasModel(strModelName))
			return it->first;
	}
	return "";
}