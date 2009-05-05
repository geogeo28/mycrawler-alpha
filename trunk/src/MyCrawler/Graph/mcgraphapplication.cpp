#include "mcgraphapplication.h"
#include "config.h"
#include "Debug/Exception.h"
MCGraphApplication* MCGraphApplication::s_instance = NULL;

MCGraphApplication* MCGraphApplication::instance(){
    return s_instance;
}
MCGraphApplication::MCGraphApplication(int &argc, char** argv)
    : IApplication(argc, argv)
{
    Assert(s_instance == NULL);
    s_instance = this;
    setInformations(_MYCRAWLER_APPNAME_" (graph)", _MYCRAWLER_ORGANIZATION_NAME_, _MYCRAWLER_ORGANIZATION_DOMAIN_);
    setApplicationVersion(_MYCRAWLER_SERVER_VERSION_);
    installTranslator();
    installLoggers();

    //installLoggerMsgBox(&GraphMainWindow);
}

void MCGraphApplication::run() {
  ILogger::Debug() << QString("Test a debug message (int %1)").arg(42);
  ILogger::Warning() << "Test a warning message (int " << 55 << ")";
  ILogger::Log(ILogger::ErrorLevel) << "Test an error message (int " << 688 << ")";
  ILogger::Log(ILogger::InformationLevel, "Test an information message (int %d)", 654);
  GraphMainWindow.show();

}
