#include "PLSSceneTemplateMainSceneItem.h"
#include "ui_PLSSceneTemplateMainSceneItem.h"
#include "PLSSceneTemplateMediaManage.h"
#include "log/log.h"
#include <chrono>

using namespace std;

PLSSceneTemplateMainSceneItem::PLSSceneTemplateMainSceneItem(QWidget *parent) : QWidget(parent), ui(new Ui::PLSSceneTemplateMainSceneItem)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_StyledBackground);
	ui->mainSceneInstallView->setVisible(false);
	ui->mainSceneTopVideoView->setVisible(false);
	QObject::connect(&m_checkMouseLeaveTimer, &QTimer::timeout, this, [this] { checkMouseLeaveEvent(); });
	m_performMouseEnterTimer.setSingleShot(true);
	QObject::connect(&m_performMouseEnterTimer, &QTimer::timeout, this, [this] { performMouseEnterEvent(); });
	connect(ui->mainSceneTopVideoView, &PLSMediaRender::clicked, this, [this] { PLS_SCENE_TEMPLATE_MEDIA_MANAGE->enterDetailScenePage(m_item); });
	connect(ui->mainSceneTopImageView, &PLSSceneTemplateImageView::clicked, this, [this] { PLS_SCENE_TEMPLATE_MEDIA_MANAGE->enterDetailScenePage(m_item); });
	connect(ui->mainSceneLeftImageView, &PLSSceneTemplateImageView::clicked, this, [this] { PLS_SCENE_TEMPLATE_MEDIA_MANAGE->enterDetailScenePage(m_item); });
	connect(ui->mainSceneRightImageView, &PLSSceneTemplateImageView::clicked, this, [this] { PLS_SCENE_TEMPLATE_MEDIA_MANAGE->enterDetailScenePage(m_item); });
}

PLSSceneTemplateMainSceneItem::~PLSSceneTemplateMainSceneItem()
{
	if (m_checkMouseLeaveTimer.isActive()) {
		m_checkMouseLeaveTimer.stop();
	}
	if (m_performMouseEnterTimer.isActive()) {
		m_performMouseEnterTimer.stop();
	}
	delete ui;
}

void PLSSceneTemplateMainSceneItem::updateUI(const SceneTemplateItem &model)
{
	auto dtStart = chrono::steady_clock::now();

	m_item = model;
	ui->mainSceneIntroView->updateUI(model);
	ui->mainSceneTopImageView->updateImagePath(m_item.resource.MainSceneImagePath);
	ui->mainSceneLeftImageView->updateImagePath(m_item.resource.MainSceneThumbnail_1);
	ui->mainSceneRightImageView->updateImagePath(m_item.resource.MainSceneThumbnail_2);
	ui->mainSceneInstallView->updateUI(model);

	auto dtEnd = chrono::steady_clock::now();
	PLS_INFO(SCENE_TEMPLATE, "%s: duration=%lld", __FUNCTION__, chrono::duration_cast<chrono::milliseconds>(dtEnd - dtStart).count());
}

void PLSSceneTemplateMainSceneItem::enterEvent(QEnterEvent *event)
{
	checkMouseEnterEvent();
}

void PLSSceneTemplateMainSceneItem::leaveEvent(QEvent *event)
{
	checkMouseLeaveEvent();
}

void PLSSceneTemplateMainSceneItem::checkMouseEnterEvent()
{
	if (rect().contains(this->mapFromGlobal(QCursor::pos())) && !m_hoverEnter) {
		m_hoverEnter = true;
		m_checkMouseLeaveTimer.stop();
		m_performMouseEnterTimer.stop();
		m_checkMouseLeaveTimer.start(100);
		m_performMouseEnterTimer.start(300);
	}
}

void PLSSceneTemplateMainSceneItem::checkMouseLeaveEvent()
{
	if (!rect().contains(this->mapFromGlobal(QCursor::pos())) && m_hoverEnter) {
		m_hoverEnter = false;
		m_checkMouseLeaveTimer.stop();
		m_performMouseEnterTimer.stop();
		performMouseLeaveEvent();
	}
}

void PLSSceneTemplateMainSceneItem::performMouseEnterEvent()
{
	if (m_item.resource.MainSceneVideoPath.length() > 0) {
		ui->mainSceneInstallView->setVisible(true);
		ui->mainSceneIntroView->setVisible(false);
		ui->mainSceneTopImageView->setVisible(false);
		ui->mainSceneTopVideoView->setVisible(true);
		PLS_SCENE_TEMPLATE_MEDIA_MANAGE->startPlayVideo(m_item.resource.MainSceneVideoPath, ui->mainSceneTopVideoView);
	}
}

void PLSSceneTemplateMainSceneItem::performMouseLeaveEvent()
{
	if (m_item.resource.MainSceneVideoPath.length() > 0) {
		ui->mainSceneIntroView->setVisible(true);
		ui->mainSceneInstallView->setVisible(false);
		ui->mainSceneTopImageView->setVisible(true);
		ui->mainSceneTopVideoView->setVisible(false);
		PLS_SCENE_TEMPLATE_MEDIA_MANAGE->stopPlayVideo(ui->mainSceneTopVideoView);
	}
}
