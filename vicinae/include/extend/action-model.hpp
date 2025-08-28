#pragma once
#include "extend/image-model.hpp"
#include <qjsonobject.h>
#include <qnamespace.h>

struct KeyboardShortcutModel {
  QString key;
  QStringList modifiers;

  static KeyboardShortcutModel submit() { return {.key = "return", .modifiers = {"shift"}}; }
  static KeyboardShortcutModel cut() { return {.key = "X", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel enter() { return {.key = "return"}; }
};

struct ActionModel {
  QString title;
  QString onAction;
  std::optional<QString> onSubmit;
  std::optional<ImageLikeModel> icon;
  std::optional<KeyboardShortcutModel> shortcut;
};

struct ActionPannelSectionModel {
  QString title;
  QList<ActionModel> actions;
};

struct ActionPannelSubmenuModel {
  QString title;
  std::optional<ImageLikeModel> icon;
  QString onOpen;
  QString onSearchTextChange;
  QList<std::variant<ActionPannelSectionModel, ActionModel>> children;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionModel, ActionPannelSubmenuModel>;

struct ActionPannelModel {
  bool dirty;
  QString title;
  std::vector<ActionPannelItem> children;
};

class ActionPannelParser {
  KeyboardShortcutModel parseKeyboardShortcut(const QJsonObject &shortcut);
  ActionModel parseAction(const QJsonObject &instance);

  ActionPannelSectionModel parseActionPannelSection(const QJsonObject &instance);
  ActionPannelSubmenuModel parseActionPannelSubmenu(const QJsonObject &instance);

public:
  ActionPannelParser();
  ActionPannelModel parse(const QJsonObject &instance);
};
