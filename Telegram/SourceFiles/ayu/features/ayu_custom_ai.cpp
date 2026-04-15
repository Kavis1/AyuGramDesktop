/*
This is the source code of AyuGram for Desktop.

We do not and cannot prevent the use of our code,
but be respectful and credit the original author.

Copyright @Radolyn, 2026
*/
#include "ayu/features/ayu_custom_ai.h"

#include "ayu/ayu_settings.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>

namespace AyuCustomAi {
namespace {

QNetworkAccessManager &networkManager() {
	static auto instance = QNetworkAccessManager();
	return instance;
}

} // namespace

bool isConfigured() {
	const auto &settings = AyuSettings::getInstance();
	return !settings.customAiApiKey().isEmpty()
		&& !settings.customAiApiUrl().isEmpty()
		&& !settings.customAiModel().isEmpty();
}

void sendRequest(
		const QString &systemPrompt,
		const QString &userMessage,
		Callback callback) {
	const auto &settings = AyuSettings::getInstance();
	const auto apiUrl = settings.customAiApiUrl();
	const auto apiKey = settings.customAiApiKey();
	const auto model = settings.customAiModel();

	if (apiKey.isEmpty()) {
		if (callback) {
			callback({}, "API key is not configured");
		}
		return;
	}

	auto messages = QJsonArray();
	if (!systemPrompt.isEmpty()) {
		messages.append(QJsonObject{
			{"role", "system"},
			{"content", systemPrompt},
		});
	}
	messages.append(QJsonObject{
		{"role", "user"},
		{"content", userMessage},
	});

	auto body = QJsonObject{
		{"model", model},
		{"messages", messages},
		{"max_tokens", 4096},
	};

	auto url = apiUrl;
	if (!url.endsWith("/chat/completions")) {
		if (!url.endsWith('/')) {
			url += '/';
		}
		url += "chat/completions";
	}

	auto request = QNetworkRequest(QUrl(url));
	request.setHeader(
		QNetworkRequest::ContentTypeHeader,
		"application/json");
	request.setRawHeader(
		"Authorization",
		("Bearer " + apiKey).toUtf8());

	auto reply = networkManager().post(
		request,
		QJsonDocument(body).toJson(QJsonDocument::Compact));

	QObject::connect(reply, &QNetworkReply::finished, [reply, callback] {
		reply->deleteLater();

		if (reply->error() != QNetworkReply::NoError) {
			if (callback) {
				callback({}, reply->errorString());
			}
			return;
		}

		const auto data = reply->readAll();
		const auto doc = QJsonDocument::fromJson(data);
		const auto choices = doc.object().value("choices").toArray();
		if (choices.isEmpty()) {
			if (callback) {
				callback({}, "Empty response from AI");
			}
			return;
		}

		const auto message = choices.first()
			.toObject()
			.value("message")
			.toObject();
		const auto content = message.value("content").toString();
		if (callback) {
			callback(content, {});
		}
	});
}

} // namespace AyuCustomAi
