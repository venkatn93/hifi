#include "svoviewer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	SvoViewer svoV(argc, argv);
	return svoV.exec();
}
