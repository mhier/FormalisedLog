/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_VERSION_SET_H_
#define INCLUDE_VERSION_SET_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "InstanceVersion.h"
#include "GroupTemplate.h"

class Group;

class VersionSet {
  public:
    VersionSet() {}

    Wt::Dbo::collection< Wt::Dbo::ptr<InstanceVersion> > instanceVersions;
    Wt::Dbo::ptr<GroupTemplate> groupTemplate;
    Wt::Dbo::collection< Wt::Dbo::ptr<Group> > groupVersionSet;

    template<class Action>
    void persist ( Action& a ) {
        Wt::Dbo::hasMany ( a, instanceVersions, Wt::Dbo::ManyToMany, "instanceVersions" );
        Wt::Dbo::belongsTo ( a, groupTemplate, "allowedVersions" );
        Wt::Dbo::hasMany ( a, groupVersionSet, Wt::Dbo::ManyToOne, "groupVersionSet" );
    }

};

#endif /* INCLUDE_VERSION_SET_H_ */
