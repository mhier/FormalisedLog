/*
 * DeployTool - Web-based software/firmware deployment tool
 *
 * Copyright (C) 2018 Martin Hierholzer, Deutsches Elektronen-Synchrotron DESY Hamburg, Germany
 *
 * This program is released under the GNU GENERAL PUBLIC LICENSE v3.0
 * See the LICENSE file for terms of use.
 */

#ifndef INCLUDE_FORM_H_
#define INCLUDE_FORM_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

class FormField;

class Form {
  public:
    Form() {}

    std::string title;
    std::string description;
    dbo::collection<dbo::ptr<FormField>> fields;

    template<class Action>
    void persist ( Action& a ) {
      Wt::Dbo::field ( a, title, "title" );
      Wt::Dbo::field ( a, description, "description" );
      dbo::hasMany ( a, fields, dbo::ManyToOne, "fields_of_form" );
    }

};

class FormField {
  public:
    FormField() {}

    std::string title;
    std::string description;
    std::string value;
    dbo::ptr<Form> form;

    template<class Action>
    void persist ( Action& a ) {
      Wt::Dbo::field ( a, title, "title" );
      Wt::Dbo::field ( a, description, "description" );
      Wt::Dbo::field ( a, value, "value" );
      dbo::belongsTo( a, form, "fields_of_form" );

    }

};

#endif /* INCLUDE_FORM_H_ */
