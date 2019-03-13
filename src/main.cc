/*
 * FormalisedLog - Tool for creation of standardised log book entires at XFEL and similar DESY facilities
 */

#include <unistd.h>

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WServer.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WLocale.h>
#include <Wt/Date/tz.h>

#include "FormalisedLog.h"
#include "Session.h"

using namespace Wt;

static WLocale theLocale;

std::unique_ptr<WApplication> createApplication(const WEnvironment& env) {
  auto app = std::make_unique<WApplication>(env);

  app->setTitle("Formalised Log Tool");

  app->root()->addStyleClass("container");
  auto theme = std::make_shared<Wt::WBootstrapTheme>();
  theme->setVersion(Wt::BootstrapVersion::v3);
  theme->setResponsive(false);
  app->setTheme(theme);

  theLocale = WLocale("en_GB");
  theLocale.setTimeZone(date::current_zone());

  app->setLocale(theLocale);

  app->useStyleSheet("FormalisedLog.css");

  app->root()->addWidget(std::make_unique<FormalisedLog>());

  return app;
}

int main(int argc, char** argv) {
  try {
    WServer server(argc, argv, WTHTTP_CONFIGURATION);

    server.addEntryPoint(EntryPointType::Application, createApplication, "");

    Session::configureAuth();

    server.run();
  }
  catch(WServer::Exception& e) {
    std::cerr << e.what() << std::endl;
  }
  catch(std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
  }
}
