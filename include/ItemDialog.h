/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_ITEM_DIALOG_H_
#define INCLUDE_ITEM_DIALOG_H_

#include "Session.h"
#include "UpdateableWidget.h"
#include "Item.h"

#include <Wt/WDialog.h>
#include <Wt/WDateEdit.h>
#include <Wt/WText.h>

using namespace Wt;

class ItemDialog : public WDialog {
  public:
    ItemDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<Item> item);

    Session &session_;
    UpdateableWidget *owner_;

    Wt::Dbo::ptr<Item> item_;

    std::vector<Wt::WLineEdit*> v_paramNames;
    std::vector<Wt::WLineEdit*> v_paramValues;

};

#endif //INCLUDE_ITEM_DIALOG_H_
