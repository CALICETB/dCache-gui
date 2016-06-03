/**
 * @file
 * @author  Eldwan Brianne <eldwan.brianne@desy.de>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 *
 */

#include "dCacheMainWindow.h"

//-- Qt headers
#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{

	/**
	 * Simple Splash screen
	 */

	cout << "#####################################" << endl;
	cout << "#                                   #" << endl;
	cout << "#            dCache-gui             #" << endl;
	cout << "#    Developed by E.Brianne (DESY)  #" << endl;
	cout << "#     GUI to copy files on dCache   #" << endl;
	cout << "#                                   #" << endl;
	cout << "#####################################" << endl;

	/**
	 * Start the GUI
	 */

	QApplication a(argc, argv);
	dCacheMainWindow w;
	w.show();

	return a.exec();
}
