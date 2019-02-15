/*
 * FormalisedLog - Tool for creation of standardised log book entires at XFEL and similar DESY facilities
 */

#ifndef INCLUDE_DEPLOY_TOOL_H_
#define INCLUDE_DEPLOY_TOOL_H_

#include <Wt/WContainerWidget.h>
#include <Wt/WMenu.h>

#include "Session.h"

using namespace Wt;

namespace Wt {
  class WStackedWidget;
  class WAnchor;
} // namespace Wt

class Session;

class DeployTool : public WContainerWidget {
 public:
  DeployTool();

  void handleInternalPath(const std::string& internalPath);

 private:
  WStackedWidget* mainStack_{nullptr};
  WStackedWidget* contentStack_{nullptr};

  Wt::Auth::AuthWidget* authWidgetPtr;

  Session session_;

  void onAuthEvent();
  void createMenu();

  void clockIn();
  void clockOut();
};

#endif // INCLUDE_DEPLOY_TOOL_H_
