#include "PLSOutro.hpp"
#include <obs.h>
#include <QTimer>
#include <QPainter>
#include <qfontmetrics.h>
#include <assert.h>
#include "obs.hpp"
#include "libutils-api.h"
#include "pls/pls-obs-api.h"
#include "liblog.h"
#include "log/module_names.h"

#define DEFAULT_DURATION_MS 3000
#define USE_QT_DRAW_TEXT 1

static vec2 GetItemSize(obs_sceneitem_t *item)
{
	obs_bounds_type boundsType = obs_sceneitem_get_bounds_type(item);
	vec2 size;

	if (boundsType != OBS_BOUNDS_NONE) {
		obs_sceneitem_get_bounds(item, &size);
	} else {
		obs_source_t *source = obs_sceneitem_get_source(item);
		obs_sceneitem_crop crop;
		vec2 scale;

		obs_sceneitem_get_scale(item, &scale);
		obs_sceneitem_get_crop(item, &crop);
		size.x = float(obs_source_get_width(source) - crop.left - crop.right) * scale.x;
		size.y = float(obs_source_get_height(source) - crop.top - crop.bottom) * scale.y;
	}

	return size;
}

static inline QColor color_from_int(long long val)
{
	return QColor(val & 0xff, (val >> 8) & 0xff, (val >> 16) & 0xff, (val >> 24) & 0xff);
}

// All the functions should only be called from UI thread.
class PLSOutroPandatv : public PLSOutro {
public:
	using PLSOutro::PLSOutro;
	virtual ~PLSOutroPandatv() { Clear(); }

#if !USE_QT_DRAW_TEXT
	OBSWeakSource m_textWeakSource;
#endif

	// Inherited via PLSOutro
	virtual void Start() override
	{
		if (m_bRunning)
			return;

		m_pScene = obs_scene_create_private("Outro Scene");
		if (!m_pScene) {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to create scene.", (void *)this);
			return;
		}

		// create a color source
		m_pColorSource = obs_source_create_private("color_source", "Outro background color", BuildBgColorSettings(m_settings));
		if (m_pColorSource) {
			m_pColorItem = obs_scene_add(m_pScene, m_pColorSource);
			obs_sceneitem_set_visible(m_pColorItem, false);
		} else {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to create color source.", (void *)this);
		}

		// create a image source
		m_pImageSource = obs_source_create_private("image_source", "Outro image", BuildImageSettings(m_settings));
		if (m_pImageSource) {
			m_pImageItem = obs_scene_add(m_pScene, m_pImageSource);
			obs_sceneitem_set_visible(m_pImageItem, false);
		} else {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to create image source.", (void *)this);
		}

#if !USE_QT_DRAW_TEXT
		// create a text source
#ifdef _WIN32
		const char *text_source_id = "text_gdiplus";
#else
		const char *text_source_id = "text_ft2_source";
#endif
		m_pTextSource = obs_source_create_private(text_source_id, "Outro text", BuildTextSettings(m_settings));
		if (m_pTextSource) {
			m_pTextItem = obs_scene_add(m_pScene, m_pTextSource);
			obs_sceneitem_set_visible(m_pTextItem, false);
			m_textWeakSource = OBSGetWeakRef(m_pTextSource);
		} else {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to create text source.", (void *)this);
		}

		obs_add_main_rendered_callback(renderdCallback, this);
#else
		m_pImageText = obs_source_create_private("image_source", "Outro text", BuildTextSettings(m_settings));
		if (m_pImageText) {
			m_pTextItem = obs_scene_add(m_pScene, m_pImageText);
			obs_sceneitem_set_visible(m_pTextItem, false);
		} else {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to create text source.", (void *)this);
		}
#endif

		// bind channel source
		obs_set_output_source(static_cast<uint32_t>(CustomChannel::CHANNEL_OUTRO), obs_scene_get_source(m_pScene));

		InitGeometry(m_settings);

		if (m_settings.contains("timeoutMs")) {
			m_durationMs = m_settings.value("timeoutMs").toInt(DEFAULT_DURATION_MS);
		} else {
			assert(false);
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to apply outro duration, use default value: %d ms.", (void *)this, DEFAULT_DURATION_MS);
			m_durationMs = DEFAULT_DURATION_MS;
		}

		if (!m_pTimer) {
			m_pTimer = new QTimer();
			QObject::connect(m_pTimer, &QTimer::timeout, [this]() { Stop(); });
		}
		m_pTimer->setInterval(m_durationMs);
		m_pTimer->setSingleShot(true);

		m_bRunning = true;

		m_pTimer->start();

		/*
		* mute all output audio
		*/
		pls_set_all_mute(true);

		if (m_callback)
			m_callback(OutroState::Started);
	}
	virtual void Stop() override
	{
		Clear();
		if (m_callback)
			m_callback(OutroState::Stopped);
	}
	virtual void Update(const QJsonObject &settings) override {}
	virtual bool IsRunning() override { return m_bRunning; }

	void AsyncUpdateGeometry()
	{
		pls_async_call_mt([this]() {
			InitGeometry(m_settings, true);
			obs_remove_main_rendered_callback(renderdCallback, this);
		});
	}

	static void renderdCallback(void *param)
	{
#if !USE_QT_DRAW_TEXT
		auto context = (PLSOutroPandatv *)param;
		if (context) {
			if (OBSSource source = OBSGetStrongRef(context->m_textWeakSource); source) {
				if (obs_source_get_width(source) > 0 && obs_source_get_height(source) > 0)
					context->AsyncUpdateGeometry();
			}
		}
#endif
	}

private:
	OBSDataAutoRelease BuildImageSettings(const QJsonObject &settings)
	{
		OBSDataAutoRelease data = obs_data_create();
		bool checkOk = false;
		do {
			if (!settings.contains("file")) {
				PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to load outro: invalid parameter.", (void *)this);
				break;
			}

			if (settings.value("file").toString().isEmpty()) {
				PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to load outro: outro file path is empty.", (void *)this);
				break;
			}

			if (!QFile::exists(settings.value("file").toString())) {
				PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to load outro: outro file does not exist.", (void *)this);
				break;
			}

			checkOk = true;

		} while (false);

		assert(checkOk);
		if (!checkOk)
			return data;

		obs_data_set_string(data, "file", settings.value("file").toString().toUtf8().constData());
		obs_data_set_bool(data, "unload", false);
		obs_data_set_bool(data, "linear_alpha", false);

		return data;
	}

	OBSDataAutoRelease BuildTextSettings(const QJsonObject &settings)
	{
		OBSDataAutoRelease data = obs_data_create();
#if USE_QT_DRAW_TEXT
		auto imagePath = GenerateTextImage(settings);
		if (imagePath.isEmpty()) {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to generate text image by Qt.", (void *)this);
			return data;
		}
		if (!QFile::exists(imagePath)) {
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] The text image generated by Qt dose not exist.", (void *)this);
			return data;
		}

		obs_data_set_string(data, "file", imagePath.toUtf8().constData());
		obs_data_set_bool(data, "unload", false);
		obs_data_set_bool(data, "linear_alpha", false);
#else
		if (!settings.contains("text") || settings.value("text").toString().isEmpty()) {
			assert(false);
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to apply outro text: outro text is empty.", (void *)this);
			return data;
		}

		obs_data_set_string(data, "text", settings.value("text").toString().toUtf8().constData());

		OBSDataAutoRelease font = obs_data_create();

		if (settings.contains("face")) {
			auto face = settings.value("face").toString().toUtf8().constData();
			obs_data_set_string(font, "face", face);
		} else {
#if defined(_WIN32)
			obs_data_set_string(font, "face", "Malgun Gothic");
#elif defined(__APPLE__)
			obs_data_set_string(font, "face", "Pretendard Regular");
#else
			obs_data_set_string(font, "face", "Monospace");
#endif
		}

		if (settings.contains("flags")) {
			obs_data_set_int(font, "flags", settings.value("flags").toInt());
		} else {
			obs_data_set_int(font, "flags", 0);
		}

		if (settings.contains("size")) {
			obs_data_set_int(font, "size", settings.value("size").toInt());
		} else {
			obs_data_set_int(font, "size", 30);
		}

		obs_data_set_obj(data, "font", font);
		obs_data_set_bool(data, "outline", false);
		obs_data_set_string(data, "align", "center");
		obs_data_set_string(data, "valign", "top");
#if __APPLE__
		obs_data_set_int(data, "color1", 0xF3F3F3FF);
		obs_data_set_int(data, "color2", 0xF3F3F3FF);
#else
		obs_data_set_int(data, "color", 0xF3F3F3);
#endif
#endif

		return data;
	}

	OBSDataAutoRelease BuildBgColorSettings(const QJsonObject &settings)
	{
		OBSDataAutoRelease data = obs_data_create();
		obs_data_set_int(data, "color", 0xFF000000);

		obs_video_info ovi;
		obs_get_video_info(&ovi);

		obs_data_set_int(data, "width", ovi.base_width);
		obs_data_set_int(data, "height", ovi.base_height);

		return data;
	}

	void InitGeometry(const QJsonObject &settings, bool asyncCall = false)
	{
		if (!m_pImageItem || !m_pTextItem || !m_pColorItem)
			return;

		vec2 pos;
		float spacing = 26.0f;

		obs_video_info ovi;
		vec3 screenCenter;
		obs_get_video_info(&ovi);
		vec3_set(&screenCenter, float(ovi.base_width), float(ovi.base_height), 0.0f);
		vec3_mulf(&screenCenter, &screenCenter, 0.5f);

		// bg color
		{
			pos.x = 0;
			pos.y = 0;

			vec2 scale;
			scale.x = 1.0;
			scale.y = 1.0;

			obs_sceneitem_set_pos(m_pColorItem, &pos);
			obs_sceneitem_set_scale(m_pColorItem, &scale);
		}

		// image
		{
			float radio = 1.0f;

			vec2 scale;
			scale.x = radio;
			scale.y = radio;

			vec2 baseSize;
			baseSize.x = (float)obs_source_get_width(m_pImageSource);
			baseSize.y = (float)obs_source_get_height(m_pImageSource);

			obs_sceneitem_set_bounds_type(m_pImageItem, OBS_BOUNDS_STRETCH);
			obs_sceneitem_set_bounds_alignment(m_pImageItem, OBS_ALIGN_CENTER);

			struct vec2 boundSize;
			do {
				auto build_horizon = [&boundSize, baseSize]() {
					boundSize.y = b2b_local::HLOGO_FIX_HEIGHT_PIX;
					auto scale_ratio = b2b_local::HLOGO_FIX_HEIGHT_PIX / baseSize.y;
					boundSize.x = std::min(b2b_local::HLOGO_MAX_WIDTH_PIX, baseSize.x * scale_ratio);
				};
				auto build_vertical = [&boundSize, baseSize]() {
					boundSize.y = b2b_local::VLOGO_FIX_HEIGHT_PIX;
					auto scale_ratio = b2b_local::VLOGO_FIX_HEIGHT_PIX / baseSize.y;
					boundSize.x = std::min(b2b_local::VLOGO_MAX_WIDTH_PIX, baseSize.x * scale_ratio);
				};

				if ((int)b2b_service::HLOGO_FIX_HEIGHT_PIX == (int)baseSize.y) {
					build_horizon();
					break;
				}

				if ((int)b2b_service::VLOGO_FIX_HEIGHT_PIX == (int)baseSize.y) {
					build_vertical();
					break;
				}

				if (baseSize.x > baseSize.y) {
					build_horizon();
				} else {
					build_vertical();
				}
			} while (false);

#if USE_QT_DRAW_TEXT
			float contentHeight = boundSize.y + (float)obs_source_get_height(m_pImageText) + spacing;
#else
			float contentHeight = boundSize.y + (float)obs_source_get_height(m_pTextSource) + spacing;
#endif
			float cy_offset = screenCenter.y - contentHeight / 2.0;

			pos.x = screenCenter.x - boundSize.x * radio / 2.0f;
			pos.y = cy_offset;

			obs_sceneitem_set_pos(m_pImageItem, &pos);
			obs_sceneitem_set_bounds(m_pImageItem, &boundSize);
		}

		//text
		{
			float radio = 1.0f;

			vec2 scale;
			scale.x = radio;
			scale.y = radio;

			vec2 baseSize;
#if USE_QT_DRAW_TEXT
			baseSize.x = (float)obs_source_get_width(m_pImageText);
			baseSize.y = (float)obs_source_get_height(m_pImageText);
#else
			baseSize.x = (float)obs_source_get_width(m_pTextSource);
			baseSize.y = (float)obs_source_get_height(m_pTextSource);
#endif

			vec2 imageItemSize = GetItemSize(m_pImageItem);

			pos.x = screenCenter.x - baseSize.x * radio / 2.0f;
			pos.y = pos.y + imageItemSize.y + spacing;

			obs_sceneitem_set_pos(m_pTextItem, &pos);
			obs_sceneitem_set_scale(m_pTextItem, &scale);
		}

#if !USE_QT_DRAW_TEXT
		if (asyncCall) {
			obs_sceneitem_set_visible(m_pImageItem, true);
			obs_sceneitem_set_visible(m_pTextItem, true);
			obs_sceneitem_set_visible(m_pColorItem, true);
		}
#else
		obs_sceneitem_set_visible(m_pImageItem, true);
		obs_sceneitem_set_visible(m_pTextItem, true);
		obs_sceneitem_set_visible(m_pColorItem, true);
#endif
	}

	void Clear()
	{
		pls_set_all_mute(false);

#if !USE_QT_DRAW_TEXT
		obs_remove_main_rendered_callback(renderdCallback, this);
#endif

		if (m_pTimer) {
			m_pTimer->stop();
			m_pTimer->deleteLater();
		}

		obs_set_output_source(m_channel, nullptr);

		if (m_pScene)
			obs_source_remove(obs_scene_get_source(m_pScene));

		//color
		if (m_pColorItem) {
			obs_sceneitem_remove(m_pColorItem);
			m_pColorItem = nullptr;
		}

		// image
		if (m_pImageItem) {
			obs_sceneitem_remove(m_pImageItem);
			m_pImageItem = nullptr;
		}

		if (m_pImageSource) {
			obs_source_release(m_pImageSource);
			m_pImageSource = nullptr;
		}

		// text
		if (m_pTextItem) {
			obs_sceneitem_remove(m_pTextItem);
			m_pTextItem = nullptr;
		}

#if USE_QT_DRAW_TEXT
		if (m_pImageText) {
			obs_source_release(m_pImageText);
			m_pImageText = nullptr;
		}
#else
		if (m_pTextSource) {
			obs_source_release(m_pTextSource);
			m_pTextSource = nullptr;
		}
#endif

		if (m_pScene) {
			obs_scene_release(m_pScene);
			m_pScene = nullptr;
		}

		m_bRunning = false;
	}

	QString GenerateTextImage(const QJsonObject &settings)
	{
		if (settings.isEmpty())
			return QString();

		QFont font;
		QString text;

		if (!settings.contains("text") || settings.value("text").toString().isEmpty()) {
			assert(false);
			PLS_WARN(PLS_MODULE_NAME_OUTRO, "[%p] Failed to apply outro text: outro text is empty.", (void *)this);
			return QString();
		}

		text = settings.value("text").toString();

		if (settings.contains("flags")) {
			font.setBold(settings.value("flags").toInt() & OBS_FONT_BOLD);
		} else {
			font.setBold(false);
		}

#if defined(_WIN32)
		font.setFamily("Malgun Gothic");
#elif defined(__APPLE__)
		font.setFamily("Pretendard Regular");
#else
		font.setFamily("Monospace");
#endif

		if (settings.contains("size")) {
			font.setPointSize(settings.value("size").toInt());
		} else {
			font.setPointSize(30);
		}

		QPen pen;
		pen.setColor(color_from_int(0xF3F3F3FF));

		obs_video_info ovi;
		vec3 screenCenter;
		obs_get_video_info(&ovi);

		QFontMetrics fontMetrics(font);
		auto screenWidth = static_cast<int>(ovi.base_width);
		int flags = Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap;
		auto textRect = fontMetrics.boundingRect(QRect(0, 0, screenWidth, 1000), flags, text);

		QPixmap pixmap(textRect.width(), textRect.height());
		pixmap.fill(color_from_int(0xFF000000));
		QPainter painter(&pixmap);
		painter.setRenderHints(painter.renderHints() | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

		painter.setFont(font);
		painter.setPen(pen);
		
		painter.drawText(pixmap.rect(), flags, text);

		auto savedFileName = pls_get_app_data_dir(QStringLiteral("PRISMLiveStudio/library/Library_Policy_PC/")) + QStringLiteral("outro_text.png");
		bool resut = pixmap.save(savedFileName);
		return resut ? savedFileName : QString();
	}

	obs_scene_t *m_pScene = nullptr;
	obs_source_t *m_pImageSource = nullptr;
#if !USE_QT_DRAW_TEXT
	obs_source_t *m_pTextSource = nullptr;
#else
	obs_source_t *m_pImageText = nullptr;
#endif
	obs_source_t *m_pColorSource = nullptr;
	obs_sceneitem_t *m_pImageItem = nullptr;
	obs_sceneitem_t *m_pTextItem = nullptr;
	obs_sceneitem_t *m_pColorItem = nullptr;
	bool m_bRunning = false;
	QPointer<QTimer> m_pTimer;
};

PLSOutro::PLSOutro(const QJsonObject &settings, uint32_t channel, OutroStateCallback callback) : m_settings(settings), m_channel(channel), m_callback(callback) {}

std::shared_ptr<PLSOutro> pls_create_outro(const QJsonObject &settings, CustomChannel channel, OutroStateCallback callback)
{
	return std::shared_ptr<PLSOutro>(new PLSOutroPandatv(settings, static_cast<uint32_t>(channel), callback));
}
