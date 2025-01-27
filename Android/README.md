
# :iphone: Android Installation Information:

<br />
<br />


This installation information is intended to provide a guide on how to install and deploy a modified version of the Dinkplay for Android Mobile Application, produced by relinking the Corresponding Application Code with a modified interface-compatible version of the Minimal Corresponding Source(Qt Libraries).
See https://www.gnu.org/licenses/lgpl-3.0.en.html#license-text

<br />
<br />

### :grey_exclamation: Disclaimer
The information provided below is intended for Educational purposes.
By proceeding with these steps, you are agreeing that the Author will not be held liable for any losses or damages that may arise due to improper or unlawful usage of its content. 

<br />
<br />

### :feet: Steps:

To be able to relink the Corresponding Application Code with a modified interface-compatible version of a Qt Library, first Download the appropriate dinkplay*.zip from the zip files available under the Dinkplay_Sources/Android/ directory of this Repo.<br />

Both the Minimal Corresponding Source and the Qt Libraries are available as shared libraries inside the /lib/ directory in the appropriate dinkplay*.zip you have downloaded. <br />

To make modifications on the portions of the Library contained in the Combined Work, you can easily use an archiving tool such as WinRAR to browse and replace any of the Qt LGPLv3 shared library, with your custom interface compatible shared library.
<br />

After making modifications, you might want to deploy the app to an android device. To do so, you must first sign your final copy of the  
The page on https://developer.android.com/tools/apksigner explains how to sign an apk using the Android-approved way.
After signing, now you can xopy the apk into your android phone and install it as you would install a normal apk application.
<br />


There you have it..



