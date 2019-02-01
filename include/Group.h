/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_GROUP_H_
#define INCLUDE_GROUP_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "GroupTemplate.h"

class Group {
  public:
    Group() {}

    std::string hostname;
    Wt::Dbo::ptr<GroupTemplate> groupTemplate;
    Wt::Dbo::ptr<VersionSet> groupVersionSet;

    template<class Action>
    void persist ( Action& a ) {
        Wt::Dbo::field ( a, hostname, "hostname" );
        Wt::Dbo::belongsTo ( a, groupTemplate, "groupTemplate" );
        Wt::Dbo::belongsTo ( a, groupVersionSet, "groupVersionSet" );
    }

};

#endif /* INCLUDE_GROUP_H_ */
