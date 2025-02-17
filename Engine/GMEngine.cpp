﻿//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		GMVolumeBasic.h
/// @brief		Galaxy-Music Engine - GMEngine.cpp
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.10
//////////////////////////////////////////////////////////////////////////

#include "GMEngine.h"
#include "GMViewWidget.h"
#include "GMCommonUniform.h"
#include "GMBaseManipulator.h"
#include "GMXml.h"
#include "GMPost.h"
#include "GMModel.h"
#include "GMLight.h"
#include <osgViewer/ViewerEventHandlers>
#include <QtCore/QTimer>

#include <iostream>

using namespace GM;

/*************************************************************************
Global Constants
*************************************************************************/
static const std::string g_strGMConfigFile = "Everlasting.cfg";	//!< 配置文件名

/*************************************************************************
 Macro Defines
*************************************************************************/

/*************************************************************************
 CGMEngine Methods
*************************************************************************/

class ResizeEventHandler : public osgGA::GUIEventHandler
{
public:
	ResizeEventHandler(CGMEngine* pEngine): _pEngine(pEngine){}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
	{
		if ((ea.getEventType() == osgGA::GUIEventAdapter::RESIZE) && _pEngine)
		{
			_pEngine->ResizeScreen(ea.getWindowWidth(), ea.getWindowHeight());
			return true;
		}
		return false;
	}
private:
	CGMEngine* _pEngine = nullptr;
};

template<> CGMEngine* CGMSingleton<CGMEngine>::msSingleton = nullptr;

/** @brief 获取单例 */
CGMEngine& CGMEngine::getSingleton(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMEngine);
	assert(msSingleton);
	return (*msSingleton);
}
CGMEngine* CGMEngine::getSingletonPtr(void)
{
	if (!msSingleton)
		msSingleton = GM_NEW(CGMEngine);
	assert(msSingleton);
	return msSingleton;
}

/** @brief 构造 */
CGMEngine::CGMEngine():
	m_pSceneTex(nullptr), m_pBackgroundTex(nullptr), m_pForegroundTex(nullptr)
{
	Init();
}

/** @brief 析构 */
CGMEngine::~CGMEngine()
{
	GM_DELETE(msSingleton);
}

/** @brief 初始化 */
bool CGMEngine::Init()
{
	if (m_bInit) return true;

	//!< 配置数据
	_LoadConfig();

	//!< 内核数据
	m_pKernelData = new SGMKernelData();

	GM_Root = new osg::Group();

	m_pSceneTex = new osg::Texture2D();
	m_pSceneTex->setTextureSize(m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pSceneTex->setInternalFormat(GL_RGBA8);
	m_pSceneTex->setSourceFormat(GL_RGBA);
	m_pSceneTex->setSourceType(GL_UNSIGNED_BYTE);
	m_pSceneTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pSceneTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pSceneTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	m_pSceneTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	m_pSceneTex->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pSceneTex->setDataVariance(osg::Object::DYNAMIC);
	m_pSceneTex->setResizeNonPowerOfTwoHint(false);

	// 初始化背景相关节点
	_InitBackground();
	// 初始化前景相关节点
	_InitForeground();

	m_pCommonUniform = new CGMCommonUniform();
	m_pManipulator = new CGMBaseManipulator();
	m_pPost = new CGMPost();
	m_pModel = new CGMModel();
	m_pLight = new CGMLight();

	m_pCommonUniform->Init(m_pKernelData, m_pConfigData);
	m_pPost->Init(m_pKernelData, m_pConfigData, m_pCommonUniform);
	m_pModel->Init(m_pKernelData, m_pConfigData, m_pCommonUniform);
	m_pLight->Init(m_pKernelData, m_pConfigData);

	//设置阴影
	m_pLight->AddShadowNode(m_pModel->GetRootNode());
	m_pModel->SetShadowMap(m_pLight->GetShadowMap());

	m_pModel->SetUniform(m_pLight->GetView2ShadowMatrixUniform());

	m_bInit = true;
	m_pKernelData->bInited = m_bInit;

	return true;
}


/** @brief 释放 */
void CGMEngine::Release()
{
	setlocale(LC_ALL, "C");
	if (GM_ViewWidget)
	{
		delete GM_ViewWidget;
	}

	GM_DELETE(m_pCommonUniform);
	GM_DELETE(m_pConfigData);
	GM_DELETE(m_pKernelData);
	GM_DELETE(msSingleton);
}

/** @brief 更新 */
bool CGMEngine::Update()
{
	if (!m_bInit)
		return false;

	if (GM_Viewer->done())
		return true;

	if (GM_Viewer->getRunFrameScheme() == osgViewer::ViewerBase::CONTINUOUS || GM_Viewer->checkNeedToDoFrame())
	{
		double timeCurrFrame = osg::Timer::instance()->time_s();
		double deltaTime = timeCurrFrame - m_dTimeLastFrame; //单位:秒
		m_dTimeLastFrame = timeCurrFrame;

		float fInnerDeltaTime = deltaTime;
		fInnerDeltaTime += m_fDeltaStep;
		float updateStep = m_fConstantStep;
		while (fInnerDeltaTime >= updateStep)
		{
			if(m_bRendering)
				_InnerUpdate(updateStep);
			fInnerDeltaTime -= updateStep;
		}
		m_fDeltaStep = fInnerDeltaTime;

		if (m_bRendering)
		{
			m_pCommonUniform->Update(deltaTime);
			m_pPost->Update(deltaTime);
			m_pModel->Update(deltaTime);
			m_pLight->Update(deltaTime);

			//GM_Viewer->advance(USE_REFERENCE_TIME);
			//GM_Viewer->eventTraversal();
			//GM_Viewer->updateTraversal();

			// 在主相机改变位置后再更新
			_UpdateLater(deltaTime);

			//GM_Viewer->renderingTraversals();
		}
	}
	return true;
}

/** @brief 加载 */
bool CGMEngine::Load()
{
	m_pPost->Load();
	m_pModel->Load();
	m_pLight->Load();
	return true;
}

/** @brief 保存 */
bool CGMEngine::Save()
{
	m_pModel->Save();
	return false;
}

void CGMEngine::ResizeScreen(const int iW, const int iH)
{
	osg::ref_ptr<osg::Camera> pMainCam = GM_Viewer->getCamera();
	if (pMainCam.valid())
	{
		double fovy, aspectRatio, zNear, zFar;
		pMainCam->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);

		pMainCam->resize(iW, iH);
		double fAspetRatio = double(iW) / double(iH);
		pMainCam->setProjectionMatrixAsPerspective(fovy, fAspetRatio, zNear, zFar);
		pMainCam->dirtyAttachmentMap();
	}

	if (m_pKernelData->pBackgroundCam.valid())
		m_pKernelData->pBackgroundCam->resize(iW, iH);
	if (m_pKernelData->pForegroundCam.valid())
		m_pKernelData->pForegroundCam->resize(iW, iH);
	if (m_pCommonUniform)
		m_pCommonUniform->ResizeScreen(iW, iH);
	if (m_pPost)
		m_pPost->ResizeScreen(iW, iH);
}

CGMViewWidget* CGMEngine::CreateViewWidget(QWidget* parent)
{
	GM_ViewWidget = new CGMViewWidget(parent);
	return GM_ViewWidget;
}

void CGMEngine::InitGMViewer()
{
	GM_Viewer->setSceneData(GM_Root);
	GM_Viewer->setCameraManipulator(m_pManipulator);
	GM_Viewer->setQuitEventSetsDone(false);
	//状态信息
	osgViewer::StatsHandler* pStatsHandler = new osgViewer::StatsHandler;
	pStatsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
	GM_Viewer->addEventHandler(pStatsHandler);
	GM_Viewer->addEventHandler(new ResizeEventHandler(this));

	m_pPost->CreatePost(m_pSceneTex.get(), m_pBackgroundTex.get(), m_pForegroundTex.get());
	if (EGMRENDER_LOW != m_pConfigData->eRenderQuality)
	{
		//m_pPost->SetVolumeEnable(true, m_pGalaxy->GetTAATex());
	}
}

/** @brief 加载配置 */
bool CGMEngine::_LoadConfig()
{
	CGMXml hXML;
	hXML.Load(g_strGMConfigFile, "Config");
	m_pConfigData = new SGMConfigData;

	// 解析系统配置
	CGMXmlNode sNode = hXML.GetChild("System");

	m_pConfigData->strCorePath = sNode.GetPropStr("corePath", m_pConfigData->strCorePath.c_str());
	m_pConfigData->strMediaPath = sNode.GetPropWStr("mediaPath", m_pConfigData->strMediaPath.c_str());
	m_pConfigData->eRenderQuality = EGMRENDER_QUALITY(sNode.GetPropInt("renderQuality", m_pConfigData->eRenderQuality));
	m_pConfigData->fFovy = sNode.GetPropFloat("fovy", m_pConfigData->fFovy);

	return true;
}

void CGMEngine::_InitBackground()
{
	m_pBackgroundTex = new osg::Texture2D();
	m_pBackgroundTex->setTextureSize(m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pBackgroundTex->setInternalFormat(GL_RGBA8);
	m_pBackgroundTex->setSourceFormat(GL_RGBA);
	m_pBackgroundTex->setSourceType(GL_UNSIGNED_BYTE);
	m_pBackgroundTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pBackgroundTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pBackgroundTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	m_pBackgroundTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	m_pBackgroundTex->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pBackgroundTex->setDataVariance(osg::Object::DYNAMIC);
	m_pBackgroundTex->setResizeNonPowerOfTwoHint(false);

	m_pKernelData->pBackgroundCam = new osg::Camera;
	m_pKernelData->pBackgroundCam->setName("backgroundCamera");
	m_pKernelData->pBackgroundCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
	m_pKernelData->pBackgroundCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pKernelData->pBackgroundCam->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	m_pKernelData->pBackgroundCam->setViewport(0, 0, m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pKernelData->pBackgroundCam->setRenderOrder(osg::Camera::PRE_RENDER, 0);
	m_pKernelData->pBackgroundCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	m_pKernelData->pBackgroundCam->attach(osg::Camera::COLOR_BUFFER, m_pBackgroundTex.get());
	m_pKernelData->pBackgroundCam->setAllowEventFocus(false);
	m_pKernelData->pBackgroundCam->setProjectionMatrixAsPerspective(
		m_pConfigData->fFovy,
		double(m_pConfigData->iScreenWidth) / double(m_pConfigData->iScreenHeight),
		2.0, 2e4);
	m_pKernelData->pBackgroundCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	GM_Root->addChild(m_pKernelData->pBackgroundCam.get());
}

void CGMEngine::_InitForeground()
{
	m_pForegroundTex = new osg::Texture2D();
	m_pForegroundTex->setTextureSize(m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pForegroundTex->setInternalFormat(GL_RGBA8);
	m_pForegroundTex->setSourceFormat(GL_RGBA);
	m_pForegroundTex->setSourceType(GL_UNSIGNED_BYTE);
	m_pForegroundTex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	m_pForegroundTex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	m_pForegroundTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	m_pForegroundTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	m_pForegroundTex->setBorderColor(osg::Vec4d(0, 0, 0, 0));
	m_pForegroundTex->setDataVariance(osg::Object::DYNAMIC);
	m_pForegroundTex->setResizeNonPowerOfTwoHint(false);
	
	m_pKernelData->pForegroundCam = new osg::Camera;
	m_pKernelData->pForegroundCam->setName("foregroundCamera");
	m_pKernelData->pForegroundCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF_INHERIT_VIEWPOINT);
	m_pKernelData->pForegroundCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pKernelData->pForegroundCam->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	m_pKernelData->pForegroundCam->setViewport(0, 0, m_pConfigData->iScreenWidth, m_pConfigData->iScreenHeight);
	m_pKernelData->pForegroundCam->setRenderOrder(osg::Camera::PRE_RENDER, 0);
	m_pKernelData->pForegroundCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	m_pKernelData->pForegroundCam->attach(osg::Camera::COLOR_BUFFER, m_pForegroundTex.get());
	m_pKernelData->pForegroundCam->setAllowEventFocus(false);
	m_pKernelData->pForegroundCam->setProjectionMatrixAsPerspective(
		m_pConfigData->fFovy,
		double(m_pConfigData->iScreenWidth) / double(m_pConfigData->iScreenHeight),
		2.0, 2e4);
	m_pKernelData->pForegroundCam->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	GM_Root->addChild(m_pKernelData->pForegroundCam.get());
}


void CGMEngine::_InnerUpdate(const float updateStep)
{

}

bool CGMEngine::_UpdateLater(const double dDeltaTime)
{
	// background camera
	osg::Vec3d vEye, vCenter, vUp;
	GM_Viewer->getCamera()->getViewMatrixAsLookAt(vEye, vCenter, vUp);
	osg::Vec3d vBackCamDir = vCenter - vEye;
	m_pKernelData->pBackgroundCam->setViewMatrixAsLookAt(osg::Vec3d(0, 0, 0), vBackCamDir, vUp);
	//前景相机和主相机有相同的姿态
	m_pKernelData->pForegroundCam->setViewMatrixAsLookAt(vEye, vCenter, vUp);
	double fFovy, fAspectRatio, fZNear, fZFar;
	GM_Viewer->getCamera()->getProjectionMatrixAsPerspective(fFovy, fAspectRatio, fZNear, fZFar);
	m_pKernelData->pBackgroundCam->setProjectionMatrixAsPerspective(fFovy, fAspectRatio, fZNear, fZFar);
	m_pKernelData->pForegroundCam->setProjectionMatrixAsPerspective(fFovy, fAspectRatio, fZNear, fZFar);

	m_pCommonUniform->UpdateLater(dDeltaTime);
	m_pPost->UpdateLater(dDeltaTime);
	m_pModel->UpdatePost(dDeltaTime);
	m_pLight->UpdatePost(dDeltaTime);
	return true;
}