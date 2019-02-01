/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_GROUP_TEMPLATE_LIST_H_
#define INCLUDE_GROUP_TEMPLATE_LIST_H_

#include "Item.h"
#include "Session.h"
#include "UpdateableWidget.h"
#include "GroupTemplateDialog.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/WContainerWidget.h>

using namespace Wt;

class GroupTemplateList : public WContainerWidget, public UpdateableWidget {
  public:
    GroupTemplateList(Session &session);

    void update() override;

  private:
    Session &session_;
    std::unique_ptr<GroupTemplateDialog> groupDialog_;
};

#endif // INCLUDE_GROUP_TEMPLATE_LIST_H_
