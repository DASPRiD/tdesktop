/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/binary_guard.h"
#include "emoji.h"
#include "lang/lang_keys.h"

namespace Ui {
namespace Emoji {
namespace internal {

[[nodiscard]] QString CacheFileFolder();
[[nodiscard]] QString SetDataPath(int id);

} // namespace internal

constexpr auto kRecentLimit = 42;

void Init();
void Clear();

void ClearIrrelevantCache();

struct Set {
	int id = 0;
	int postId = 0;
	int size = 0;
	QString name;
	QString previewPath;
};

// Thread safe, callback is called on main thread.
void SwitchToSet(int id, Fn<void(bool)> callback);

tr::phrase<> CategoryTitle(int index);

std::vector<Set> Sets();
int CurrentSetId();
bool SetIsReady(int id);
rpl::producer<> Updated();

int GetSizeNormal();
int GetSizeLarge();
#if defined Q_OS_MAC && !defined OS_MAC_OLD
int GetSizeTouchbar();
#endif

class One {
	struct CreationTag {
	};

public:
	One(One &&other) = default;
	One(const QString &id, EmojiPtr original, uint32 index, bool hasPostfix, bool colorizable, const CreationTag &)
	: _id(id)
	, _original(original)
	, _index(index)
	, _hasPostfix(hasPostfix)
	, _colorizable(colorizable) {
		Expects(!_colorizable || !colored());
	}

	QString id() const {
		return _id;
	}
	QString text() const {
		return hasPostfix() ? (_id + QChar(kPostfix)) : _id;
	}

	bool colored() const {
		return (_original != nullptr);
	}
	EmojiPtr original() const {
		return _original ? _original : this;
	}
	QString nonColoredId() const {
		return original()->id();
	}

	bool hasPostfix() const {
		return _hasPostfix;
	}

	bool hasVariants() const {
		return _colorizable || colored();
	}
	int variantsCount() const;
	int variantIndex(EmojiPtr variant) const;
	EmojiPtr variant(int index) const;

	int index() const {
		return _index;
	}
	int sprite() const {
		return int(_index >> 9);
	}
	int row() const {
		return int((_index >> 5) & 0x0FU);
	}
	int column() const {
		return int(_index & 0x1FU);
	}

	QString toUrl() const {
		return qsl("emoji://e.") + QString::number(index());
	}

private:
	const QString _id;
	const EmojiPtr _original = nullptr;
	const uint32 _index = 0;
	const bool _hasPostfix = false;
	const bool _colorizable = false;

	friend void internal::Init();

};

inline EmojiPtr FromUrl(const QString &url) {
	auto start = qstr("emoji://e.");
	if (url.startsWith(start)) {
		return internal::ByIndex(url.midRef(start.size()).toInt()); // skip emoji://e.
	}
	return nullptr;
}

inline EmojiPtr Find(const QChar *start, const QChar *end, int *outLength = nullptr) {
	return internal::Find(start, end, outLength);
}

inline EmojiPtr Find(const QString &text, int *outLength = nullptr) {
	return Find(text.constBegin(), text.constEnd(), outLength);
}

QString IdFromOldKey(uint64 oldKey);

inline EmojiPtr FromOldKey(uint64 oldKey) {
	return Find(IdFromOldKey(oldKey));
}

inline int ColorIndexFromCode(uint32 code) {
	switch (code) {
	case 0xD83CDFFBU: return 1;
	case 0xD83CDFFCU: return 2;
	case 0xD83CDFFDU: return 3;
	case 0xD83CDFFEU: return 4;
	case 0xD83CDFFFU: return 5;
	}
	return 0;
}

inline int ColorIndexFromOldKey(uint64 oldKey) {
	return ColorIndexFromCode(uint32(oldKey & 0xFFFFFFFFLLU));
}

void ReplaceInText(TextWithEntities &result);
RecentEmojiPack &GetRecent();
void AddRecent(EmojiPtr emoji);
rpl::producer<> UpdatedRecent();

const QPixmap &SinglePixmap(EmojiPtr emoji, int fontHeight);
void Draw(QPainter &p, EmojiPtr emoji, int size, int x, int y);

class UniversalImages {
public:
	explicit UniversalImages(int id);

	int id() const;
	bool ensureLoaded();
	void clear();

	void draw(QPainter &p, EmojiPtr emoji, int size, int x, int y) const;

	// This method must be thread safe and so it is called after
	// the _id value is fixed and all _sprites are loaded.
	QImage generate(int size, int index) const;

private:
	const int _id = 0;
	std::vector<QImage> _sprites;

};

const std::shared_ptr<UniversalImages> &SourceImages();
void ClearSourceImages(const std::shared_ptr<UniversalImages> &images);

} // namespace Emoji
} // namespace Ui
