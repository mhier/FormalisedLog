/*
 * WtTimeTrack - Web-based time tracker
 *
 * Copyright (C) 2017 Martin Hierholzer, Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_USER_H_
#define INCLUDE_USER_H_

#include <Wt/WDateTime.h>
#include <Wt/Dbo/Types.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/Auth/Dbo/AuthInfo.h>

#include <string>
#include <list>

using namespace Wt;

namespace dbo = Wt::Dbo;

class User;
class Session;
typedef Auth::Dbo::AuthInfo<User> AuthInfo;
typedef dbo::collection< dbo::ptr<User> > Users;

class User {
  public:
    User();
    virtual ~User();

    std::string name; /* a copy of auth info's user name */

    dbo::collection<dbo::ptr<AuthInfo>> authInfos;

    template<class Action>
    void persist ( Action& a ) {
      dbo::field ( a, name, "name" );

      dbo::hasMany ( a, authInfos, dbo::ManyToOne, "user" );
    }

};

DBO_EXTERN_TEMPLATES ( User );

#endif // INCLUDE_USER_H_
