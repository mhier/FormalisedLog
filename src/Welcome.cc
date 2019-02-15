/*
 * FormalisedLog - Tool for creation of standardised log book entires at XFEL and similar DESY facilities
 */

#include "Welcome.h"

#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WDate.h>
#include <Wt/WPushButton.h>
#include <Wt/WPanel.h>
#include <Wt/WCheckBox.h>

Welcome::Welcome(Session& session) : session_(session) {
  clear();

  auto user = session_.user();

  dbo::Transaction transaction(session_.session_);

  addWidget(std::make_unique<WText>("<h2>Welcome!</h2>"));

  addWidget(std::make_unique<WText>("Please select a form from the left menu."));
}
