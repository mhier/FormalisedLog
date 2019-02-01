/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
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
}

class Session;

class DeployTool : public WContainerWidget {
  public:
    DeployTool();

    void handleInternalPath(const std::string &internalPath);

  private:
    WStackedWidget    *mainStack_{nullptr};
    WStackedWidget    *contentStack_{nullptr};

    Wt::Auth::AuthWidget *authWidgetPtr;

    Session session_;

    void onAuthEvent();
    void createMenu();

    void clockIn();
    void clockOut();
};

#endif //INCLUDE_DEPLOY_TOOL_H_
