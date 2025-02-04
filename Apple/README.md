# :iphone: iOS Installation Information:

<br />
<br />


This installation information is intended to provide a guide on how to install and deploy a modified version of the Dinkplay for iOS Application, produced by relinking the Corresponding Application Code with a modified interface-compatible version of the Minimal Corresponding Source(Qt Libraries).
See https://www.gnu.org/licenses/lgpl-3.0.en.html#license-text

<br />
<br />

### :grey_exclamation: Disclaimer
The information provided below is intended for Educational purposes.
By proceeding with these steps, you are agreeing that the Author will not be held liable for any losses or damages that may arise due to improper or unlawful usage of its content. 

<br />
<br />

### :feet: Steps:

#1. To be able to relink the Corresponding Application Code with a modified interface-compatible version of the Minimal Corresponding Source, you will need access to a development machine running MacOS (version >= Sonoma). Then on this machine, you need to setup Xcode Version 16, and Qt 6.8.<br />
I followed this setup guide provided by Qt Group for installing all these tools: https://doc.qt.io/qt-6/ios.html <br />
Then install Qt Creator on MacOS using this guide from QT Group https://doc.qt.io/qtcreator/creator-how-to-install.html<br />
You must complete the setup guide provided by Qt Group in the links above before moving on to Step #2 below, same way you must complete every single step before moving to the next one.

<br />

#2. Download the Corresponding Application Code of this project by cloning this repo:
```bash
git clone https://github.com/Adinkra-Ideas/Dinkplay_Sources.git && cd Dinkplay_Sources
```
Or alternately, you can download and extract the zip file from here [Dinkplay_Sources](https://github.com/Adinkra-Ideas/Dinkplay_Sources/archive/refs/heads/main.zip)

<br />

#3. After following the setup guide provided by Qt Group (linked in Step #1 above), you should have Xcode and Qt Creator running on your dev machine at this point. <br />
Now you can open the project from your Qt Creator. <br />
To do so from Qt Creator, click on "Open Project ..." from the welcome screen, and from the Dialog, select the CMakeLists.txt file inside the Dinkplay_Sources directory created in Step #2 above.<br />
The lines where the LGPLv3 Minimal Corresponding Source Qt Libraries were included into the Combined Work are present inside the DEFINES.qml and CMakeLists.txt files; both located inside the root directory of the project. <br />
I made these lines prominent therein, using the "LGPLv3 Compatible" text as EOL Comments. <br />
To modify, you can replace any of these Includes with another interface-compatible version of your choice. 


<br />


#4. When you're done modifying, you may want to build the project.<br />
To do so without an arm64 Simulator, first you have to turn on developer mode option on the deployment arm64 iOS device. Apple has a tutorial for how to turn on developer mode here https://developer.apple.com/documentation/xcode/enabling-developer-mode-on-a-device.

To deploy the built executable into your iOS device, you will need to create an Apple development provisioning profile.<br />
Apple provides a guide on this link on how to do so: https://developer.apple.com/help/account/manage-profiles/create-a-development-provisioning-profile/ <br />
After following the aforementioned guide and setting up the development provisioning profile, you may modify the Dinkplay_Sources/Apple/iOS/appDinkplay.entitlements file to comment out and replace the two occurrences of the texts "XXXXXXXXXX" with your unique Team Identifier that you received.

Now you can connect your iOS device to your development machine, then click the Run icon from the opened project in Qt creator. <br />
Qt will now build and deploy Dinkplay into your connected iOS device.<br />


<br />


There you have it..

