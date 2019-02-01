/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_EDIT_FORM_H_
#define INCLUDE_EDIT_FORM_H_

#include "Session.h"
#include "Form.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WDialog.h>

using namespace Wt;

class FormDialog;

class EditForm : public WContainerWidget {
  public:
    EditForm(Session &session);

    void update();

  private:
    Session &session_;
    std::unique_ptr<FormDialog> formDialog_;
};

class FormDialog : public WDialog {
  public:
    FormDialog(EditForm &owner, Session &session, Wt::Dbo::ptr<Form> form)
      : owner_(owner), session_(session), form_(form)
    {
      if(form_.get() == nullptr) {
        createNew = true;
        form_ = std::make_unique<Form>();
      }
      update();
    }

    void update();

    EditForm &owner_;
    Session &session_;
    Wt::Dbo::ptr<Form> form_;
    bool createNew{false};
};

#endif // INCLUDE_EDIT_FORM_H_
