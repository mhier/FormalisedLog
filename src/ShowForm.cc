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

#include "ElogBackend.h"

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

  std::string logEntry;

  int nRows = 0;
  for(auto field : owner->form_->fields) {
    auto value = owner->fieldValues[static_cast<size_t>(nRows)]->text().toUTF8();
    logEntry += field->title + ": " + value + "\r\n";
    ++nRows;
  }

  // obtain date to simulate the title line
  auto now = std::chrono::system_clock::now();
  auto tt = std::chrono::system_clock::to_time_t(now);
  tm local_tm = *localtime(&tt);

  // form the simulated title line
  std::string titleLine = std::to_string(local_tm.tm_mon + 1) + "/" + std::to_string(local_tm.tm_mday) + "/" +
      std::to_string(local_tm.tm_year + 1900) + " " + std::to_string(local_tm.tm_hour) + ":" +
      std::to_string(local_tm.tm_min);
  titleLine += " AutomatedEntry <b>" + owner->form_->title + "</b>";

  layout->addWidget(std::make_unique<Wt::WText>("<pre>" + titleLine + "</pre>"));
  layout->addWidget(std::make_unique<Wt::WText>("<pre>" + logEntry + "</pre>"));

  auto submit = footer()->addWidget(std::make_unique<WPushButton>("Submit to " + owner->form_->logbook));
  submit->clicked().connect([=] {
    backend.setTitle(owner->form_->title);
    backend.setText(logEntry);
    backend.prepareNew(owner->form_->logbook);
    backend.post();
    this->hide();
  });

  auto cancel = footer()->addWidget(std::make_unique<WPushButton>("Back"));
  cancel->clicked().connect([this] { this->hide(); });
}
