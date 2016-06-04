# GUI for dCache

## Information

This program has been developed by E. Brianne at DESY. 
This tools is used to upload testbeam data from the CALICE Collaboration directly to the dCache at DESY. 
A wiki is available at http://flcwiki.desy.de/GridInstall to install the environment needed for the tools (Grid proxy, gfal-utils...)

## Requirements

* g++ 4.7 minimum
* CMake 2.6 minimum
* Qt version 5 REQUIRED
* Doxygen (optional)
* Git (optional)
* Grid tools REQUIRED (gfal-utils and voms)

##Installation

Installation is simple. Get the latest version from git : https://github.com/ebrianne/dCache-gui.git
<pre>
git clone https://github.com/ebrianne/dCache-gui.git
</pre>
Create a build folder :
<pre>
mkdir build
cd build
</pre>
Then do the build by typing :
<pre>
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt5/ -DIS_DEVEL=OFF -DCMAKE_INSTALL_PREFIX=/path/to/dCache_src/build -DUSE_GIT=ON -DBUILD_DOCUMENTATION=ON ..
make
make install
</pre>
or
<pre>
export QTDIR=/path/to/Qt5/
cmake -DIS_DEVEL=OFF -DCMAKE_INSTALL_PREFIX=/path/to/dCache_src/build -DUSE_GIT=ON -DBUILD_DOCUMENTATION=ON ..
make
make install
</pre>
