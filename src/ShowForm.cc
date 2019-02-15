/*
 * FormalisedLog - Tool for creation of standardised log book entires at XFEL and similar DESY facilities
 */

#include "ShowForm.h"
#include "Form.h"

#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WTextArea.h>
#include <Wt/WApplication.h>

ShowForm::ShowForm(Session& session, const std::string& identifier) : session_(session) {
  auto user = session_.user();
  auto layout = setLayout(std::make_unique<Wt::WVBoxLayout>());

  dbo::Transaction transaction(session_.session_);
  auto forms = session_.session_.find<Form>().where("identifier = ?").bind(identifier).resultList();
  if(forms.size() != 1) {
    layout->addWidget(std::make_unique<WText>("Error: form not found."));
    return;
  }
  form_ = forms.front();

  layout->addWidget(std::make_unique<WText>("<h2>" + form_->title + "</h2>"));
  layout->addWidget(std::make_unique<WText>(form_->description));

  table = layout->addWidget(std::make_unique<WTable>());
  table->setHeaderCount(1);
  table->setWidth(WLength("100%"));
  table->addStyleClass("table form-inline table-hover");

  table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Name"));
  table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Value"));

  int nRows = 1;
  for(auto field : form_->fields) {
    auto title = table->elementAt(nRows, 0)->addWidget(std::make_unique<WText>(field->title));
    title->setToolTip(field->description);

    fieldValues.push_back(table->elementAt(nRows, 1)->addWidget(std::make_unique<WLineEdit>()));
    fieldValues.back()->setToolTip(field->description);
    fieldValues.back()->setWidth(WLength("100%"));
    ++nRows;
  }

  auto preview = layout->addWidget(std::make_unique<WPushButton>("Preview and submit"));
  preview->clicked().connect([=] {
    previewDialog_ = std::make_unique<PreviewDialog>(session_, this);
    previewDialog_->show();
  });
}

PreviewDialog::PreviewDialog(Session& session, ShowForm* owner) {
  contents()->addStyleClass("form-group");
  Dbo::Transaction transaction(session.session_);

  titleBar()->addWidget(std::make_unique<WText>("Preview"));

  auto layout = contents()->setLayout(std::make_unique<Wt::WVBoxLayout>());

  auto formIdentifier = layout->addWidget(std::make_unique<WLineEdit>());
  formIdentifier->setPlaceholderText("Identifier (cannot be changed later)");
  formIdentifier->setText(form_.get()->identifier);
  formIdentifier->setMaxLength(Form::identifierLength);
  if(!createNew) formIdentifier->setReadOnly(true);

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
  save->clicked().connect([=] {
    Dbo::Transaction transact(session_.session_);
    if(createNew) form_.modify()->identifier = formIdentifier->text().toUTF8();
    form_.modify()->title = formTitle->text().toUTF8();
    form_.modify()->description = formDescription->text().toUTF8();
    if(createNew) session_.session_.add(form_);
    form_.modify()->fields.clear();
    for(size_t i = 0; i < nRows; ++i) {
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
  cancel->clicked().connect([this] { this->hide(); });
}
