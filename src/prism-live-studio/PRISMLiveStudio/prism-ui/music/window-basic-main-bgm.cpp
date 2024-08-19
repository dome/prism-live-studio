#include "PLSBackgroundMusicView.h"
#include "PLSSceneDataMgr.h"
#include "PLSBasic.h"
#include "window-basic-main.hpp"
#include "pls-common-define.hpp"
#include "pls/pls-obs-api.h"
#include "liblog.h"
#include "log/module_names.h"
#include "PLSVirtualBgManager.h"
#include "PLSAction.h"
#include "pls/pls-source.h"
using namespace common;
using namespace action;
Q_DECLARE_METATYPE(BgmSourceVecType);

QByteArray buildPlatformViewerCount();

void PLSBasic::OnBgmClicked()
{
	if (!backgroundMusicView) {
		createBackgroundMusicView();
		pls_async_call(this, [this]() { backgroundMusicView->show(); });
		return;
	}

	SetBgmViewVisible(!backgroundMusicView->isVisible());
}

void PLSBasic::createBackgroundMusicView()
{
	if (!backgroundMusicView) {
		DialogInfo info;
		info.configId = ConfigId::BgmConfig;
		info.defaultHeight = 817;
		info.defaultWidth = 298;
		backgroundMusicView = pls_new<PLSBackgroundMusicView>(info);
		connect(
			backgroundMusicView, &PLSBackgroundMusicView::RefreshSourceProperty, this,
			[this](const QString &name, bool) {
				if (!properties) {
					return;
				}

				if (name == obs_source_get_name(properties->GetSource())) {
					properties->ReloadProperties();
				}
			},
			Qt::QueuedConnection);
	}
}

void PLSBasic::SetBgmViewVisible(bool visible)
{
	if (backgroundMusicView) {
		backgroundMusicView->setVisible(visible);
		if (visible) {
			backgroundMusicView->UpdateSourceSelectUI();
			backgroundMusicView->raise();
		}
	}
}

void PLSBasic::OnSetBgmViewVisible(bool state)
{
	if (backgroundMusicView) {
		SetBgmViewVisible(state);
	} else if (state) {
		OnBgmClicked();
		SetBgmViewVisible(state);
	}
}

void PLSBasic::ReorderBgmSourceList()
{
	if (!backgroundMusicView) {
		return;
	}

	backgroundMusicView->OnSceneChanged();
}

bool PLSBasic::GetBgmViewVisible() const
{
	if (!backgroundMusicView) {
		return false;
	}

	return backgroundMusicView->isVisible();
}

static bool enumBgmItem(obs_scene_t *, obs_sceneitem_t *item, void *param)
{
	BgmSourceVecType &items = *static_cast<BgmSourceVecType *>(param);
	if (obs_sceneitem_is_group(item)) {
		obs_scene_t *scene = obs_sceneitem_group_get_scene(item);
		obs_scene_enum_items(scene, enumBgmItem, &items);
	}

	const obs_source_t *source = obs_sceneitem_get_source(item);
	if (!source) {
		return true;
	}
	const char *id = obs_source_get_id(source);
	if (!id) {
		return true;
	}

	if (0 != strcmp(id, BGM_SOURCE_ID)) {
		return true;
	}

	const char *name = obs_source_get_name(source);
	items.emplace(items.begin(), BgmSourceVecType::value_type(name, quint64(item)));

	return true;
}

BgmSourceVecType PLSBasic::GetCurrentSceneBgmSourceList()
{
	BgmSourceVecType sourceList{};
	OBSScene scene = GetCurrentScene();
	obs_scene_enum_items(scene, enumBgmItem, &sourceList);
	return sourceList;
}

bool PLSBasic::isBgmSourceBySourceId(const obs_scene_item *item) const
{
	if (!item) {
		return false;
	}

	OBSSource source = obs_sceneitem_get_source(item);
	if (!source) {
		return false;
	}

	const char *id = obs_source_get_id(source);
	if (id && 0 == strcmp(id, BGM_SOURCE_ID)) {
		return true;
	}

	return false;
}

void PLSBasic::AddBgmItem(obs_scene_item *item)
{
	if (!backgroundMusicView) {
		return;
	}

	if (isBgmSourceBySourceId(item)) {
		const obs_source_t *source = obs_sceneitem_get_source(item);
		if (!source) {
			return;
		}

		QMetaObject::invokeMethod(backgroundMusicView, "AddSourceAndRefresh", Qt::QueuedConnection, Q_ARG(const QString &, obs_source_get_name(source)), Q_ARG(quint64, (quint64)item));

	} else if (obs_sceneitem_is_group(item)) {
		if (!backgroundMusicView) {
			return;
		}

		QMetaObject::invokeMethod(backgroundMusicView, "OnSceneChanged", Qt::QueuedConnection);
	}
}

void PLSBasic::RemoveBgmItem(obs_scene_item *item)
{
	if (!backgroundMusicView) {
		return;
	}

	auto removeBgmItem = [=](obs_sceneitem_t *item_) {
		if (!isBgmSourceBySourceId(item_)) {
			return;
		}
		const obs_source_t *source = obs_sceneitem_get_source(item_);
		if (!source) {
			return;
		}

		QMetaObject::invokeMethod(backgroundMusicView, "RemoveSourceAndRefresh", Qt::QueuedConnection, Q_ARG(const QString &, obs_source_get_name(source)), Q_ARG(quint64, (quint64)item_));
	};

	if (obs_sceneitem_is_group(item)) {
		BgmSourceVecType bgmList{};
		obs_sceneitem_group_enum_items(item, enumBgmItem, &bgmList);
		for (auto iter : bgmList) {
			removeBgmItem((obs_sceneitem_t *)iter.second);
		}
	} else {
		removeBgmItem(item);
	}
}

void PLSBasic::RemoveBgmList(obs_scene_t *scene)
{
	if (!scene) {
		return;
	}

	if (!backgroundMusicView) {
		return;
	}

	BgmSourceVecType bgmList;
	obs_scene_enum_items(scene, enumBgmItem, &bgmList);
	QMetaObject::invokeMethod(backgroundMusicView, "RemoveBgmSourceList", Qt::QueuedConnection, Q_ARG(const BgmSourceVecType &, bgmList));
}

obs_sceneitem_t *PLSBasic::GetCurrentSceneItemBySourceName(const QString &name)
{
	BgmSourceVecType sourceList;
	obs_scene_enum_items(GetCurrentScene(), enumBgmItem, &sourceList);

	for (const auto &source : sourceList) {
		if (source.first == name) {
			return (obs_sceneitem_t *)source.second;
		}
		continue;
	}

	return nullptr;
}

obs_sceneitem_t *PLSBasic::GetSceneItemBySourceName(const QString &name)
{
	SceneDisplayVector scenes = PLSSceneDataMgr::Instance()->GetDisplayVector();
	for (auto scene : scenes) {
		const PLSSceneItemView *sceneItem = scene.second;
		if (!sceneItem) {
			continue;
		}
		BgmSourceVecType sourceList;
		obs_scene_enum_items(sceneItem->GetData(), enumBgmItem, &sourceList);

		for (const auto &source : sourceList) {
			if (source.first == name) {
				return (obs_sceneitem_t *)source.second;
			}
			continue;
		}
	}
	return nullptr;
}

bool PLSBasic::GetSelectBgmSourceName(QString &selectSourceName, quint64 &item) const
{
	for (int i = 0; i < ui->sources->Count(); i++) {
		obs_sceneitem_t *sceneItem = ui->sources->Get(i);
		const obs_source_t *source = obs_sceneitem_get_source(sceneItem);
		if (!source) {
			continue;
		}

		QString sourceId = obs_source_get_id(source);
		if (0 == sourceId.compare(BGM_SOURCE_ID)) {
			if (!obs_sceneitem_selected(sceneItem) || !obs_sceneitem_visible(sceneItem)) {
				continue;
			}
			selectSourceName = obs_source_get_name(source);
			item = (quint64)sceneItem;
			return true;
		}
	}
	return false;
}

void PLSBasic::SetBgmItemSelected(obs_scene_item *item, bool isSelected_)
{
	if (!item || !backgroundMusicView || !isBgmSourceBySourceId(item)) {
		return;
	}

	const obs_source_t *itemSource = obs_sceneitem_get_source(item);
	if (!itemSource) {
		return;
	}

	// select top item when ctrl+A
	BgmSourceVecType sourceList;
	OBSScene scene = GetCurrentScene();
	obs_scene_enum_items(scene, enumBgmItem, &sourceList);

	obs_sceneitem_t *topSelectItem{};
	for (auto const &source : sourceList) {
		auto sceneitem = (obs_sceneitem_t *)source.second;
		if (obs_sceneitem_selected(sceneitem)) {
			topSelectItem = sceneitem;
			break;
		}
	}

	const obs_source_t *topSource = obs_sceneitem_get_source(topSelectItem);
	if (!isSelected_ && !topSource) {
		return;
	}
	if (!topSource || topSource == itemSource) {
		QMetaObject::invokeMethod(backgroundMusicView, "SetSourceSelect", Qt::QueuedConnection, Q_ARG(const QString &, obs_source_get_name(itemSource)), Q_ARG(quint64, (quint64)item),
					  Q_ARG(bool, isSelected_));
		return;
	} else {
		itemSource = topSource;
		item = topSelectItem;
	}

	QMetaObject::invokeMethod(backgroundMusicView, "SetSourceSelect", Qt::QueuedConnection, Q_ARG(const QString &, obs_source_get_name(itemSource)), Q_ARG(quint64, (quint64)item),
				  Q_ARG(bool, isSelected_));
}

void PLSBasic::SetBgmItemVisible(obs_scene_item *item, bool isVisible)
{
	if (!backgroundMusicView) {
		return;
	}

	if (!isBgmSourceBySourceId(item)) {
		return;
	}

	const obs_source_t *itemSource = obs_sceneitem_get_source(item);
	if (!itemSource) {
		return;
	}

	QMetaObject::invokeMethod(backgroundMusicView, "SetSourceVisible", Qt::QueuedConnection, Q_ARG(const QString &, obs_source_get_name(itemSource)), Q_ARG(quint64, (quint64)item),
				  Q_ARG(bool, isVisible));
}

void PLSBasic::RenameBgmSourceName(obs_sceneitem_t *sceneItem, const QString &newName, const QString &prevName)
{
	if (!backgroundMusicView || !sceneItem) {
		return;
	}

	backgroundMusicView->RenameSourceName((qint64)sceneItem, newName, prevName);
}

BgmSourceVecType PLSBasic::EnumAllBgmSource() const
{
	BgmSourceVecType sourceList;
	SceneDisplayVector scenes = PLSSceneDataMgr::Instance()->GetDisplayVector();
	for (auto scene : scenes) {
		const PLSSceneItemView *sceneItem = scene.second;
		if (!sceneItem) {
			continue;
		}
		obs_scene_enum_items(sceneItem->GetData(), enumBgmItem, &sourceList);
	}
	return sourceList;
}

void PLSBasic::ClearMusicResource()
{
	if (backgroundMusicView) {
		backgroundMusicView->ClearUrlInfo();
	}
}

void PLSBasic::PropertiesChanged(void *, calldata_t *calldata)
{
	auto basic = static_cast<PLSBasic *>(App()->GetMainWindow());
	auto source = (obs_source_t *)calldata_ptr(calldata, "source");
	QString name = obs_source_get_name(source);

	if (basic && basic->backgroundMusicView) {
		QMetaObject::invokeMethod(basic->backgroundMusicView, "OnPropertiesChanged", Qt::QueuedConnection, Q_ARG(const QString &, name));
	}
}
