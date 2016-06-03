#include "dCacheMainWindow.h"
#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{

	cout << "#####################################" << endl;
	cout << "#                                   #" << endl;
	cout << "#            dCache-gui             #" << endl;
	cout << "#    Developed by E.Brianne (DESY)  #" << endl;
	cout << "#     GUI to copy files on dCache   #" << endl;
	cout << "#                                   #" << endl;
	cout << "#####################################" << endl;

	QApplication a(argc, argv);
	dCacheMainWindow w;
	w.show();

	return a.exec();
}
