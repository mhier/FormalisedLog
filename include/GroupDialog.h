/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_GROUP_DIALOG_H_
#define INCLUDE_GROUP_DIALOG_H_

#include "Session.h"
#include "UpdateableWidget.h"
#include "Group.h"
#include "ItemInstanceDialog.h"

#include <Wt/WDialog.h>
#include <Wt/WDateEdit.h>
#include <Wt/WText.h>

using namespace Wt;

class GroupDialog : public WDialog, public UpdateableWidget {
  public:
    GroupDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<Group> group);

    void update() override;

    Session &session_;
    UpdateableWidget *owner_;

    Wt::Dbo::ptr<Group> group_;

};

#endif // INCLUDE_GROUP_DIALOG_H_
