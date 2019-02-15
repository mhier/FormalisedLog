/*
 * FormalisedLog - Tool for creation of standardised log book entires at XFEL and similar DESY facilities
 */

#ifndef INCLUDE_FORM_H_
#define INCLUDE_FORM_H_

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/WtSqlTraits.h>

class FormField;

class Form {
 public:
  Form() {}

  std::string identifier;
  std::string title;
  std::string description;
  Wt::Dbo::collection<Wt::Dbo::ptr<FormField>> fields;

  static constexpr size_t identifierLength{20};

  template<class Action>
  void persist(Action& a) {
    Wt::Dbo::id(a, identifier, "identifier", identifierLength); // special database key, also see below
    Wt::Dbo::field(a, title, "title");
    Wt::Dbo::field(a, description, "description");
    Wt::Dbo::hasMany(a, fields, Wt::Dbo::ManyToOne, "fields_of_form");
  }
};

// special configuration for database key
namespace Wt { namespace Dbo {
  template<>
  struct dbo_traits<Form> : public dbo_default_traits {
    typedef std::string IdType;
    static IdType invalidId() { return std::string(); }
    static const char* surrogateIdField() { return 0; }
  };
}} // namespace Wt::Dbo

class FormField {
 public:
  FormField() {}

  std::string title;
  std::string description;
  std::string value;
  dbo::ptr<Form> form;

  template<class Action>
  void persist(Action& a) {
    Wt::Dbo::field(a, title, "title");
    Wt::Dbo::field(a, description, "description");
    Wt::Dbo::field(a, value, "value");
    dbo::belongsTo(a, form, "fields_of_form");
  }
};

#endif /* INCLUDE_FORM_H_ */
