/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include "EditForm.h"
#include "Form.h"

#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WInPlaceEdit.h>
#include <Wt/WPushButton.h>

class FormDialog;

EditForm::EditForm(Session &session)
: session_(session)
{
    update();
}

void EditForm::update() {
    clear();

    auto user = session_.user();

    dbo::Transaction transaction(session_.session_);

    addWidget(std::make_unique<WText>("<h2>Create and edit forms</h2>"));

    auto table = std::make_unique<WTable>();
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");

    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("#"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Hostname"));
    table->elementAt(0, 2)->addWidget(std::make_unique<WText>("Template"));

    auto forms = session_.session_.find<Form>().resultList();
    int row = 0;
    for(auto form : forms) {
      row++;

      table->elementAt(row, 0)->addWidget(std::make_unique<WText>(WString("{1}").arg(row)));
      table->elementAt(row, 1)->addWidget(std::make_unique<WText>(form->title));

      for(int i=0; i<2; ++i) {
        table->elementAt(row,i)->clicked().connect(this, [=] {
//          groupDialog_ = std::make_unique<GroupDialog>(this, session_, item);
//          groupDialog_->show();
        });
      }

    }

    addWidget(std::move(table));

    auto newForm = addWidget(std::make_unique<Wt::WPushButton>("Create form..."));
    newForm->clicked().connect(this, [=] {
      formDialog_ = std::make_unique<FormDialog>(*this, session_, nullptr);
      formDialog_->show();
    } );

}

/**********************************************************************************************************************/

void FormDialog::update() {
    contents()->clear();
    footer()->clear();
    contents()->addStyleClass("form-group");
    Dbo::Transaction transaction(session_.session_);

    auto layout = contents()->setLayout(std::make_unique<Wt::WVBoxLayout>());

    auto formTitle = layout->addWidget(std::make_unique<WInPlaceEdit>());
    formTitle->setPlaceholderText("Title of the form");
    formTitle->setButtonsEnabled(false);
    auto formDescription = layout->addWidget(std::make_unique<WInPlaceEdit>());
    formDescription->setPlaceholderText("Detailled description displayed on top");
    formDescription->setButtonsEnabled(false);

    auto table = layout->addWidget(std::make_unique<WTable>());
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");

    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Field title"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Description"));
    int row = 0;
    for(auto field : form_.get()->fields) {
      ++row;
      table->elementAt(row, 0)->addWidget(std::make_unique<WInPlaceEdit>(field->title));
      table->elementAt(row, 1)->addWidget(std::make_unique<WInPlaceEdit>(field->description));
    }
    ++row;
    {
      auto title = table->elementAt(row, 0)->addWidget(std::make_unique<WInPlaceEdit>());
      title->setPlaceholderText("Field title");
      title->setButtonsEnabled(false);
      auto description = table->elementAt(row, 1)->addWidget(std::make_unique<WInPlaceEdit>());
      description->setPlaceholderText("Description / help text");
      description->setButtonsEnabled(false);
    }

}

