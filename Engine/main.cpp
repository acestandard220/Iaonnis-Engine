#include "Application.h"


int main()
{
	Iaonnis::Application application;
	application.InitializeApplication();
	application.OnUpdate();
	application.Shutdown();
}


