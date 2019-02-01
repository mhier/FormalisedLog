/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_ITEM_H_
#define INCLUDE_ITEM_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

#include "KeyValue.h"

class ItemInstance;

class Item {
  public:
    Item() {}


    std::string name;
    std::string sourcePattern;
    std::string installCommand;
    Wt::Dbo::collection< Wt::Dbo::ptr<ItemInstance> > instances;

    // known parameters with default values
    Wt::Dbo::collection< Wt::Dbo::ptr<KeyValue<Item>> > parameters;

    template<class Action>
    void persist ( Action& a ) {
        Wt::Dbo::field ( a, name, "name" );
        Wt::Dbo::field ( a, sourcePattern, "sourcePattern" );
        Wt::Dbo::field ( a, installCommand, "installCommand" );
        Wt::Dbo::hasMany ( a, instances, Wt::Dbo::ManyToOne, "itemInstances" );
        Wt::Dbo::hasMany ( a, parameters, Wt::Dbo::ManyToMany, KeyValue<Item>::name );
    }

};

#endif /* INCLUDE_ITEM_H_ */
