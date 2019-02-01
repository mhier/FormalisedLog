/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_INSTANCE_VERSION_H_
#define INCLUDE_INSTANCE_VERSION_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

class VersionSet;
class ItemInstance;

class InstanceVersion {
  public:
    InstanceVersion() {}

    Wt::Dbo::collection< Wt::Dbo::ptr<VersionSet> > groupVersion;
    Wt::Dbo::collection< Wt::Dbo::ptr<ItemInstance> > itemInstance;
    std::string version;

    template<class Action>
    void persist ( Action& a ) {
        Wt::Dbo::hasMany ( a, groupVersion, Wt::Dbo::ManyToMany, "instanceVersions" );
        Wt::Dbo::hasMany ( a, itemInstance, Wt::Dbo::ManyToMany, "itemInstanceVersions" );
        Wt::Dbo::field ( a, version, "the_version" );
    }

};

#endif /* INCLUDE_INSTANCE_VERSION_H_ */
