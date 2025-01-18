# iOS Installation Information:


This installation information is intended to provide a guide on how to install and deploy a modified version of the Dinkplay for iOS Application, produced by relinking the Corresponding Application Code with a modified interface-compatible version of the Minimal Corresponding Source(Qt Libraries).
See https://www.gnu.org/licenses/lgpl-3.0.en.html#license-text

Disclaimer: The tutorial outlined below is intended for Educational purpose.
By proceeding with these steps, you are agreeing that the Author will not be held liable for any loss or damage that may occur due to improper or unlawful usage of its content. 

Steps:

#1 To be able to relink the Corresponding Application Code with a modified interface-compatible version of the Minimal Corresponding Source, you will need a computer running the MacOS (version >= Sonoma), Xcode Version 16, and Qt 6.8.
I followed this guide provided by Qt Group to install all of these: https://doc.qt.io/qt-6/macos.html
You have to complete this step #1 before moving on to step #2 below, and keep doing so with every single step.

#2 Download the Corresponding Application Code and Minimal Corresponding Source of this project by cloning this repo:
git clone ... && cd ./...
Or alternately, download and extract the zip file for here https

#3 After following the guide provided by Qt Group (linked in #1 above), you should have Xcode and Qt Creator running on your dev machine. Now you can open the project from your Qt Creator. To do so, click on "open existing project"), and from the dialogue, select the cmakelist.txt file  from inside the directory created in #2 above.
The lines where the LGPLv3 Minimal Corresponding Source Qt Libraries were included into the Combined Work are present inside the DEFINES.qml and Cmakelist.txt files; both located inside the root directory of the project. 
I commented these lines with "LGPLv3 compatible" to make them prominent. 
To modify, you can replace any of these includes with another interface-compatible version provided of your choice. 

#4 To build the project, simply click on the Hammer Icon at the bottom left of Qt Creator. But to deploy to your iOS device, you will need to create a development provisioning profile for your Xcode. Apple provides a guide on this link on how to do so: https://developer.apple.com/help/account/manage-profiles/create-a-development-provisioning-profile/
After following the aforementioned guide and setting up the development provisioning profile, you can now connect your iOS device to your development machine, then click the Run icon from the opened project in Qt creator. 
Qt will now build and deploy Dinkplay into your connected iOS device.

There you have it..
