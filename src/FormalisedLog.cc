/*
 * FormalisedLog - Tool for creation of standardised log book entires at XFEL and similar DESY facilities
 */

#include <Wt/WApplication.h>
#include <Wt/Auth/AuthWidget.h>
#include <Wt/Auth/RegistrationModel.h>
#include <Wt/WText.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WCalendar.h>
#include <Wt/WPushButton.h>
#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Auth/Dbo/UserDatabase.h>

#include "Session.h"
#include "FormalisedLog.h"
#include "EditForm.h"
#include "ShowForm.h"
#include "Welcome.h"
#include "Form.h"

FormalisedLog::FormalisedLog() {
  std::unique_ptr<WText> title(std::make_unique<WText>("<h1>Formalised Log Tool</h1>"));
  addWidget(std::move(title));

  auto contentStack = std::make_unique<Wt::WStackedWidget>();
  contentStack->setHeight("100vH");
  contentStack->addStyleClass("contents");
  contentStack->setOverflow(Wt::Overflow::Auto);
  contentStack_ = addWidget(std::move(contentStack));

  WApplication::instance()->internalPathChanged().connect(this, &FormalisedLog::handleInternalPath);

  handleInternalPath(WApplication::instance()->internalPath());
}

void FormalisedLog::handleInternalPath(const std::string& internalPath) {
  Wt::Dbo::Transaction transaction(session_.session_);
  contentStack_->clear();
  if(internalPath == "/") {
    contentStack_->addWidget(std::make_unique<Welcome>(session_));
  }
  else if(internalPath == "/edit") {
    contentStack_->addWidget(std::make_unique<EditForm>(session_));
  }
  else if(internalPath.substr(0, 6) == "/show/") {
    contentStack_->addWidget(std::make_unique<ShowForm>(session_, internalPath.substr(6)));
  }
  else {
    WApplication::instance()->setInternalPath("/", true);
  }
}
