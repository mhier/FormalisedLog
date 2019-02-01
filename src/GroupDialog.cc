/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "GroupDialog.h"

#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>

GroupDialog::GroupDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<Group> group)
: Wt::WDialog("Group"), session_(session), owner_(owner), group_(group)
{
    update();
}

void GroupDialog::update() {
    contents()->clear();
    footer()->clear();

    contents()->addStyleClass("form-group");

    Dbo::Transaction transaction(session_.session_);
    auto user = session_.user();

    bool createNew = false;
    if(group_.get() == nullptr) {
      createNew = true;
      group_ = Wt::Dbo::ptr<Group>(std::make_unique<Group>());
      session_.session_.add(group_);
    }

    auto grid = contents()->setLayout(std::make_unique<Wt::WGridLayout>());
    grid->setColumnStretch(0,0);
    grid->setColumnStretch(1,1);

    grid->addWidget(std::make_unique<Wt::WText>("Template: "), 0, 0);
    auto w_template = grid->addWidget(std::make_unique<Wt::WComboBox>(), 0, 1);
    int index = 0;
    auto templates = session_.session_.find<GroupTemplate>().resultList();
    for(auto t : templates) {
      w_template->addItem(t->name);
      if(group_->groupTemplate == t) w_template->setCurrentIndex(index);
      index++;
    }

    grid->addWidget(std::make_unique<Wt::WText>("Hostname: "), 1, 0);
    auto w_hostname = grid->addWidget(std::make_unique<Wt::WLineEdit>(), 1, 1);
    w_hostname->setText(group_->hostname);

    if(!createNew) {
      Wt::WPushButton *del = footer()->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
      del->clicked().connect(this, [=] {
        dbo::Transaction transaction(session_.session_);
        group_.remove();
        owner_->update();
        hide();
      } );
    }

    grid->addWidget(std::make_unique<Wt::WText>("Group content: "), 3, 0);

    auto table = grid->addWidget(std::make_unique<WTable>(), 3,1);
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");
    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Item name"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Parameters"));

    if(group_->groupTemplate) {
      auto &instances = group_->groupTemplate->deployItemInstances;
      int row=1;
      for(const auto &i : instances) {
        std::string name = "(none)";
        if(i->deployItem) name = i->deployItem->name;
        table->elementAt(row,0)->addWidget(std::make_unique<WText>(name));
        std::string params;
        for(const auto &p : i->parameters) {
          params += p->key + "=" + p->value + "; ";
        }
        table->elementAt(row,1)->addWidget(std::make_unique<WText>(params));

        row++;
      }
    }


    auto ok = footer()->addWidget(std::make_unique<Wt::WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(this, [=] {
      dbo::Transaction transaction(session_.session_);

      // obtain the selected group template
      Wt::Dbo::ptr<GroupTemplate> tpl;
      auto templates = session_.session_.find<GroupTemplate>().resultList();
      for(auto t : templates) {
        if(w_template->currentText().toUTF8() == t->name) {
          tpl = t;
          break;
        }
      }

      // need to update the group template?
      if(group_->groupTemplate != tpl) {

        // remove group from current template, if already in a group
        if(group_->groupTemplate) group_->groupTemplate.modify()->groups.erase(group_);

        // add group to selected template
        tpl.modify()->groups.insert(group_);
      }

      // update the hostname
      group_.modify()->hostname = w_hostname->text().toUTF8();

      owner_->update();
      hide();
    } );

    if(!createNew) {
      auto cancel = footer()->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
      cancel->clicked().connect(this, [=] {hide();} );
    }
    else {
      Wt::WPushButton *del = footer()->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
      del->clicked().connect(this, [=] {
        dbo::Transaction transaction(session_.session_);
        group_.remove();
        owner_->update();
        hide();
      } );
    }

}
