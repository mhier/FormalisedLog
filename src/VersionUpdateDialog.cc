/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "VersionUpdateDialog.h"

#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>

VersionUpdateDialog::VersionUpdateDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<Group> group)
: Wt::WDialog("Update versions in group"), session_(session), owner_(owner), group_(group)
{
    update();
}

void VersionUpdateDialog::update() {
    contents()->clear();
    footer()->clear();

    contents()->addStyleClass("form-group");

    assert(group_.get() != nullptr);

    auto grid = contents()->setLayout(std::make_unique<Wt::WGridLayout>());
    grid->setColumnStretch(0,0);
    grid->setColumnStretch(1,1);

    // if group template has not been selected for some reason, only display error message
    if(!group_->groupTemplate) {
      grid->addWidget(std::make_unique<Wt::WText>("No group template selected for this group!"), 0, 0);
      auto cancel = footer()->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
      cancel->clicked().connect(this, [=] {hide();} );
      return;
    }

    // display static information
    grid->addWidget(std::make_unique<Wt::WText>("Template: "), 0, 0);
    grid->addWidget(std::make_unique<Wt::WText>(group_->groupTemplate->name), 0, 1);

    grid->addWidget(std::make_unique<Wt::WText>("Hostname: "), 1, 0);
    grid->addWidget(std::make_unique<Wt::WText>(group_->hostname), 1, 1);

    // create drop-down menu to select the versions set
    grid->addWidget(std::make_unique<Wt::WText>("Version set: "), 2, 0);
    auto w_versionSet = grid->addWidget(std::make_unique<Wt::WComboBox>(), 2, 1);
    int index = 0;
    {
      Dbo::Transaction transaction(session_.session_);
      auto groupVersions = session_.session_.find<VersionSet>().resultList();
      for(auto gv : groupVersions) {
        if(gv->groupTemplate != group_->groupTemplate) continue;
        std::string versionSummary;
        for(auto iv : gv->instanceVersions) versionSummary += iv->version + ";";
        w_versionSet->addItem(versionSummary);
        if(group_->groupVersionSet == gv) w_versionSet->setCurrentIndex(index);
        index++;
      }
    }

    grid->addWidget(std::make_unique<Wt::WText>("Group content: "), 3, 0);

    auto table = grid->addWidget(std::make_unique<WTable>(), 3,1);
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");
    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Item name"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Parameters"));
    table->elementAt(0, 2)->addWidget(std::make_unique<WText>("Version"));

    // when changing the selected version set, update the table
    w_versionSet->changed().connect(this, [=] {
      dbo::Transaction transaction(session_.session_);

      // find the selected version set
      Wt::Dbo::ptr<VersionSet> selectedSet;
      std::string selectedVersionSummary = w_versionSet->currentText().toUTF8();
      auto groupVersions = session_.session_.find<VersionSet>().resultList();
      for(auto gv : groupVersions) {
        if(gv->groupTemplate != group_->groupTemplate) continue;
        std::string versionSummary;
        for(auto iv : gv->instanceVersions) versionSummary += iv->version + ";";
        if(versionSummary == selectedVersionSummary) {
          selectedSet = gv;
          break;
        }
      }
      if(!selectedSet) return;

      // update the shown versions in the table
      auto &instances = group_->groupTemplate->deployItemInstances;
      int row=1;
      for(const auto &i : instances) {
        std::string name = "(none)";
        if(i->deployItem) name = i->deployItem->name;
        table->elementAt(row,0)->clear();
        table->elementAt(row,0)->addWidget(std::make_unique<WText>(name));
        std::string params;
        for(const auto &p : i->parameters) {
          params += p->key + "=" + p->value + "; ";
        }
        table->elementAt(row,1)->clear();
        table->elementAt(row,1)->addWidget(std::make_unique<WText>(params));

        std::string version = "not set";
        for(auto iv : selectedSet->instanceVersions) {
          for(auto i2 : iv->itemInstance) {
            if(i2 == i) {
              version = iv->version;
              goto found;
            }
          }
        }
found:

        table->elementAt(row,2)->clear();
        table->elementAt(row,2)->addWidget(std::make_unique<WText>(version));

        row++;
      }

    });

    // force update of the tableonce when showing the dialog
    w_versionSet->changed().emit();


    auto ok = footer()->addWidget(std::make_unique<Wt::WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(this, [=] {

      {
        dbo::Transaction transaction(session_.session_);

        // find the selected version set
        Wt::Dbo::ptr<VersionSet> selectedSet;
        std::string selectedVersionSummary = w_versionSet->currentText().toUTF8();
        auto groupVersions = session_.session_.find<VersionSet>().resultList();
        for(auto gv : groupVersions) {
          if(gv->groupTemplate != group_->groupTemplate) continue;
          std::string versionSummary;
          for(auto iv : gv->instanceVersions) versionSummary += iv->version + ";";
          if(versionSummary == selectedVersionSummary) {
            selectedSet = gv;
            break;
          }
        }
        if(!selectedSet) return;    // TODO display error message

        // update the version set of the group
        group_.modify()->groupVersionSet = selectedSet;
      }

      owner_->update();
      hide();
    } );

    auto cancel = footer()->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancel->clicked().connect(this, [=] {hide();} );

}
