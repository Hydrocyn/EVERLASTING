#pragma once

#include <QMainWindow>
#include "ui_GMMainWindow.h"

/*************************************************************************
Class
*************************************************************************/
class CGMViewWidget;

class CGMMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	CGMMainWindow(QWidget *parent = Q_NULLPTR);
	~CGMMainWindow();

	/** @brief ��ʼ�� */
	bool Init();
	/** @brief ���� */
	void Update();

	/**
	* @brief ����ȫ���л�
	* @param bFull trueΪȫ����false Ϊ��ȫ��
	*/
	void SetFullScreen(const bool bFull);
	/**
	* @brief ��ȡ����ȫ��״̬
	* @return bool		�Ƿ���ȫ��״̬
	*/
	bool GetFullScreen() const;

public slots:

	/** @brief ��С��/��ԭ */
	void _slotMinimum();
	/** @brief ���/��ԭ */
	void _slotMaximum();
	/** @brief �ر� */
	void _slotClose();
	/** @brief ����ȫ�� */
	void _slotFullScreen();

protected:
	void changeEvent(QEvent* event);
	void resizeEvent(QResizeEvent* event);
	void closeEvent(QCloseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent* event);
	/** @brief �����ϵļ����¼� */
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private slots:

	/** @brief ��ʼ��GM_Viewer */
	void _slotInitGMViewer();

private:
	Ui::GMMainWindow					ui;
	CGMViewWidget*						m_pSceneWidget = nullptr;

	bool								m_bInit = false;
	bool								m_bFull = false;			//!< �Ƿ�ȫ��
	bool								m_bPressed = false;			//!< �Ƿ������
	QPoint								m_vPos;						//!< ���ڵ�λ��
};