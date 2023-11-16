//#include <Windows.h>

#include "PLSPlatformNaverTV.h"

#include <ctime>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <qurlquery.h>

#include "pls-net-url.hpp"
#include "pls-common-define.hpp"

#include "PLSChannelDataAPI.h"
#include "ChannelCommonFunctions.h"

#include "../common/PLSDateFormate.h"
#include "../PLSLiveInfoDialogs.h"
#include "../PLSPlatformApi.h"

#include "../PLSScheLiveNotice.h"

#include "PLSLiveInfoNaverTV.h"

#include "prism/PLSPlatformPrism.h"
#include "liblog.h"
#include "libutils-api.h"
#include "libhttp-client.h"
#include "PLSChannelsVirualAPI.h"
#include "PLSApp.h"
using namespace common;

#define SUB_CHANNEL_NAME getSubChannelName().toUtf8().constData()
#define SUB_CHANNEL_NAME_MASK pls_masking_person_info(getSubChannelName()).toUtf8().constData()

constexpr const char *CSTR_OBJECT_URL = "http://tv.naver.com";

constexpr const char *CSTR_CODE = "code";
constexpr const char *CSTR_MESSAGE = "message";
constexpr const char *CSTR_ERROR_CODE = "errorCode";
constexpr const char *CSTR_ERROR_MESSAGE = "errorMessage";
constexpr const char *CSTR_RTN_CD = "rtn_cd";
constexpr const char *CSTR_RTN_MSG = "rtn_msg";
constexpr const char *CSTR_HEADER = "header";

constexpr const char *CSTR_NAME = "name";
constexpr const char *CSTR_OBJECTID = "objectId";
constexpr const char *CSTR_GROUPID = "groupId";
constexpr const char *CSTR_LANG = "lang";
constexpr const char *CSTR_TICKET = "ticket";
constexpr const char *CSTR_TEMPLATEID = "templateId";
constexpr const char *CSTR_USERTYPE = "userType";
constexpr const char *CSTR_URI = "URI";
constexpr const char *CSTR_GLOBAL_URI = "GLOBAL_URI";
constexpr const char *CSTR_HMAC_SALT = "HMAC_SALT";
constexpr const char *CSTR_NID_SES = "NID_SES";
constexpr const char *CSTR_NID_INF = "nid_inf";
constexpr const char *CSTR_NID_AUT = "NID_AUT";
constexpr const char *CSTR_LIVENO = "liveNo";
constexpr const char *CSTR_OLIVEID = "oliveId";
constexpr const char *CSTR_STARTDATE = "startDate";
constexpr const char *CSTR_ENDDATE = "endDate";
constexpr const char *CSTR_TITLE = "title";
constexpr const char *CSTR_STATUS = "status";
constexpr const char *CSTR_CHANNELID = "channelId";
constexpr const char *CSTR_CATEGORYCODE = "categoryCode";
constexpr const char *CSTR_OPENYN = "openYn";
constexpr const char *CSTR_NOTICE = "notice";
constexpr const char *CSTR_THUMBNAILIMAGEURL = "thumbnailImageUrl";
constexpr const char *CSTR_CHANNELNAME = "channelName";
constexpr const char *CSTR_CHANNELEMBLEM = "channelEmblem";
constexpr const char *CSTR_STREAMKEY = "streamKey";
constexpr const char *CSTR_SERVERURL = "serverUrl";
constexpr const char *CSTR_PUBLISHING = "publishing";
constexpr const char *CSTR_REHEARSALYN = "rehearsalYn";
constexpr const char *CSTR_TICKETID = "ticketId";
constexpr const char *CSTR_LIVECOMMENTOPTIONS = "liveCommentOptions";
constexpr const char *CSTR_IDNO = "idNo";
constexpr const char *CSTR_MANAGER = "manager";
constexpr const char *CSTR_OLIVEMODEL = "oliveModel";
constexpr const char *CSTR_ID = "id";
constexpr const char *CSTR_LINK = "link";
constexpr const char *CSTR_PCLIVEURL = "pcLiveUrl";
constexpr const char *CSTR_MOBILELIVEURL = "mobileLiveUrl";
constexpr const char *CSTR_CURRENTSTAT = "currentStat";
constexpr const char *CSTR_PV = "pv";
constexpr const char *CSTR_THUMBNAIL = "thumbnail";
constexpr const char *CSTR_LIKECOUNT = "likeCount";
constexpr const char *CSTR_COMMENTCOUNT = "commentCount";
constexpr const char *CSTR_MANAGER_U = "MANAGER";
constexpr const char *CSTR_OBJECTURL = "objectUrl";
constexpr const char *CSTR_USERID = "userId";
constexpr const char *CSTR_USERNAME = "username";
constexpr const char *CSTR_REFERER = "Referer";
constexpr const char *CSTR_CLIENT_ID = "client_id";
constexpr const char *CSTR_CLIENT_ID_VALUE = "jZNIoee3IBi6sujuw4w0";
constexpr const char *CSTR_CLIENT_SECRET = "client_secret";
constexpr const char *CSTR_CLIENT_SECRET_VALUE = "8nJdGLYNMb";
constexpr const char *CSTR_GRANT_TYPE = "grant_type";
constexpr const char *CSTR_GRANT_TYPE_AUTHORIZATION_CODE = "authorization_code";
constexpr const char *CSTR_RESPONSE_TYPE = "response_type";
constexpr const char *CSTR_REDIRECT_URL = "redirect_url";
constexpr const char *CSTR_ERROR = "error";
constexpr const char *CSTR_ERROR_DESCRIPTION = "error_description";
constexpr const char *CSTR_LOCATION_REPLACE = "location.replace";
constexpr const char *CSTR_CODE_E = "code=";
constexpr const char *CSTR_IS_REHEARSAL = "isRehearsal";
constexpr const char *CSTR_DISABLE = "disable";
constexpr const char *CSTR_LIVE_ID = "liveId";
constexpr const char *CSTR_SCHEDULED = "scheduled";
constexpr const char *CSTR_SIMULCAST_CHANNEL = "simulcastChannel";
constexpr const char *CSTR_NAVERTV_TOKEN = "NaverTVToken";
constexpr const char *CSTR_ACCESS_TOKEN = "access_token";
constexpr const char *CSTR_REFRESH_TOKEN = "refresh_token";
constexpr const char *CSTR_TOKEN_TYPE = "token_type";
constexpr const char *CSTR_EXPIRES_IN = "expires_in";

static const QString LANGUAGE = "ko";

static const QString DEFAULT_NICK_NAME = "No Name";
static const QString KEY_NICK_NAME = "nick_name";
static const QString KEY_IMAGE_URL = "image_url";
static const QString IMAGE_FILE_NAME_PREFIX = "navertv-";

static const int CHECK_PUBLISHING_MAX_TIMES = 15;
static const int CHECK_PUBLISHING_TIMESPAN = 1000;
static const int CHECK_STATUS_TIMESPAN = 5000;

namespace {
QString getValue(const QJsonObject &object, const QString &name, const QString &defaultValue = QString())
{
	QString value = object[name].toString();
	if (!value.isEmpty()) {
		return value;
	} else {
		return defaultValue;
	}
}
void deleteTimer(QTimer *&timer)
{
	if (timer) {
		timer->stop();
		pls_delete(timer, nullptr);
	}
}
template<typename T> void deleteObject(T *&object)
{
	if (object) {
		pls_delete(object, nullptr);
	}
}
template<typename Callback> QTimer *startTimer(PLSPlatformNaverTV *platform, QTimer *&timer, int timeout, Callback callback, bool singleShot = true)
{
	deleteTimer(timer);
	timer = pls_new<QTimer>(platform);
	timer->setSingleShot(singleShot);
	QObject::connect(timer, &QTimer::timeout, platform, callback);
	timer->start(timeout);
	return timer;
}
QMap<QString, QString> parseCookie(const QString &cookie)
{
	QMap<QString, QString> map;
	QList<QString> list = cookie.split('\n');
	for (const auto &c : list) {
		int i = c.indexOf('=');
		if (i <= 0) {
			continue;
		}

		QString key = c.left(i);
		QString value = c.mid(i + 1);
		if (!key.isEmpty() && !value.isEmpty()) {
			map.insert(key, value);
		}
	}
	return map;
}
bool titleIsEmtpy(const QString &title)
{
	if (!title.isEmpty()) {
		return std::all_of(title.begin(), title.end(), [](const QChar &ch) { return ch.isSpace(); });
	}
	return true;
}
QString processThumbnailImageUrl(const QString &thumbnailImageUrl)
{
	return thumbnailImageUrl.isEmpty() ? "null" : thumbnailImageUrl;
}
bool findLocationUrl(QString &url, const QByteArray &body)
{
	int index = body.indexOf(CSTR_LOCATION_REPLACE);
	if (index < 0) {
		return false;
	}

	int start = index + int(strlen(CSTR_LOCATION_REPLACE)) + 2;
	int end = start;
	for (; end < body.length(); ++end) {
		if (body[end] == '\"' || body[end] == '\'') {
			break;
		}
	}
	url = QString::fromUtf8(body.mid(start, end - start));
	return true;
}
QString getUrlCode(const QString &url)
{
	QStringList items = QUrl(url).query(QUrl::FullyDecoded).split('&');
	for (const auto &item : items) {
		QStringList param = item.split('=');
		if (param.length() == 2 && param.at(0) == CSTR_CODE) {
			return param.at(1);
		}
	}
	return QString();
}
void loadToken(PLSPlatformNaverTV::Token &token)
{
	auto config = PLSApp::plsApp()->CookieConfig();
	token.accessToken = QString::fromUtf8(config_get_string(config, CSTR_NAVERTV_TOKEN, CSTR_ACCESS_TOKEN));
	token.refreshToken = QString::fromUtf8(config_get_string(config, CSTR_NAVERTV_TOKEN, CSTR_REFRESH_TOKEN));
	token.tokenType = QString::fromUtf8(config_get_string(config, CSTR_NAVERTV_TOKEN, CSTR_TOKEN_TYPE));
	token.expiresIn = config_get_int(config, CSTR_NAVERTV_TOKEN, CSTR_EXPIRES_IN);
}
void saveToken(const PLSPlatformNaverTV::Token &token)
{
	auto config = PLSApp::plsApp()->CookieConfig();
	config_set_string(config, CSTR_NAVERTV_TOKEN, CSTR_ACCESS_TOKEN, token.accessToken.toUtf8().constData());
	config_set_string(config, CSTR_NAVERTV_TOKEN, CSTR_REFRESH_TOKEN, token.refreshToken.toUtf8().constData());
	config_set_string(config, CSTR_NAVERTV_TOKEN, CSTR_TOKEN_TYPE, token.tokenType.toUtf8().constData());
	config_set_int(config, CSTR_NAVERTV_TOKEN, CSTR_EXPIRES_IN, token.expiresIn);
	config_save(config);
}
void clearToken()
{
	auto config = PLSApp::plsApp()->CookieConfig();
	config_remove_value(config, CSTR_NAVERTV_TOKEN, CSTR_ACCESS_TOKEN);
	config_remove_value(config, CSTR_NAVERTV_TOKEN, CSTR_REFRESH_TOKEN);
	config_remove_value(config, CSTR_NAVERTV_TOKEN, CSTR_TOKEN_TYPE);
	config_remove_value(config, CSTR_NAVERTV_TOKEN, CSTR_EXPIRES_IN);
	config_save(config);
}
}

PLSPlatformNaverTV::Token::Token(const QJsonObject &object)
{
	accessToken = object[CSTR_ACCESS_TOKEN].toString();
	refreshToken = object[CSTR_REFRESH_TOKEN].toString();
	tokenType = object[CSTR_TOKEN_TYPE].toString();
	expiresIn = object["expires_in"].toString().toLongLong();
}

PLSPlatformNaverTV::LiveInfo::LiveInfo(const QJsonObject &object) : isScheLive(true)
{
	liveNo = object[CSTR_LIVENO].toInt();
	oliveId = object[CSTR_OLIVEID].toInt();
	startDate = (qint64)object[CSTR_STARTDATE].toDouble();
	endDate = (qint64)object[CSTR_ENDDATE].toDouble();
	title = object[CSTR_TITLE].toString();
	status = object[CSTR_STATUS].toString();
	channelId = object[CSTR_CHANNELID].toString();
	categoryCode = object[CSTR_CATEGORYCODE].toString();
	openYn = object[CSTR_OPENYN].toString();
	notice = object[CSTR_NOTICE].toString();
	thumbnailImageUrl = object[CSTR_THUMBNAILIMAGEURL].toString();
	if (!thumbnailImageUrl.isEmpty() && thumbnailImageUrl.contains(QLatin1String("?type="))) {
		thumbnailImageUrl += "?type=f880_495";
	}
}

bool PLSPlatformNaverTV::LiveInfo::needUploadThumbnail() const
{
	return !thumbnailImagePath.isEmpty() && thumbnailImageUrl.isEmpty();
}

PLSServiceType PLSPlatformNaverTV::getServiceType() const
{
	return PLSServiceType::ST_NAVERTV;
}
void PLSPlatformNaverTV::onPrepareLive(bool value)
{
	clearLive();

	if (!value) {
		PLSPlatformBase::onPrepareLive(value);
		return;
	}

	PLS_INFO(MODULE_PLATFORM_NAVERTV, "%s %s show liveinfo value(%s)", PrepareInfoPrefix, __FUNCTION__, BOOL2STR(value));
	value = pls_exec_live_Info_navertv(this) == QDialog::Accepted;
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "%s %s liveinfo closed value(%s)", PrepareInfoPrefix, __FUNCTION__, BOOL2STR(value));
	PLSPlatformBase::onPrepareLive(value);

	if (!value) {
		return;
	}
	auto latInfo = PLSCHANNELS_API->getChannelInfo(getChannelUUID());
	if (isRehearsal()) {
		latInfo.remove(ChannelData::g_viewers);
	} else {
		latInfo.insert(ChannelData::g_viewers, "0");
	}
	PLSCHANNELS_API->setChannelInfos(latInfo, true);

	// notify golive button change status
	if (value && isRehearsal()) {
		PLSCHANNELS_API->rehearsalBegin();
	}
}
void PLSPlatformNaverTV::onAllPrepareLive(bool value)
{
	if (!value && (getLiveId() >= 0) && !isRehearsal() && !getLiveInfo()->isScheLive) {
		PLSCHANNELS_API->setValueOfChannel(getChannelUUID(), ChannelData::g_shareUrl, QString());

		// cancel or fail
		// immediate live
		liveClose([this, value](bool) { PLSPlatformBase::onAllPrepareLive(value); });
	} else {
		PLSPlatformBase::onAllPrepareLive(value);
	}
}
void PLSPlatformNaverTV::onLiveStarted()
{
	if (PLSCHANNELS_API->currentSelectedCount() > 1) {
		// multiple platform
		if (!isRehearsal() && getLiveInfo()->isScheLive) {
			// scheduled live
			scheduleStart([this](bool ok, int) {
				if (!ok) {
					clearLive();
					PLSCHANNELS_API->setChannelStatus(getChannelUUID(), ChannelData::ChannelStatus::Error);
				} else {
					PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV,
						  {
							  {"platformName", NAVER_TV},
							  {"startLiveStatus", "Success"},
						  },
						  "Naver TV start live success");
				}
				liveStartedCallback(ok);
			});
		} else {
			liveStartedCallback(true);
		}
	} else {
		// single platform
		if (!isRehearsal() && getLiveInfo()->isScheLive) {
			// scheduled live
			scheduleStart([this](bool ok, int) {
				if (!ok) {
					clearLive();
				} else {
					PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV,
						  {
							  {"platformName", NAVER_TV},
							  {"startLiveStatus", "Success"},
						  },
						  "Naver TV start live success");
				}
				liveStartedCallback(ok);
			});
		} else {
			liveStartedCallback(true);
		}
	}
}
void PLSPlatformNaverTV::onAlLiveStarted(bool value)
{
	PLSPlatformBase::setIsScheduleLive(isScheduleLive());
	PLSPlatformBase::onAlLiveStarted(value);
}
void PLSPlatformNaverTV::onLiveEnded()
{
	PLSCHANNELS_API->setValueOfChannel(getChannelUUID(), ChannelData::g_shareUrl, QString());

	checkStreamPublishingRetryTimes = 0;
	deleteTimer(checkStreamPublishingTimer);
	deleteTimer(checkStatusTimer);

	if (!this->isRehearsal() && (getLiveId() >= 0)) {
		if (isLiveEndedByMqtt) {
			PLSPlatformBase::onLiveEnded();
			clearLiveInfo();
			clearLive();
		} else {
			liveClose([this](bool) {
				PLSPlatformBase::onLiveEnded();
				clearLiveInfo();
			});
		}
	} else {
		PLSPlatformBase::onLiveEnded();
		clearLive();
	}
}
void PLSPlatformNaverTV::onActive()
{
	PLSPlatformBase::onActive();

	pls_async_call_mt(this, [this]() { onShowScheLiveNotice(); });
}
QString PLSPlatformNaverTV::getShareUrl()
{
	return live ? live->pcLiveUrl : QString();
}
QJsonObject PLSPlatformNaverTV::getWebChatParams()
{
	QJsonObject object;
	object[CSTR_NAME] = "NAVERTV";

	if (live) {
		auto cookies = parseCookie(getChannelCookie());
		object[CSTR_OBJECTID] = live->objectId;
		object[CSTR_GROUPID] = live->groupId;
		object[CSTR_LANG] = LANGUAGE;
		object[CSTR_TICKET] = live->ticketId;
		object[CSTR_TEMPLATEID] = live->templateId;
		object[CSTR_USERTYPE] = live->manager ? CSTR_MANAGER_U : "";
		object[CSTR_GLOBAL_URI] = PRISM_SSL;
		object[CSTR_HMAC_SALT] = PLS_PC_HMAC_KEY;
		object[CSTR_NID_SES] = cookies[CSTR_NID_SES];
		object[CSTR_NID_INF] = cookies[CSTR_NID_INF];
		object[CSTR_NID_AUT] = cookies[CSTR_NID_AUT];
		object[CSTR_OBJECTURL] = CSTR_OBJECT_URL;
		object[CSTR_USERID] = live->idNo;
		object[CSTR_USERNAME] = nickName;
		object[CSTR_IS_REHEARSAL] = isRehearsal();
		object[CSTR_DISABLE] = getLiveId() == -1;
	}

	return object;
}
bool PLSPlatformNaverTV::isSendChatToMqtt() const
{
	return true;
}
QJsonObject PLSPlatformNaverTV::getLiveStartParams()
{
	QJsonObject platform = PLSPlatformBase::getLiveStartParams();

	platform[CSTR_LIVE_ID] = getLiveId();
	platform[CSTR_IS_REHEARSAL] = isRehearsal();
	platform[CSTR_CHANNELID] = getSubChannelId();
	platform[CSTR_SCHEDULED] = isScheduleLive();
	platform[CSTR_SIMULCAST_CHANNEL] = getSubChannelName();

	return platform;
}

void PLSPlatformNaverTV::onInitDataChanged()
{
	auto platforms = PLS_PLATFORM_API->getPlatformNaverTV();
	auto iter = std::find_if(platforms.begin(), platforms.end(), [](const PLSPlatformNaverTV *platform) { return platform->isPrimary(); });
	if (iter == platforms.end()) {
		return;
	}

	auto primaryNavertv = *iter;
	if (this != primaryNavertv) {
		token = primaryNavertv->token;
		nickName = primaryNavertv->nickName;
		headImageUrl = primaryNavertv->headImageUrl;
	}
}

bool PLSPlatformNaverTV::onMQTTMessage(PLSPlatformMqttTopic top, const QJsonObject &jsonObject)
{
	return true;
}

bool PLSPlatformNaverTV::isMqttChatCanShow(const QJsonObject &)
{
	return getLiveId() > 0;
}

QMap<QString, QString> PLSPlatformNaverTV::getDirectEndParams()
{
	return {{"serviceLiveLink", live ? live->pcLiveUrl : ""}};
}

void PLSPlatformNaverTV::getChannelEmblem(const QList<QString> &urls, const QObject *receiver, const ChannelEmblemCallback &callback) const
{
	if (urls.isEmpty()) {
		pls_invoke_safe(callback, QList<QPair<bool, QString>>());
		return;
	}

	pls::http::Requests requests;
	for (auto &url : urls) {
		requests.add(pls::http::Request() //
				     .method(pls::http::Method::Get)
				     .url(url)
				     .cookie(getChannelCookie())
				     .forDownload(true)
				     .saveDir(getTmpCacheDir())
				     .saveFileNamePrefix(IMAGE_FILE_NAME_PREFIX)
				     .withLog(pls_masking_person_info(url)));
	}

	pls::http::requests(requests //
				    .receiver({this, receiver})
				    .results([this, receiver, callback](const pls::http::Replies &replies) {
					    QList<QPair<bool, QString>> images;
					    replies.replies([&images](const pls::http::Reply &reply) {
						    if (reply.isOk()) {
							    images.append({true, reply.downloadFilePath()});
						    } else {
							    QString err = reply.errors();
							    images.append({false, QString()});
						    }
					    });
					    pls_async_call_mt({this, receiver}, [callback, images]() { pls_invoke_safe(callback, images); });
				    }));
}

void PLSPlatformNaverTV::getAuth(const QString &channelName, const QVariantMap &srcInfo, const UpdateCallback &finishedCall, bool isClearLiveInfo)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get channel list, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get channel list, channel: %s", SUB_CHANNEL_NAME_MASK);

	for (auto *platform : PLS_PLATFORM_API->getPlatformNaverTV()) {
		platform->primary = false;
	}

	if (isClearLiveInfo) {
		for (auto platform : PLS_PLATFORM_API->getPlatformNaverTV()) {
			platform->clearLiveInfo();
		}
	}

	primary = true;

	auto processGetTokenFailed = [this, srcInfo, finishedCall](QNetworkReply::NetworkError error, const QString &loginFailed) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get token failed, channel: %s, load lastest saved.", SUB_CHANNEL_NAME);
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV get token failed, channel: %s, load lastest saved.", SUB_CHANNEL_NAME_MASK);

		loadToken(token);
		if (token.accessToken.isEmpty()) {
			PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV lastest saved token is empty, channel: %s.", SUB_CHANNEL_NAME);
			PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV lastest saved token is empty, channel: %s.", SUB_CHANNEL_NAME_MASK);

			QVariantMap info = srcInfo;
			info[ChannelData::g_channelStatus] = ChannelData::ChannelStatus::Error;
			info[ChannelData::g_errorType] = (error >= QNetworkReply::ConnectionRefusedError && error <= QNetworkReply::UnknownNetworkError)
								 ? ChannelData::NetWorkErrorType::NetWorkNoStable
								 : ChannelData::NetWorkErrorType::UnknownError;
			info[ChannelData::g_channelSreLoginFailed] = loginFailed;
			pls_invoke_safe(finishedCall, InfosList{info});
			return false;
		}
		return true;
	};

	getToken([this, channelName, srcInfo, finishedCall, processGetTokenFailed](bool ok, QNetworkReply::NetworkError error, const QString &loginFailed) {
		if (!ok && !processGetTokenFailed(error, loginFailed)) {
			return;
		}

		getJson(
			Url(QUrl(CHANNEL_NAVERTV_GET_AUTH)), "Naver TV get channel list",
			[this, channelName, srcInfo, finishedCall](const QJsonDocument &json) { processGetChannelListResult(channelName, srcInfo, finishedCall, json); },
			[srcInfo, finishedCall](bool expired, int code) {
				QVariantMap info = srcInfo;
				if (expired) {
					info[ChannelData::g_channelStatus] = ChannelData::ChannelStatus::Expired;
				} else if (code == ERROR_PERMITEXCEPTION) {
					info[ChannelData::g_channelStatus] = ChannelData::ChannelStatus::EmptyChannel;
				} else {
					info[ChannelData::g_channelStatus] = ChannelData::ChannelStatus::Error;
					info[ChannelData::g_channelSreLoginFailed] = QStringLiteral("Get Channel List Failed");
				}
				pls_invoke_safe(finishedCall, InfosList{info});
			},
			this, ApiId::Other, QVariantMap(), false, false);
	});
}

static QVariantMap buildInfo(int i, const QString &channelName, const QVariantMap &srcInfo, const QString &accessToken, const QJsonObject &metaObject, const QList<pls::http::ReplyImplPtr> &emblems)
{
	QVariantMap info;
	info[ChannelData::g_channelCookie] = srcInfo[ChannelData::g_channelCookie];
	info[ChannelData::g_channelToken] = accessToken;
	info[ChannelData::g_platformName] = channelName;
	info[ChannelData::g_channelStatus] = ChannelData::ChannelStatus::Valid;
	info[ChannelData::g_subChannelId] = metaObject[CSTR_CHANNELID].toString();
	info[ChannelData::g_nickName] = metaObject[CSTR_CHANNELNAME].toString();
	info[ChannelData::g_shareUrl] = QString();
	info[ChannelData::g_viewers] = "0";
	info[ChannelData::g_viewersPix] = ChannelData::g_naverTvViewersIcon;
	if (i >= emblems.count()) {
		return info;
	} else if (pls::http::Reply emblem = emblems.at(i); emblem.isDownloadOk()) {
		info[ChannelData::g_userIconCachePath] = emblem.downloadFilePath();
		return info;
	}
	return info;
}

void PLSPlatformNaverTV::processGetChannelListResult(const QString &channelName, const QVariantMap &srcInfo, const UpdateCallback &finishedCall, const QJsonDocument &json)
{
	QJsonObject object = json.object();
	QString representChannelId = object.value("representChannelId").toString();
	QJsonArray meta = object.value("meta").toArray();
	if (meta.isEmpty()) {
		QVariantMap info = srcInfo;
		info[ChannelData::g_channelStatus] = ChannelData::ChannelStatus::EmptyChannel;
		pls_invoke_safe(finishedCall, InfosList{info});
		return;
	}

	// download image
	pls::http::Requests requests;
	for (auto i : meta) {
		QString url = i.toObject()[CSTR_CHANNELEMBLEM].toString();
		requests.add(pls::http::Request()
				     .method(pls::http::Method::Get)
				     .hmacUrl(url, PLS_PC_HMAC_KEY.toUtf8())
				     .cookie(getChannelCookie())
				     .forDownload(true)
				     .saveDir(getTmpCacheDir())
				     .saveFileNamePrefix(IMAGE_FILE_NAME_PREFIX)
				     .withLog(pls_masking_person_info(url)));
	}

	pls::http::requests(requests                //
				    .receiver(this) //
				    .results([channelName, srcInfo, finishedCall, meta, this](const pls::http::Replies &replies) {
					    auto emblems = replies.replies();

					    QList<QVariantMap> infos;
					    for (int i = 0, count = meta.count(); i < count; ++i) {
						    auto metaObject = meta[i].toObject();
						    QVariantMap info = buildInfo(i, channelName, srcInfo, token.accessToken, metaObject, emblems);
						    infos.append(info);
					    }

					    pls_sync_call_mt(this, [finishedCall, infos, this]() {
						    getUserInfo([finishedCall, infos](bool) { pls_invoke_safe(finishedCall, infos); }); //
					    });
				    }));
}

void PLSPlatformNaverTV::getCode(const CodeCallback &callback, const QString &url, bool redirectUrl)
{
	getAuth2(
		url, "Naver TV get code",
		[this, callback](bool ok, QNetworkReply::NetworkError error, const QByteArray &body) {
			if (!ok) {
				pls_invoke_safe(callback, false, error, QString());
				return;
			}

			QString nextUrl;
			if (!findLocationUrl(nextUrl, body)) {
				PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get code failed, channel: %s, reason: location url not found.", SUB_CHANNEL_NAME);
				PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV get code failed, channel: %s, reason: location url not found.", SUB_CHANNEL_NAME_MASK);
				pls_invoke_safe(callback, false, error, QString());
			} else if (nextUrl.contains(CHANNEL_NAVERTV_AUTHORIZE_REDIRECT) && nextUrl.contains(CSTR_CODE_E)) {
				pls_invoke_safe(callback, true, error, getUrlCode(nextUrl));
			} else {
				getCode(callback, nextUrl, true);
			}
		},
		!redirectUrl ? QVariantMap{{CSTR_CLIENT_ID, CSTR_CLIENT_ID_VALUE}, {CSTR_RESPONSE_TYPE, CSTR_CODE}, {CSTR_REDIRECT_URL, CHANNEL_NAVERTV_AUTHORIZE_REDIRECT}} : QVariantMap{});
}
void PLSPlatformNaverTV::getToken(const TokenCallback &callback)
{
	getCode(
		[this, callback](bool ok, QNetworkReply::NetworkError error, const QString &code) {
			if (ok) {
				getToken(code, callback);
			} else {
				pls_invoke_safe(callback, false, error, QStringLiteral("Get Code Failed"));
			}
		},
		CHANNEL_NAVERTV_AUTHORIZE, false);
}

void PLSPlatformNaverTV::getToken(const QString &code, const TokenCallback &callback)
{
	getAuth2(CHANNEL_NAVERTV_TOKEN, "Naver TV get token", [this, callback](bool ok, QNetworkReply::NetworkError error, const QByteArray &body) { processGetTokenOk(ok, error, body, callback); },
		 {{CSTR_CODE, code}, {CSTR_GRANT_TYPE, CSTR_GRANT_TYPE_AUTHORIZATION_CODE}, {CSTR_CLIENT_ID, CSTR_CLIENT_ID_VALUE}, {CSTR_CLIENT_SECRET, CSTR_CLIENT_SECRET_VALUE}});
}
void PLSPlatformNaverTV::processGetTokenOk(bool ok, QNetworkReply::NetworkError error, const QByteArray &body, const TokenCallback &callback)
{
	if (!ok) {
		pls_invoke_safe(callback, false, error, QStringLiteral("Get Token Failed"));
		return;
	}

	QJsonParseError jsonError;
	QJsonDocument respjson = QJsonDocument::fromJson(body, &jsonError);
	if (jsonError.error == QJsonParseError::NoError) {
		PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get token success, channel: %s", SUB_CHANNEL_NAME);
		PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get token success, channel: %s", SUB_CHANNEL_NAME_MASK);
		token = Token(respjson.object());
		saveToken(token);
		pls_invoke_safe(callback, true, error, QString());
	} else {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get token failed, channel: %s, reason: %s", SUB_CHANNEL_NAME, jsonError.errorString().toUtf8().constData());
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV get token failed, channel: %s, reason: %s", SUB_CHANNEL_NAME_MASK, jsonError.errorString().toUtf8().constData());
		pls_invoke_safe(callback, false, error, QStringLiteral("Get Token Failed"));
	}
}
void PLSPlatformNaverTV::getAuth2(const QString &url, const char *log, const Auth2Callback &callback, const QVariantMap &urlQueries) const
{
	auto failCallback = [this, log, callback](const QNetworkReply *reply, int, const QByteArray &data, QNetworkReply::NetworkError error, const void *) {
		processGetAuth2Fail(reply, data, error, log, callback);
	};

	pls::http::request(pls::http::Request()
				   .method(pls::http::Method::Get)
				   .hmacUrl(url, PLS_PC_HMAC_KEY.toUtf8())
				   .cookie(getChannelCookie())
				   .jsonContentType()
				   .urlParams(urlQueries)
				   .receiver(this)
				   .withLog(pls_masking_person_info(url))
				   .okResult([this, log, callback](const pls::http::Reply &reply) {
					   QByteArray data = reply.data();
					   if (data.contains(CSTR_ERROR) && data.contains(CSTR_ERROR_DESCRIPTION)) {
						   QJsonParseError jsonError;
						   QJsonDocument respJson = QJsonDocument::fromJson(data, &jsonError);
						   if (jsonError.error == QJsonParseError::NoError) {
							   PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME,
									respJson.object()[CSTR_ERROR_DESCRIPTION].toString().toUtf8().constData());
							   PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK,
								     respJson.object()[CSTR_ERROR_DESCRIPTION].toString().toUtf8().constData());
							   pls_async_call_mt(this, [callback]() { pls_invoke_safe(callback, false, QNetworkReply::NoError, QByteArray()); });
						   } else {
							   pls_async_call_mt(this, [callback, data]() { pls_invoke_safe(callback, true, QNetworkReply::NoError, data); });
						   }
					   } else {
						   pls_async_call_mt(this, [callback, data]() { pls_invoke_safe(callback, true, QNetworkReply::NoError, data); });
					   }
				   })
				   .failResult([this, log, callback](const pls::http::Reply &reply) { processGetAuth2Fail(reply.reply(), reply.data(), reply.error(), log, callback); }));
}
void PLSPlatformNaverTV::processGetAuth2Fail(const QNetworkReply *reply, const QByteArray &data, QNetworkReply::NetworkError error, const char *log, const Auth2Callback &callback) const
{
	if (!data.isEmpty()) {
		QJsonParseError jsonError;
		QJsonDocument respJson = QJsonDocument::fromJson(data, &jsonError);
		if (jsonError.error == QJsonParseError::NoError) {
			PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, respJson.object()[CSTR_ERROR_DESCRIPTION].toString().toUtf8().constData());
			PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, respJson.object()[CSTR_ERROR_DESCRIPTION].toString().toUtf8().constData());
			pls_async_call_mt(this, [callback, error]() { pls_invoke_safe(callback, false, error, QByteArray()); });
		} else {
			PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME);
			PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME_MASK);
			pls_async_call_mt(this, [callback, error]() { pls_invoke_safe(callback, false, error, QByteArray()); });
		}
	} else {

		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, reply->errorString().toUtf8().constData());
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, reply->errorString().toUtf8().constData());
		pls_async_call_mt(this, [callback, error]() { pls_invoke_safe(callback, false, error, QByteArray()); });
	}
}
void PLSPlatformNaverTV::getUserInfo(const UserInfoCallback &callback)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get user info, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get user info, channel: %s", SUB_CHANNEL_NAME_MASK);

	pls::http::request(
		pls::http::Request()
			.method(pls::http::Method::Get)
			.hmacUrl(QUrl(CHANNEL_NAVERTV_GET_USERINFO), PLS_PC_HMAC_KEY.toUtf8())
			.rawHeader(QByteArray(CSTR_REFERER), QByteArray(CSTR_OBJECT_URL))
			.cookie(getChannelCookie())
			.jsonContentType()
			.receiver(this)
			.withLog()
			.objectOkResult(
				[this, callback](const pls::http::Reply &, const QJsonObject &object) {
					PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get user info success, channel: %s", SUB_CHANNEL_NAME);
					PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get user info success, channel: %s", SUB_CHANNEL_NAME_MASK);
					nickName = getValue(object, KEY_NICK_NAME, DEFAULT_NICK_NAME);
					headImageUrl = getValue(object, KEY_IMAGE_URL, CHANNEL_NAVERTV_DEFAULT_HEAD_IMAGE_URL);
					pls_sync_call_mt(this, [callback]() { pls_invoke_safe(callback, true); });
				},
				[this, callback](const pls::http::Reply &, const QJsonParseError &error) {
					PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get user info failed, channel: %s, reason: %s", SUB_CHANNEL_NAME, error.errorString().toUtf8().constData());
					PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV get user info failed, channel: %s, reason: %s", SUB_CHANNEL_NAME_MASK, error.errorString().toUtf8().constData());
					nickName = DEFAULT_NICK_NAME;
					headImageUrl = CHANNEL_NAVERTV_DEFAULT_HEAD_IMAGE_URL;
					pls_sync_call_mt(this, [callback]() { pls_invoke_safe(callback, false); });
				})
			.jsonFailResult(
				[this, callback](const pls::http::Reply &reply, const QJsonDocument &json) {
					processFailed(
						"Naver TV get user info", json,
						[this, callback](bool, int) {
							nickName = DEFAULT_NICK_NAME;
							headImageUrl = CHANNEL_NAVERTV_DEFAULT_HEAD_IMAGE_URL;
							pls_sync_call_mt(this, [callback]() { pls_invoke_safe(callback, false); });
						},
						this, reply.error(), reply.statusCode(), false, false, false, ApiId::Other);
				},
				[this, callback](const pls::http::Reply &, const QJsonParseError &) {
					PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get user info failed, channel: %s, reason: unknown", SUB_CHANNEL_NAME);
					PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV get user info failed, channel: %s, reason: unknown", SUB_CHANNEL_NAME_MASK);
					nickName = DEFAULT_NICK_NAME;
					headImageUrl = CHANNEL_NAVERTV_DEFAULT_HEAD_IMAGE_URL;
					pls_sync_call_mt(this, [callback]() { pls_invoke_safe(callback, false); });
				}));
}

void PLSPlatformNaverTV::getScheLives(const LiveInfosCallback &callback, const QObject *receiver, bool popupNeedShow, bool popupGenericError, bool expiredNotify)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get schedule live list, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get schedule live list, channel: %s", SUB_CHANNEL_NAME_MASK);

	getScheLives(30 * 60, 3 * 24 * 60 * 60, callback, receiver, popupNeedShow, popupGenericError, expiredNotify);
}
void PLSPlatformNaverTV::getScheLives(const LiveInfosCallbackEx &callback, const QObject *receiver, bool popupNeedShow, bool popupGenericError, bool expiredNotify)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get schedule live list, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get schedule live list, channel: %s", SUB_CHANNEL_NAME_MASK);

	getScheLives(30 * 60, 3 * 24 * 60 * 60, callback, receiver, popupNeedShow, popupGenericError, expiredNotify);
}
void PLSPlatformNaverTV::getScheLives(int before, int after, const LiveInfosCallback &callback, const QObject *receiver, bool popupNeedShow, bool popupGenericError, bool expiredNotify)
{
	getScheLives(
		before, after, [callback](bool ok, int, const QList<LiveInfo> &scheLiveInfos) { callback(ok, scheLiveInfos); }, receiver, popupNeedShow, popupGenericError, expiredNotify);
}
void PLSPlatformNaverTV::getScheLives(int before, int after, const LiveInfosCallbackEx &callback, const QObject *receiver, bool popupNeedShow, bool popupGenericError, bool expiredNotify)
{
	setlocale(LC_TIME, "en_US");
	time_t now = std::time(nullptr) + 9 * 60 * 60;

	struct tm tm;

	time_t fromTime = now - before;
#if defined(Q_OS_WIN)
#define os_gmtime_s(tm, time) gmtime_s(tm, time)
#else
#define os_gmtime_s(tm, time) gmtime_r(time, tm)
#endif
	os_gmtime_s(&tm, &fromTime);
	std::array<char, 128> fromDate;
	strftime(fromDate.data(), sizeof(fromDate), "%FT%T+09:00", &tm);

	time_t toTime = now + after;
	os_gmtime_s(&tm, &toTime);
	std::array<char, 128> toDate;
	strftime(toDate.data(), sizeof(toDate), "%FT%T+09:00", &tm);

	getScheLives(QString::fromUtf8(fromDate.data()), QString::fromUtf8(toDate.data()), callback, receiver, popupNeedShow, popupGenericError, expiredNotify);
}
void PLSPlatformNaverTV::getScheLives(const QString &fromDate, const QString &toDate, const LiveInfosCallbackEx &callback, const QObject *receiver, bool popupNeedShow, bool popupGenericError,
				      bool expiredNotify)
{
	QUrl url = CHANNEL_NAVERTV_GET_LIVES.arg(PRISM_SSL).arg(getSubChannelId());

	QUrlQuery query;
	query.addQueryItem(QStringLiteral("fromDate"), QUrl::toPercentEncoding(fromDate));
	query.addQueryItem(QStringLiteral("toDate"), QUrl::toPercentEncoding(toDate));
	url.setQuery(query);

	getJson(
		Url(url, CHANNEL_NAVERTV_GET_LIVES.arg(PRISM_SSL).arg(pls_masking_person_info(getSubChannelId())) + '?' + query.toString(QUrl::FullyEncoded)), "Naver TV get schedule live list",
		[this, receiver, callback](const QJsonDocument &json) {
			QList<LiveInfo> liveInfos;
			QJsonObject object = json.object();
			QJsonArray meta = object["meta"].toArray();
			for (auto i : meta) {
				liveInfos.append(LiveInfo(i.toObject()));
			}

			QList<QString> urls;
			for (const auto &li : liveInfos) {
				urls.append(li.thumbnailImageUrl);
			}
			getChannelEmblem(urls, receiver, [this, receiver, liveInfos, callback](const QList<QPair<bool, QString>> &images) mutable {
				pls_chk_for_each(
					images.begin(), images.end(), [](const QPair<bool, QString> &thumbnail) { return thumbnail.first; },
					[&liveInfos](const QPair<bool, QString> &thumbnail, int index) { liveInfos[index].thumbnailImagePath = thumbnail.second; });

				pls_async_call_mt({this, receiver}, [callback, liveInfos]() {
					pls_invoke_safe(callback, true, 0, liveInfos); //
				});
			});
		},
		[this, receiver, callback](bool, int code) {
			pls_async_call_mt({this, receiver}, [callback, code]() {
				pls_invoke_safe(callback, false, code, QList<LiveInfo>()); //
			});
		},
		receiver, ApiId::Other, QVariantMap(), expiredNotify, popupNeedShow, popupGenericError);
}
void PLSPlatformNaverTV::checkScheLive(int oliveId, const CheckScheLiveCallback &callback)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV check schedule live is valid, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV check schedule live is valid, channel: %s", SUB_CHANNEL_NAME_MASK);

	getScheLives(
		[oliveId, callback](bool ok, const QList<LiveInfo> &scheLiveInfos) {
			if (ok) {
				if (std::find_if(scheLiveInfos.begin(), scheLiveInfos.end(), [=](const LiveInfo &li) { return li.oliveId == oliveId; }) != scheLiveInfos.end()) {
					pls_invoke_safe(callback, true, true);
				} else {
					pls_invoke_safe(callback, true, false);
				}
			} else {
				pls_invoke_safe(callback, false, false);
			}
		},
		this);
}

void PLSPlatformNaverTV::getStreamInfo(const StreamInfoCallback &callback, const char *log, bool popupGenericError)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "%s, channel: %s", log, SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "%s, channel: %s", log, SUB_CHANNEL_NAME_MASK);

	getJson(
		Url(CHANNEL_NAVERTV_GET_STREAM_INFO.arg(PRISM_SSL).arg(getSubChannelId()), CHANNEL_NAVERTV_GET_STREAM_INFO.arg(PRISM_SSL).arg(pls_masking_person_info(getSubChannelId()))), log,
		[this, callback](const QJsonDocument &json) {
			QJsonObject object = json.object();
			QString streamKey = object[CSTR_STREAMKEY].toString();
			QString serverUrl = object[CSTR_SERVERURL].toString();
			bool publishing = object[CSTR_PUBLISHING].toBool();
			QString channelId = object[CSTR_CHANNELID].toString();
			PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV stream info, stream key: %s, server url: %s, publishing: %s channel: %s", streamKey.toUtf8().constData(),
				    serverUrl.toUtf8().constData(), publishing ? "true" : "false", SUB_CHANNEL_NAME);
			PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV stream info, stream key: %s, server url: %s, publishing: %s channel: %s", pls_masking_person_info(streamKey).toUtf8().constData(),
				 serverUrl.toUtf8().constData(), publishing ? "true" : "false", SUB_CHANNEL_NAME_MASK);
			setStreamKey(streamKey.toStdString());
			setStreamServer(serverUrl.toStdString());
			pls_invoke_safe(callback, true, publishing, 0);
		},
		[callback](bool, int code) {
			pls_invoke_safe(callback, false, false, code); //
		},
		this, ApiId::Other, QVariantMap(), true, true, popupGenericError);
}
void PLSPlatformNaverTV::checkStreamPublishing(const CheckStreamPublishingCallback &callback)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV check stream publishing, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV check stream publishing, channel: %s", SUB_CHANNEL_NAME_MASK);
	getStreamInfo([callback](bool ok, bool publishing, int code) { pls_invoke_safe(callback, ok, publishing, code); }, "Naver TV check stream publishing");
}

void PLSPlatformNaverTV::immediateStart(const QuickStartCallback &callback, bool isShareOnFacebook, bool isShareOnTwitter, bool popupGenericError)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV immediate live, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV immediate live, channel: %s", SUB_CHANNEL_NAME_MASK);

	auto quickStartCallback = [callback](bool ok, int code) {
		if (!ok) {
			pls_invoke_safe(callback, false, code);
		} else if (!PLS_PLATFORM_API->isPrismLive()) {
			pls_invoke_safe(callback, true, 0);
		} else {
			pls_invoke_safe(callback, true, 0);
		}
	};

	auto cLiveInfo = getLiveInfo();
	if (cLiveInfo->needUploadThumbnail()) {
		uploadImage(cLiveInfo->thumbnailImagePath, [this, callback, isShareOnFacebook, isShareOnTwitter, popupGenericError, cLiveInfo, quickStartCallback](bool ok, const QString &imageUrl) {
			if (ok) {
				cLiveInfo->thumbnailImageUrl = imageUrl;
				quickStart(quickStartCallback, isShareOnFacebook, isShareOnTwitter, popupGenericError);
			} else {
				pls_invoke_safe(callback, false, ERROR_OTHERS);
			}
		});
	} else {
		quickStart(quickStartCallback, isShareOnFacebook, isShareOnTwitter, popupGenericError);
	}
}
void PLSPlatformNaverTV::scheduleStart(const LiveOpenCallback &callback, bool isShareOnFacebook, bool isShareOnTwitter)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV schedule live, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV schedule live, channel: %s", SUB_CHANNEL_NAME_MASK);

	checkStreamPublishingRetryTimes = 0;
	deleteTimer(checkStreamPublishingTimer);
	deleteTimer(checkStatusTimer);

	scheduleStartWithCheck(callback, isShareOnFacebook, isShareOnTwitter);
}
void PLSPlatformNaverTV::scheduleStartWithCheck(const LiveOpenCallback &callback, bool isShareOnFacebook, bool isShareOnTwitter)
{
	auto getCommentOptionsProcess = [this, callback]() {
		getCommentOptions([this, callback](bool ok, int code) {
			if (ok) {
				if (!PLS_PLATFORM_API->isPrismLive()) {
					pls_invoke_safe(callback, true, 0);
				} else {
					pls_invoke_safe(callback, true, 0);
				}
			} else {
				PLSCHANNELS_API->setValueOfChannel(getChannelUUID(), ChannelData::g_shareUrl, QString());
				liveClose([callback, code](bool) { pls_invoke_safe(callback, false, code); });

				checkStreamPublishingRetryTimes = 0;
				deleteTimer(checkStreamPublishingTimer);
				deleteTimer(checkStatusTimer);
			}
		});
	};

	auto liveOpenCallback = [callback, getCommentOptionsProcess](bool ok, int code) {
		if (!ok) {
			pls_invoke_safe(callback, false, code);
		} else {
			getCommentOptionsProcess();
		}
	};

	this->checkStreamPublishing([this, callback, isShareOnFacebook, isShareOnTwitter, liveOpenCallback](bool ok, bool publishing, int code) {
		if (!ok) {
			PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV,
				  {{"platformName", NAVER_TV}, {"startLiveStatus", "Failed"}, {"startLiveFailed", "Schedule Start Failed, Check Stream Publishing Failed"}},
				  "Naver TV start live failed");
			pls_invoke_safe(callback, false, code);
		} else if (publishing) {
			liveOpen(liveOpenCallback, isShareOnFacebook, isShareOnTwitter);
		} else if (checkStreamPublishingRetryTimes < CHECK_PUBLISHING_MAX_TIMES) {
			PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV stream is not publishing, wait 5s and retry, channel: %s", SUB_CHANNEL_NAME);
			PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV stream is not publishing, wait 5s and retry, channel: %s", SUB_CHANNEL_NAME_MASK);
			::startTimer(this, checkStreamPublishingTimer, CHECK_PUBLISHING_TIMESPAN,
				     [this, callback, isShareOnFacebook, isShareOnTwitter]() { scheduleStartWithCheck(callback, isShareOnFacebook, isShareOnTwitter); });
			++checkStreamPublishingRetryTimes;
		} else {
			PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV,
				  {{"platformName", NAVER_TV}, {"startLiveStatus", "Failed"}, {"startLiveFailed", "Schedule Start Failed, Check Stream Publishing Timeout"}},
				  "Naver TV start live failed");
			pls_invoke_safe(callback, false, ERROR_OTHERS);
		}
	});
}

void PLSPlatformNaverTV::quickStart(const QuickStartCallback &callback, bool isShareOnFacebook, bool isShareOnTwitter, bool popupGenericError)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV quick start, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV quick start, channel: %s", SUB_CHANNEL_NAME_MASK);

	auto cLiveInfo = getLiveInfo();

	QJsonObject reqJson;
	reqJson[CSTR_TITLE] = cLiveInfo->title;
	reqJson[CSTR_REHEARSALYN] = cLiveInfo->isRehearsal ? "Y" : "N";
	reqJson[CSTR_THUMBNAILIMAGEURL] = processThumbnailImageUrl(cLiveInfo->thumbnailImageUrl);
	if (!cLiveInfo->isRehearsal && (isShareOnFacebook || isShareOnTwitter)) {
		QJsonObject share;
		if (isShareOnFacebook) {
#if 0
			share["facebookToken"] = ;
#endif
		}

		if (isShareOnTwitter) {
#if 0
			 share["twitterConsumerKey"] = ;
			 share["twitterConsumerSecret"] = ;
			 share["twitterToken"] = ;
			 share["twitterTokenSecret"] = ;
#endif
		}

		reqJson["share"] = share;
	}

	postJson(
		Url(CHANNEL_NAVERTV_QUICK_START.arg(PRISM_SSL).arg(getSubChannelId()), CHANNEL_NAVERTV_QUICK_START.arg(PRISM_SSL).arg(pls_masking_person_info(getSubChannelId()))), reqJson,
		"Post Naver TV quick start",
		[this, callback](const QJsonDocument &respJson) {
			auto curlive = getLive();

			QJsonObject object = respJson.object();
			QJsonObject liveCommentOptions = object[CSTR_LIVECOMMENTOPTIONS].toObject();
			curlive->groupId = liveCommentOptions[CSTR_GROUPID].toString();
			curlive->ticketId = liveCommentOptions[CSTR_TICKETID].toString();
			curlive->objectId = liveCommentOptions[CSTR_OBJECTID].toString();
			curlive->templateId = liveCommentOptions[CSTR_TEMPLATEID].toString();
			curlive->idNo = liveCommentOptions[CSTR_IDNO].toString();
			curlive->manager = liveCommentOptions[CSTR_MANAGER].toBool();

			QJsonObject oliveModel = object[CSTR_OLIVEMODEL].toObject();
			curlive->oliveId = oliveModel[CSTR_ID].toInt();

			QJsonObject link = oliveModel[CSTR_LINK].toObject();
			curlive->pcLiveUrl = link[CSTR_PCLIVEURL].toString();
			curlive->mobileLiveUrl = link[CSTR_MOBILELIVEURL].toString();

			PLSCHANNELS_API->setValueOfChannel(getChannelUUID(), ChannelData::g_shareUrl, curlive->pcLiveUrl);
			pls_invoke_safe(callback, true, 0);
		},
		[callback](bool, int code) {
			pls_invoke_safe(callback, false, code); //
		},
		this, ApiId::Other, QVariantMap(), true, true, popupGenericError);
}

void PLSPlatformNaverTV::liveOpen(const LiveOpenCallback &callback, bool isShareOnFacebook, bool isShareOnTwitter)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV open live, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV open live, channel: %s", SUB_CHANNEL_NAME_MASK);

	if (getLiveId() < 0) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV open live failed, live id invalid, channel: %s", SUB_CHANNEL_NAME);
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV open live failed, live id invalid, channel: %s", SUB_CHANNEL_NAME_MASK);
		PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV, {{"platformName", NAVER_TV}, {"startLiveStatus", "Failed"}, {"startLiveFailed", "Schedule Start Failed, Invalid Schedule"}},
			  "Naver TV start live failed");
		pls_invoke_safe(callback, false, 0);
		return;
	}

	QJsonObject reqJson;
	if (isShareOnFacebook || isShareOnTwitter) {
		QJsonObject share;
		if (isShareOnFacebook) {
#if 0
			share["facebookToken"] = ;
#endif
		}

		if (isShareOnTwitter) {
#if 0
			 share["twitterConsumerKey"] = ;
			 share["twitterConsumerSecret"] = ;
			 share["twitterToken"] = ;
			 share["twitterTokenSecret"] = ;
#endif
		}

		reqJson["share"] = share;
	}

	postJson(
		Url(QUrl(CHANNEL_NAVERTV_OPEN.arg(PRISM_SSL).arg(getLiveId())), false), reqJson, "Post Naver TV live open",
		[this, callback](const QJsonDocument &respJson) {
			auto live_ = getLive();

			QJsonObject object = respJson.object();
			QJsonObject currentStat = object[CSTR_CURRENTSTAT].toObject();
			live_->watchCount = currentStat[CSTR_PV].toInt();

			QJsonObject link = object[CSTR_LINK].toObject();
			live_->pcLiveUrl = link[CSTR_PCLIVEURL].toString();
			live_->mobileLiveUrl = link[CSTR_MOBILELIVEURL].toString();
			PLSCHANNELS_API->setValueOfChannel(getChannelUUID(), ChannelData::g_shareUrl, live_->pcLiveUrl);
			pls_invoke_safe(callback, true, 0);
		},
		[callback](bool, int code) {
			PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV, {{"platformName", NAVER_TV}, {"startLiveStatus", "Failed"}, {"startLiveFailed", "Schedule Start Failed, Live Open Lailed"}},
				  "Naver TV start live failed");
			pls_invoke_safe(callback, false, code); //
		},
		this, ApiId::LiveOpen);
}
void PLSPlatformNaverTV::liveClose(const LiveCloseCallback &callback)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV close live, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV close live, channel: %s", SUB_CHANNEL_NAME_MASK);

	if (getLiveId() < 0) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV close live failed, live id invalid, channel: %s", SUB_CHANNEL_NAME);
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV close live failed, live id invalid, channel: %s", SUB_CHANNEL_NAME_MASK);
		pls_invoke_safe(callback, true);
		return;
	}

	QJsonObject reqJson;

	postJson(
		Url(QUrl(CHANNEL_NAVERTV_CLOSE.arg(PRISM_SSL).arg(getLiveId())), false), reqJson, "Naver TV close live",
		[this, callback](const QJsonDocument &) {
			clearLive();
			pls_invoke_safe(callback, true);
		},
		[callback](bool, int) {
			pls_invoke_safe(callback, false); //
		},
		this, ApiId::Other, QVariantMap(), false, false);
}
void PLSPlatformNaverTV::liveModify(const QString &title, const QString &thumbnailImageUrl, const LiveModifyCallback &callback)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV modify live info, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV modify live info, channel: %s", SUB_CHANNEL_NAME_MASK);

	QJsonObject reqJson;
	reqJson[CSTR_TITLE] = title;
	reqJson[CSTR_THUMBNAILIMAGEURL] = thumbnailImageUrl;

	postJson(
		Url(QUrl(CHANNEL_NAVERTV_MODIFY.arg(PRISM_SSL).arg(getLiveId()))), reqJson, "Naver TV modify live info",
		[this, callback](const QJsonDocument &respJson) {
			QJsonObject object = respJson.object();
			auto liveInfo_ = getLiveInfo();
			liveInfo_->title = object[CSTR_TITLE].toString();
			liveInfo_->thumbnailImageUrl = object[CSTR_THUMBNAIL].toString();
			pls_invoke_safe(callback, true);
		},
		[callback](bool, int) {
			pls_invoke_safe(callback, false); //
		});
}
void PLSPlatformNaverTV::liveStatus(const LiveStatusCallback &callback)
{
	if (getLiveId() < 0) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get live status failed, live id invalid, channel: %s", SUB_CHANNEL_NAME);
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV get live status failed, live id invalid, channel: %s", SUB_CHANNEL_NAME_MASK);
		pls_invoke_safe(callback, false, 0, 0, 0, false);
		return;
	}

	auto clive = getLive();

	QVariantMap headers;
	headers[CSTR_TICKET] = clive->ticketId;
	headers[CSTR_OBJECTID] = clive->objectId;
	headers[CSTR_TEMPLATEID] = clive->templateId;
	headers[CSTR_LANG] = LANGUAGE;

	getJson(
		Url(QUrl(CHANNEL_NAVERTV_STATUS.arg(PRISM_SSL).arg(getLiveId()))), "Naver TV get live status",
		[this, clive, callback](const QJsonDocument &respJson) {
			QJsonObject object = respJson.object();
			QString status = object[CSTR_STATUS].toString();
			clive->watchCount = (qint64)object[CSTR_PV].toDouble();
			clive->likeCount = (qint64)object[CSTR_LIKECOUNT].toDouble();
			clive->commentCount = (qint64)object[CSTR_COMMENTCOUNT].toDouble();
			PLSCHANNELS_API->setValueOfChannel(getChannelUUID(), ChannelData::g_viewers, QString::number(clive->watchCount));
			pls_invoke_safe(callback, true, clive->watchCount, clive->likeCount, clive->commentCount, status == "CLOSED");
		},
		[callback](bool, int) {
			pls_invoke_safe(callback, false, 0, 0, 0, false); //
		},
		this, ApiId::Other, headers, true, false, false, true);
}

void PLSPlatformNaverTV::getCommentOptions(const CommentOptionsCallback &callback)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV get live comment options, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV get live comment options, channel: %s", SUB_CHANNEL_NAME_MASK);

	getJson(
		Url(QUrl(CHANNEL_NAVERTV_COMMENT_OPTIONS.arg(getLiveId()))), "Naver TV get live comment options",
		[this, callback](const QJsonDocument &respJson) {
			auto live_ = getLive();
			QJsonObject object = respJson.object();
			live_->groupId = object[CSTR_GROUPID].toString();
			live_->ticketId = object[CSTR_TICKETID].toString();
			live_->objectId = object[CSTR_OBJECTID].toString();
			live_->templateId = object[CSTR_TEMPLATEID].toString();
			live_->idNo = object[CSTR_IDNO].toString();
			live_->manager = object[CSTR_MANAGER].toBool();
			pls_invoke_safe(callback, true, 0);
		},
		[callback](bool, int code) {
			pls_invoke_safe(callback, false, code); //
		});
}

void PLSPlatformNaverTV::uploadImage(const QString &imageFilePath, const UploadImageCallback &callback) const
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV upload image, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV upload image, channel: %s", SUB_CHANNEL_NAME_MASK);

	pls::http::request(pls::http::Request()
				   .method(pls::http::Method::Post)
				   .hmacUrl(CHANNEL_NAVERTV_UPLOAD_IMAGE.arg(getSubChannelId()), PLS_PC_HMAC_KEY.toUtf8())
				   .cookie(getChannelCookie())
				   .withLog(CHANNEL_NAVERTV_UPLOAD_IMAGE.arg(pls_masking_person_info(getSubChannelId())))
				   .receiver(this)
				   .form("imageFile", imageFilePath, true)
				   .objectOkResult([this, callback](const pls::http::Reply &, const QJsonObject &object) {
					   QString imageUrl = object["imageUrl"].toString();
					   PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV upload image success, channel: %s, image url: %s", SUB_CHANNEL_NAME, imageUrl.toUtf8().constData());
					   PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV upload image success, channel: %s, image url: %s", SUB_CHANNEL_NAME_MASK,
						    pls_masking_person_info(imageUrl).toUtf8().constData());
					   pls_async_call_mt(this, [callback, imageUrl]() { pls_invoke_safe(callback, true, imageUrl); });
				   })
				   .failResult([this, callback](const pls::http::Reply &reply) {
					   PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "Naver TV upload image failed, channel: %s, reason: %s", SUB_CHANNEL_NAME, reply.errors().toUtf8().constData());
					   PLS_ERROR(MODULE_PLATFORM_NAVERTV, "Naver TV upload image failed, channel: %s, reason: %s", SUB_CHANNEL_NAME_MASK, reply.errors().toUtf8().constData());
					   pls_async_call_mt(this, [callback]() { pls_invoke_safe(callback, false, QString()); });
				   }));
}

bool PLSPlatformNaverTV::isPrimary() const
{
	return primary;
}
QString PLSPlatformNaverTV::getSubChannelId() const
{
	return mySharedData().m_mapInitData[ChannelData::g_subChannelId].toString();
}
QString PLSPlatformNaverTV::getSubChannelName() const
{
	return mySharedData().m_mapInitData[ChannelData::g_nickName].toString();
}
int PLSPlatformNaverTV::getLiveId() const
{
	return live ? live->oliveId : -1;
}
bool PLSPlatformNaverTV::isRehearsal() const
{
	return liveInfo ? liveInfo->isRehearsal : false;
}
bool PLSPlatformNaverTV::isKnownError(int code) const
{
	return code == ERROR_AUTHEXCEPTION || code == ERROR_LIVENOTFOUND || code == ERROR_PERMITEXCEPTION || code == ERROR_LIVESTATUSEXCEPTION || code == ERROR_START30MINEXCEPTION ||
	       code == ERROR_ALREADYONAIR || code == ERROR_PAIDSPONSORSHIPINFO || code == ERROR_NETWORK_ERROR;
}

const PLSPlatformNaverTV::Token *PLSPlatformNaverTV::getToken() const
{
	return &token;
}
PLSPlatformNaverTV::LiveInfo *PLSPlatformNaverTV::getLiveInfo()
{
	return (!liveInfo) ? getNewLiveInfo() : liveInfo;
}
PLSPlatformNaverTV::LiveInfo *PLSPlatformNaverTV::getNewLiveInfo()
{
	deleteObject(liveInfo);
	liveInfo = pls_new<LiveInfo>(*getSelectedLiveInfo());
	setIsScheduleLive(liveInfo->isScheLive);
	return liveInfo;
}
void PLSPlatformNaverTV::clearLiveInfo()
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV clear live info, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV clear live info, channel: %s", SUB_CHANNEL_NAME_MASK);
	selectedScheLiveInfoId = -1;
	deleteObject(liveInfo);
}

PLSPlatformNaverTV::Live *PLSPlatformNaverTV::getLive() const
{
	if (live) {
		return live;
	}

	live = pls_new<Live>();
	return live;
}
void PLSPlatformNaverTV::clearLive()
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV clear live, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV clear live, channel: %s", SUB_CHANNEL_NAME_MASK);
	isLiveEndedByMqtt = false;
	deleteObject(live);
}

const PLSPlatformNaverTV::LiveInfo *PLSPlatformNaverTV::getSelectedLiveInfo() const
{
	auto scheLiveInfo = getScheLiveInfo(selectedScheLiveInfoId);
	if (scheLiveInfo) {
		return scheLiveInfo;
	}
	return getImmediateLiveInfo();
}
const PLSPlatformNaverTV::LiveInfo *PLSPlatformNaverTV::getScheLiveInfo(int scheLiveId) const
{
	for (auto &scheLive : scheLiveList) {
		if (scheLive.oliveId == scheLiveId) {
			return &scheLive;
		}
	}
	return nullptr;
}
const PLSPlatformNaverTV::LiveInfo *PLSPlatformNaverTV::getImmediateLiveInfo() const
{
	if (immediateLive.title.isEmpty()) {
		initImmediateLiveInfoTitle();
	}
	return &immediateLive;
}
const QList<PLSPlatformNaverTV::LiveInfo> &PLSPlatformNaverTV::getScheLiveInfoList() const
{
	return scheLiveList;
}
bool PLSPlatformNaverTV::isLiveInfoModified(int oliveId)
{
	if (getLiveInfo()->oliveId != oliveId) {
		return true;
	}
	return false;
}
bool PLSPlatformNaverTV::isLiveInfoModified(int oliveId, const QString &title, const QString &thumbnailImagePath)
{
	auto cliveInfo = getLiveInfo();
	if ((cliveInfo->oliveId != oliveId) || (cliveInfo->title != title) || (cliveInfo->thumbnailImagePath != thumbnailImagePath)) {
		return true;
	}
	return false;
}
void PLSPlatformNaverTV::updateLiveInfo(const QList<LiveInfo> &scheLiveInfos, int oliveId, bool isRehearsal, const QString &title, const QString &thumbnailImagePath,
					const UpdateLiveInfoCallback &callback)
{
	if (selectedScheLiveInfoId != oliveId) {
		selectedScheLiveInfoId = oliveId;
		scheLiveList = scheLiveInfos;
	}

	auto cliveInfo = getNewLiveInfo();
	cliveInfo->isRehearsal = isRehearsal;
	cliveInfo->title = titleIsEmtpy(title) ? getImmediateLiveInfo()->title : title;
	if (cliveInfo->thumbnailImagePath != thumbnailImagePath) {
		cliveInfo->thumbnailImagePath = thumbnailImagePath;
		cliveInfo->thumbnailImageUrl.clear();
	}

	// save title to prism api
	setTitle(cliveInfo->title.toStdString());

	if (PLS_PLATFORM_API->isPrepareLive()) {
		getStreamInfo(
			[this, callback, cliveInfo](bool ok, bool, int code) {
				if (!ok) {
					pls_invoke_safe(callback, false, code);
					PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV, {{"platformName", NAVER_TV}, {"startLiveStatus", "Failed"}, {"startLiveFailed", "Get Stream Info Failed"}},
						  "Naver TV start live failed");
				} else if (!cliveInfo->isScheLive) {
					// immediate live
					immediateStart(
						[this, callback](bool ok_, int code_) {
							if (!ok_) {
								clearLive();
								PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV,
									  {{"platformName", NAVER_TV}, {"startLiveStatus", "Failed"}, {"startLiveFailed", "Immediate Start Failed"}},
									  "Naver TV start live failed");
							} else {
								PLS_LOGEX(PLS_LOG_INFO, MODULE_PLATFORM_NAVERTV,
									  {
										  {"platformName", NAVER_TV},
										  {"startLiveStatus", "Success"},
									  },
									  "Naver TV start live success");
							}
							pls_invoke_safe(callback, ok_, ok_ ? 0 : code_);
						},
						false, false, false);
				} else {
					// scheduled live
					getLive()->oliveId = cliveInfo->oliveId;
					pls_invoke_safe(callback, true, 0);
				}
			},
			"Naver TV get stream key", false);
	} else {
		pls_invoke_safe(callback, true, 0);
	}
}
void PLSPlatformNaverTV::initImmediateLiveInfoTitle() const
{
	immediateLive.title = tr("LiveInfo.live.title.suffix").arg(getSubChannelName());
}

void PLSPlatformNaverTV::showScheLiveNotice(const LiveInfo &liveInfo_)
{
	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV show schedule live notice, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV show schedule live notice, channel: %s", SUB_CHANNEL_NAME_MASK);

	isScheLiveNoticeShown = true;

	PLSScheLiveNotice scheLiveNotice(this, liveInfo_.title, PLSDateFormate::timeStampToUTCString(liveInfo_.startDate / 1000), App()->getMainView());
	if (scheLiveNotice.exec() != PLSScheLiveNotice::Accepted) {
		return;
	}

#if 0
	 getScheLives([=](bool ok, const QList<LiveInfo> &) {
	 	if (ok) {
	 		PLSCHANNELS_API->toStartBroadcast();
	 		setSelectedScheLiveInfoId(liveInfo_.oliveId);
	 		clearLiveInfo();
	 	} else {
	 	}
	 });
#endif
}

void PLSPlatformNaverTV::updateScheduleList()
{
	auto refreshScheduleList = [this](bool isOk, const QList<LiveInfo> &scheLiveInfos) {
		if (isOk) {
			scheLiveList = scheLiveInfos;
		}
		emit scheduleListUpdateFinished();
	};
	this->getScheLives(30 * 60, 3 * 24 * 60 * 60, refreshScheduleList, this, false, false, false);
}

void PLSPlatformNaverTV::convertScheduleListToMapList()
{
	mySharedData().m_scheduleList.clear();
	auto uuid = this->getChannelUUID();
	auto tmpList = scheLiveList;
	QVariantList tmpRet;
	auto convertData = [&uuid](const LiveInfo &data) {
		QVariantMap mapData;
		mapData.insert(ChannelData::g_timeStamp, data.startDate / 1000);
		mapData.insert(ChannelData::g_nickName, data.title);
		mapData.insert(ChannelData::g_channelUUID, uuid);
		mapData.insert(ChannelData::g_platformName, NAVER_TV);
		return QVariant::fromValue(mapData);
	};
	std::transform(tmpList.cbegin(), tmpList.cend(), std::back_inserter(tmpRet), convertData);
	mySharedData().m_scheduleList = tmpRet;
}

void PLSPlatformNaverTV::onCheckStatus()
{
	liveStatus([this](bool ok, qint64, qint64, qint64, bool closed) {
		if (ok && closed) {
			checkStreamPublishingRetryTimes = 0;
			deleteTimer(checkStreamPublishingTimer);
			deleteTimer(checkStatusTimer);

			pls_alert_error_message(nullptr, tr("Alert.Title"), tr("LiveInfo.live.error.stoped.byRemote").arg("NAVER TV"));

			clearLiveInfo();
			clearLive();
			const char *platformName = getNameForChannelType();
			const char *abortReason = "live abort because live platform stop";
			PLS_LOGEX(PLS_LOG_WARN, MODULE_PlatformService, {{"liveAbortService", platformName}, {"liveAbortType", abortReason}}, "%s abort living.", platformName);

			PLS_LIVE_INFO_KR(MODULE_PLATFORM_NAVERTV, "FinishedBy Naver TV server end live, channel: %s", SUB_CHANNEL_NAME);
			PLS_LIVE_ABORT_INFO(MODULE_PlatformService, abortReason, "FinishedBy Naver TV server end live, channel: %s", SUB_CHANNEL_NAME_MASK);
			PLS_PLATFORM_API->sendLiveAnalog(false, "live abort because navertv stop streaming", common::ANALOG_LIVE_ABORT_NAVERTV_STOP);
			PLS_PLATFORM_API->stopStreaming("end live because Naver TV server end live");
		}
	});
}

void PLSPlatformNaverTV::onShowScheLiveNotice()
{
	if (isScheLiveNoticeShown) {
		return;
	}

	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV check schedule live notice, channel: %s", SUB_CHANNEL_NAME);
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV check schedule live notice, channel: %s", SUB_CHANNEL_NAME_MASK);

	getScheLives(
		0, 30 * 60,
		[this](bool ok, const QList<LiveInfo> &scheLiveInfos) {
			if (!ok || scheLiveInfos.isEmpty()) {
				return;
			}

			qint64 current = QDateTime::currentMSecsSinceEpoch();
			const LiveInfo *cliveInfo = nullptr;
			for (auto &sli : scheLiveInfos) {
				if (!cliveInfo) {
					cliveInfo = &sli;
				} else if (qAbs(current - sli.startDate) < qAbs(current - cliveInfo->startDate)) {
					cliveInfo = &sli;
				}
			}

			if (cliveInfo) {
				showScheLiveNotice(*cliveInfo);
			}
		},
		this, false, false);
}

QString PLSPlatformNaverTV::getChannelInfo(const QString &key, const QString &defaultValue) const
{
	auto iter = mySharedData().m_mapInitData.constFind(key);
	if (iter != mySharedData().m_mapInitData.constEnd()) {
		return iter.value().toString();
	}
	return defaultValue;
}
void PLSPlatformNaverTV::getJson(const Url &url, const char *log, const std::function<void(const QJsonDocument &)> &ok, const std::function<void(bool expired, int code)> &fail,
				 const QObject *receiver, ApiId apiId, const QVariantMap &headers, bool expiredNotify, bool popupNeedShow, bool popupGenericError, bool onlyFailLog)
{
	pls::http::Request request;
	if (!onlyFailLog) {
		request.withLog(url.maskingUrl);
	} else {
		request.withAfterLog(url.maskingUrl, pls::http::LogInclude::Fail);
	}

	QSet<const QObject *> receivers{this};
	if (receiver)
		receivers.insert(receiver);

	pls::http::request(
		request.method(pls::http::Method::Get)
			.hmacUrl(url.url, PLS_PC_HMAC_KEY.toUtf8())
			.rawHeaders(headers)
			.cookie(getChannelCookie())
			.jsonContentType()
			.receiver(receivers)
			.allowAbort(url.allowAbort)
			.jsonOkResult(
				[this, receivers, log, ok, onlyFailLog](const pls::http::Reply &, const QJsonDocument &json) {
					if (!onlyFailLog) {
						PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "%s success, channel: %s", log, SUB_CHANNEL_NAME);
						PLS_INFO(MODULE_PLATFORM_NAVERTV, "%s success, channel: %s", log, SUB_CHANNEL_NAME_MASK);
					}
					pls_async_call_mt(receivers, [ok, json]() { pls_invoke_safe(ok, json); });
				},
				[this, log, fail, receivers, receiver, expiredNotify, popupNeedShow, popupGenericError, apiId](const pls::http::Reply &reply, const QJsonParseError &error) {
					PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, error.errorString().toUtf8().constData());
					PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, error.errorString().toUtf8().constData());
					pls_async_call_mt(receivers, [receiver, statusCode = reply.statusCode(), expiredNotify, popupNeedShow, popupGenericError, apiId, fail, this]() {
						bool expired = false;
						int code = processError(expired, receiver, QNetworkReply::NoError, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
						pls_invoke_safe(fail, false, code);
					});
				})
			.jsonFailResult(
				[this, log, fail, receivers, receiver, expiredNotify, popupNeedShow, popupGenericError, apiId](const pls::http::Reply &reply, const QJsonDocument &json) {
					pls_async_call_mt(receivers,
							  [log, json, fail, receiver, error = reply.error(), statusCode = reply.statusCode(), expiredNotify, popupNeedShow, popupGenericError, apiId,
							   this]() { processFailed(log, json, fail, receiver, error, statusCode, expiredNotify, popupNeedShow, popupGenericError, apiId); });
				},
				[this, log, fail, receivers, receiver, expiredNotify, popupNeedShow, popupGenericError, apiId](const pls::http::Reply &reply, const QJsonParseError &) {
					PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME);
					PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME_MASK);
					pls_async_call_mt(receivers,
							  [receiver, fail, error = reply.error(), statusCode = reply.statusCode(), expiredNotify, popupNeedShow, popupGenericError, apiId, this]() {
								  bool expired = false;
								  int code = processError(expired, receiver, error, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
								  pls_invoke_safe(fail, false, code);
							  });
				}));
}
void PLSPlatformNaverTV::getJson(const Url &url, const QJsonObject &reqJson, const char *log, const std::function<void(const QJsonDocument &)> &ok,
				 const std::function<void(bool expired, int code)> &fail, const QObject *receiver, ApiId apiId, const QVariantMap &headers, bool expiredNotify, bool popupNeedShow,
				 bool popupGenericError)
{
	getOrPostJson(pls::http::Method::Get, url, reqJson, log, ok, fail, receiver, apiId, headers, expiredNotify, popupNeedShow, popupGenericError);
}
void PLSPlatformNaverTV::postJson(const Url &url, const QJsonObject &reqJson, const char *log, const std::function<void(const QJsonDocument &)> &ok,
				  const std::function<void(bool expired, int code)> &fail, const QObject *receiver, ApiId apiId, const QVariantMap &headers, bool expiredNotify, bool popupNeedShow,
				  bool popupGenericError)
{
	getOrPostJson(pls::http::Method::Post, url, reqJson, log, ok, fail, receiver, apiId, headers, expiredNotify, popupNeedShow, popupGenericError);
}
void PLSPlatformNaverTV::getOrPostJson(pls::http::Method method, const Url &url, const QJsonObject &reqJson, const char *log, const std::function<void(const QJsonDocument &)> &ok,
				       const std::function<void(bool expired, int code)> &fail, const QObject *receiver, ApiId apiId, const QVariantMap &headers, bool expiredNotify,
				       bool popupNeedShow, bool popupGenericError)
{
	QSet<const QObject *> receivers{this};
	if (receiver)
		receivers.insert(receiver);

	pls::http::request(
		pls::http::Request()
			.method(method)
			.hmacUrl(url.url, PLS_PC_HMAC_KEY.toUtf8())
			.rawHeaders(headers)
			.cookie(getChannelCookie())
			.jsonContentType()
			.withLog(url.maskingUrl)
			.receiver(receivers)
			.body(reqJson)
			.allowAbort(url.allowAbort)
			.jsonOkResult(
				[this, receivers, ok, log](const pls::http::Reply &, const QJsonDocument &json) {
					PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "%s success, channel: %s", log, SUB_CHANNEL_NAME);
					PLS_INFO(MODULE_PLATFORM_NAVERTV, "%s success, channel: %s", log, SUB_CHANNEL_NAME_MASK);
					pls_async_call_mt(receivers, [ok, json]() { pls_invoke_safe(ok, json); });
				},
				[this, fail, log, receivers, receiver, expiredNotify, popupNeedShow, popupGenericError, apiId](const pls::http::Reply &reply, const QJsonParseError &error) {
					PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, error.errorString().toUtf8().constData());
					PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, error.errorString().toUtf8().constData());
					pls_async_call_mt(receivers, [receiver, fail, statusCode = reply.statusCode(), expiredNotify, popupNeedShow, popupGenericError, apiId, this]() {
						bool expired = false;
						int code = processError(expired, receiver, QNetworkReply::NoError, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
						pls_invoke_safe(fail, false, code);
					});
				})
			.jsonFailResult(
				[this, fail, log, receivers, receiver, expiredNotify, popupNeedShow, popupGenericError, apiId](const pls::http::Reply &reply, const QJsonDocument &json) {
					pls_async_call_mt(receivers,
							  [log, json, fail, receiver, error = reply.error(), statusCode = reply.statusCode(), expiredNotify, popupNeedShow, popupGenericError, apiId,
							   this]() { processFailed(log, json, fail, receiver, error, statusCode, expiredNotify, popupNeedShow, popupGenericError, apiId); });
				},
				[this, fail, log, receivers, receiver, expiredNotify, popupNeedShow, popupGenericError, apiId](const pls::http::Reply &reply, const QJsonParseError &) {
					PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME);
					PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME_MASK);
					pls_async_call_mt(receivers,
							  [receiver, fail, error = reply.error(), statusCode = reply.statusCode(), expiredNotify, popupNeedShow, popupGenericError, apiId, this]() {
								  bool expired = false;
								  int code = processError(expired, receiver, error, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
								  pls_invoke_safe(fail, false, code);
							  });
				}));
}

void PLSPlatformNaverTV::processFailed(const char *log, const QJsonDocument &respJson, const std::function<void(bool expired, int code)> &fail, const QObject *receiver,
				       QNetworkReply::NetworkError networkError, int statusCode, bool expiredNotify, bool popupNeedShow, bool popupGenericError, ApiId apiId)
{
	QJsonObject object = respJson.object();
	if (object.contains(CSTR_CODE) && object.contains(CSTR_MESSAGE)) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, object.value(CSTR_MESSAGE).toString().toUtf8().constData());
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, object.value(CSTR_MESSAGE).toString().toUtf8().constData());

		bool expired = false;
		int code = object.value(CSTR_CODE).toInt();
		code = processError(expired, receiver, networkError, statusCode, code, expiredNotify, popupNeedShow, popupGenericError, apiId);
		pls_invoke_safe(fail, expired, code);
	} else if (object.contains(CSTR_ERROR_CODE) && object.contains(CSTR_ERROR_MESSAGE)) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, object.value(CSTR_ERROR_MESSAGE).toString().toUtf8().constData());
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, object.value(CSTR_ERROR_MESSAGE).toString().toUtf8().constData());

		bool expired = false;
		int code = processError(expired, receiver, networkError, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
		pls_invoke_safe(fail, expired, code);
	} else if (object.contains(CSTR_RTN_CD) && object.contains(CSTR_RTN_MSG)) {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, object.value(CSTR_RTN_MSG).toString().toUtf8().constData());
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, object.value(CSTR_RTN_MSG).toString().toUtf8().constData());

		bool expired = false;
		int code = processError(expired, receiver, networkError, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
		pls_invoke_safe(fail, expired, code);
	} else if (object.contains(CSTR_HEADER)) {
		QJsonObject header = object[CSTR_HEADER].toObject();
		if (header.contains(CSTR_MESSAGE) && header.contains(CSTR_CODE)) {
			PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME, header.value(CSTR_MESSAGE).toString().toUtf8().constData());
			PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: %s", log, SUB_CHANNEL_NAME_MASK, header.value(CSTR_MESSAGE).toString().toUtf8().constData());
		} else {
			PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME);
			PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME_MASK);
		}

		bool expired = false;
		int code = processError(expired, receiver, networkError, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
		pls_invoke_safe(fail, expired, code);
	} else {
		PLS_ERROR_KR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME);
		PLS_ERROR(MODULE_PLATFORM_NAVERTV, "%s failed, channel: %s, reason: unknown", log, SUB_CHANNEL_NAME_MASK);

		bool expired = false;
		int code = processError(expired, receiver, networkError, statusCode, ERROR_OTHERS, expiredNotify, popupNeedShow, popupGenericError, apiId);
		pls_invoke_safe(fail, false, code);
	}
}
int PLSPlatformNaverTV::processError(bool &expired, const QObject *receiver, QNetworkReply::NetworkError networkError, int statusCode, int code, bool expiredNotify, bool popupNeedShow,
				     bool popupGenericError, ApiId apiId)
{
	if ((networkError >= QNetworkReply::ConnectionRefusedError) && (networkError <= QNetworkReply::UnknownNetworkError)) {
		mySharedData().m_lastError = createScheduleGetError(getChannelName(), channel_data::NetWorkErrorType::NetWorkNoStable);
		if (popupNeedShow && pls_object_is_valid(receiver)) {
			PLSAlertView::warning(nullptr, tr("Alert.Title"), tr("login.check.note.network"));
		}
		return ERROR_NETWORK_ERROR;
	}

	if (statusCode == 401 && code == ERROR_AUTHEXCEPTION) {
		mySharedData().m_lastError = createScheduleGetError(getChannelName(), channel_data::NetWorkErrorType::PlatformExpired);
		expired = true;
		emit apiRequestFailed(true);
		tokenExpired(expiredNotify, popupNeedShow);
		return code;
	}

	emit apiRequestFailed(false);
	if (!popupNeedShow) {
		return code;
	}

	switch (code) {
	case ERROR_LIVENOTFOUND:
		pls_alert_error_message(nullptr, tr("Alert.Title"), tr("broadcast.invalid.schedule"), QString::number(ERROR_LIVENOTFOUND));
		break;
	case ERROR_PERMITEXCEPTION:
		pls_alert_error_message(nullptr, tr("Alert.Title"), tr("main.message.error.navertv.service.10003"), QString::number(ERROR_PERMITEXCEPTION));
		break;
	case ERROR_LIVESTATUSEXCEPTION:
		pls_alert_error_message(nullptr, tr("Alert.Title"), tr("broadcast.no.longer.valid"), QString::number(ERROR_LIVESTATUSEXCEPTION));
		break;
	case ERROR_START30MINEXCEPTION:
		pls_alert_error_message(nullptr, tr("Alert.Title"), tr("main.message.error.navertv.service.10007"));
		break;
	case ERROR_ALREADYONAIR:
		pls_alert_error_message(nullptr, tr("Alert.Title"), tr("main.message.error.navertv.service.10008"));
		break;
	case ERROR_PAIDSPONSORSHIPINFO:
		pls_alert_error_message(nullptr, tr("Alert.Title"), tr("main.message.error.navertv.service.10010"), QString::number(ERROR_PAIDSPONSORSHIPINFO));
		break;
	default:
		if (popupGenericError) {
			if (apiId == ApiId::LiveOpen) {
				pls_alert_error_message(nullptr, tr("Alert.Title"), tr("main.message.error.navertv.service.unknown"));
			} else {
				pls_alert_error_message(nullptr, tr("Alert.Title"), tr("server.unknown.error"));
			}
		}
		break;
	}
	return code;
}

void PLSPlatformNaverTV::tokenExpired(bool expiredNotify, bool popupNeedShow)
{
	auto navertv = pls_qobject_ptr<PLSPlatformNaverTV>(this);

	PLS_INFO_KR(MODULE_PLATFORM_NAVERTV, "Naver TV platform token expired, channel: %s, UUID = %s.", SUB_CHANNEL_NAME, getChannelUUID().toUtf8().constData());
	PLS_INFO(MODULE_PLATFORM_NAVERTV, "Naver TV platform token expired, channel: %s, UUID = %s.", SUB_CHANNEL_NAME_MASK, getChannelUUID().toUtf8().constData());

	if (popupNeedShow && (pls_alert_error_message(nullptr, tr("Alert.Title"), tr("Live.Check.LiveInfo.Refresh.NaverTV.Expired")) == PLSAlertView::Button::NoButton)) {
		expiredNotify = false;
	}

	if (expiredNotify) {
		PLSCHANNELS_API->channelExpired(getChannelUUID(), !popupNeedShow);
	}

	if (navertv.valid()) {
		emit closeDialogByExpired();
	}

	clearToken();
}
