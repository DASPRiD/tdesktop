/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Platform {

void start();
void finish();

enum class PermissionStatus {
	Granted,
	CanRequest,
	Denied,
};

enum class PermissionType {
	Microphone,
};

enum class SystemSettingsType {
	Audio,
};

void SetWatchingMediaKeys(bool watching);
bool IsApplicationActive();
void SetApplicationIcon(const QIcon &icon);
bool TranslucentWindowsSupported(QPoint globalPosition);
void StartTranslucentPaint(QPainter &p, QPaintEvent *e);
void InitOnTopPanel(QWidget *panel);
void DeInitOnTopPanel(QWidget *panel);
void ReInitOnTopPanel(QWidget *panel);
void RegisterCustomScheme();
PermissionStatus GetPermissionStatus(PermissionType type);
void RequestPermission(PermissionType type, Fn<void(PermissionStatus)> resultCallback);
void OpenSystemSettingsForPermission(PermissionType type);
bool OpenSystemSettings(SystemSettingsType type);

[[nodiscard]] std::optional<crl::time> LastUserInputTime();
[[nodiscard]] inline bool LastUserInputTimeSupported() {
	return LastUserInputTime().has_value();
}

[[nodiscard]] constexpr bool UseMainQueueGeneric();
void DrainMainQueue(); // Needed only if UseMainQueueGeneric() is false.

namespace ThirdParty {

void start();
void finish();

} // namespace ThirdParty
} // namespace Platform

#ifdef Q_OS_MAC
#include "platform/mac/specific_mac.h"
#elif defined Q_OS_LINUX // Q_OS_MAC
#include "platform/linux/specific_linux.h"
#elif defined Q_OS_WIN // Q_OS_MAC || Q_OS_LINUX
#include "platform/win/specific_win.h"
#endif // Q_OS_MAC || Q_OS_LINUX || Q_OS_WIN
