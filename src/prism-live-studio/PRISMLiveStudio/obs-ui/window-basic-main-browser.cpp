/******************************************************************************
    Copyright (C) 2018 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <QDir>
#include <QThread>
#include <QMessageBox>
#include "qt-wrappers.hpp"

#include <random>
#include "pls-net-url.hpp"

#ifdef BROWSER_AVAILABLE
#include <browser-panel.hpp>
#endif
#include "pls-channel-const.h"
#include "platform.hpp"

#include "liblog.h"
#include "PLSBasic.h"
#include "PLSApp.h"
#include "libutils-api.h"

extern QCef *cef;
extern QCefCookieManager *panel_cookies;
constexpr auto NID_AUT = "NID_AUT";
constexpr auto NID_SES = "NID_SES";
constexpr auto NID_INF = "nid_inf";
constexpr auto NAVERCOOKIE = "naverCookie";
namespace {
struct GlobalCookieVars {
	static QMap<QString, QCefCookieManager *> pannel_cookies_collect;
};
}
QMap<QString, QCefCookieManager *> GlobalCookieVars::pannel_cookies_collect = {};

constexpr auto COOKIE_MANAGER = "Cookie Manager Module";
static std::string GenId()
{
	std::random_device rd;
	std::mt19937_64 e2(rd());
	std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);

	uint64_t id = dist(e2);

	pls::chars<20> id_str;
	snprintf(id_str, id_str.size(), "%16llX", id);
	return std::string(id_str);
}

void CheckExistingCookieId()
{
	if (config_has_user_value(PLSApp::plsApp()->GlobalConfig(), "Panels",
				  "CookieId"))
		return;
	std::string cookieIdStr = GenId();
	PLS_INFO(COOKIE_MANAGER, "create a new CookieId = %s",
		 cookieIdStr.c_str());
	config_set_string(PLSApp::plsApp()->GlobalConfig(), "Panels",
			  "CookieId", cookieIdStr.c_str());
	config_save_safe(PLSApp::plsApp()->GlobalConfig(), "tmp", nullptr);
}

void setNaverTvCookie()
{
	QCefCookieManager *naver_cookie_mgr =
		PLSBasic::getBrowserPannelCookieMgr(NAVER_TV);
	if (naver_cookie_mgr) {
		auto url = pls_get_gpop_connection().ssl.toStdString();
		naver_cookie_mgr->DeleteCookies(url, NID_SES);
		naver_cookie_mgr->DeleteCookies(url, NID_AUT);
		naver_cookie_mgr->DeleteCookies(url, NID_INF);

		if (config_get_string(PLSApp::plsApp()->CookieConfig(),
				      NAVERCOOKIE, NID_SES)) {

			std::string ses(config_get_string(
				PLSApp::plsApp()->CookieConfig(), NAVERCOOKIE,
				NID_SES));
			std::string aut(config_get_string(
				PLSApp::plsApp()->CookieConfig(), NAVERCOOKIE,
				NID_AUT));
			std::string inf(config_get_string(
				PLSApp::plsApp()->CookieConfig(), NAVERCOOKIE,
				NID_INF));

			naver_cookie_mgr->SetCookie(url, NID_SES, ses,
						    ".naver.com", "/", false);
			naver_cookie_mgr->SetCookie(url, NID_AUT, aut,
						    ".naver.com", "/", false);
			naver_cookie_mgr->SetCookie(url, NID_INF, inf,
						    ".naver.com", "/", false);
		}
	}
}

#ifdef BROWSER_AVAILABLE
static void InitPanelCookieManager()
{
	if (!cef)
		return;
	if (panel_cookies)
		return;

	CheckExistingCookieId();

	const char *cookie_id = config_get_string(
		PLSApp::plsApp()->GlobalConfig(), "Panels", "CookieId");

	std::string sub_path;
	sub_path += "prism_profile_cookies/";
	sub_path += cookie_id;

	panel_cookies = cef->create_cookie_manager(sub_path);
}

static void InitPanelCookieManager(const QString &pannelName)
{
	if (!cef)
		return;

	CheckExistingCookieId();

	const char *cookie_id = config_get_string(
		PLSApp::plsApp()->GlobalConfig(), "Panels", "CookieId");

	std::string sub_path;
	sub_path += "prism_profile_cookies/";
	sub_path += cookie_id;
	sub_path += pannelName.toUtf8().data();

	QCefCookieManager *cookieMgr = cef->create_cookie_manager(sub_path);
	GlobalCookieVars::pannel_cookies_collect.insert(pannelName, cookieMgr);
	if (0 == pannelName.compare(NAVER_TV, Qt::CaseInsensitive)) {
		setNaverTvCookie();
	}
	PLS_INFO(COOKIE_MANAGER, "create new cookie mangaer ; save path = %s",
		 pls_get_path_file_name(sub_path.c_str()));
}
#endif

void DestroyPanelCookieManager()
{
#ifdef BROWSER_AVAILABLE
	if (panel_cookies) {
		panel_cookies->FlushStore();
		pls_delete(panel_cookies, nullptr);
	}
#endif
}

void DeleteCookies()
{
#ifdef BROWSER_AVAILABLE
	if (panel_cookies) {
		panel_cookies->DeleteCookies("", "");
	}
#endif
}

void PLSBasic::InitBrowserPanelSafeBlock()
{
#ifdef BROWSER_AVAILABLE
	if (!cef)
		return;
	if (cef->init_browser()) {
		InitPanelCookieManager();
		return;
	}

	ExecThreadedWithoutBlocking([] { cef->wait_for_browser_init(); },
				    QTStr("BrowserPanelInit.Title"),
				    QTStr("BrowserPanelInit.Text"));
	InitPanelCookieManager();
#endif
}

QCefCookieManager *
PLSBasic::getBrowserPannelCookieMgr(const QString &pannelName)
{
#ifdef BROWSER_AVAILABLE

	if (!cef)
		return nullptr;

	if (GlobalCookieVars::pannel_cookies_collect.find(pannelName) !=
	    GlobalCookieVars::pannel_cookies_collect.end()) {
		PLS_INFO(
			COOKIE_MANAGER,
			"the cookie manager has been exist, the pannel name is %s",
			pannelName.toUtf8().data());
		return GlobalCookieVars::pannel_cookies_collect.value(
			pannelName);
	} else {
		if (!cef->init_browser()) {
			ExecThreadedWithoutBlocking(
				[] { cef->wait_for_browser_init(); },
				QTStr("BrowserPanelInit.Title"),
				QTStr("BrowserPanelInit.Text"));
		}
		InitPanelCookieManager(pannelName);
		PLS_INFO(
			COOKIE_MANAGER,
			"the cookie manager is new create, the pannel name is %s",
			pannelName.toUtf8().data());

		return GlobalCookieVars::pannel_cookies_collect.value(
			pannelName);
	}
#endif
}

void PLSBasic::delPannelCookies(const QString &pannelName)
{
#ifdef BROWSER_AVAILABLE
	PLS_INFO(COOKIE_MANAGER,
		 "delete pannel cookie: paltform/channel name is %s ",
		 pannelName.toUtf8().data());
	QCefCookieManager *pannel_cookie =
		getBrowserPannelCookieMgr(pannelName);
	if (pannel_cookie) {
		pannel_cookie->DeleteCookies("", "");
	}

#endif
}

void PLSBasic::setManualPannelCookies(const QString &pannelName)
{
	if (pannelName == NAVER_TV) {
		setNaverTvCookie();
	}
}

QString PLSBasic::cookiePath(const QString &pannelName)
{
	const char *cookie_id = config_get_string(
		PLSApp::plsApp()->GlobalConfig(), "Panels", "CookieId");

	std::string sub_path;
	sub_path += "prism_profile_cookies/";
	sub_path += cookie_id;
	sub_path += pannelName.toUtf8().data();
	return sub_path.c_str();
}

void OBSBasic::InitBrowserPanelSafeBlock()
{
#ifdef BROWSER_AVAILABLE
	if (!cef)
		return;
	if (cef->init_browser()) {
		InitPanelCookieManager();
		return;
	}

	ExecThreadedWithoutBlocking([] { cef->wait_for_browser_init(); },
				    QTStr("BrowserPanelInit.Title"),
				    QTStr("BrowserPanelInit.Text"));
	InitPanelCookieManager();
#endif
}
void DuplicateCurrentCookieProfile(ConfigFile &config)
{
#ifdef BROWSER_AVAILABLE
	if (cef) {
		OBSBasic *main = OBSBasic::Get();
		std::string cookie_id = config_get_string(
			PLSApp::plsApp()->GlobalConfig(), "Panels", "CookieId");

		std::string src_path;
		src_path += "obs_profile_cookies/";
		src_path += cookie_id;

		std::string new_id = GenId();

		std::string dst_path;
		dst_path += "obs_profile_cookies/";
		dst_path += new_id;

		BPtr<char> src_path_full = cef->get_cookie_path(src_path);
		BPtr<char> dst_path_full = cef->get_cookie_path(dst_path);

		QDir srcDir(src_path_full.Get());
		QDir dstDir(dst_path_full.Get());

		if (srcDir.exists()) {
			if (!dstDir.exists())
				dstDir.mkdir(dst_path_full.Get());

			QStringList files = srcDir.entryList(QDir::Files);
			for (const QString &file : files) {
				QString src = QString(src_path_full);
				QString dst = QString(dst_path_full);
				src += QDir::separator() + file;
				dst += QDir::separator() + file;
				QFile::copy(src, dst);
			}
		}

		config_set_string(config, "Panels", "CookieId",
				  cookie_id.c_str());
		config_set_string(PLSApp::plsApp()->GlobalConfig(), "Panels",
				  "CookieId", new_id.c_str());
	}
#else
	UNUSED_PARAMETER(config);
#endif
}
