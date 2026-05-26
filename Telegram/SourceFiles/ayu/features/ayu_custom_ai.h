/*
This is the source code of AyuGram for Desktop.

We do not and cannot prevent the use of our code,
but be respectful and credit the original author.

Copyright @Radolyn, 2026
*/
#pragma once

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <functional>

namespace AyuCustomAi {

using Callback = std::function<void(const QString &result, const QString &error)>;

void sendRequest(
	const QString &systemPrompt,
	const QString &userMessage,
	Callback callback);

bool isConfigured();

} // namespace AyuCustomAi
