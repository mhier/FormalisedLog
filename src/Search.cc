/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include "Search.h"

#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WDate.h>
#include <Wt/WPushButton.h>
#include <Wt/WPanel.h>
#include <Wt/WCheckBox.h>

Search::Search(Session &session)
: session_(session)
{
    update();
}

void Search::update() {
    clear();

    auto user = session_.user();

    dbo::Transaction transaction(session_.session_);

    addWidget(std::make_unique<WText>("<h2>Search and update</h2>"));

    auto searchPanel = addWidget(std::make_unique<Wt::WPanel>());
    searchPanel->setTitle("Search criteria");
    searchPanel->setCollapsible(true);
    searchPanel->setWidth(WLength("100%"));
    searchPanel->setCentralWidget(std::make_unique<Wt::WText>("todo"));

    auto table = std::make_unique<WTable>();
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");

    table->elementAt(0, 0)->addWidget(std::make_unique<WText>(""));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Hostname"));
    table->elementAt(0, 2)->addWidget(std::make_unique<WText>("Template"));
    table->elementAt(0, 3)->addWidget(std::make_unique<WText>("Current versions"));

    auto items = session_.session_.find<Group>().resultList();
    int row = 0;
    for(auto item : items) {
      row++;

      table->elementAt(row, 0)->addWidget(std::make_unique<WCheckBox>());
      table->elementAt(row, 1)->addWidget(std::make_unique<WText>(item->hostname));
      table->elementAt(row, 2)->addWidget(std::make_unique<WText>(item->groupTemplate->name));

      for(int i=1; i<4; ++i) {
        table->elementAt(row,i)->clicked().connect(this, [=] {
          versionUpdateDialog_ = std::make_unique<VersionUpdateDialog>(this, session_, item);
          versionUpdateDialog_->show();
        });
      }

    }

    addWidget(std::move(table));

    auto newGroup = addWidget(std::make_unique<Wt::WPushButton>("Update selected..."));
    newGroup->clicked().connect(this, [=] {
      //versionUpdateDialog_ = std::make_unique<VersionUpdateDialog>(this, session_, nullptr);
      //versionUpdateDialog_->show();
    } );

}
