/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_VERSION_SET_DIALOG_H_
#define INCLUDE_VERSION_SET_DIALOG_H_

#include "Session.h"
#include "UpdateableWidget.h"
#include "Group.h"
#include "ItemInstanceDialog.h"

#include <Wt/WDialog.h>
#include <Wt/WDateEdit.h>
#include <Wt/WText.h>

using namespace Wt;

class VersionSetDialog : public WDialog, public UpdateableWidget {
  public:
    VersionSetDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<VersionSet> groupVersion);

    void update() override;

    Session &session_;
    UpdateableWidget *owner_;

    Wt::Dbo::ptr<VersionSet> groupVersion_;

    std::vector<WLineEdit*> versions_;

};

#endif // INCLUDE_VERSION_SET_DIALOG_H_
