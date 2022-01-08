Windows Sensor Plugin for OpenCPN
==============================

A simple plugin that uses the Windows Sensor API to provide a position fix to OpenCPN.
Designed for Microsoft Surface computers that incorporate a GNSS chipset.

Obtaining the source code
-------------------------

git clone https://github.com/twocanplugin/WindowsSensor.git


Build Environment
-----------------

This plugin, is only designed for Windows.

This plugin builds outside of the OpenCPN source tree

Refer to the OpenCPN developer manual for details regarding the other requirements such as git, cmake and wxWidgets.

This plugin uses the Continuous Integration (CI) build process

Build Commands
--------------
 mkdir WindowsSensor/build

 cd WindowsSensor/build

 cmake ..

 cmake --build . --config debug

  or

 cmake --build . --config release

Installation
------------

This plugin can be installed using thr OpenCPN Plugin Manager.

Problems
--------

Please send bug reports/questions/comments to the opencpn forum or via email to twocanplugin@hotmail.com

License
-------
The plugin code is licensed under the terms of the GPL v3 or, at your convenience, a later version.