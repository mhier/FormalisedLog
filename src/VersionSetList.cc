/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include "VersionSetList.h"
#include "VersionSet.h"

#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WDate.h>
#include <Wt/WPushButton.h>

VersionSetList::VersionSetList(Session &session)
: session_(session)
{
    update();
}

void VersionSetList::update() {
    clear();

    auto user = session_.user();

    dbo::Transaction transaction(session_.session_);

    addWidget(std::make_unique<WText>("<h2>List of possible version combinations</h2>"));

    auto table = std::make_unique<WTable>();
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");

    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("#"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Template"));
    table->elementAt(0, 2)->addWidget(std::make_unique<WText>("Versions"));

    auto items = session_.session_.find<VersionSet>().resultList();
    int row = 0;
    for(auto item : items) {
      row++;

      table->elementAt(row, 0)->addWidget(std::make_unique<WText>(WString("{1}").arg(row)));
      table->elementAt(row, 1)->addWidget(std::make_unique<WText>(item->groupTemplate->name));
      std::string versionSummary;
      for(auto version : item->instanceVersions) {
        versionSummary += version->version + ";";
      }
      table->elementAt(row, 2)->addWidget(std::make_unique<WText>(versionSummary));

      for(int i=0; i<3; ++i) {
        table->elementAt(row,i)->clicked().connect(this, [=] {
          groupVersionDialog_ = std::make_unique<VersionSetDialog>(this, session_, item);
          groupVersionDialog_->show();
        });
      }

    }

    addWidget(std::move(table));

    auto newGroup = addWidget(std::make_unique<Wt::WPushButton>("Add version set..."));
    newGroup->clicked().connect(this, [=] {
      groupVersionDialog_ = std::make_unique<VersionSetDialog>(this, session_, nullptr);
      groupVersionDialog_->show();
    } );

}
