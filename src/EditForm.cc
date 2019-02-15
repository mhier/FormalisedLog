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
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTextArea.h>
#include <Wt/WApplication.h>

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
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Form title"));
    table->elementAt(0, 2)->addWidget(std::make_unique<WText>("Description"));

    auto forms = session_.session_.find<Form>().resultList();
    int row = 0;
    for(auto form : forms) {
      row++;

      table->elementAt(row, 0)->addWidget(std::make_unique<WText>(WString("{1}").arg(row)));
      table->elementAt(row, 1)->addWidget(std::make_unique<WText>(form->title));
      table->elementAt(row, 2)->addWidget(std::make_unique<WText>(form->description));

      for(int i=0; i<3; ++i) {
        table->elementAt(row,i)->clicked().connect(this, [=] {
        formDialog_ = std::make_unique<FormDialog>(*this, session_, form);
        formDialog_->show();
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
    titleBar()->clear();
    contents()->clear();
    footer()->clear();
    contents()->addStyleClass("form-group");
    Dbo::Transaction transaction(session_.session_);

    titleBar()->addWidget(std::make_unique<WText>("Edit form"));

    auto layout = contents()->setLayout(std::make_unique<Wt::WVBoxLayout>());

    auto formTitle = layout->addWidget(std::make_unique<WLineEdit>());
    formTitle->setPlaceholderText("Title of the form");
    formTitle->setText(form_.get()->title);
    auto formDescription = layout->addWidget(std::make_unique<WTextArea>());
    formDescription->setPlaceholderText("Detailled description displayed on top");
    formDescription->setText(form_.get()->description);

    table = layout->addWidget(std::make_unique<WTable>());
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");

    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Field title"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Description"));
    for(auto field : form_.get()->fields) addRow(field->title, field->description);
    addRow();

    auto save = footer()->addWidget(std::make_unique<WPushButton>("Save"));
    save->clicked().connect([=]{
      Dbo::Transaction transact(session_.session_);
      form_.modify()->title = formTitle->text().toUTF8();
      form_.modify()->description = formDescription->text().toUTF8();
      if(createNew) session_.session_.add(form_);
      form_.modify()->fields.clear();
      for(size_t i=0; i<nRows; ++i) {
        if(fieldTitles[i]->text().toUTF8().size() == 0) continue;
        Wt::Dbo::ptr<FormField> field = std::make_unique<FormField>();
        field.modify()->form = form_;
        field.modify()->title = fieldTitles[i]->text().toUTF8();
        field.modify()->description = fieldDescriptions[i]->text().toUTF8();
        form_.modify()->fields.insert(field);
      }
      this->hide();
      owner_.update();
    });
    auto cancel = footer()->addWidget(std::make_unique<WPushButton>("Cancel"));
    cancel->clicked().connect([this]{
      this->hide();
    });

}

void FormDialog::addRow(const std::string &titleValue, const std::string &descriptionValue) {
    ++nRows;
    auto title = table->elementAt(nRows, 0)->addWidget(std::make_unique<WLineEdit>());
    fieldTitles.push_back(title);
    title->setPlaceholderText("Field title");
    title->setText(titleValue);
    title->setWidth(WLength("100%"));
    size_t thisRow = nRows;
    title->changed().connect([=] {
      if(title->text().toUTF8().size() > 0 && thisRow == this->nRows) addRow();
    });
    auto description = table->elementAt(nRows, 1)->addWidget(std::make_unique<WLineEdit>());
    fieldDescriptions.push_back(description);
    description->setPlaceholderText("Description / help text");
    description->setText(descriptionValue);
    description->setWidth(WLength("100%"));
}
