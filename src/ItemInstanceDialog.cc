/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "GroupTemplate.h"
#include "ItemInstanceDialog.h"

#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>

ItemInstanceDialog::ItemInstanceDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<ItemInstance> instance)
: Wt::WDialog("Deploy Item Instance"), session_(session), owner_(owner), instance_(instance)
{
    contents()->addStyleClass("form-group");

    // define lambda to update the parameter table
    auto updateParameterTable = [=] {
      parameterTable_->elementAt(0, 0)->addWidget(std::make_unique<WText>("Parameter name"));
      parameterTable_->elementAt(0, 1)->addWidget(std::make_unique<WText>("Parameter value"));
      Dbo::Transaction transaction(session_.session_);
      v_paramNames.clear();
      v_paramValues.clear();
      parameterTable_->clear();
      auto items = session_.session_.find<Item>().resultList();
      for(auto item : items) {
        if(w_item->currentText() == item->name) {
          auto &parameters = item->parameters;
          for(const auto &p : parameters) {
            int row = static_cast<int>(v_paramNames.size())+1;
            v_paramNames.push_back(parameterTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WLineEdit>()));
            v_paramValues.push_back(parameterTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WLineEdit>()));
            v_paramNames.back()->setText(p->key);
            v_paramValues.back()->setText(p->value);
          }
        }
      }
      transaction.commit();
    };

    {
      Dbo::Transaction transaction(session_.session_);
      auto user = session_.user();

      assert(instance_.get() != nullptr);

      auto grid = contents()->setLayout(std::make_unique<Wt::WGridLayout>());
      grid->setColumnStretch(0,0);
      grid->setColumnStretch(1,1);

      grid->addWidget(std::make_unique<Wt::WText>("Deploy item: "), 0, 0);
      w_item = grid->addWidget(std::make_unique<Wt::WComboBox>(), 0, 1);

      parameterTable_ = grid->addWidget(std::make_unique<WTable>(), 3,1);
      parameterTable_->setHeaderCount(1);
      parameterTable_->setWidth(WLength("100%"));
      parameterTable_->elementAt(0, 0)->addWidget(std::make_unique<WText>("Parameter name"));
      parameterTable_->elementAt(0, 1)->addWidget(std::make_unique<WText>("Parameter value"));

      // when selecting a different DeployItem, update the table of parameter edit fields
      w_item->changed().connect( updateParameterTable );

      // fill the item selection box with the defined items and select the correct item
      int index = 0;
      w_item->setCurrentIndex(-1);
      auto items = session_.session_.find<Item>().resultList();
      for(auto item : items) {
        w_item->addItem(item->name);
        if(instance_->deployItem == item) w_item->setCurrentIndex(index);
        index++;
      }
      transaction.commit();
    }

    // update the parameter edit fileds table upon show
    updateParameterTable();

    Wt::WPushButton *del = footer()->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
    del->clicked().connect(this, [=] {
      dbo::Transaction transaction(session_.session_);
      instance_.remove();
      owner_->update();
      hide();
    } );

    auto ok = footer()->addWidget(std::make_unique<Wt::WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(this, [=] {
      dbo::Transaction transaction(session_.session_);

      // update the database object
      auto items = session_.session_.find<Item>().resultList();
      for(auto item : items) {
        if(w_item->currentText() == item->name) {
          instance_.modify()->deployItem = item;
        }
      }

      // update parameters
      size_t ip = 0;
      size_t np = v_paramNames.size();
      auto &parameters = instance_.modify()->parameters;
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
        Wt::Dbo::ptr<KeyValue<ItemInstance>> p(std::make_unique<KeyValue<ItemInstance>>());
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
