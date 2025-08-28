#include "root-search/extensions/extension-root-provider.hpp"
#include "actions/extension/extension-actions.hpp"
#include "action-panel/action-panel.hpp"
#include "actions/fallback-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "command-actions.hpp"
#include "extension/extension-command.hpp"
#include "navigation-controller.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/action-pannel/action.hpp"

QString CommandRootItem::displayName() const { return m_command->name(); }
QString CommandRootItem::subtitle() const { return m_command->repositoryDisplayName(); }
ImageURL CommandRootItem::iconUrl() const { return m_command->iconUrl(); }
ArgumentList CommandRootItem::arguments() const { return m_command->arguments(); }
QString CommandRootItem::providerId() const { return "command"; }
bool CommandRootItem::isSuitableForFallback() const { return m_command->isFallback(); }
double CommandRootItem::baseScoreWeight() const { return 1.1; }
QString CommandRootItem::typeDisplayName() const { return "Command"; }

std::unique_ptr<ActionPanelState> CommandRootItem::newActionPanel(ApplicationContext *ctx,
                                                                  const RootItemMetadata &metadata) {
  auto panel = std::make_unique<ActionPanelState>();
  auto open = new OpenBuiltinCommandAction(m_command, "Open command");
  auto resetRanking = new ResetItemRanking(uniqueId());
  auto markAsFavorite = new ToggleItemAsFavorite(uniqueId(), metadata.favorite);
  auto mainSection = panel->createSection();
  auto itemSection = panel->createSection();
  auto dangerSection = panel->createSection();
  auto copyDeeplink = new CopyToClipboardAction(Clipboard::Text(m_command->deeplink()), "Copy deeplink");

  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));
  itemSection->addAction(resetRanking);
  itemSection->addAction(markAsFavorite);
  itemSection->addAction(copyDeeplink);
  dangerSection->addAction(new DisableApplication(uniqueId()));

  if (m_command->type() == CommandType::CommandTypeExtension) {
    auto cmd = static_cast<ExtensionCommand *>(m_command.get());

    dangerSection->addAction(new UninstallExtensionAction(cmd->extensionId()));
  }

  return panel;
}

ActionPanelView *CommandRootItem::fallbackActionPanel() const {
  auto panel = new ActionPanelStaticListView;
  // auto ui = ServiceRegistry::instance()->UI();

  // TODO: fix this
  panel->addAction(new OpenBuiltinCommandAction(m_command, "Open command", ""));
  panel->addAction(new ManageFallbackActions);

  return panel;
}

QString CommandRootItem::uniqueId() const { return QString("extension.%1").arg(m_command->uniqueId()); }

AccessoryList CommandRootItem::accessories() const {
  return {{.text = "Command", .color = SemanticColor::TextSecondary}};
}

std::vector<std::shared_ptr<RootItem>> ExtensionRootProvider::loadItems() const {
  return m_repo->commands() | std::views::transform([](const auto &command) {
           return std::static_pointer_cast<RootItem>(std::make_shared<CommandRootItem>(command));
         }) |
         std::ranges::to<std::vector>();
}
