/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_VERSION_SET_LIST_H_
#define INCLUDE_VERSION_SET_LIST_H_

#include "Item.h"
#include "Session.h"
#include "UpdateableWidget.h"
#include "VersionSetDialog.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/WContainerWidget.h>

using namespace Wt;

class VersionSetList : public WContainerWidget, public UpdateableWidget {
  public:
    VersionSetList(Session &session);

    void update() override;

  private:
    Session &session_;
    std::unique_ptr<VersionSetDialog> groupVersionDialog_;
};

#endif // INCLUDE_VERSION_SET_LIST_H_
