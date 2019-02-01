/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_GROUP_TEMPLATE_H_
#define INCLUDE_GROUP_TEMPLATE_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "ItemInstance.h"
#include "VersionSet.h"

class Group;

class GroupTemplate {
  public:
    GroupTemplate() {}

    std::string name;
    Wt::Dbo::collection< Wt::Dbo::ptr<ItemInstance> > deployItemInstances;
    Wt::Dbo::collection< Wt::Dbo::ptr<Group> > groups;
    Wt::Dbo::collection< Wt::Dbo::ptr<VersionSet> > allowedVersions;

    template<class Action>
    void persist ( Action& a ) {
        Wt::Dbo::field ( a, name, "name" );
        Wt::Dbo::hasMany ( a, deployItemInstances, Wt::Dbo::ManyToOne, "groupInstances" );
        Wt::Dbo::hasMany ( a, groups, Wt::Dbo::ManyToOne, "groupTemplate" );
        Wt::Dbo::hasMany ( a, allowedVersions, Wt::Dbo::ManyToOne, "allowedVersions" );
    }

};

#endif /* INCLUDE_GROUP_TEMPLATE_H_ */
