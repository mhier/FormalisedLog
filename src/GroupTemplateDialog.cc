/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "GroupTemplateDialog.h"

#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>

GroupTemplateDialog::GroupTemplateDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<GroupTemplate> group)
: Wt::WDialog("Group template"), session_(session), owner_(owner), group_(group)
{
    update();
}

void GroupTemplateDialog::update() {
    contents()->clear();
    footer()->clear();

    contents()->addStyleClass("form-group");

    Dbo::Transaction transaction(session_.session_);
    auto user = session_.user();

    bool createNew = false;
    if(group_.get() == nullptr) {
      createNew = true;
      group_ = Wt::Dbo::ptr<GroupTemplate>(std::make_unique<GroupTemplate>());
      session_.session_.add(group_);
    }

    auto grid = contents()->setLayout(std::make_unique<Wt::WGridLayout>());
    grid->setColumnStretch(0,0);
    grid->setColumnStretch(1,1);

    grid->addWidget(std::make_unique<Wt::WText>("Name: "), 0, 0);
    auto w_name = grid->addWidget(std::make_unique<Wt::WLineEdit>(), 0, 1);
    w_name->setText(group_->name);

    grid->addWidget(std::make_unique<Wt::WText>("Group content: "), 3, 0);

    auto table = grid->addWidget(std::make_unique<WTable>(), 3,1);
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");
    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Item name"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Parameters"));

    auto &instances = group_->deployItemInstances;
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

      // make item list rows clickable
      for(int ic=0; ic<2; ++ic) {
        table->elementAt(row,ic)->clicked().connect(this, [=] {
          instanceDialog_ = std::make_unique<ItemInstanceDialog>(this, session_, i);
          instanceDialog_->show();
        });
      }

      row++;
    }

    auto w_addparam = grid->addWidget(std::make_unique<Wt::WPushButton>("Add item"), 4, 1);
    w_addparam->clicked().connect(this, [=] {
      Dbo::Transaction transaction(session_.session_);
      Wt::Dbo::ptr<ItemInstance> instance_(std::make_unique<ItemInstance>());
      group_.modify()->deployItemInstances.insert(instance_);
      session_.session_.flush();

      instanceDialog_ = std::make_unique<ItemInstanceDialog>(this, session_, instance_);
      instanceDialog_->show();
    } );

    if(!createNew) {
      Wt::WPushButton *del = footer()->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
      del->clicked().connect(this, [=] {
        dbo::Transaction transaction(session_.session_);
        for(auto &i : group_.modify()->deployItemInstances) i.remove();
        group_.remove();
        owner_->update();
        hide();
      } );
    }

    auto ok = footer()->addWidget(std::make_unique<Wt::WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(this, [=] {
      dbo::Transaction transaction(session_.session_);

      // update the database object
      group_.modify()->name = w_name->text().toUTF8();

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
        for(auto &i : group_.modify()->deployItemInstances) i.remove();
        group_.remove();
        owner_->update();
        hide();
      } );
    }

}
