/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "ItemDialog.h"

#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>

ItemDialog::ItemDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<Item> item)
: Wt::WDialog("Deploy Item"), session_(session), owner_(owner), item_(item)
{
    contents()->addStyleClass("form-group");

    Dbo::Transaction transaction(session_.session_);
    auto user = session_.user();

    bool createNew = false;
    if(item_.get() == nullptr) {
      createNew = true;
      item_ = Wt::Dbo::ptr<Item>(std::make_unique<Item>());
    }

    auto grid = contents()->setLayout(std::make_unique<Wt::WGridLayout>());
    grid->setColumnStretch(0,0);
    grid->setColumnStretch(1,1);

    grid->addWidget(std::make_unique<Wt::WText>("Name: "), 0, 0);
    auto w_name = grid->addWidget(std::make_unique<Wt::WLineEdit>(), 0, 1);
    w_name->setText(item_->name);

    grid->addWidget(std::make_unique<Wt::WText>("Source pattern: "), 1, 0);
    auto w_srcpat = grid->addWidget(std::make_unique<Wt::WLineEdit>(), 1, 1);
    w_srcpat->setText(item_->sourcePattern);

    grid->addWidget(std::make_unique<Wt::WText>("Install command: "), 2, 0);
    auto w_instcmd = grid->addWidget(std::make_unique<Wt::WLineEdit>(), 2, 1);
    w_instcmd->setText(item_->installCommand);

    grid->addWidget(std::make_unique<Wt::WText>("Parameters: "), 3, 0);

    auto table = grid->addWidget(std::make_unique<WTable>(), 3,1);
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Parameter name"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Default value"));

    auto &parameters = item_->parameters;
    for(const auto &p : parameters) {
      int row = static_cast<int>(v_paramNames.size())+1;
      v_paramNames.push_back(table->elementAt(row, 0)->addWidget(std::make_unique<Wt::WLineEdit>()));
      v_paramValues.push_back(table->elementAt(row, 1)->addWidget(std::make_unique<Wt::WLineEdit>()));
      v_paramNames.back()->setText(p->key);
      v_paramValues.back()->setText(p->value);
    }

    auto w_addparam = grid->addWidget(std::make_unique<Wt::WPushButton>("Add parameter"), 4, 1);
    w_addparam->clicked().connect(this, [=] {
      int row = static_cast<int>(v_paramNames.size())+1;
      v_paramNames.push_back(table->elementAt(row, 0)->addWidget(std::make_unique<Wt::WLineEdit>()));
      v_paramValues.push_back(table->elementAt(row, 1)->addWidget(std::make_unique<Wt::WLineEdit>()));
    } );

    if(!createNew) {   // existing item might be deleted
      Wt::WPushButton *del = footer()->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
      del->clicked().connect(this, [=] {
        dbo::Transaction transaction(session_.session_);
        item_.remove();
        owner_->update();
        hide();
      } );
    }

    auto ok = footer()->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    ok->setDefault(true);
    ok->clicked().connect(this, [=] {
      dbo::Transaction transaction(session_.session_);

      // update the database object
      item_.modify()->name = w_name->text().toUTF8();
      item_.modify()->sourcePattern = w_srcpat->text().toUTF8();
      item_.modify()->installCommand = w_instcmd->text().toUTF8();

      if(createNew) {   // create new?
        session_.session_.add(item_);
      }

      // update parameters
      size_t ip = 0;
      size_t np = v_paramNames.size();
      auto &parameters = item_.modify()->parameters;
      for(auto &p : parameters) {
        if(ip < np) {
          p.modify()->key = v_paramNames[ip]->text().toUTF8();
          p.modify()->value = v_paramValues[ip]->text().toUTF8();
        }
        else {
          //parameters.erase(p);
        }
        ++ip;
      }
      for(; ip < np; ++ip) {
        Wt::Dbo::ptr<KeyValue<Item>> p(std::make_unique<KeyValue<Item>>());
        p.modify()->key = v_paramNames[ip]->text().toUTF8();
        p.modify()->value = v_paramValues[ip]->text().toUTF8();
        parameters.insert(p);
      }
      owner_->update();
      hide();
    } );

    auto cancel = footer()->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancel->clicked().connect(this, [=] {hide();} );

}
