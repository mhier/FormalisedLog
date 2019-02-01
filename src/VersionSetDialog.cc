/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "VersionSetDialog.h"

#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/WComboBox.h>
#include <Wt/WTable.h>

VersionSetDialog::VersionSetDialog(UpdateableWidget *owner, Session &session, Wt::Dbo::ptr<VersionSet> groupVersion)
: Wt::WDialog("Versions set"), session_(session), owner_(owner), groupVersion_(groupVersion)
{
    update();
}

void VersionSetDialog::update() {
    contents()->clear();
    footer()->clear();

    contents()->addStyleClass("form-group");

    Dbo::Transaction transaction(session_.session_);
    auto user = session_.user();

    bool createNew = false;
    if(groupVersion_.get() == nullptr) {
      createNew = true;
      groupVersion_ = Wt::Dbo::ptr<VersionSet>(std::make_unique<VersionSet>());
      session_.session_.add(groupVersion_);
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
      if(groupVersion_->groupTemplate == t) w_template->setCurrentIndex(index);
      index++;
    }

    grid->addWidget(std::make_unique<Wt::WText>("Versions: "), 3, 0);

    auto table = grid->addWidget(std::make_unique<WTable>(), 3,1);
    table->setHeaderCount(1);
    table->setWidth(WLength("100%"));
    table->addStyleClass("table form-inline table-hover");
    table->elementAt(0, 0)->addWidget(std::make_unique<WText>("Item name"));
    table->elementAt(0, 1)->addWidget(std::make_unique<WText>("Parameters"));
    table->elementAt(0, 2)->addWidget(std::make_unique<WText>("Version"));

    if(groupVersion_->groupTemplate) {
      auto &instances = groupVersion_->groupTemplate->deployItemInstances;
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

        versions_.push_back( table->elementAt(row,2)->addWidget(std::make_unique<WLineEdit>()) );

        // search for the matching version. TODO: replace with proper database search
        std::string version = "not set";
        for(auto v : i->versions) {
          for(auto gv : v->groupVersion) {
            if(gv == groupVersion_) {
              version = v->version;
              goto found;
            }
          }
        }
found:

        versions_.back()->setText(version);

        row++;
      }
    }


    if(!createNew) {
      Wt::WPushButton *del = footer()->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
      del->clicked().connect(this, [=] {
        for(auto iv : groupVersion_.modify()->instanceVersions) iv.remove();
        groupVersion_.remove();
        owner_->update();
        hide();
      } );
    }

    auto ok = footer()->addWidget(std::make_unique<Wt::WPushButton>("Ok"));
    ok->setDefault(true);
    ok->clicked().connect(this, [=] {
      {
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
        if(groupVersion_->groupTemplate != tpl) {

          // remove group from current template, if already in a group
          if(groupVersion_->groupTemplate) groupVersion_->groupTemplate.modify()->allowedVersions.erase(groupVersion_);

          // add group to selected template
          tpl.modify()->allowedVersions.insert(groupVersion_);
        }

        // update the versions
        std::vector<Wt::Dbo::ptr<InstanceVersion>> ivs;

        for(auto iv : groupVersion_.modify()->instanceVersions) iv.remove();
        groupVersion_.modify()->instanceVersions.clear();

        size_t index = 0;
        for(auto itemInstance : groupVersion_->groupTemplate->deployItemInstances ) {
          Wt::Dbo::ptr<InstanceVersion> iv(std::make_unique<InstanceVersion>());
          session_.session_.add(iv);
          if(versions_.size() >= index+1) {
            auto &version = versions_[index];
            iv.modify()->version = version->text().toUTF8();
            iv.modify()->itemInstance.insert(itemInstance);
            iv.modify()->groupVersion.insert(groupVersion_);
          }
          ivs.push_back(iv);
          ++index;
        }

        transaction.commit();
      }

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
        groupVersion_.remove();
        owner_->update();
        hide();
      } );
    }

}
