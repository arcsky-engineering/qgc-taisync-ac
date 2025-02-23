diff --git a/android/AndroidManifest.xml b/android/AndroidManifest.xml
index 1a97b94f4..65f62f478 100644
--- a/android/AndroidManifest.xml
+++ b/android/AndroidManifest.xml
@@ -1,5 +1,5 @@
 <?xml version="1.0"?>
-<manifest package="org.mavlink.qgroundcontrol" xmlns:android="http://schemas.android.com/apk/res/android" android:versionName="-- %%INSERT_VERSION_NAME%% --" android:versionCode="-- %%INSERT_VERSION_CODE%% --" android:installLocation="auto">
+<manifest package="%%QGC_INSERT_PACKAGE_NAME%%" xmlns:android="http://schemas.android.com/apk/res/android" android:versionName="-- %%INSERT_VERSION_NAME%% --" android:versionCode="-- %%INSERT_VERSION_CODE%% --" android:installLocation="auto">
     <!-- The following comment will be replaced upon deployment with default permissions based on the dependencies of the application.
          Remove the comment if you do not require these default permissions. -->
     <!-- %%INSERT_PERMISSIONS -->
@@ -9,8 +9,8 @@
     <!-- %%INSERT_FEATURES -->
 
     <supports-screens android:largeScreens="true" android:normalScreens="true" android:anyDensity="true" android:smallScreens="true"/>
-    <application android:hardwareAccelerated="true" android:name="org.qtproject.qt5.android.bindings.QtApplication" android:label="-- %%INSERT_APP_NAME%% --" android:extractNativeLibs="true" android:icon="@drawable/icon">
-        <activity android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|layoutDirection|locale|fontScale|keyboard|keyboardHidden|navigation|mcc|mnc|density" android:name="org.mavlink.qgroundcontrol.QGCActivity" android:label="-- %%INSERT_APP_NAME%% --" android:screenOrientation="sensorLandscape" android:launchMode="singleTask" android:keepScreenOn="true">
+    <application android:hardwareAccelerated="true" android:name="org.qtproject.qt5.android.bindings.QtApplication" android:label="Arcsky Control" android:extractNativeLibs="true" android:icon="@drawable/icon">
+        <activity android:configChanges="orientation|uiMode|screenLayout|screenSize|smallestScreenSize|layoutDirection|locale|fontScale|keyboard|keyboardHidden|navigation|mcc|mnc|density" android:name="org.mavlink.qgroundcontrol.QGCActivity" android:label="Arcsky Control" android:screenOrientation="sensorLandscape" android:launchMode="singleTask" android:keepScreenOn="true">
             <intent-filter>
                 <action android:name="android.intent.action.MAIN"/>
                 <category android:name="android.intent.category.LAUNCHER"/>
diff --git a/android/res/drawable-hdpi/icon.png b/android/res/drawable-hdpi/icon.png
old mode 100644
new mode 100755
index f6323cd90..cc1a64b02
Binary files a/android/res/drawable-hdpi/icon.png and b/android/res/drawable-hdpi/icon.png differ
diff --git a/android/res/drawable-ldpi/icon.png b/android/res/drawable-ldpi/icon.png
old mode 100644
new mode 100755
index 8676e6502..013b7a905
Binary files a/android/res/drawable-ldpi/icon.png and b/android/res/drawable-ldpi/icon.png differ
diff --git a/android/res/drawable-mdpi/icon.png b/android/res/drawable-mdpi/icon.png
old mode 100644
new mode 100755
index c1c760349..803beeca6
Binary files a/android/res/drawable-mdpi/icon.png and b/android/res/drawable-mdpi/icon.png differ
diff --git a/android/res/drawable-xhdpi/icon.png b/android/res/drawable-xhdpi/icon.png
old mode 100644
new mode 100755
index 58a1454d6..8146acf4c
Binary files a/android/res/drawable-xhdpi/icon.png and b/android/res/drawable-xhdpi/icon.png differ
diff --git a/android/res/drawable-xxhdpi/icon.png b/android/res/drawable-xxhdpi/icon.png
old mode 100644
new mode 100755
index f74d76f41..eff393363
Binary files a/android/res/drawable-xxhdpi/icon.png and b/android/res/drawable-xxhdpi/icon.png differ
diff --git a/android/res/drawable-xxxhdpi/icon.png b/android/res/drawable-xxxhdpi/icon.png
old mode 100644
new mode 100755
index f74d76f41..c00e24f8c
Binary files a/android/res/drawable-xxxhdpi/icon.png and b/android/res/drawable-xxxhdpi/icon.png differ
diff --git a/libs/Frameworks/SDL2.framework/SDL2 b/libs/Frameworks/SDL2.framework/SDL2
index 250a9d6fc..3798c6ff6 120000
Binary files a/libs/Frameworks/SDL2.framework/SDL2 and b/libs/Frameworks/SDL2.framework/SDL2 differ
diff --git a/qgcimages.qrc b/qgcimages.qrc
index e300b5f6b..1a3356d3d 100644
--- a/qgcimages.qrc
+++ b/qgcimages.qrc
@@ -207,5 +207,6 @@
         <file alias="Yield.svg">src/ui/toolbar/Images/Yield.svg</file>
         <file alias="ZoomMinus.svg">src/FlightMap/Images/ZoomMinus.svg</file>
         <file alias="ZoomPlus.svg">src/FlightMap/Images/ZoomPlus.svg</file>
+        <file alias="FuelTank.svg">src/ui/toolbar/Images/FuelTank.svg</file>
     </qresource>
 </RCC>
diff --git a/qgroundcontrol.pro b/qgroundcontrol.pro
index 300bfa7d2..dab70a957 100644
--- a/qgroundcontrol.pro
+++ b/qgroundcontrol.pro
@@ -63,11 +63,11 @@ WindowsBuild {
 # Branding
 #
 
-QGC_APP_NAME        = "QGroundControl"
-QGC_ORG_NAME        = "QGroundControl.org"
-QGC_ORG_DOMAIN      = "org.qgroundcontrol"
-QGC_APP_DESCRIPTION = "Open source ground control app provided by QGroundControl dev team"
-QGC_APP_COPYRIGHT   = "Copyright (C) 2019 QGroundControl Development Team. All rights reserved."
+QGC_APP_NAME        = "Arcsky Control"
+QGC_ORG_NAME        = "Arcsky"
+QGC_ORG_DOMAIN      = "www.arcskytech.com"
+QGC_APP_DESCRIPTION = "Arcsky Ground Control Station"
+QGC_APP_COPYRIGHT   = "Copyright (C) 2024 Arcsky. All rights reserved."
 
 WindowsBuild {
     QGC_INSTALLER_SCRIPT        = "$$SOURCE_DIR\\deploy\\windows\\nullsoft_installer.nsi"
@@ -359,9 +359,9 @@ CustomBuild {
         RESOURCES += $$PWD/resources/InstrumentValueIcons/InstrumentValueIcons.qrc
     }
 } else {
-    DEFINES += QGC_APPLICATION_NAME=\"\\\"QGroundControl\\\"\"
-    DEFINES += QGC_ORG_NAME=\"\\\"QGroundControl.org\\\"\"
-    DEFINES += QGC_ORG_DOMAIN=\"\\\"org.qgroundcontrol\\\"\"
+    DEFINES += QGC_APPLICATION_NAME=\"\\\"Arcsky Control\\\"\"
+    DEFINES += QGC_ORG_NAME=\"\\\"www.arcskytech.com\\\"\"
+    DEFINES += QGC_ORG_DOMAIN=\"\\\"www.arcskytech.com\\\"\"
     RESOURCES += \
         $$PWD/qgroundcontrol.qrc \
         $$PWD/qgcresources.qrc \
diff --git a/resources/NoVideoBackground.jpg b/resources/NoVideoBackground.jpg
old mode 100644
new mode 100755
index 678aa1460..63eb32d06
Binary files a/resources/NoVideoBackground.jpg and b/resources/NoVideoBackground.jpg differ
diff --git a/resources/QGCLogoBlack.svg b/resources/QGCLogoBlack.svg
old mode 100644
new mode 100755
index b242ff8c9..e057ebc8a
--- a/resources/QGCLogoBlack.svg
+++ b/resources/QGCLogoBlack.svg
@@ -1,30 +1,25 @@
-<?xml version="1.0" encoding="utf-8"?>
-<!-- Generator: Adobe Illustrator 19.2.1, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
-<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
-	 viewBox="0 0 215 215" style="enable-background:new 0 0 215 215;" xml:space="preserve">
-<style type="text/css">
-	.st0{fill:#231F20;}
-	.st1{fill:#5ECBF1;}
-	.st2{fill:none;stroke:#231F20;stroke-width:8;stroke-miterlimit:10;}
-</style>
-<path class="st0" d="M164.063,148.971l-10.179-10.179c5.993-8.679,9.511-19.197,9.511-30.54h-0.044
-	c0-27.905-21.19-50.857-48.345-53.617v23.318c15.334,2.768,26.72,16.891,25.137,33.349c-0.364,3.787-1.443,7.384-3.088,10.66
-	l-11.846-11.846l-16.406,16.406l10.812,10.812c-2.239,0.785-4.59,1.32-7.02,1.557c-16.415,1.601-30.514-9.699-33.36-24.959H55.903
-	c2.84,27.071,25.752,48.171,53.597,48.171v0.044c10.015,0,19.385-2.745,27.417-7.511l10.74,10.74L164.063,148.971z"/>
-<path class="st1" d="M103.747,39.064c-0.129,0-0.377,0.018-0.385,0.019c-33.409,2.944-60.044,29.582-62.971,63.031
-	c-0.002,0.023-0.021,0.329-0.022,0.358c-0.001,0.021-0.009,0.19-0.009,0.264c0,2.159,1.75,3.909,3.909,3.909
-	c2.097,0,3.808-1.651,3.904-3.723c0,0-0.002-0.001-0.002-0.001c2.546-29.719,26.159-53.412,55.822-56.045l0,0
-	c1.981-0.125,3.565-1.728,3.658-3.717c0.003-0.062,0.004-0.123,0.004-0.186C107.656,40.814,105.906,39.064,103.747,39.064z"/>
-<path class="st1" d="M103.752,54.604c-0.14,0-0.278,0.01-0.414,0.024c-0.002,0-0.056,0.005-0.061,0.006
-	c-25.063,2.893-45.007,23.272-47.393,47.639c0,0-0.03,0.32-0.03,0.486c0,2.159,1.75,3.909,3.909,3.909
-	c2.097,0,3.808-1.651,3.904-3.723c0.001-0.021,0.001-0.042,0.001-0.063c2.451-21.156,19.198-37.949,40.325-40.463l0-0.019
-	c1.982-0.124,3.569-1.709,3.662-3.7c0.003-0.061,0.004-0.123,0.004-0.185C107.66,56.354,105.91,54.604,103.752,54.604z"/>
-<path class="st1" d="M103.747,70.208c-0.247,0-0.494,0.024-0.729,0.068C86.917,73.008,74.167,85.81,71.513,101.94
-	c-0.009,0.043-0.015,0.086-0.023,0.129c-0.042,0.236-0.071,0.562-0.071,0.704c0,2.159,1.75,3.909,3.909,3.909
-	c0.155,0,0.308-0.009,0.458-0.027c1.826-0.213,3.268-1.686,3.434-3.526c0.005-0.057,0.031-0.316,0.038-0.357
-	c2.255-12.578,12.164-22.477,24.735-24.752c0,0,0.142-0.013,0.212-0.022c1.882-0.22,3.356-1.777,3.446-3.697
-	c0.003-0.062,0.004-0.123,0.004-0.186C107.656,71.958,105.906,70.208,103.747,70.208z"/>
-<g>
-	<path class="st2" d="M211.5,151.5c0,33-27,60-60,60h-88c-33,0-60-27-60-60v-88c0-33,27-60,60-60h88c33,0,60,27,60,60V151.5z"/>
-</g>
-</svg>
+<?xml version="1.0" encoding="UTF-8"?>
+<svg id="Layer_2" data-name="Layer 2" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 88.13 105.27">
+  <defs>
+    <style>
+      .cls-1 {
+        fill: #fff;
+        stroke-width: 0px;
+      }
+    </style>
+  </defs>
+  <g id="Logo_Only_White">
+    <g>
+      <g>
+        <path class="cls-1" d="M22.23,77.9L0,89,18.62,18.71h0c1.53-7.01,5.8-12.11,11.63-15.28l-8.03,74.47Z"/>
+        <polygon class="cls-1" points="24.24 82.18 40.84 94.25 17.34 105.27 .49 94.04 24.24 82.18"/>
+        <path class="cls-1" d="M40.84,0v88.41l-13.93-10.13L35.21,1.32c1.79-.59,3.68-1.03,5.63-1.32Z"/>
+      </g>
+      <g>
+        <path class="cls-1" d="M65.9,77.9l22.23,11.1-18.62-70.29h0c-1.53-7.01-5.8-12.11-11.63-15.28l8.03,74.47Z"/>
+        <polygon class="cls-1" points="63.89 82.18 47.29 94.25 70.79 105.27 87.64 94.04 63.89 82.18"/>
+        <path class="cls-1" d="M47.29,0v88.41s13.93-10.13,13.93-10.13L52.92,1.32C51.13.73,49.24.29,47.29,0Z"/>
+      </g>
+    </g>
+  </g>
+</svg>
\ No newline at end of file
diff --git a/resources/QGCLogoFull.svg b/resources/QGCLogoFull.svg
old mode 100644
new mode 100755
index ebfb890a6..e057ebc8a
--- a/resources/QGCLogoFull.svg
+++ b/resources/QGCLogoFull.svg
@@ -1,27 +1,25 @@
-<?xml version="1.0" encoding="utf-8"?>
-<!-- Generator: Adobe Illustrator 21.1.0, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
-<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
-	 width="72px" height="72px" viewBox="0 0 72 72" style="enable-background:new 0 0 72 72;" xml:space="preserve">
-<style type="text/css">
-	.st0{fill:#FFFFFF;}
-	.st1{fill:#4B2C6D;stroke:#FFFFFF;stroke-width:3;stroke-miterlimit:10;}
-	.st2{fill:#5ECBF1;}
-</style>
-<rect x="10.555" y="10.213" class="st0" width="50.924" height="50.582"/>
-<path class="st1" d="M50.296,1.8H21.704C10.726,1.8,1.8,10.726,1.8,21.704v28.591C1.8,61.274,10.726,70.2,21.704,70.2h28.591
-	c10.978,0,19.904-8.926,19.904-19.904V21.704C70.2,10.726,61.274,1.8,50.296,1.8z M49.748,55.802l-3.659-3.659
-	c-2.736,1.642-5.951,2.565-9.371,2.565l0,0c-9.508,0-17.374-7.216-18.331-16.484h7.969c0.958,5.233,5.78,9.097,11.423,8.55
-	c0.821-0.068,1.642-0.274,2.394-0.547l-3.728-3.728l5.609-5.609l4.07,4.036c0.547-1.129,0.923-2.36,1.06-3.659
-	c0.547-5.643-3.352-10.465-8.584-11.389v-7.969c9.268,0.958,16.519,8.789,16.519,18.331l0,0c0,3.865-1.197,7.49-3.249,10.431
-	l3.488,3.488L49.748,55.802z"/>
-<path class="st2" d="M34.7,12.607c-0.034,0-0.137,0-0.137,0c-11.423,0.992-20.52,10.123-21.546,21.546c0,0,0,0.103,0,0.137
-	c0,0,0,0.068,0,0.103c0,0.752,0.616,1.334,1.334,1.334c0.718,0,1.3-0.581,1.334-1.265l0,0c0.855-10.157,8.96-18.263,19.084-19.152
-	l0,0c0.684-0.034,1.231-0.581,1.265-1.265c0-0.034,0-0.034,0-0.068C36.068,13.189,35.453,12.607,34.7,12.607z"/>
-<path class="st2" d="M34.7,17.908c-0.034,0-0.103,0-0.137,0h-0.034c-8.584,0.992-15.39,7.969-16.211,16.279c0,0,0,0.103,0,0.171
-	c0,0.752,0.616,1.334,1.334,1.334c0.718,0,1.3-0.581,1.334-1.265c0,0,0,0,0-0.034c0.855-7.25,6.566-12.996,13.783-13.851l0,0
-	c0.684-0.034,1.231-0.581,1.265-1.265c0-0.034,0-0.034,0-0.068C36.068,18.49,35.453,17.908,34.7,17.908z"/>
-<path class="st2" d="M34.7,23.243c-0.068,0-0.171,0-0.239,0.034c-5.506,0.923-9.85,5.301-10.773,10.807v0.034
-	c0,0.068-0.034,0.205-0.034,0.239c0,0.752,0.581,1.334,1.334,1.334c0.068,0,0.103,0,0.171,0c0.616-0.068,1.129-0.581,1.163-1.197
-	c0-0.034,0-0.103,0-0.137c0.787-4.275,4.172-7.661,8.482-8.447c0,0,0.034,0,0.068,0c0.65-0.068,1.163-0.616,1.163-1.265
-	c0-0.034,0-0.034,0-0.068C36.068,23.859,35.453,23.243,34.7,23.243z"/>
-</svg>
+<?xml version="1.0" encoding="UTF-8"?>
+<svg id="Layer_2" data-name="Layer 2" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 88.13 105.27">
+  <defs>
+    <style>
+      .cls-1 {
+        fill: #fff;
+        stroke-width: 0px;
+      }
+    </style>
+  </defs>
+  <g id="Logo_Only_White">
+    <g>
+      <g>
+        <path class="cls-1" d="M22.23,77.9L0,89,18.62,18.71h0c1.53-7.01,5.8-12.11,11.63-15.28l-8.03,74.47Z"/>
+        <polygon class="cls-1" points="24.24 82.18 40.84 94.25 17.34 105.27 .49 94.04 24.24 82.18"/>
+        <path class="cls-1" d="M40.84,0v88.41l-13.93-10.13L35.21,1.32c1.79-.59,3.68-1.03,5.63-1.32Z"/>
+      </g>
+      <g>
+        <path class="cls-1" d="M65.9,77.9l22.23,11.1-18.62-70.29h0c-1.53-7.01-5.8-12.11-11.63-15.28l8.03,74.47Z"/>
+        <polygon class="cls-1" points="63.89 82.18 47.29 94.25 70.79 105.27 87.64 94.04 63.89 82.18"/>
+        <path class="cls-1" d="M47.29,0v88.41s13.93-10.13,13.93-10.13L52.92,1.32C51.13.73,49.24.29,47.29,0Z"/>
+      </g>
+    </g>
+  </g>
+</svg>
\ No newline at end of file
diff --git a/resources/QGCLogoWhite.svg b/resources/QGCLogoWhite.svg
old mode 100644
new mode 100755
index 9f045f9b2..e057ebc8a
--- a/resources/QGCLogoWhite.svg
+++ b/resources/QGCLogoWhite.svg
@@ -1,30 +1,25 @@
-<?xml version="1.0" encoding="utf-8"?>
-<!-- Generator: Adobe Illustrator 19.2.1, SVG Export Plug-In . SVG Version: 6.00 Build 0)  -->
-<svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px"
-	 viewBox="0 0 215 215" style="enable-background:new 0 0 215 215;" xml:space="preserve">
-<style type="text/css">
-	.st0{fill:#FFFFFF;}
-	.st1{fill:#5ECBF1;}
-	.st2{fill:none;stroke:#FFFFFF;stroke-width:8;stroke-miterlimit:10;}
-</style>
-<path class="st0" d="M164.063,148.971l-10.179-10.179c5.993-8.679,9.511-19.197,9.511-30.54h-0.044
-	c0-27.905-21.19-50.857-48.345-53.617v23.318c15.334,2.768,26.72,16.891,25.137,33.349c-0.364,3.787-1.443,7.384-3.088,10.66
-	l-11.846-11.846l-16.406,16.406l10.812,10.812c-2.239,0.785-4.59,1.32-7.02,1.557c-16.415,1.601-30.514-9.699-33.36-24.959H55.903
-	c2.84,27.071,25.752,48.171,53.597,48.171v0.044c10.015,0,19.385-2.745,27.417-7.511l10.74,10.74L164.063,148.971z"/>
-<path class="st1" d="M103.747,39.064c-0.129,0-0.377,0.018-0.385,0.019c-33.409,2.944-60.044,29.582-62.971,63.031
-	c-0.002,0.023-0.021,0.329-0.022,0.358c-0.001,0.021-0.009,0.19-0.009,0.264c0,2.159,1.75,3.909,3.909,3.909
-	c2.097,0,3.808-1.651,3.904-3.723c0,0-0.002-0.001-0.002-0.001c2.546-29.719,26.159-53.412,55.822-56.045l0,0
-	c1.981-0.125,3.565-1.728,3.658-3.717c0.003-0.062,0.004-0.123,0.004-0.186C107.656,40.814,105.906,39.064,103.747,39.064z"/>
-<path class="st1" d="M103.752,54.604c-0.14,0-0.278,0.01-0.414,0.024c-0.002,0-0.056,0.005-0.061,0.006
-	c-25.063,2.893-45.007,23.272-47.393,47.639c0,0-0.03,0.32-0.03,0.486c0,2.159,1.75,3.909,3.909,3.909
-	c2.097,0,3.808-1.651,3.904-3.723c0.001-0.021,0.001-0.042,0.001-0.063c2.451-21.156,19.198-37.949,40.325-40.463l0-0.019
-	c1.982-0.124,3.569-1.709,3.662-3.7c0.003-0.061,0.004-0.123,0.004-0.185C107.66,56.354,105.91,54.604,103.752,54.604z"/>
-<path class="st1" d="M103.747,70.208c-0.247,0-0.494,0.024-0.729,0.068C86.917,73.008,74.167,85.81,71.513,101.94
-	c-0.009,0.043-0.015,0.086-0.023,0.129c-0.042,0.236-0.071,0.562-0.071,0.704c0,2.159,1.75,3.909,3.909,3.909
-	c0.155,0,0.308-0.009,0.458-0.027c1.826-0.213,3.268-1.686,3.434-3.526c0.005-0.057,0.031-0.316,0.038-0.357
-	c2.255-12.578,12.164-22.477,24.735-24.752c0,0,0.142-0.013,0.212-0.022c1.882-0.22,3.356-1.777,3.446-3.697
-	c0.003-0.062,0.004-0.123,0.004-0.186C107.656,71.958,105.906,70.208,103.747,70.208z"/>
-<g>
-	<path class="st2" d="M211.5,151.5c0,33-27,60-60,60h-88c-33,0-60-27-60-60v-88c0-33,27-60,60-60h88c33,0,60,27,60,60V151.5z"/>
-</g>
-</svg>
+<?xml version="1.0" encoding="UTF-8"?>
+<svg id="Layer_2" data-name="Layer 2" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 88.13 105.27">
+  <defs>
+    <style>
+      .cls-1 {
+        fill: #fff;
+        stroke-width: 0px;
+      }
+    </style>
+  </defs>
+  <g id="Logo_Only_White">
+    <g>
+      <g>
+        <path class="cls-1" d="M22.23,77.9L0,89,18.62,18.71h0c1.53-7.01,5.8-12.11,11.63-15.28l-8.03,74.47Z"/>
+        <polygon class="cls-1" points="24.24 82.18 40.84 94.25 17.34 105.27 .49 94.04 24.24 82.18"/>
+        <path class="cls-1" d="M40.84,0v88.41l-13.93-10.13L35.21,1.32c1.79-.59,3.68-1.03,5.63-1.32Z"/>
+      </g>
+      <g>
+        <path class="cls-1" d="M65.9,77.9l22.23,11.1-18.62-70.29h0c-1.53-7.01-5.8-12.11-11.63-15.28l8.03,74.47Z"/>
+        <polygon class="cls-1" points="63.89 82.18 47.29 94.25 70.79 105.27 87.64 94.04 63.89 82.18"/>
+        <path class="cls-1" d="M47.29,0v88.41s13.93-10.13,13.93-10.13L52.92,1.32C51.13.73,49.24.29,47.29,0Z"/>
+      </g>
+    </g>
+  </g>
+</svg>
\ No newline at end of file
diff --git a/resources/icons/android_512x512.png b/resources/icons/android_512x512.png
old mode 100644
new mode 100755
index 6c3e9913b..eacf71913
Binary files a/resources/icons/android_512x512.png and b/resources/icons/android_512x512.png differ
diff --git a/resources/icons/android_dev_512x512.png b/resources/icons/android_dev_512x512.png
old mode 100644
new mode 100755
index 41f369561..eacf71913
Binary files a/resources/icons/android_dev_512x512.png and b/resources/icons/android_dev_512x512.png differ
diff --git a/resources/icons/qgroundcontrol.ico b/resources/icons/qgroundcontrol.ico
old mode 100644
new mode 100755
index 3cd85b3aa..790658a5e
Binary files a/resources/icons/qgroundcontrol.ico and b/resources/icons/qgroundcontrol.ico differ
diff --git a/resources/icons/qgroundcontrol.png b/resources/icons/qgroundcontrol.png
old mode 100644
new mode 100755
index 8a313ccde..eacf71913
Binary files a/resources/icons/qgroundcontrol.png and b/resources/icons/qgroundcontrol.png differ
diff --git a/src/AnalyzeView/AnalyzeView.qml b/src/AnalyzeView/AnalyzeView.qml
index 798006cf8..0f91d8ca5 100644
--- a/src/AnalyzeView/AnalyzeView.qml
+++ b/src/AnalyzeView/AnalyzeView.qml
@@ -89,6 +89,7 @@ Rectangle {
                     setupIndicator:     false
                     exclusiveGroup:     setupButtonGroup
                     text:               modelData.title
+                    //visible:            modelData.title !== "qrc:/qml/LogDownloadPage.qml" && modelData.url !== "qrc:/qml/MavlinkConsolePage.qml" && modelData.url !== "qrc:/qml/VibrationPage.qml"
 
                     onClicked: {
                         panelLoader.source  = modelData.url
diff --git a/src/AutoPilotPlugins/APM/APMFlightModesComponent.qml b/src/AutoPilotPlugins/APM/APMFlightModesComponent.qml
index 11b3f4069..e98c7e329 100644
--- a/src/AutoPilotPlugins/APM/APMFlightModesComponent.qml
+++ b/src/AutoPilotPlugins/APM/APMFlightModesComponent.qml
@@ -50,132 +50,132 @@ SetupPage {
             width:      availableWidth
             spacing:     _margins
 
-            Column {
-                spacing: _margins
-
-                QGCLabel {
-                    id:             flightModeLabel
-                    text:           qsTr("Flight Mode Settings") + (_fltmodeChExists ? "" : qsTr(" (Channel 5)"))
-                    font.family:    ScreenTools.demiboldFontFamily
-                }
-
-                Rectangle {
-                    id:     flightModeSettings
-                    width:  flightModeColumn.width + (_margins * 2)
-                    height: flightModeColumn.height + ScreenTools.defaultFontPixelHeight
-                    color:  qgcPal.windowShade
-
-                    Column {
-                        id:                 flightModeColumn
-                        anchors.margins:    ScreenTools.defaultFontPixelWidth
-                        anchors.left:       parent.left
-                        anchors.top:        parent.top
-                        spacing:            ScreenTools.defaultFontPixelHeight
-
-                        Row {
-                            spacing:    _margins
-                            visible:    _fltmodeChExists
-
-                            QGCLabel {
-                                id:                 modeChannelLabel
-                                anchors.baseline:   modeChannelCombo.baseline
-                                text:               qsTr("Flight mode channel:")
-                            }
-
-                            QGCComboBox {
-                                id:             modeChannelCombo
-                                width:          ScreenTools.defaultFontPixelWidth * 15
-                                model:          [ qsTr("Not assigned"), qsTr("Channel 1"), qsTr("Channel 2"),
-                                    qsTr("Channel 3"),    qsTr("Channel 4"), qsTr("Channel 5"),
-                                    qsTr("Channel 6"),    qsTr("Channel 7"), qsTr("Channel 8") ]
-
-                                currentIndex:   _fltmodeCh.value
-                                onActivated:    _fltmodeCh.value = index
-                            }
-                        }
-
-                        GridLayout {
-                            rows:   _customSimpleMode ? 7 : 6
-                            flow:   GridLayout.TopToBottom
-
-                            QGCLabel { text: ""; visible: _customSimpleMode }
-                            Repeater {
-                                model:  6
-
-                                QGCLabel {
-                                    text:   qsTr("Flight Mode ") + index
-                                    color:  controller.activeFlightMode == index ? "yellow" : qgcPal.text
-
-                                    property int index: modelData + 1
-                                }
-                            }
-
-                            QGCLabel { text: ""; visible: _customSimpleMode }
-                            Repeater {
-                                model:  6
-
-                                FactComboBox {
-                                    Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 15
-                                    fact:                   controller.getParameterFact(-1, _modeParamPrefix + index)
-                                    indexModel:             false
-
-                                    property int index: modelData + 1
-                                }
-                            }
-
-                            QGCLabel {
-                                text:           qsTr("Simple")
-                                font.pointSize: ScreenTools.smallFontPointSize
-                                visible:        _customSimpleMode
-                            }
-                            Repeater {
-                                model:  controller.simpleModeEnabled
-                                QGCCheckBox {
-                                    Layout.alignment:   Qt.AlignHCenter
-                                    visible:            _customSimpleMode
-                                    checked:            modelData
-                                    onClicked:          controller.setSimpleMode(index, checked)
-                                }
-                            }
-
-                            QGCLabel {
-                                text:           qsTr("Super-Simple")
-                                font.pointSize: ScreenTools.smallFontPointSize
-                                visible:        _customSimpleMode
-                            }
-                            Repeater {
-                                model:  controller.superSimpleModeEnabled
-                                QGCCheckBox {
-                                    Layout.alignment:   Qt.AlignHCenter
-                                    visible:            _customSimpleMode
-                                    checked:            modelData
-                                    onClicked:          controller.setSuperSimpleMode(index, checked)
-                                }
-                            }
-
-                            QGCLabel { text: ""; visible: _customSimpleMode }
-                            Repeater {
-                                model:  6
-
-                                QGCLabel { text: _pwmStrings[modelData] }
-                            }
-                        }
-
-                        RowLayout {
-                            spacing: _margins
-                            visible: controller.simpleModesSupported
-
-                            QGCLabel { text: qsTr("Simple Mode") }
-
-                            QGCComboBox {
-                                model:          controller.simpleModeNames
-                                currentIndex:   controller.simpleMode
-                                onActivated:    controller.simpleMode = index
-                            }
-                        }
-                    } // Column - Flight Modes
-                } // Rectangle - Flight Modes
-            } // Column - Flight Modes
+//            Column {
+//                spacing: _margins
+
+//                QGCLabel {
+//                    id:             flightModeLabel
+//                    text:           qsTr("Flight Mode Settings") + (_fltmodeChExists ? "" : qsTr(" (Channel 5)"))
+//                    font.family:    ScreenTools.demiboldFontFamily
+//                }
+
+//                Rectangle {
+//                    id:     flightModeSettings
+//                    width:  flightModeColumn.width + (_margins * 2)
+//                    height: flightModeColumn.height + ScreenTools.defaultFontPixelHeight
+//                    color:  qgcPal.windowShade
+
+//                    Column {
+//                        id:                 flightModeColumn
+//                        anchors.margins:    ScreenTools.defaultFontPixelWidth
+//                        anchors.left:       parent.left
+//                        anchors.top:        parent.top
+//                        spacing:            ScreenTools.defaultFontPixelHeight
+
+//                        Row {
+//                            spacing:    _margins
+//                            visible:    _fltmodeChExists
+
+//                            QGCLabel {
+//                                id:                 modeChannelLabel
+//                                anchors.baseline:   modeChannelCombo.baseline
+//                                text:               qsTr("Flight mode channel:")
+//                            }
+
+//                            QGCComboBox {
+//                                id:             modeChannelCombo
+//                                width:          ScreenTools.defaultFontPixelWidth * 15
+//                                model:          [ qsTr("Not assigned"), qsTr("Channel 1"), qsTr("Channel 2"),
+//                                    qsTr("Channel 3"),    qsTr("Channel 4"), qsTr("Channel 5"),
+//                                    qsTr("Channel 6"),    qsTr("Channel 7"), qsTr("Channel 8") ]
+
+//                                currentIndex:   _fltmodeCh.value
+//                                onActivated:    _fltmodeCh.value = index
+//                            }
+//                        }
+
+//                        GridLayout {
+//                            rows:   _customSimpleMode ? 7 : 6
+//                            flow:   GridLayout.TopToBottom
+
+//                            QGCLabel { text: ""; visible: _customSimpleMode }
+//                            Repeater {
+//                                model:  6
+
+//                                QGCLabel {
+//                                    text:   qsTr("Flight Mode ") + index
+//                                    color:  controller.activeFlightMode == index ? "yellow" : qgcPal.text
+
+//                                    property int index: modelData + 1
+//                                }
+//                            }
+
+//                            QGCLabel { text: ""; visible: _customSimpleMode }
+//                            Repeater {
+//                                model:  6
+
+//                                FactComboBox {
+//                                    Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 15
+//                                    fact:                   controller.getParameterFact(-1, _modeParamPrefix + index)
+//                                    indexModel:             false
+
+//                                    property int index: modelData + 1
+//                                }
+//                            }
+
+//                            QGCLabel {
+//                                text:           qsTr("Simple")
+//                                font.pointSize: ScreenTools.smallFontPointSize
+//                                visible:        _customSimpleMode
+//                            }
+//                            Repeater {
+//                                model:  controller.simpleModeEnabled
+//                                QGCCheckBox {
+//                                    Layout.alignment:   Qt.AlignHCenter
+//                                    visible:            _customSimpleMode
+//                                    checked:            modelData
+//                                    onClicked:          controller.setSimpleMode(index, checked)
+//                                }
+//                            }
+
+//                            QGCLabel {
+//                                text:           qsTr("Super-Simple")
+//                                font.pointSize: ScreenTools.smallFontPointSize
+//                                visible:        _customSimpleMode
+//                            }
+//                            Repeater {
+//                                model:  controller.superSimpleModeEnabled
+//                                QGCCheckBox {
+//                                    Layout.alignment:   Qt.AlignHCenter
+//                                    visible:            _customSimpleMode
+//                                    checked:            modelData
+//                                    onClicked:          controller.setSuperSimpleMode(index, checked)
+//                                }
+//                            }
+
+//                            QGCLabel { text: ""; visible: _customSimpleMode }
+//                            Repeater {
+//                                model:  6
+
+//                                QGCLabel { text: _pwmStrings[modelData] }
+//                            }
+//                        }
+
+//                        RowLayout {
+//                            spacing: _margins
+//                            visible: controller.simpleModesSupported
+
+//                            QGCLabel { text: qsTr("Simple Mode") }
+
+//                            QGCComboBox {
+//                                model:          controller.simpleModeNames
+//                                currentIndex:   controller.simpleMode
+//                                onActivated:    controller.simpleMode = index
+//                            }
+//                        }
+//                    } // Column - Flight Modes
+//                } // Rectangle - Flight Modes
+//            } // Column - Flight Modes
 
             Column {
                 spacing: _margins
diff --git a/src/AutoPilotPlugins/APM/APMPowerComponent.qml b/src/AutoPilotPlugins/APM/APMPowerComponent.qml
index 8baf43577..86514c669 100644
--- a/src/AutoPilotPlugins/APM/APMPowerComponent.qml
+++ b/src/AutoPilotPlugins/APM/APMPowerComponent.qml
@@ -124,161 +124,161 @@ SetupPage {
                         sourceComponent:    _batt1FullSettings.visible ? powerSetupComponent : undefined
 
                         property Fact armVoltMin:       controller.getParameterFact(-1, "r.BATT_ARM_VOLT", false /* reportMissing */)
-                        property Fact battAmpPerVolt:   controller.getParameterFact(-1, "r.BATT_AMP_PERVLT", false /* reportMissing */)
-                        property Fact battAmpOffset:    controller.getParameterFact(-1, "BATT_AMP_OFFSET", false /* reportMissing */)
+                        //property Fact battAmpPerVolt:   controller.getParameterFact(-1, "r.BATT_AMP_PERVLT", false /* reportMissing */)
+                        //property Fact battAmpOffset:    controller.getParameterFact(-1, "BATT_AMP_OFFSET", false /* reportMissing */)
                         property Fact battCapacity:     controller.getParameterFact(-1, "BATT_CAPACITY", false /* reportMissing */)
-                        property Fact battCurrPin:      controller.getParameterFact(-1, "BATT_CURR_PIN", false /* reportMissing */)
-                        property Fact battMonitor:      controller.getParameterFact(-1, "BATT_MONITOR", false /* reportMissing */)
-                        property Fact battVoltMult:     controller.getParameterFact(-1, "BATT_VOLT_MULT", false /* reportMissing */)
-                        property Fact battVoltPin:      controller.getParameterFact(-1, "BATT_VOLT_PIN", false /* reportMissing */)
+                        //property Fact battCurrPin:      controller.getParameterFact(-1, "BATT_CURR_PIN", false /* reportMissing */)
+                        //property Fact battMonitor:      controller.getParameterFact(-1, "BATT_MONITOR", false /* reportMissing */)
+                        //property Fact battVoltMult:     controller.getParameterFact(-1, "BATT_VOLT_MULT", false /* reportMissing */)
+                        //property Fact battVoltPin:      controller.getParameterFact(-1, "BATT_VOLT_PIN", false /* reportMissing */)
                         property FactGroup  _batteryFactGroup:  _batt1FullSettings.visible ? controller.vehicle.getFactGroup("battery0") : null
-                        property Fact vehicleVoltage:   _batteryFactGroup ? _batteryFactGroup.voltage : null
-                        property Fact vehicleCurrent:   _batteryFactGroup ? _batteryFactGroup.current : null
+                        //property Fact vehicleVoltage:   _batteryFactGroup ? _batteryFactGroup.voltage : null
+                        //property Fact vehicleCurrent:   _batteryFactGroup ? _batteryFactGroup.current : null
                     }
                 }
             }
 
             // Battery2 Monitor settings only - used when only monitor param is available
-            Column {
-                spacing: _margins / 2
-                visible: !_batt2MonitorEnabled || !_batt2ParamsAvailable
-
-                QGCLabel {
-                    text:       qsTr("Battery 2")
-                    font.family: ScreenTools.demiboldFontFamily
-                }
-
-                Rectangle {
-                    width:  batt2Column.x + batt2Column.width + _margins
-                    height: batt2Column.y + batt2Column.height + _margins
-                    color:  ggcPal.windowShade
-
-                    ColumnLayout {
-                        id:                 batt2Column
-                        anchors.margins:    _margins
-                        anchors.top:        parent.top
-                        anchors.left:       parent.left
-                        spacing:            ScreenTools.defaultFontPixelWidth
-
-                        RowLayout {
-                            id:                 batt2MonitorRow
-                            spacing:            ScreenTools.defaultFontPixelWidth
-
-                            QGCLabel { text: qsTr("Battery2 monitor:") }
-                            FactComboBox {
-                                id:         monitor2Combo
-                                fact:       _batt2Monitor
-                                indexModel: false
-                                sizeToContents: true
-                            }
-                        }
-
-                        QGCLabel {
-                            text:       _restartRequired
-                            visible:    _showBatt2Reboot
-                        }
-
-                        QGCButton {
-                            text:       qsTr("Reboot vehicle")
-                            visible:    _showBatt2Reboot
-                            onClicked:  controller.vehicle.rebootVehicle()
-                        }
-                    }
-                }
-            }
-
-            // Battery 2 settings - Used when full params are available
-            Column {
-                id:         batt2FullSettings
-                spacing:    _margins / 2
-                visible:    _batt2MonitorEnabled && _batt2ParamsAvailable
-
-                QGCLabel {
-                    text:       qsTr("Battery 2")
-                    font.family: ScreenTools.demiboldFontFamily
-                }
-
-                Rectangle {
-                    width:  battery2Loader.x + battery2Loader.width + _margins
-                    height: battery2Loader.y + battery2Loader.height + _margins
-                    color:  ggcPal.windowShade
-
-                    Loader {
-                        id:                 battery2Loader
-                        anchors.margins:    _margins
-                        anchors.top:        parent.top
-                        anchors.left:       parent.left
-                        sourceComponent:    batt2FullSettings.visible ? powerSetupComponent : undefined
-
-                        property Fact armVoltMin:       controller.getParameterFact(-1, "r.BATT2_ARM_VOLT", false /* reportMissing */)
-                        property Fact battAmpPerVolt:   controller.getParameterFact(-1, "r.BATT2_AMP_PERVLT", false /* reportMissing */)
-                        property Fact battAmpOffset:    controller.getParameterFact(-1, "BATT2_AMP_OFFSET", false /* reportMissing */)
-                        property Fact battCapacity:     controller.getParameterFact(-1, "BATT2_CAPACITY", false /* reportMissing */)
-                        property Fact battCurrPin:      controller.getParameterFact(-1, "BATT2_CURR_PIN", false /* reportMissing */)
-                        property Fact battMonitor:      controller.getParameterFact(-1, "BATT2_MONITOR", false /* reportMissing */)
-                        property Fact battVoltMult:     controller.getParameterFact(-1, "BATT2_VOLT_MULT", false /* reportMissing */)
-                        property Fact battVoltPin:      controller.getParameterFact(-1, "BATT2_VOLT_PIN", false /* reportMissing */)
-                        property FactGroup  _batteryFactGroup:  batt2FullSettings.visible ? controller.vehicle.getFactGroup("battery1") : null
-                        property Fact vehicleVoltage:   _batteryFactGroup ? _batteryFactGroup.voltage : null
-                        property Fact vehicleCurrent:   _batteryFactGroup ? _batteryFactGroup.current : null
-                    }
-                }
-            }
-
-            Column {
-                spacing:    _margins / 2
-                visible:    _escCalibrationAvailable
-
-                QGCLabel {
-                    text:       qsTr("ESC Calibration")
-                    font.family: ScreenTools.demiboldFontFamily
-                }
-
-                Rectangle {
-                    width:  escCalibrationHolder.x + escCalibrationHolder.width + _margins
-                    height: escCalibrationHolder.y + escCalibrationHolder.height + _margins
-                    color:  ggcPal.windowShade
-
-                    Column {
-                        id:         escCalibrationHolder
-                        x:          _margins
-                        y:          _margins
-                        spacing:    _margins
-
-                        Column {
-                            spacing: _margins
-
-                            QGCLabel {
-                                text:   qsTr("WARNING: Remove props prior to calibration!")
-                                color:  qgcPal.warningText
-                            }
-
-                            Row {
-                                spacing: _margins
-
-                                QGCButton {
-                                    text: qsTr("Calibrate")
-                                    enabled:    _escCalibration && _escCalibration.rawValue === 0
-                                    onClicked:  if(_escCalibration) _escCalibration.rawValue = 3
-                                }
-
-                                Column {
-                                    enabled: _escCalibration && _escCalibration.rawValue === 3
-                                    QGCLabel { text:   _escCalibration ? (_escCalibration.rawValue === 3 ? qsTr("Now perform these steps:") : qsTr("Click Calibrate to start, then:")) : "" }
-                                    QGCLabel { text:   qsTr("- Disconnect USB and battery so flight controller powers down") }
-                                    QGCLabel { text:   qsTr("- Connect the battery") }
-                                    QGCLabel { text:   qsTr("- The arming tone will be played (if the vehicle has a buzzer attached)") }
-                                    QGCLabel { text:   qsTr("- If using a flight controller with a safety button press it until it displays solid red") }
-                                    QGCLabel { text:   qsTr("- You will hear a musical tone then two beeps") }
-                                    QGCLabel { text:   qsTr("- A few seconds later you should hear a number of beeps (one for each battery cell you're using)") }
-                                    QGCLabel { text:   qsTr("- And finally a single long beep indicating the end points have been set and the ESC is calibrated") }
-                                    QGCLabel { text:   qsTr("- Disconnect the battery and power up again normally") }
-                                }
-                            }
-                        }
-                    }
-                }
-            }
+//            Column {
+//                spacing: _margins / 2
+//                visible: !_batt2MonitorEnabled || !_batt2ParamsAvailable
+
+//                QGCLabel {
+//                    text:       qsTr("Battery 2")
+//                    font.family: ScreenTools.demiboldFontFamily
+//                }
+
+//                Rectangle {
+//                    width:  batt2Column.x + batt2Column.width + _margins
+//                    height: batt2Column.y + batt2Column.height + _margins
+//                    color:  ggcPal.windowShade
+
+//                    ColumnLayout {
+//                        id:                 batt2Column
+//                        anchors.margins:    _margins
+//                        anchors.top:        parent.top
+//                        anchors.left:       parent.left
+//                        spacing:            ScreenTools.defaultFontPixelWidth
+
+//                        RowLayout {
+//                            id:                 batt2MonitorRow
+//                            spacing:            ScreenTools.defaultFontPixelWidth
+
+//                            QGCLabel { text: qsTr("Battery2 monitor:") }
+//                            FactComboBox {
+//                                id:         monitor2Combo
+//                                fact:       _batt2Monitor
+//                                indexModel: false
+//                                sizeToContents: true
+//                            }
+//                        }
+
+//                        QGCLabel {
+//                            text:       _restartRequired
+//                            visible:    _showBatt2Reboot
+//                        }
+
+//                        QGCButton {
+//                            text:       qsTr("Reboot vehicle")
+//                            visible:    _showBatt2Reboot
+//                            onClicked:  controller.vehicle.rebootVehicle()
+//                        }
+//                    }
+//                }
+//            }
+
+//            // Battery 2 settings - Used when full params are available
+//            Column {
+//                id:         batt2FullSettings
+//                spacing:    _margins / 2
+//                visible:    _batt2MonitorEnabled && _batt2ParamsAvailable
+
+//                QGCLabel {
+//                    text:       qsTr("Battery 2")
+//                    font.family: ScreenTools.demiboldFontFamily
+//                }
+
+//                Rectangle {
+//                    width:  battery2Loader.x + battery2Loader.width + _margins
+//                    height: battery2Loader.y + battery2Loader.height + _margins
+//                    color:  ggcPal.windowShade
+
+//                    Loader {
+//                        id:                 battery2Loader
+//                        anchors.margins:    _margins
+//                        anchors.top:        parent.top
+//                        anchors.left:       parent.left
+//                        sourceComponent:    batt2FullSettings.visible ? powerSetupComponent : undefined
+
+//                        property Fact armVoltMin:       controller.getParameterFact(-1, "r.BATT2_ARM_VOLT", false /* reportMissing */)
+//                        property Fact battAmpPerVolt:   controller.getParameterFact(-1, "r.BATT2_AMP_PERVLT", false /* reportMissing */)
+//                        property Fact battAmpOffset:    controller.getParameterFact(-1, "BATT2_AMP_OFFSET", false /* reportMissing */)
+//                        property Fact battCapacity:     controller.getParameterFact(-1, "BATT2_CAPACITY", false /* reportMissing */)
+//                        property Fact battCurrPin:      controller.getParameterFact(-1, "BATT2_CURR_PIN", false /* reportMissing */)
+//                        property Fact battMonitor:      controller.getParameterFact(-1, "BATT2_MONITOR", false /* reportMissing */)
+//                        property Fact battVoltMult:     controller.getParameterFact(-1, "BATT2_VOLT_MULT", false /* reportMissing */)
+//                        property Fact battVoltPin:      controller.getParameterFact(-1, "BATT2_VOLT_PIN", false /* reportMissing */)
+//                        property FactGroup  _batteryFactGroup:  batt2FullSettings.visible ? controller.vehicle.getFactGroup("battery1") : null
+//                        property Fact vehicleVoltage:   _batteryFactGroup ? _batteryFactGroup.voltage : null
+//                        property Fact vehicleCurrent:   _batteryFactGroup ? _batteryFactGroup.current : null
+//                    }
+//                }
+//            }
+
+//            Column {
+//                spacing:    _margins / 2
+//                visible:    _escCalibrationAvailable
+
+//                QGCLabel {
+//                    text:       qsTr("ESC Calibration")
+//                    font.family: ScreenTools.demiboldFontFamily
+//                }
+
+//                Rectangle {
+//                    width:  escCalibrationHolder.x + escCalibrationHolder.width + _margins
+//                    height: escCalibrationHolder.y + escCalibrationHolder.height + _margins
+//                    color:  ggcPal.windowShade
+
+//                    Column {
+//                        id:         escCalibrationHolder
+//                        x:          _margins
+//                        y:          _margins
+//                        spacing:    _margins
+
+//                        Column {
+//                            spacing: _margins
+
+//                            QGCLabel {
+//                                text:   qsTr("WARNING: Remove props prior to calibration!")
+//                                color:  qgcPal.warningText
+//                            }
+
+//                            Row {
+//                                spacing: _margins
+
+//                                QGCButton {
+//                                    text: qsTr("Calibrate")
+//                                    enabled:    _escCalibration && _escCalibration.rawValue === 0
+//                                    onClicked:  if(_escCalibration) _escCalibration.rawValue = 3
+//                                }
+
+//                                Column {
+//                                    enabled: _escCalibration && _escCalibration.rawValue === 3
+//                                    QGCLabel { text:   _escCalibration ? (_escCalibration.rawValue === 3 ? qsTr("Now perform these steps:") : qsTr("Click Calibrate to start, then:")) : "" }
+//                                    QGCLabel { text:   qsTr("- Disconnect USB and battery so flight controller powers down") }
+//                                    QGCLabel { text:   qsTr("- Connect the battery") }
+//                                    QGCLabel { text:   qsTr("- The arming tone will be played (if the vehicle has a buzzer attached)") }
+//                                    QGCLabel { text:   qsTr("- If using a flight controller with a safety button press it until it displays solid red") }
+//                                    QGCLabel { text:   qsTr("- You will hear a musical tone then two beeps") }
+//                                    QGCLabel { text:   qsTr("- A few seconds later you should hear a number of beeps (one for each battery cell you're using)") }
+//                                    QGCLabel { text:   qsTr("- And finally a single long beep indicating the end points have been set and the ESC is calibrated") }
+//                                    QGCLabel { text:   qsTr("- Disconnect the battery and power up again normally") }
+//                                }
+//                            }
+//                        }
+//                    }
+//                }
+//            }
         } // Flow
     } // Component - powerPageComponent
 
@@ -289,24 +289,24 @@ SetupPage {
             spacing: _margins
 
             property real _margins:         ScreenTools.defaultFontPixelHeight / 2
-            property bool _showAdvanced:    sensorCombo.currentIndex === sensorModel.count - 1
+            property bool _showAdvanced:    false // sensorCombo.currentIndex === sensorModel.count - 1
             property real _fieldWidth:      ScreenTools.defaultFontPixelWidth * 25
 
-            Component.onCompleted: calcSensor()
-
-            function calcSensor() {
-                for (var i=0; i<sensorModel.count - 1; i++) {
-                    if (sensorModel.get(i).voltPin === battVoltPin.value &&
-                            sensorModel.get(i).currPin === battCurrPin.value &&
-                            Math.abs(sensorModel.get(i).voltMult - battVoltMult.value) < 0.001 &&
-                            Math.abs(sensorModel.get(i).ampPerVolt - battAmpPerVolt.value) < 0.0001 &&
-                            Math.abs(sensorModel.get(i).ampOffset - battAmpOffset.value) < 0.0001) {
-                        sensorCombo.currentIndex = i
-                        return
-                    }
-                }
-                sensorCombo.currentIndex = sensorModel.count - 1
-            }
+//            Component.onCompleted: calcSensor()
+
+//            function calcSensor() {
+//                for (var i=0; i<sensorModel.count - 1; i++) {
+//                    if (sensorModel.get(i).voltPin === battVoltPin.value &&
+//                            sensorModel.get(i).currPin === battCurrPin.value &&
+//                            Math.abs(sensorModel.get(i).voltMult - battVoltMult.value) < 0.001 &&
+//                            Math.abs(sensorModel.get(i).ampPerVolt - battAmpPerVolt.value) < 0.0001 &&
+//                            Math.abs(sensorModel.get(i).ampOffset - battAmpOffset.value) < 0.0001) {
+//                        sensorCombo.currentIndex = i
+//                        return
+//                    }
+//                }
+//                sensorCombo.currentIndex = sensorModel.count - 1
+//            }
 
             QGCPalette { id: palette; colorGroupEnabled: true }
 
@@ -369,14 +369,14 @@ SetupPage {
                 rowSpacing:     _margins
                 columnSpacing:  _margins
 
-                QGCLabel { text: qsTr("Battery monitor:") }
+                //QGCLabel { text: qsTr("Battery monitor:") }
 
-                FactComboBox {
-                    id:         monitorCombo
-                    fact:       battMonitor
-                    indexModel: false
-                    sizeToContents: true
-                }
+//                FactComboBox {
+//                    id:         monitorCombo
+//                    fact:       battMonitor
+//                    indexModel: false
+//                    sizeToContents: true
+//                }
 
                 QGCLabel {
                     Layout.row:     1
@@ -402,88 +402,88 @@ SetupPage {
                     fact:   armVoltMin
                 }
 
-                QGCLabel {
-                    Layout.row:     3
-                    Layout.column:  0
-                    text:           qsTr("Power sensor:")
-                }
-
-                QGCComboBox {
-                    id:                     sensorCombo
-                    Layout.minimumWidth:    _fieldWidth
-                    model:                  sensorModel
-                    textRole:               "text"
-
-                    onActivated: {
-                        if (index < sensorModel.count - 1) {
-                            battVoltPin.value = sensorModel.get(index).voltPin
-                            battCurrPin.value = sensorModel.get(index).currPin
-                            battVoltMult.value = sensorModel.get(index).voltMult
-                            battAmpPerVolt.value = sensorModel.get(index).ampPerVolt
-                            battAmpOffset.value = sensorModel.get(index).ampOffset
-                        } else {
-
-                        }
-                    }
-                }
-
-                QGCLabel {
-                    Layout.row:     4
-                    Layout.column:  0
-                    text:           qsTr("Current pin:")
-                    visible:        _showAdvanced
-                }
-
-                FactComboBox {
-                    Layout.minimumWidth:    _fieldWidth
-                    fact:                   battCurrPin
-                    indexModel:             false
-                    visible:                _showAdvanced
-                    sizeToContents:         true
-                }
-
-                QGCLabel {
-                    Layout.row:     5
-                    Layout.column:  0
-                    text:           qsTr("Voltage pin:")
-                    visible:        _showAdvanced
-                }
-
-                FactComboBox {
-                    Layout.minimumWidth:    _fieldWidth
-                    fact:                   battVoltPin
-                    indexModel:             false
-                    visible:                _showAdvanced
-                    sizeToContents:         true
-                }
-
-                QGCLabel {
-                    Layout.row:     6
-                    Layout.column:  0
-                    text:           qsTr("Voltage multiplier:")
-                    visible:        _showAdvanced
-                }
-
-                FactTextField {
-                    width:      _fieldWidth
-                    fact:       battVoltMult
-                    visible:    _showAdvanced
-                }
-
-                QGCButton {
-                    text:       qsTr("Calculate")
-                    visible:    _showAdvanced
-                    onClicked:  calcVoltageMultiplierDlgComponent.createObject(mainWindow, { vehicleVoltageFact: vehicleVoltage, battVoltMultFact: battVoltMult }).open()
-                }
-
-                QGCLabel {
-                    Layout.columnSpan:  3
-                    Layout.fillWidth:   true
-                    font.pointSize:     ScreenTools.smallFontPointSize
-                    wrapMode:           Text.WordWrap
-                    text:               qsTr("If the battery voltage reported by the vehicle is largely different than the voltage read externally using a voltmeter you can adjust the voltage multiplier value to correct this. Click the Calculate button for help with calculating a new value.")
-                    visible:            _showAdvanced
-                }
+//                QGCLabel {
+//                    Layout.row:     3
+//                    Layout.column:  0
+//                    text:           qsTr("Power sensor:")
+//                }
+
+//                QGCComboBox {
+//                    id:                     sensorCombo
+//                    Layout.minimumWidth:    _fieldWidth
+//                    model:                  sensorModel
+//                    textRole:               "text"
+
+//                    onActivated: {
+//                        if (index < sensorModel.count - 1) {
+//                            battVoltPin.value = sensorModel.get(index).voltPin
+//                            battCurrPin.value = sensorModel.get(index).currPin
+//                            battVoltMult.value = sensorModel.get(index).voltMult
+//                            battAmpPerVolt.value = sensorModel.get(index).ampPerVolt
+//                            battAmpOffset.value = sensorModel.get(index).ampOffset
+//                        } else {
+
+//                        }
+//                    }
+//                }
+
+//                QGCLabel {
+//                    Layout.row:     4
+//                    Layout.column:  0
+//                    text:           qsTr("Current pin:")
+//                    visible:        _showAdvanced
+//                }
+
+//                FactComboBox {
+//                    Layout.minimumWidth:    _fieldWidth
+//                    fact:                   battCurrPin
+//                    indexModel:             false
+//                    visible:                _showAdvanced
+//                    sizeToContents:         true
+//                }
+
+//                QGCLabel {
+//                    Layout.row:     5
+//                    Layout.column:  0
+//                    text:           qsTr("Voltage pin:")
+//                    visible:        _showAdvanced
+//                }
+
+//                FactComboBox {
+//                    Layout.minimumWidth:    _fieldWidth
+//                    fact:                   battVoltPin
+//                    indexModel:             false
+//                    visible:                _showAdvanced
+//                    sizeToContents:         true
+//                }
+
+//                QGCLabel {
+//                    Layout.row:     6
+//                    Layout.column:  0
+//                    text:           qsTr("Voltage multiplier:")
+//                    visible:        _showAdvanced
+//                }
+
+//                FactTextField {
+//                    width:      _fieldWidth
+//                    fact:       battVoltMult
+//                    visible:    _showAdvanced
+//                }
+
+//                QGCButton {
+//                    text:       qsTr("Calculate")
+//                    visible:    _showAdvanced
+//                    onClicked:  calcVoltageMultiplierDlgComponent.createObject(mainWindow, { vehicleVoltageFact: vehicleVoltage, battVoltMultFact: battVoltMult }).open()
+//                }
+
+//                QGCLabel {
+//                    Layout.columnSpan:  3
+//                    Layout.fillWidth:   true
+//                    font.pointSize:     ScreenTools.smallFontPointSize
+//                    wrapMode:           Text.WordWrap
+//                    text:               qsTr("If the battery voltage reported by the vehicle is largely different than the voltage read externally using a voltmeter you can adjust the voltage multiplier value to correct this. Click the Calculate button for help with calculating a new value.")
+//                    visible:            _showAdvanced
+//                }
 
                 QGCLabel {
                     text:       qsTr("Amps per volt:")
diff --git a/src/AutoPilotPlugins/APM/APMSafetyComponent.qml b/src/AutoPilotPlugins/APM/APMSafetyComponent.qml
index 8c75d700b..6a768a850 100644
--- a/src/AutoPilotPlugins/APM/APMSafetyComponent.qml
+++ b/src/AutoPilotPlugins/APM/APMSafetyComponent.qml
@@ -718,47 +718,47 @@ SetupPage {
                 sourceComponent: controller.vehicle.fixedWing ? planeRTL : undefined
             }
 
-            Column {
-                spacing: _margins / 2
-
-                QGCLabel {
-                    text:           qsTr("Arming Checks")
-                    font.family:    ScreenTools.demiboldFontFamily
-                }
-
-                Rectangle {
-                    width:  flowLayout.width
-                    height: armingCheckInnerColumn.height + (_margins * 2)
-                    color:  ggcPal.windowShade
-
-                    Column {
-                        id:                 armingCheckInnerColumn
-                        anchors.margins:    _margins
-                        anchors.top:        parent.top
-                        anchors.left:       parent.left
-                        anchors.right:      parent.right
-                        spacing: _margins
-
-                        FactBitmask {
-                            id:                 armingCheckBitmask
-                            anchors.left:       parent.left
-                            anchors.right:      parent.right
-                            firstEntryIsAll:    true
-                            fact:               _armingCheck
-                        }
-
-                        QGCLabel {
-                            id:             armingCheckWarning
-                            anchors.left:   parent.left
-                            anchors.right:  parent.right
-                            wrapMode:       Text.WordWrap
-                            color:          qgcPal.warningText
-                            text:            qsTr("Warning: Turning off arming checks can lead to loss of Vehicle control.")
-                            visible:        _armingCheck.value != 1
-                        }
-                    }
-                } // Rectangle - Arming checks
-            } // Column - Arming Checks
+//            Column {
+//                spacing: _margins / 2
+
+//                QGCLabel {
+//                    text:           qsTr("Arming Checks")
+//                    font.family:    ScreenTools.demiboldFontFamily
+//                }
+
+//                Rectangle {
+//                    width:  flowLayout.width
+//                    height: armingCheckInnerColumn.height + (_margins * 2)
+//                    color:  ggcPal.windowShade
+
+//                    Column {
+//                        id:                 armingCheckInnerColumn
+//                        anchors.margins:    _margins
+//                        anchors.top:        parent.top
+//                        anchors.left:       parent.left
+//                        anchors.right:      parent.right
+//                        spacing: _margins
+
+//                        FactBitmask {
+//                            id:                 armingCheckBitmask
+//                            anchors.left:       parent.left
+//                            anchors.right:      parent.right
+//                            firstEntryIsAll:    true
+//                            fact:               _armingCheck
+//                        }
+
+//                        QGCLabel {
+//                            id:             armingCheckWarning
+//                            anchors.left:   parent.left
+//                            anchors.right:  parent.right
+//                            wrapMode:       Text.WordWrap
+//                            color:          qgcPal.warningText
+//                            text:            qsTr("Warning: Turning off arming checks can lead to loss of Vehicle control.")
+//                            visible:        _armingCheck.value != 1
+//                        }
+//                    }
+//                } // Rectangle - Arming checks
+//            } // Column - Arming Checks
         } // Flow
     } // Component - safetyPageComponent
 } // SetupView
diff --git a/src/AutoPilotPlugins/APM/APMSensorsComponent.qml b/src/AutoPilotPlugins/APM/APMSensorsComponent.qml
index b032bca7b..289bf357b 100644
--- a/src/AutoPilotPlugins/APM/APMSensorsComponent.qml
+++ b/src/AutoPilotPlugins/APM/APMSensorsComponent.qml
@@ -446,150 +446,150 @@ SetupPage {
                         }
                     }
 
-                    Column {
-                        width:      40 * ScreenTools.defaultFontPixelWidth
-                        spacing:    ScreenTools.defaultFontPixelHeight
-
-                        QGCLabel {
-                            width:      parent.width
-                            wrapMode:   Text.WordWrap
-                            text:       _orientationDialogHelp
-                        }
-
-                        Column {
-                            QGCLabel { text: qsTr("Autopilot Rotation:") }
-
-                            FactComboBox {
-                                width:      rotationColumnWidth
-                                indexModel: false
-                                fact:       boardRot
-                            }
-                        }
-
-                        Column {
-
-                            visible: _orientationDialogCalType == _calTypeAccel
-                            spacing: ScreenTools.defaultFontPixelHeight
-
-                            QGCLabel {
-                                width:      parent.width
-                                wrapMode:   Text.WordWrap
-                                text: qsTr("Simple accelerometer calibration is less precise but allows calibrating without rotating the vehicle. Check this if you have a large/heavy vehicle.")
-                            }
-
-                            QGCCheckBox {
-                                text: "Simple Accelerometer Calibration"
-                                onClicked: _doSimpleAccelCal = this.checked
-                            }
-                        }
-
-                        Repeater {
-                            model:      _orientationsDialogShowCompass ? 3 : 0
-                            delegate:   singleCompassSettingsComponent
-                        }
-
-                        QGCLabel {
-                            id:         magneticDeclinationLabel
-                            width:      parent.width
-                            visible:    globals.activeVehicle.sub && _orientationsDialogShowCompass
-                            text:       qsTr("Magnetic Declination")
-                        }
-
-                        Column {
-                            visible:            magneticDeclinationLabel.visible
-                            anchors.margins:    ScreenTools.defaultFontPixelWidth
-                            anchors.left:       parent.left
-                            anchors.right:      parent.right
-                            spacing:            ScreenTools.defaultFontPixelHeight
-
-                            QGCCheckBox {
-                                id:                           manualMagneticDeclinationCheckBox
-                                text:                         qsTr("Manual Magnetic Declination")
-                                property Fact autoDecFact:    controller.getParameterFact(-1, "COMPASS_AUTODEC")
-                                property int manual:          0
-                                property int automatic:       1
-
-                                checked:    autoDecFact.rawValue === manual
-                                onClicked:  autoDecFact.value = (checked ? manual : automatic)
-                            }
-
-                            FactTextField {
-                                fact:       sensorParams.declinationFact
-                                enabled:    manualMagneticDeclinationCheckBox.checked
-                            }
-                        }
-
-                        Item { height: ScreenTools.defaultFontPixelHeight; width: 10 } // spacer
-
-                        QGCLabel {
-                            id:         northCalibrationLabel
-                            width:      parent.width
-                            visible:    _orientationsDialogShowCompass
-                            wrapMode:   Text.WordWrap
-                            text:       qsTr("Fast compass calibration given vehicle position and yaw. This ") +
-                                        qsTr("results in zero diagonal and off-diagonal elements, so is only ") +
-                                        qsTr("suitable for vehicles where the field is close to spherical. It is ") +
-                                        qsTr("useful for large vehicles where moving the vehicle to calibrate it ") +
-                                        qsTr("is difficult. Point the vehicle North before using it.")
-                        }
-
-                        Column {
-                            visible:            northCalibrationLabel.visible
-                            anchors.margins:    ScreenTools.defaultFontPixelWidth
-                            anchors.left:       parent.left
-                            anchors.right:      parent.right
-                            spacing:            ScreenTools.defaultFontPixelHeight
-
-                            QGCCheckBox {
-                                id:             northCalibrationCheckBox
-                                visible:        northCalibrationLabel.visible
-                                text:           qsTr("Fast Calibration")
-                            }
-
-                            QGCLabel {
-                                id:         northCalibrationManualPosition
-                                width:      parent.width
-                                visible:    northCalibrationCheckBox.checked && !globals.activeVehicle.coordinate.isValid
-                                wrapMode:   Text.WordWrap
-                                text:       qsTr("Vehicle has no Valid positon, please provide it")
-                            }
-
-                            QGCCheckBox {
-                                visible:    northCalibrationManualPosition.visible && _gcsPosition.isValid
-                                id:         useGcsPositionCheckbox
-                                text:       qsTr("Use GCS position instead")
-                                checked:    _gcsPosition.isValid
-                            }
-                            QGCCheckBox {
-                                visible:    northCalibrationManualPosition.visible && !_gcsPosition.isValid
-                                id:         useMapPositionCheckbox
-                                text:       qsTr("Use current map position instead")
-                            }
-
-                            QGCLabel {
-                                width:      parent.width
-                                visible:    useMapPositionCheckbox.checked
-                                wrapMode:   Text.WordWrap
-                                text:       qsTr(`Lat: ${_mapPosition.latitude.toFixed(4)} Lon: ${_mapPosition.longitude.toFixed(4)}`)
-                            }
-
-                            FactTextField {
-                                id:         northCalLat
-                                visible:    !useGcsPositionCheckbox.checked && !useMapPositionCheckbox.checked && northCalibrationCheckBox.checked
-                                text:       "0.00"
-                                textColor:  isNaN(parseFloat(text)) ? qgcPal.warningText: qgcPal.textFieldText
-                                enabled:    !useGcsPositionCheckbox.checked
-                            }
-                            FactTextField {
-                                id:         northCalLon
-                                visible:    !useGcsPositionCheckbox.checked && !useMapPositionCheckbox.checked && northCalibrationCheckBox.checked
-                                text:       "0.00"
-                                textColor:  isNaN(parseFloat(text)) ? qgcPal.warningText: qgcPal.textFieldText
-                                enabled:    !useGcsPositionCheckbox.checked
-                            }
-
-                        }
-                    }
+//                    Column {
+//                        width:      40 * ScreenTools.defaultFontPixelWidth
+//                        spacing:    ScreenTools.defaultFontPixelHeight
+
+//                        QGCLabel {
+//                            width:      parent.width
+//                            wrapMode:   Text.WordWrap
+//                            text:       _orientationDialogHelp
+//                        }
+
+//                        Column {
+//                            QGCLabel { text: qsTr("Autopilot Rotation:") }
+
+//                            FactComboBox {
+//                                width:      rotationColumnWidth
+//                                indexModel: false
+//                                fact:       boardRot
+//                            }
+//                        }
+
+//                        Column {
+
+//                            visible: _orientationDialogCalType == _calTypeAccel
+//                            spacing: ScreenTools.defaultFontPixelHeight
+
+//                            QGCLabel {
+//                                width:      parent.width
+//                                wrapMode:   Text.WordWrap
+//                                text: qsTr("Simple accelerometer calibration is less precise but allows calibrating without rotating the vehicle. Check this if you have a large/heavy vehicle.")
+//                            }
+
+//                            QGCCheckBox {
+//                                text: "Simple Accelerometer Calibration"
+//                                onClicked: _doSimpleAccelCal = this.checked
+//                            }
+//                        }
+
+//                        Repeater {
+//                            model:      _orientationsDialogShowCompass ? 3 : 0
+//                            delegate:   singleCompassSettingsComponent
+//                        }
+
+//                        QGCLabel {
+//                            id:         magneticDeclinationLabel
+//                            width:      parent.width
+//                            visible:    globals.activeVehicle.sub && _orientationsDialogShowCompass
+//                            text:       qsTr("Magnetic Declination")
+//                        }
+
+//                        Column {
+//                            visible:            magneticDeclinationLabel.visible
+//                            anchors.margins:    ScreenTools.defaultFontPixelWidth
+//                            anchors.left:       parent.left
+//                            anchors.right:      parent.right
+//                            spacing:            ScreenTools.defaultFontPixelHeight
+
+//                            QGCCheckBox {
+//                                id:                           manualMagneticDeclinationCheckBox
+//                                text:                         qsTr("Manual Magnetic Declination")
+//                                property Fact autoDecFact:    controller.getParameterFact(-1, "COMPASS_AUTODEC")
+//                                property int manual:          0
+//                                property int automatic:       1
+
+//                                checked:    autoDecFact.rawValue === manual
+//                                onClicked:  autoDecFact.value = (checked ? manual : automatic)
+//                            }
+
+//                            FactTextField {
+//                                fact:       sensorParams.declinationFact
+//                                enabled:    manualMagneticDeclinationCheckBox.checked
+//                            }
+//                        }
+
+//                        Item { height: ScreenTools.defaultFontPixelHeight; width: 10 } // spacer
+
+//                        QGCLabel {
+//                            id:         northCalibrationLabel
+//                            width:      parent.width
+//                            visible:    _orientationsDialogShowCompass
+//                            wrapMode:   Text.WordWrap
+//                            text:       qsTr("Fast compass calibration given vehicle position and yaw. This ") +
+//                                        qsTr("results in zero diagonal and off-diagonal elements, so is only ") +
+//                                        qsTr("suitable for vehicles where the field is close to spherical. It is ") +
+//                                        qsTr("useful for large vehicles where moving the vehicle to calibrate it ") +
+//                                        qsTr("is difficult. Point the vehicle North before using it.")
+//                        }
+
+//                        Column {
+//                            visible:            northCalibrationLabel.visible
+//                            anchors.margins:    ScreenTools.defaultFontPixelWidth
+//                            anchors.left:       parent.left
+//                            anchors.right:      parent.right
+//                            spacing:            ScreenTools.defaultFontPixelHeight
+
+//                            QGCCheckBox {
+//                                id:             northCalibrationCheckBox
+//                                visible:        northCalibrationLabel.visible
+//                                text:           qsTr("Fast Calibration")
+//                            }
+
+//                            QGCLabel {
+//                                id:         northCalibrationManualPosition
+//                                width:      parent.width
+//                                visible:    northCalibrationCheckBox.checked && !globals.activeVehicle.coordinate.isValid
+//                                wrapMode:   Text.WordWrap
+//                                text:       qsTr("Vehicle has no Valid positon, please provide it")
+//                            }
+
+//                            QGCCheckBox {
+//                                visible:    northCalibrationManualPosition.visible && _gcsPosition.isValid
+//                                id:         useGcsPositionCheckbox
+//                                text:       qsTr("Use GCS position instead")
+//                                checked:    _gcsPosition.isValid
+//                            }
+//                            QGCCheckBox {
+//                                visible:    northCalibrationManualPosition.visible && !_gcsPosition.isValid
+//                                id:         useMapPositionCheckbox
+//                                text:       qsTr("Use current map position instead")
+//                            }
+
+//                            QGCLabel {
+//                                width:      parent.width
+//                                visible:    useMapPositionCheckbox.checked
+//                                wrapMode:   Text.WordWrap
+//                                text:       qsTr(`Lat: ${_mapPosition.latitude.toFixed(4)} Lon: ${_mapPosition.longitude.toFixed(4)}`)
+//                            }
+
+//                            FactTextField {
+//                                id:         northCalLat
+//                                visible:    !useGcsPositionCheckbox.checked && !useMapPositionCheckbox.checked && northCalibrationCheckBox.checked
+//                                text:       "0.00"
+//                                textColor:  isNaN(parseFloat(text)) ? qgcPal.warningText: qgcPal.textFieldText
+//                                enabled:    !useGcsPositionCheckbox.checked
+//                            }
+//                            FactTextField {
+//                                id:         northCalLon
+//                                visible:    !useGcsPositionCheckbox.checked && !useMapPositionCheckbox.checked && northCalibrationCheckBox.checked
+//                                text:       "0.00"
+//                                textColor:  isNaN(parseFloat(text)) ? qgcPal.warningText: qgcPal.textFieldText
+//                                enabled:    !useGcsPositionCheckbox.checked
+//                            }
+
+//                        }
+//                    }
                 }
             }
 
@@ -687,59 +687,59 @@ SetupPage {
                         }
                     }
 
-                    QGCButton {
-                        width:  _buttonWidth
-                        text:   _levelHorizonText
-
-                        readonly property string _levelHorizonText: qsTr("Level Horizon")
-
-                        onClicked: {
-                            if (controller.accelSetupNeeded) {
-                                mainWindow.showMessageDialog(_levelHorizonText, qsTr("Accelerometer must be calibrated prior to Level Horizon."))
-                            } else {
-                                mainWindow.showMessageDialog(_levelHorizonText,
-                                                             qsTr("To level the horizon you need to place the vehicle in its level flight position and press Ok."),
-                                                             StandardButton.Cancel | StandardButton.Ok,
-                                                             function() { controller.levelHorizon() })
-                            }
-                        }
-                    }
-
-                    QGCButton {
-                        width:      _buttonWidth
-                        text:       qsTr("Gyro")
-                        visible:    globals.activeVehicle && (globals.activeVehicle.multiRotor | globals.activeVehicle.rover | globals.activeVehicle.sub)
-                        onClicked:  mainWindow.showMessageDialog(qsTr("Calibrate Gyro"),
-                                                                 qsTr("For Gyroscope calibration you will need to place your vehicle on a surface and leave it still.\n\nClick Ok to start calibration."),
-                                                                 StandardButton.Cancel | StandardButton.Ok,
-                                                                 function() { controller.calibrateGyro() })
-                    }
-
-                    QGCButton {
-                        width:      _buttonWidth
-                        text:       _calibratePressureText
-                        onClicked:  mainWindow.showMessageDialog(_calibratePressureText,
-                                                                 qsTr("Pressure calibration will set the %1 to zero at the current pressure reading. %2").arg(_altText).arg(_helpTextFW),
-                                                                 StandardButton.Cancel | StandardButton.Ok,
-                                                                 function() { controller.calibratePressure() })
-
-                        readonly property string _altText:                  globals.activeVehicle.sub ? qsTr("depth") : qsTr("altitude")
-                        readonly property string _helpTextFW:               globals.activeVehicle.fixedWing ? qsTr("To calibrate the airspeed sensor shield it from the wind. Do not touch the sensor or obstruct any holes during the calibration.") : ""
-                        readonly property string _calibratePressureText:    globals.activeVehicle.fixedWing ? qsTr("Baro/Airspeed") : qsTr("Pressure")
-                    }
-
-                    QGCButton {
-                        width:      _buttonWidth
-                        text:       qsTr("CompassMot")
-                        visible:    globals.activeVehicle ? globals.activeVehicle.supportsMotorInterference : false
-                        onClicked:  compassMotDialogComponent.createObject(mainWindow).open()
-                    }
-
-                    QGCButton {
-                        width:      _buttonWidth
-                        text:       qsTr("Sensor Settings")
-                        onClicked:  showOrientationsDialog(_calTypeSet)
-                    }
+//                    QGCButton {
+//                        width:  _buttonWidth
+//                        text:   _levelHorizonText
+
+//                        readonly property string _levelHorizonText: qsTr("Level Horizon")
+
+//                        onClicked: {
+//                            if (controller.accelSetupNeeded) {
+//                                mainWindow.showMessageDialog(_levelHorizonText, qsTr("Accelerometer must be calibrated prior to Level Horizon."))
+//                            } else {
+//                                mainWindow.showMessageDialog(_levelHorizonText,
+//                                                             qsTr("To level the horizon you need to place the vehicle in its level flight position and press Ok."),
+//                                                             StandardButton.Cancel | StandardButton.Ok,
+//                                                             function() { controller.levelHorizon() })
+//                            }
+//                        }
+//                    }
+
+//                    QGCButton {
+//                        width:      _buttonWidth
+//                        text:       qsTr("Gyro")
+//                        visible:    globals.activeVehicle && (globals.activeVehicle.multiRotor | globals.activeVehicle.rover | globals.activeVehicle.sub)
+//                        onClicked:  mainWindow.showMessageDialog(qsTr("Calibrate Gyro"),
+//                                                                 qsTr("For Gyroscope calibration you will need to place your vehicle on a surface and leave it still.\n\nClick Ok to start calibration."),
+//                                                                 StandardButton.Cancel | StandardButton.Ok,
+//                                                                 function() { controller.calibrateGyro() })
+//                    }
+
+//                    QGCButton {
+//                        width:      _buttonWidth
+//                        text:       _calibratePressureText
+//                        onClicked:  mainWindow.showMessageDialog(_calibratePressureText,
+//                                                                 qsTr("Pressure calibration will set the %1 to zero at the current pressure reading. %2").arg(_altText).arg(_helpTextFW),
+//                                                                 StandardButton.Cancel | StandardButton.Ok,
+//                                                                 function() { controller.calibratePressure() })
+
+//                        readonly property string _altText:                  globals.activeVehicle.sub ? qsTr("depth") : qsTr("altitude")
+//                        readonly property string _helpTextFW:               globals.activeVehicle.fixedWing ? qsTr("To calibrate the airspeed sensor shield it from the wind. Do not touch the sensor or obstruct any holes during the calibration.") : ""
+//                        readonly property string _calibratePressureText:    globals.activeVehicle.fixedWing ? qsTr("Baro/Airspeed") : qsTr("Pressure")
+//                    }
+
+//                    QGCButton {
+//                        width:      _buttonWidth
+//                        text:       qsTr("CompassMot")
+//                        visible:    globals.activeVehicle ? globals.activeVehicle.supportsMotorInterference : false
+//                        onClicked:  compassMotDialogComponent.createObject(mainWindow).open()
+//                    }
+
+//                    QGCButton {
+//                        width:      _buttonWidth
+//                        text:       qsTr("Sensor Settings")
+//                        onClicked:  showOrientationsDialog(_calTypeSet)
+//                    }
                 } // Column - Cal Buttons
 
                 Column {
diff --git a/src/AutoPilotPlugins/Common/RadioComponent.qml b/src/AutoPilotPlugins/Common/RadioComponent.qml
index b429e815b..7437fadb8 100644
--- a/src/AutoPilotPlugins/Common/RadioComponent.qml
+++ b/src/AutoPilotPlugins/Common/RadioComponent.qml
@@ -347,7 +347,7 @@ SetupPage {
                     border.width:   1
                 }
 
-                QGCLabel { text: qsTr("Additional Radio setup:") }
+                //QGCLabel { text: qsTr("Additional Radio setup:") }
 
                 GridLayout {
                     id:                 switchSettingsGrid
@@ -381,21 +381,21 @@ SetupPage {
                     }
                 }
 
-                RowLayout {
-                    QGCButton {
-                        id:         bindButton
-                        text:       qsTr("Spektrum Bind")
-                        onClicked:  spektrumBindDialogComponent.createObject(mainWindow).open()
-                    }
-
-                    QGCButton {
-                        text:       qsTr("Copy Trims")
-                        onClicked:  mainWindow.showMessageDialog(qsTr("Copy Trims"),
-                                                                 qsTr("Center your sticks and move throttle all the way down, then press Ok to copy trims. After pressing Ok, reset the trims on your radio back to zero."),
-                                                                 StandardButton.Ok | StandardButton.Cancel,
-                                                                 function() { controller.copyTrims() })
-                    }
-                }
+//                RowLayout {
+//                    QGCButton {
+//                        id:         bindButton
+//                        text:       qsTr("Spektrum Bind")
+//                        onClicked:  spektrumBindDialogComponent.createObject(mainWindow).open()
+//                    }
+
+//                    QGCButton {
+//                        text:       qsTr("Copy Trims")
+//                        onClicked:  mainWindow.showMessageDialog(qsTr("Copy Trims"),
+//                                                                 qsTr("Center your sticks and move throttle all the way down, then press Ok to copy trims. After pressing Ok, reset the trims on your radio back to zero."),
+//                                                                 StandardButton.Ok | StandardButton.Cancel,
+//                                                                 function() { controller.copyTrims() })
+//                    }
+//                }
             } // Column - Left Column
 
             Item {
diff --git a/src/FirmwarePlugin/APM/APMBrandImage.png b/src/FirmwarePlugin/APM/APMBrandImage.png
old mode 100644
new mode 100755
index 2a0692eca..178e4c7f9
Binary files a/src/FirmwarePlugin/APM/APMBrandImage.png and b/src/FirmwarePlugin/APM/APMBrandImage.png differ
diff --git a/src/FirmwarePlugin/APM/ArduCopterFirmwarePlugin.cc b/src/FirmwarePlugin/APM/ArduCopterFirmwarePlugin.cc
index d54e25a3f..4f2f97cfe 100644
--- a/src/FirmwarePlugin/APM/ArduCopterFirmwarePlugin.cc
+++ b/src/FirmwarePlugin/APM/ArduCopterFirmwarePlugin.cc
@@ -56,31 +56,31 @@ ArduCopterFirmwarePlugin::ArduCopterFirmwarePlugin(void)
 {
     setSupportedModes({
         APMCopterMode(APMCopterMode::STABILIZE,     true),
-        APMCopterMode(APMCopterMode::ACRO,          true),
+        APMCopterMode(APMCopterMode::ACRO,          false),
         APMCopterMode(APMCopterMode::ALT_HOLD,      true),
         APMCopterMode(APMCopterMode::AUTO,          true),
-        APMCopterMode(APMCopterMode::GUIDED,        true),
+        APMCopterMode(APMCopterMode::GUIDED,        false),
         APMCopterMode(APMCopterMode::LOITER,        true),
         APMCopterMode(APMCopterMode::RTL,           true),
-        APMCopterMode(APMCopterMode::CIRCLE,        true),
+        APMCopterMode(APMCopterMode::CIRCLE,        false),
         APMCopterMode(APMCopterMode::LAND,          true),
-        APMCopterMode(APMCopterMode::DRIFT,         true),
-        APMCopterMode(APMCopterMode::SPORT,         true),
-        APMCopterMode(APMCopterMode::FLIP,          true),
-        APMCopterMode(APMCopterMode::AUTOTUNE,      true),
-        APMCopterMode(APMCopterMode::POS_HOLD,      true),
-        APMCopterMode(APMCopterMode::BRAKE,         true),
-        APMCopterMode(APMCopterMode::THROW,         true),
-        APMCopterMode(APMCopterMode::AVOID_ADSB,    true),
-        APMCopterMode(APMCopterMode::GUIDED_NOGPS,  true),
-        APMCopterMode(APMCopterMode::SMART_RTL,     true),
-        APMCopterMode(APMCopterMode::FLOWHOLD,      true),
-        APMCopterMode(APMCopterMode::FOLLOW,        true),
-        APMCopterMode(APMCopterMode::ZIGZAG,        true),
-        APMCopterMode(APMCopterMode::SYSTEMID,      true),
-        APMCopterMode(APMCopterMode::AUTOROTATE,    true),
-        APMCopterMode(APMCopterMode::AUTO_RTL,      true),
-        APMCopterMode(APMCopterMode::TURTLE,        true),
+        APMCopterMode(APMCopterMode::DRIFT,         false),
+        APMCopterMode(APMCopterMode::SPORT,         false),
+        APMCopterMode(APMCopterMode::FLIP,          false),
+        APMCopterMode(APMCopterMode::AUTOTUNE,      false),
+        APMCopterMode(APMCopterMode::POS_HOLD,      false),
+        APMCopterMode(APMCopterMode::BRAKE,         false),
+        APMCopterMode(APMCopterMode::THROW,         false),
+        APMCopterMode(APMCopterMode::AVOID_ADSB,    false),
+        APMCopterMode(APMCopterMode::GUIDED_NOGPS,  false),
+        APMCopterMode(APMCopterMode::SMART_RTL,     false),
+        APMCopterMode(APMCopterMode::FLOWHOLD,      false),
+        APMCopterMode(APMCopterMode::FOLLOW,        false),
+        APMCopterMode(APMCopterMode::ZIGZAG,        false),
+        APMCopterMode(APMCopterMode::SYSTEMID,      false),
+        APMCopterMode(APMCopterMode::AUTOROTATE,    false),
+        APMCopterMode(APMCopterMode::AUTO_RTL,      false),
+        APMCopterMode(APMCopterMode::TURTLE,        false),
     });
 
     if (!_remapParamNameIntialized) {
diff --git a/src/FirmwarePlugin/FirmwarePlugin.cc b/src/FirmwarePlugin/FirmwarePlugin.cc
index b8d753c61..46789e8ff 100644
--- a/src/FirmwarePlugin/FirmwarePlugin.cc
+++ b/src/FirmwarePlugin/FirmwarePlugin.cc
@@ -357,577 +357,577 @@ const QVariantList& FirmwarePlugin::cameraList(const Vehicle*)
         CameraMetaData* metaData;
 
         metaData = new CameraMetaData(
-                    // Canon S100 @ 5.2mm f/2
-                    "Canon S100 PowerShot",     // canonical name saved in plan file
-                    tr("Canon"),                // brand
-                    tr("S100 PowerShot"),       // model
-                    7.6,                        // sensorWidth
-                    5.7,                        // sensorHeight
-                    4000,                       // imageWidth
-                    3000,                       // imageHeight
-                    5.2,                        // focalLength
+                    // Sony ILX-LR1 with 35mm Lens
+                    "Sony ILX-LR1-35mm",     // canonical name saved in plan file
+                    tr("Sony"),                // brand
+                    tr("ILX-LR1-35mm"),       // model
+                    35.81,                        // sensorWidth
+                    23.88,                        // sensorHeight
+                    7360,                       // imageWidth
+                    4912,                       // imageHeight
+                    35,                        // focalLength
                     true,                       // true: landscape orientation
                     false,                      // true: camera is fixed orientation
-                    0,                          // minimum trigger interval
-                    tr("Canon S100 PowerShot"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+                    0.4,                          // minimum trigger interval
+                    tr("Sony ILX-LR1-35mm"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
                     this);                      // parent
         _cameraList.append(QVariant::fromValue(metaData));
 
         metaData = new CameraMetaData(
-                    //tr("Canon EOS-M 22mm f/2"),
-                    "Canon EOS-M 22mm",
-                    tr("Canon"),
-                    tr("EOS-M 22mm"),
-                    22.3,                   // sensorWidth
-                    14.9,                   // sensorHeight
-                    5184,                   // imageWidth
-                    3456,                   // imageHeight
-                    22,                     // focalLength
-                    true,                   // true: landscape orientation
-                    false,                  // true: camera is fixed orientation
-                    0,                      // minimum trigger interval
-                    tr("Canon EOS-M 22mm"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                  // parent
+                    // Sony ILX-LR1 with 21mm Lens
+                    "Sony ILX-LR1-21mm",     // canonical name saved in plan file
+                    tr("Sony"),                // brand
+                    tr("ILX-LR1-21mm"),       // model
+                    35.81,                        // sensorWidth
+                    23.88,                        // sensorHeight
+                    7360,                       // imageWidth
+                    4912,                       // imageHeight
+                    21,                        // focalLength
+                    true,                       // true: landscape orientation
+                    false,                      // true: camera is fixed orientation
+                    0.4,                          // minimum trigger interval
+                    tr("Sony ILX-LR1-21mm"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+                    this);                      // parent
         _cameraList.append(QVariant::fromValue(metaData));
 
         metaData = new CameraMetaData(
-                    // Canon G9X @ 10.2mm f/2
-                    "Canon G9 X PowerShot",
-                    tr("Canon"),
-                    tr("G9 X PowerShot"),
-                    13.2,                       // sensorWidth
-                    8.8,                        // sensorHeight
-                    5488,                       // imageWidth
-                    3680,                       // imageHeight
-                    10.2,                       // focalLength
+                    // RESEPI HESAI XT32 LiDAR
+                    "Hesai XT32",
+                    tr("Inertial Labs"),
+                    tr("RESEPI HESAI XT32"),
+                    23.50,                       // sensorWidth
+                    15.60,                        // sensorHeight
+                    6058,                       // imageWidth
+                    4012,                       // imageHeight
+                    18,                        // focalLength
                     true,                       // true: landscape orientation
                     false,                      // true: camera is fixed orientation
                     0,                          // minimum trigger interval
-                    tr("Canon G9 X PowerShot"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+                    tr("RESEPI HESAI XT32"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
                     this);                      // parent
         _cameraList.append(QVariant::fromValue(metaData));
 
         metaData = new CameraMetaData(
-                    // Canon SX260 HS @ 4.5mm f/3.5
-                    "Canon SX260 HS PowerShot",
-                    tr("Canon"),
-                    tr("SX260 HS PowerShot"),
-                    6.17,                           // sensorWidth
-                    4.55,                           // sensorHeight
-                    4000,                           // imageWidth
-                    3000,                           // imageHeight
-                    4.5,                            // focalLength
+                    // Phase One P3
+                    "Phase One P3",
+                    tr("Phase One"),
+                    tr("P3"),
+                    43.90,                           // sensorWidth
+                    32.90,                           // sensorHeight
+                    11664,                           // imageWidth
+                    8750,                           // imageHeight
+                    63.0,                            // focalLength
                     true,                           // true: landscape orientation
                     false,                          // true: camera is fixed orientation
-                    0,                              // minimum trigger interval
-                    tr("Canon SX260 HS PowerShot"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+                    0.7,                              // minimum trigger interval
+                    tr("Phase One P3"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
                     this);                          // parent
         _cameraList.append(QVariant::fromValue(metaData));
 
-        metaData = new CameraMetaData(
-                    "GoPro Hero 4",
-                    tr("GoPro"),
-                    tr("Hero 4"),
-                    6.17,               // sensorWidth
-                    4.55,               // sendsorHeight
-                    4000,               // imageWidth
-                    3000,               // imageHeight
-                    2.98,               // focalLength
-                    true,               // landscape
-                    false,              // fixedOrientation
-                    0,                  // minTriggerInterval
-                    tr("GoPro Hero 4"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Parrot Sequioa RGB",
-                    tr("Parrot"),
-                    tr("Sequioa RGB"),
-                    6.17,                       // sensorWidth
-                    4.63,                       // sendsorHeight
-                    4608,                       // imageWidth
-                    3456,                       // imageHeight
-                    4.9,                        // focalLength
-                    true,                       // landscape
-                    false,                      // fixedOrientation
-                    1,                          // minTriggerInterval
-                    tr("Parrot Sequioa RGB"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Parrot Sequioa Monochrome",
-                    tr("Parrot"),
-                    tr("Sequioa Monochrome"),
-                    4.8,                                // sensorWidth
-                    3.6,                                // sendsorHeight
-                    1280,                               // imageWidth
-                    960,                                // imageHeight
-                    4.0,                                // focalLength
-                    true,                               // landscape
-                    false,                              // fixedOrientation
-                    0.8,                                // minTriggerInterval
-                    tr("Parrot Sequioa Monochrome"),    // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "RedEdge",
-                    tr("RedEdge"),
-                    tr("RedEdge"),
-                    4.8,            // sensorWidth
-                    3.6,            // sendsorHeight
-                    1280,           // imageWidth
-                    960,            // imageHeight
-                    5.5,            // focalLength
-                    true,           // landscape
-                    false,          // fixedOrientation
-                    0,              // minTriggerInterval
-                    tr("RedEdge"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    // Ricoh GR II 18.3mm f/2.8
-                    "Ricoh GR II",
-                    tr("Ricoh"),
-                    tr("GR II"),
-                    23.7,               // sensorWidth
-                    15.7,               // sendsorHeight
-                    4928,               // imageWidth
-                    3264,               // imageHeight
-                    18.3,               // focalLength
-                    true,               // landscape
-                    false,              // fixedOrientation
-                    0,                  // minTriggerInterval
-                    tr("Ricoh GR II"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sentera Double 4K Sensor",
-                    tr("Sentera"),
-                    tr("Double 4K Sensor"),
-                    6.2,                // sensorWidth
-                    4.65,               // sendsorHeight
-                    4000,               // imageWidth
-                    3000,               // imageHeight
-                    5.4,                // focalLength
-                    true,               // landscape
-                    false,              // fixedOrientation
-                    0.8,                // minTriggerInterval
-                    tr("Sentera Double 4K Sensor"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sentera NDVI Single Sensor",
-                    tr("Sentera"),
-                    tr("NDVI Single Sensor"),
-                    4.68,               // sensorWidth
-                    3.56,               // sendsorHeight
-                    1248,               // imageWidth
-                    952,                // imageHeight
-                    4.14,               // focalLength
-                    true,               // landscape
-                    false,              // fixedOrientation
-                    0.5,                // minTriggerInterval
-                    tr("Sentera NDVI Single Sensor"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sentera 6X Sensor",
-                    tr("Sentera"),
-                    tr("6X Sensor"),
-                    6.57,               // sensorWidth
-                    4.93,               // sendsorHeight
-                    1904,               // imageWidth
-                    1428,               // imageHeight
-                    8.0,                // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    0.2,                // minimum trigger interval
-                    tr(""),             // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sentera 65R Sensor",
-                    tr("Sentera"),
-                    tr("65R Sensor"),
-                    29.9,                // sensorWidth
-                    22.4,                // sendsorHeight
-                    9344,                // imageWidth
-                    7000,                // imageHeight
-                    27.4,                // focalLength
-                    true,                // landscape
-                    false,               // fixedOrientation
-                    0.3,                 // minTriggerInterval
-                    tr(""),              // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);               // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    //-- http://www.sony.co.uk/electronics/interchangeable-lens-cameras/ilce-6000-body-kit#product_details_default
-                    // Sony a6000 Sony 16mm f/2.8"
-                    "Sony a6000 16mm",
-                    tr("Sony"),
-                    tr("a6000 16mm"),
-                    23.5,                   // sensorWidth
-                    15.6,                   // sensorHeight
-                    6000,                   // imageWidth
-                    4000,                   // imageHeight
-                    16,                     // focalLength
-                    true,                   // true: landscape orientation
-                    false,                  // true: camera is fixed orientation
-                    1.0,                    // minimum trigger interval
-                    tr("Sony a6000 16mm"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                  // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a6000 35mm",
-                    tr("Sony"),
-                    tr("a6000 35mm"),
-                    23.5,               // sensorWidth
-                    15.6,               // sensorHeight
-                    6000,               // imageWidth
-                    4000,               // imageHeight
-                    35,                 // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.0,                // minimum trigger interval
-                    "",
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a6300 Zeiss 21mm f/2.8",
-                    tr("Sony"),
-                    tr("a6300 Zeiss 21mm f/2.8"),
-                    23.5,               // sensorWidth
-                    15.6,               // sensorHeight
-                    6000,               // imageWidth
-                    4000,               // imageHeight
-                    21,                 // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.0,                // minimum trigger interval
-                    tr("Sony a6300 Zeiss 21mm f/2.8"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a6300 Sony 28mm f/2.0",
-                    tr("Sony"),
-                    tr("a6300 Sony 28mm f/2.0"),
-                    23.5,                               // sensorWidth
-                    15.6,                               // sensorHeight
-                    6000,                               // imageWidth
-                    4000,                               // imageHeight
-                    28,                                 // focalLength
-                    true,                               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.0,                                // minimum trigger interval
-                    tr("Sony a6300 Sony 28mm f/2.0"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a7R II Zeiss 21mm f/2.8",
-                    tr("Sony"),
-                    tr("a7R II Zeiss 21mm f/2.8"),
-                    35.814,                             // sensorWidth
-                    23.876,                             // sensorHeight
-                    7952,                               // imageWidth
-                    5304,                               // imageHeight
-                    21,                                 // focalLength
-                    true,                               // true: landscape orientation
-                    true,                               // true: camera is fixed orientation
-                    1.0,                                // minimum trigger interval
-                    tr("Sony a7R II Zeiss 21mm f/2.8"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a7R II Sony 28mm f/2.0",
-                    tr("Sony"),
-                    tr("a7R II Sony 28mm f/2.0"),
-                    35.814,             // sensorWidth
-                    23.876,             // sensorHeight
-                    7952,               // imageWidth
-                    5304,               // imageHeight
-                    28,                 // focalLength
-                    true,               // true: landscape orientation
-                    true,               // true: camera is fixed orientation
-                    1.0,                // minimum trigger interval
-                    tr("Sony a7R II Sony 28mm f/2.0"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a7r III 35mm",
-                    tr("Sony"),
-                    tr("a7r III 35mm"),
-                    35.9,               // sensorWidth
-                    24.0,               // sensorHeight
-                    7952,               // imageWidth
-                    5304,               // imageHeight
-                    35,                 // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.0,                // minimum trigger interval
-                    "",
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony a7r IV 35mm",
-                    tr("Sony"),
-                    tr("a7r IV 35mm"),
-                    35.7,               // sensorWidth
-                    23.8,               // sensorHeight
-                    9504,               // imageWidth
-                    6336,               // imageHeight
-                    35,                 // focalLength
-                    true,               // true: landscape orientation
-                    false,               // true: camera is fixed orientation
-                    1.0,                // minimum trigger interval
-                    "",
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony DSC-QX30U @ 4.3mm f/3.5",
-                    tr("Sony"),
-                    tr("DSC-QX30U @ 4.3mm f/3.5"),
-                    7.82,                               // sensorWidth
-                    5.865,                              // sensorHeight
-                    5184,                               // imageWidth
-                    3888,                               // imageHeight
-                    4.3,                                // focalLength
-                    true,                               // true: landscape orientation
-                    false,                              // true: camera is fixed orientation
-                    2.0,                                // minimum trigger interval
-                    tr("Sony DSC-QX30U @ 4.3mm f/3.5"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony DSC-RX0",
-                    tr("Sony"),
-                    tr("DSC-RX0"),
-                    13.2,               // sensorWidth
-                    8.8,                // sensorHeight
-                    4800,               // imageWidth
-                    3200,               // imageHeight
-                    7.7,                // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    0,                  // minimum trigger interval
-                    tr("Sony DSC-RX0"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Sony DSC-RX1R II 35mm",
-                    tr("Sony"),
-                    tr("DSC-RX1R II 35mm"),
-                    35.9,             // sensorWidth
-                    24.0,             // sensorHeight
-                    7952,               // imageWidth
-                    5304,               // imageHeight
-                    35,                 // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.0,                // minimum trigger interval
-                    "",
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    //-- http://www.sony.co.uk/electronics/interchangeable-lens-cameras/ilce-qx1-body-kit/specifications
-                    //-- http://www.sony.com/electronics/camera-lenses/sel16f28/specifications
-                    //tr("Sony ILCE-QX1 Sony 16mm f/2.8"),
-                    "Sony ILCE-QX1",
-                    tr("Sony"),
-                    tr("ILCE-QX1"),
-                    23.2,                   // sensorWidth
-                    15.4,                   // sensorHeight
-                    5456,                   // imageWidth
-                    3632,                   // imageHeight
-                    16,                     // focalLength
-                    true,                   // true: landscape orientation
-                    false,                  // true: camera is fixed orientation
-                    0,                      // minimum trigger interval
-                    tr("Sony ILCE-QX1"),    // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                  // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    //-- http://www.sony.co.uk/electronics/interchangeable-lens-cameras/ilce-qx1-body-kit/specifications
-                    // Sony NEX-5R Sony 20mm f/2.8"
-                    "Sony NEX-5R 20mm",
-                    tr("Sony"),
-                    tr("NEX-5R 20mm"),
-                    23.2,                   // sensorWidth
-                    15.4,                   // sensorHeight
-                    4912,                   // imageWidth
-                    3264,                   // imageHeight
-                    20,                     // focalLength
-                    true,                   // true: landscape orientation
-                    false,                  // true: camera is fixed orientation
-                    1,                      // minimum trigger interval
-                    tr("Sony NEX-5R 20mm"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);                  // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    // Sony RX100 II @ 10.4mm f/1.8
-                    "Sony RX100 II 28mm",
-                    tr("Sony"),
-                    tr("RX100 II 28mm"),
-                    13.2,                // sensorWidth
-                    8.8,                 // sensorHeight
-                    5472,                // imageWidth
-                    3648,                // imageHeight
-                    10.4,                // focalLength
-                    true,                // true: landscape orientation
-                    false,               // true: camera is fixed orientation
-                    0,                   // minimum trigger interval
-                    tr("Sony RX100 II 28mm"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);               // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Yuneec CGOET",
-                    tr("Yuneec"),
-                    tr("CGOET"),
-                    5.6405,             // sensorWidth
-                    3.1813,             // sensorHeight
-                    1920,               // imageWidth
-                    1080,               // imageHeight
-                    3.5,                // focalLength
-                    true,               // true: landscape orientation
-                    true,               // true: camera is fixed orientation
-                    1.3,                // minimum trigger interval
-                    tr("Yuneec CGOET"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Yuneec E10T",
-                    tr("Yuneec"),
-                    tr("E10T"),
-                    5.6405,             // sensorWidth
-                    3.1813,             // sensorHeight
-                    1920,               // imageWidth
-                    1080,               // imageHeight
-                    23,                 // focalLength
-                    true,               // true: landscape orientation
-                    true,               // true: camera is fixed orientation
-                    1.3,                // minimum trigger interval
-                    tr("Yuneec E10T"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Yuneec E50",
-                    tr("Yuneec"),
-                    tr("E50"),
-                    6.2372,             // sensorWidth
-                    4.7058,             // sensorHeight
-                    4000,               // imageWidth
-                    3000,               // imageHeight
-                    7.2,                // focalLength
-                    true,               // true: landscape orientation
-                    true,               // true: camera is fixed orientation
-                    1.3,                // minimum trigger interval
-                    tr("Yuneec E50"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Yuneec E90",
-                    tr("Yuneec"),
-                    tr("E90"),
-                    13.3056,            // sensorWidth
-                    8.656,              // sensorHeight
-                    5472,               // imageWidth
-                    3648,               // imageHeight
-                    8.29,               // focalLength
-                    true,               // true: landscape orientation
-                    true,               // true: camera is fixed orientation
-                    1.3,                // minimum trigger interval
-                    tr("Yuneec E90"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Flir Duo R",
-                    tr("Flir"),
-                    tr("Duo R"),
-                    160,                // sensorWidth
-                    120,                // sensorHeight
-                    1920,               // imageWidth
-                    1080,               // imageHeight
-                    1.9,                // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    0,                  // minimum trigger interval
-                    tr("Flir Duo R"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Flir Duo Pro R",
-                    tr("Flir"),
-                    tr("Duo Pro R"),
-                    10.88,                // sensorWidth
-                    8.704,                // sensorHeight
-                    640,               // imageWidth
-                    512,               // imageHeight
-                    19,                // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.0,                  // minimum trigger interval
-                    "",   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Workswell Wiris Security Thermal Camera",
-                    tr("Workswell"),
-                    tr("Wiris Security"),
-                    13.6,                // sensorWidth
-                    10.2,                // sensorHeight
-                    800,               // imageWidth
-                    600,               // imageHeight
-                    35,                // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.8,                  // minimum trigger interval
-                    "",   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
-
-        metaData = new CameraMetaData(
-                    "Workswell Wiris Security Visual Camera",
-                    tr("Workswell"),
-                    tr("Wiris Security"),
-                    4.826,                // sensorWidth
-                    3.556,                // sensorHeight
-                    1920,               // imageWidth
-                    1080,               // imageHeight
-                    4.3,                // focalLength
-                    true,               // true: landscape orientation
-                    false,              // true: camera is fixed orientation
-                    1.8,                  // minimum trigger interval
-                    "",   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
-                    this);              // parent
-        _cameraList.append(QVariant::fromValue(metaData));
+//        metaData = new CameraMetaData(
+//                    "GoPro Hero 4",
+//                    tr("GoPro"),
+//                    tr("Hero 4"),
+//                    6.17,               // sensorWidth
+//                    4.55,               // sendsorHeight
+//                    4000,               // imageWidth
+//                    3000,               // imageHeight
+//                    2.98,               // focalLength
+//                    true,               // landscape
+//                    false,              // fixedOrientation
+//                    0,                  // minTriggerInterval
+//                    tr("GoPro Hero 4"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Parrot Sequioa RGB",
+//                    tr("Parrot"),
+//                    tr("Sequioa RGB"),
+//                    6.17,                       // sensorWidth
+//                    4.63,                       // sendsorHeight
+//                    4608,                       // imageWidth
+//                    3456,                       // imageHeight
+//                    4.9,                        // focalLength
+//                    true,                       // landscape
+//                    false,                      // fixedOrientation
+//                    1,                          // minTriggerInterval
+//                    tr("Parrot Sequioa RGB"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Parrot Sequioa Monochrome",
+//                    tr("Parrot"),
+//                    tr("Sequioa Monochrome"),
+//                    4.8,                                // sensorWidth
+//                    3.6,                                // sendsorHeight
+//                    1280,                               // imageWidth
+//                    960,                                // imageHeight
+//                    4.0,                                // focalLength
+//                    true,                               // landscape
+//                    false,                              // fixedOrientation
+//                    0.8,                                // minTriggerInterval
+//                    tr("Parrot Sequioa Monochrome"),    // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "RedEdge",
+//                    tr("RedEdge"),
+//                    tr("RedEdge"),
+//                    4.8,            // sensorWidth
+//                    3.6,            // sendsorHeight
+//                    1280,           // imageWidth
+//                    960,            // imageHeight
+//                    5.5,            // focalLength
+//                    true,           // landscape
+//                    false,          // fixedOrientation
+//                    0,              // minTriggerInterval
+//                    tr("RedEdge"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    // Ricoh GR II 18.3mm f/2.8
+//                    "Ricoh GR II",
+//                    tr("Ricoh"),
+//                    tr("GR II"),
+//                    23.7,               // sensorWidth
+//                    15.7,               // sendsorHeight
+//                    4928,               // imageWidth
+//                    3264,               // imageHeight
+//                    18.3,               // focalLength
+//                    true,               // landscape
+//                    false,              // fixedOrientation
+//                    0,                  // minTriggerInterval
+//                    tr("Ricoh GR II"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sentera Double 4K Sensor",
+//                    tr("Sentera"),
+//                    tr("Double 4K Sensor"),
+//                    6.2,                // sensorWidth
+//                    4.65,               // sendsorHeight
+//                    4000,               // imageWidth
+//                    3000,               // imageHeight
+//                    5.4,                // focalLength
+//                    true,               // landscape
+//                    false,              // fixedOrientation
+//                    0.8,                // minTriggerInterval
+//                    tr("Sentera Double 4K Sensor"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sentera NDVI Single Sensor",
+//                    tr("Sentera"),
+//                    tr("NDVI Single Sensor"),
+//                    4.68,               // sensorWidth
+//                    3.56,               // sendsorHeight
+//                    1248,               // imageWidth
+//                    952,                // imageHeight
+//                    4.14,               // focalLength
+//                    true,               // landscape
+//                    false,              // fixedOrientation
+//                    0.5,                // minTriggerInterval
+//                    tr("Sentera NDVI Single Sensor"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sentera 6X Sensor",
+//                    tr("Sentera"),
+//                    tr("6X Sensor"),
+//                    6.57,               // sensorWidth
+//                    4.93,               // sendsorHeight
+//                    1904,               // imageWidth
+//                    1428,               // imageHeight
+//                    8.0,                // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    0.2,                // minimum trigger interval
+//                    tr(""),             // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sentera 65R Sensor",
+//                    tr("Sentera"),
+//                    tr("65R Sensor"),
+//                    29.9,                // sensorWidth
+//                    22.4,                // sendsorHeight
+//                    9344,                // imageWidth
+//                    7000,                // imageHeight
+//                    27.4,                // focalLength
+//                    true,                // landscape
+//                    false,               // fixedOrientation
+//                    0.3,                 // minTriggerInterval
+//                    tr(""),              // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);               // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    //-- http://www.sony.co.uk/electronics/interchangeable-lens-cameras/ilce-6000-body-kit#product_details_default
+//                    // Sony a6000 Sony 16mm f/2.8"
+//                    "Sony a6000 16mm",
+//                    tr("Sony"),
+//                    tr("a6000 16mm"),
+//                    23.5,                   // sensorWidth
+//                    15.6,                   // sensorHeight
+//                    6000,                   // imageWidth
+//                    4000,                   // imageHeight
+//                    16,                     // focalLength
+//                    true,                   // true: landscape orientation
+//                    false,                  // true: camera is fixed orientation
+//                    1.0,                    // minimum trigger interval
+//                    tr("Sony a6000 16mm"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);                  // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a6000 35mm",
+//                    tr("Sony"),
+//                    tr("a6000 35mm"),
+//                    23.5,               // sensorWidth
+//                    15.6,               // sensorHeight
+//                    6000,               // imageWidth
+//                    4000,               // imageHeight
+//                    35,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.0,                // minimum trigger interval
+//                    "",
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a6300 Zeiss 21mm f/2.8",
+//                    tr("Sony"),
+//                    tr("a6300 Zeiss 21mm f/2.8"),
+//                    23.5,               // sensorWidth
+//                    15.6,               // sensorHeight
+//                    6000,               // imageWidth
+//                    4000,               // imageHeight
+//                    21,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.0,                // minimum trigger interval
+//                    tr("Sony a6300 Zeiss 21mm f/2.8"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a6300 Sony 28mm f/2.0",
+//                    tr("Sony"),
+//                    tr("a6300 Sony 28mm f/2.0"),
+//                    23.5,                               // sensorWidth
+//                    15.6,                               // sensorHeight
+//                    6000,                               // imageWidth
+//                    4000,                               // imageHeight
+//                    28,                                 // focalLength
+//                    true,                               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.0,                                // minimum trigger interval
+//                    tr("Sony a6300 Sony 28mm f/2.0"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);                              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a7R II Zeiss 21mm f/2.8",
+//                    tr("Sony"),
+//                    tr("a7R II Zeiss 21mm f/2.8"),
+//                    35.814,                             // sensorWidth
+//                    23.876,                             // sensorHeight
+//                    7952,                               // imageWidth
+//                    5304,                               // imageHeight
+//                    21,                                 // focalLength
+//                    true,                               // true: landscape orientation
+//                    true,                               // true: camera is fixed orientation
+//                    1.0,                                // minimum trigger interval
+//                    tr("Sony a7R II Zeiss 21mm f/2.8"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);                              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a7R II Sony 28mm f/2.0",
+//                    tr("Sony"),
+//                    tr("a7R II Sony 28mm f/2.0"),
+//                    35.814,             // sensorWidth
+//                    23.876,             // sensorHeight
+//                    7952,               // imageWidth
+//                    5304,               // imageHeight
+//                    28,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    true,               // true: camera is fixed orientation
+//                    1.0,                // minimum trigger interval
+//                    tr("Sony a7R II Sony 28mm f/2.0"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a7r III 35mm",
+//                    tr("Sony"),
+//                    tr("a7r III 35mm"),
+//                    35.9,               // sensorWidth
+//                    24.0,               // sensorHeight
+//                    7952,               // imageWidth
+//                    5304,               // imageHeight
+//                    35,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.0,                // minimum trigger interval
+//                    "",
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony a7r IV 35mm",
+//                    tr("Sony"),
+//                    tr("a7r IV 35mm"),
+//                    35.7,               // sensorWidth
+//                    23.8,               // sensorHeight
+//                    9504,               // imageWidth
+//                    6336,               // imageHeight
+//                    35,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    false,               // true: camera is fixed orientation
+//                    1.0,                // minimum trigger interval
+//                    "",
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony DSC-QX30U @ 4.3mm f/3.5",
+//                    tr("Sony"),
+//                    tr("DSC-QX30U @ 4.3mm f/3.5"),
+//                    7.82,                               // sensorWidth
+//                    5.865,                              // sensorHeight
+//                    5184,                               // imageWidth
+//                    3888,                               // imageHeight
+//                    4.3,                                // focalLength
+//                    true,                               // true: landscape orientation
+//                    false,                              // true: camera is fixed orientation
+//                    2.0,                                // minimum trigger interval
+//                    tr("Sony DSC-QX30U @ 4.3mm f/3.5"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);                              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony DSC-RX0",
+//                    tr("Sony"),
+//                    tr("DSC-RX0"),
+//                    13.2,               // sensorWidth
+//                    8.8,                // sensorHeight
+//                    4800,               // imageWidth
+//                    3200,               // imageHeight
+//                    7.7,                // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    0,                  // minimum trigger interval
+//                    tr("Sony DSC-RX0"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Sony DSC-RX1R II 35mm",
+//                    tr("Sony"),
+//                    tr("DSC-RX1R II 35mm"),
+//                    35.9,             // sensorWidth
+//                    24.0,             // sensorHeight
+//                    7952,               // imageWidth
+//                    5304,               // imageHeight
+//                    35,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.0,                // minimum trigger interval
+//                    "",
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    //-- http://www.sony.co.uk/electronics/interchangeable-lens-cameras/ilce-qx1-body-kit/specifications
+//                    //-- http://www.sony.com/electronics/camera-lenses/sel16f28/specifications
+//                    //tr("Sony ILCE-QX1 Sony 16mm f/2.8"),
+//                    "Sony ILCE-QX1",
+//                    tr("Sony"),
+//                    tr("ILCE-QX1"),
+//                    23.2,                   // sensorWidth
+//                    15.4,                   // sensorHeight
+//                    5456,                   // imageWidth
+//                    3632,                   // imageHeight
+//                    16,                     // focalLength
+//                    true,                   // true: landscape orientation
+//                    false,                  // true: camera is fixed orientation
+//                    0,                      // minimum trigger interval
+//                    tr("Sony ILCE-QX1"),    // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);                  // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    //-- http://www.sony.co.uk/electronics/interchangeable-lens-cameras/ilce-qx1-body-kit/specifications
+//                    // Sony NEX-5R Sony 20mm f/2.8"
+//                    "Sony NEX-5R 20mm",
+//                    tr("Sony"),
+//                    tr("NEX-5R 20mm"),
+//                    23.2,                   // sensorWidth
+//                    15.4,                   // sensorHeight
+//                    4912,                   // imageWidth
+//                    3264,                   // imageHeight
+//                    20,                     // focalLength
+//                    true,                   // true: landscape orientation
+//                    false,                  // true: camera is fixed orientation
+//                    1,                      // minimum trigger interval
+//                    tr("Sony NEX-5R 20mm"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);                  // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    // Sony RX100 II @ 10.4mm f/1.8
+//                    "Sony RX100 II 28mm",
+//                    tr("Sony"),
+//                    tr("RX100 II 28mm"),
+//                    13.2,                // sensorWidth
+//                    8.8,                 // sensorHeight
+//                    5472,                // imageWidth
+//                    3648,                // imageHeight
+//                    10.4,                // focalLength
+//                    true,                // true: landscape orientation
+//                    false,               // true: camera is fixed orientation
+//                    0,                   // minimum trigger interval
+//                    tr("Sony RX100 II 28mm"),// SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);               // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Yuneec CGOET",
+//                    tr("Yuneec"),
+//                    tr("CGOET"),
+//                    5.6405,             // sensorWidth
+//                    3.1813,             // sensorHeight
+//                    1920,               // imageWidth
+//                    1080,               // imageHeight
+//                    3.5,                // focalLength
+//                    true,               // true: landscape orientation
+//                    true,               // true: camera is fixed orientation
+//                    1.3,                // minimum trigger interval
+//                    tr("Yuneec CGOET"), // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Yuneec E10T",
+//                    tr("Yuneec"),
+//                    tr("E10T"),
+//                    5.6405,             // sensorWidth
+//                    3.1813,             // sensorHeight
+//                    1920,               // imageWidth
+//                    1080,               // imageHeight
+//                    23,                 // focalLength
+//                    true,               // true: landscape orientation
+//                    true,               // true: camera is fixed orientation
+//                    1.3,                // minimum trigger interval
+//                    tr("Yuneec E10T"),  // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Yuneec E50",
+//                    tr("Yuneec"),
+//                    tr("E50"),
+//                    6.2372,             // sensorWidth
+//                    4.7058,             // sensorHeight
+//                    4000,               // imageWidth
+//                    3000,               // imageHeight
+//                    7.2,                // focalLength
+//                    true,               // true: landscape orientation
+//                    true,               // true: camera is fixed orientation
+//                    1.3,                // minimum trigger interval
+//                    tr("Yuneec E50"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Yuneec E90",
+//                    tr("Yuneec"),
+//                    tr("E90"),
+//                    13.3056,            // sensorWidth
+//                    8.656,              // sensorHeight
+//                    5472,               // imageWidth
+//                    3648,               // imageHeight
+//                    8.29,               // focalLength
+//                    true,               // true: landscape orientation
+//                    true,               // true: camera is fixed orientation
+//                    1.3,                // minimum trigger interval
+//                    tr("Yuneec E90"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Flir Duo R",
+//                    tr("Flir"),
+//                    tr("Duo R"),
+//                    160,                // sensorWidth
+//                    120,                // sensorHeight
+//                    1920,               // imageWidth
+//                    1080,               // imageHeight
+//                    1.9,                // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    0,                  // minimum trigger interval
+//                    tr("Flir Duo R"),   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Flir Duo Pro R",
+//                    tr("Flir"),
+//                    tr("Duo Pro R"),
+//                    10.88,                // sensorWidth
+//                    8.704,                // sensorHeight
+//                    640,               // imageWidth
+//                    512,               // imageHeight
+//                    19,                // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.0,                  // minimum trigger interval
+//                    "",   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Workswell Wiris Security Thermal Camera",
+//                    tr("Workswell"),
+//                    tr("Wiris Security"),
+//                    13.6,                // sensorWidth
+//                    10.2,                // sensorHeight
+//                    800,               // imageWidth
+//                    600,               // imageHeight
+//                    35,                // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.8,                  // minimum trigger interval
+//                    "",   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
+
+//        metaData = new CameraMetaData(
+//                    "Workswell Wiris Security Visual Camera",
+//                    tr("Workswell"),
+//                    tr("Wiris Security"),
+//                    4.826,                // sensorWidth
+//                    3.556,                // sensorHeight
+//                    1920,               // imageWidth
+//                    1080,               // imageHeight
+//                    4.3,                // focalLength
+//                    true,               // true: landscape orientation
+//                    false,              // true: camera is fixed orientation
+//                    1.8,                  // minimum trigger interval
+//                    "",   // SHOULD BE BLANK FOR NEWLY ADDED CAMERAS. Deprecated translation from older builds.
+//                    this);              // parent
+//        _cameraList.append(QVariant::fromValue(metaData));
     }
 
     return _cameraList;
diff --git a/src/FlightDisplay/FlightDisplayViewWidgets.qml b/src/FlightDisplay/FlightDisplayViewWidgets.qml
index f0174e409..b15ddf66f 100644
--- a/src/FlightDisplay/FlightDisplayViewWidgets.qml
+++ b/src/FlightDisplay/FlightDisplayViewWidgets.qml
@@ -23,9 +23,19 @@ import QGroundControl.Vehicle       1.0
 import QGroundControl.FlightMap     1.0
 
 Loader {
+    id: loader
+
+    //signal minimizeRequested() // define signal
+
     width:  parent.width
     source: QGroundControl.settingsManager.flyViewSettings.alternateInstrumentPanel.rawValue ?
                 "qrc:/qml/QGCInstrumentWidgetAlternate.qml" : "qrc:/qml/QGCInstrumentWidget.qml"
 
     property var missionController
+
+//    onLoaded: {
+//        if (loader.item && loader.item.hasOwnProperty("minimizeRequested")) {
+//            loader.item.minimizeRequested.connect(minimizeRequested)
+//        }
+//    }
 }
diff --git a/src/FlightDisplay/FlyView.qml b/src/FlightDisplay/FlyView.qml
index 68c2a8b2a..92f63921e 100644
--- a/src/FlightDisplay/FlyView.qml
+++ b/src/FlightDisplay/FlyView.qml
@@ -149,6 +149,28 @@ Item {
         visible:            false
     }
 
+
+
+    // Custom Video Switch
+//    Column {
+//        spacing: 10
+//        anchors.top: parent.top
+//        anchors.right: parent.right
+//        anchors.margins: 20
+//        visible: true
+
+//        Button {
+//            text: "Switch Stream"
+//            //onClicked: QGroundControl.videoManager.streamingChanged()
+//        }
+
+//        Text {
+//            text: "Current Stream: " + QGroundControl.videoManager.objectName
+//            color: "white"
+//        }
+//    }
+
+
     FlyViewMap {
         id:                     mapControl
         planMasterController:   _planController
@@ -175,4 +197,58 @@ Item {
         show:                   !QGroundControl.videoManager.fullScreen &&
                                     (videoControl.pipState.state === videoControl.pipState.pipState || mapControl.pipState.state === mapControl.pipState.pipState)
     }
-}
+
+    Button {
+        text: "Stream\n#"
+        onClicked: QGroundControl.videoManager.switchRTSPStream()
+        width: 100
+        height: 100
+        anchors.bottom: parent.bottom
+        anchors.right: parent.right
+        anchors.margins: 20
+        background: Rectangle {
+            color: "#444"
+            radius: 20
+            opacity: 0.6
+        }
+        contentItem: Text {
+            text: "Stream\n" + QGroundControl.videoManager.currentStream
+            color: "white"
+            font.bold: true
+            font.pixelSize: 20
+            horizontalAlignment: Text.AlignHCenter
+            verticalAlignment: Text.AlignVCenter
+        }
+    }
+//    Text {
+//        text: "Current Stream: "// + //QGroundControl.videoManager._currentStreamIndex
+//        color: "white"
+//    }
+
+    Button {
+        text: "Full\nScreen"
+        onClicked: {
+            //console.log("Full screen button clicked");
+            QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
+        }
+        width: 50
+        height: 50
+        anchors.verticalCenter: parent.verticalCenter
+        anchors.left: parent.left
+        anchors.margins: 10
+        background: Rectangle {
+            color: "#444"
+            radius: 6
+            opacity: 0.4
+        }
+        contentItem: Text {
+            text: "Full\nScreen"
+            color: "white"
+            font.bold: true
+            font.pixelSize: 10
+            horizontalAlignment: Text.AlignHCenter
+            verticalAlignment: Text.AlignVCenter
+        }
+    } // Fullscreen button
+
+} // end of item
diff --git a/src/FlightDisplay/FlyViewInstrumentPanel.qml b/src/FlightDisplay/FlyViewInstrumentPanel.qml
index 32ab45e05..47c60f928 100644
--- a/src/FlightDisplay/FlyViewInstrumentPanel.qml
+++ b/src/FlightDisplay/FlyViewInstrumentPanel.qml
@@ -14,16 +14,26 @@ import QGroundControl.Controls      1.0
 import QGroundControl.ScreenTools   1.0
 
 // This control contains the instruments as well and the instrument pages which include values, camera, ...
+
+
 Column {
     id:         _root
     spacing:    _toolsMargin
     z:          QGroundControl.zOrderWidgets
 
     property real availableHeight
+    property bool isMinimized
 
     FlightDisplayViewWidgets {
         id:                 flightDisplayViewWidgets
         width:              parent.width
         missionController:  _missionController
+
+//        onMinimizeRequested: {
+//            root.isMinimized = true
+//            root.minimizeRequested()
+//        }
     }
 }
+
+
diff --git a/src/FlightDisplay/FlyViewVideo.qml b/src/FlightDisplay/FlyViewVideo.qml
index 65b4232c5..8b2156f1f 100644
--- a/src/FlightDisplay/FlyViewVideo.qml
+++ b/src/FlightDisplay/FlyViewVideo.qml
@@ -68,7 +68,7 @@ Item {
     }
 
     QGCLabel {
-        text: qsTr("Double-click to exit full screen")
+        text: qsTr(" ")
         font.pointSize: ScreenTools.largeFontPointSize
         visible: QGroundControl.videoManager.fullScreen && flyViewVideoMouseArea.containsMouse
         anchors.centerIn: parent
diff --git a/src/FlightDisplay/FlyViewWidgetLayer.qml b/src/FlightDisplay/FlyViewWidgetLayer.qml
index 543dfd511..21cb2166a 100644
--- a/src/FlightDisplay/FlyViewWidgetLayer.qml
+++ b/src/FlightDisplay/FlyViewWidgetLayer.qml
@@ -48,6 +48,9 @@ Item {
     property real   _rightPanelWidth:       ScreenTools.defaultFontPixelWidth * 30
     property alias  _gripperMenu:           gripperOptions
 
+    property bool   isInstrumentPanelMinimized: false
+    property bool   isPhotoVideoMinimized: false
+
     QGCToolInsets {
         id:                     _totalToolInsets
         leftEdgeTopInset:       toolStrip.leftEdgeTopInset
@@ -122,11 +125,52 @@ Item {
         anchors.right:              parent.right
         width:                      _rightPanelWidth
         spacing:                    _toolsMargin
-        visible:                    QGroundControl.corePlugin.options.flyView.showInstrumentPanel && multiVehiclePanelSelector.showSingleVehiclePanel
+        visible:                    !_root.isInstrumentPanelMinimized//QGroundControl.corePlugin.options.flyView.showInstrumentPanel && multiVehiclePanelSelector.showSingleVehiclePanel
         availableHeight:            parent.height - y - _toolsMargin
 
         property real rightEdgeTopInset: visible ? parent.width - x : 0
         property real topEdgeRightInset: visible ? y + height : 0
+
+        //onMinimizeRequested: _root.isInstrumentPanelMinimized = true
+        Image {
+            id:             minimizeButton
+            source:         "/qmlimages/pipHide.svg"
+            mipmap:         true
+            rotation:       180
+            fillMode:       Image.PreserveAspectFit
+            anchors.right:  parent.right
+            anchors.top:    parent.top
+            visible:        !_root.isInstrumentPanelMinimized // only when minimized
+            //visible:        _isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse)
+            height:         ScreenTools.defaultFontPixelHeight * 2.0
+            width:          ScreenTools.defaultFontPixelHeight * 2.0
+            opacity:        0.5
+            sourceSize.height:  height
+            MouseArea {
+                anchors.fill:   parent
+                onClicked:      _root.isInstrumentPanelMinimized = true
+            }
+        }
+    }
+
+    // add a button to restore it
+    Image {
+        id:             instRestoreButton
+        source:         "/qmlimages/pipHide.svg"
+        mipmap:         true
+        fillMode:       Image.PreserveAspectFit
+        anchors.right:  parent.right
+        anchors.top:    parent.top
+        visible:        _root.isInstrumentPanelMinimized // only when minimized
+        //visible:        _isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse)
+        height:         ScreenTools.defaultFontPixelHeight * 2.0
+        width:          ScreenTools.defaultFontPixelHeight * 2.0
+        opacity:        0.5
+        sourceSize.height:  height
+        MouseArea {
+            anchors.fill:   parent
+            onClicked:      _root.isInstrumentPanelMinimized = false
+        }
     }
 
     PhotoVideoControl {
@@ -134,6 +178,7 @@ Item {
         anchors.margins:        _toolsMargin
         anchors.right:          parent.right
         width:                  _rightPanelWidth
+        visible:                !_root.isPhotoVideoMinimized
 
         property real rightEdgeCenterInset: visible ? parent.width - x : 0
 
@@ -158,6 +203,46 @@ Item {
         ]
 
         property bool _verticalCenter: !QGroundControl.settingsManager.flyViewSettings.alternateInstrumentPanel.rawValue
+
+        Image {
+            id:             photoMinimizeButton
+            source:         "/qmlimages/pipHide.svg"
+            mipmap:         true
+            rotation:       180
+            fillMode:       Image.PreserveAspectFit
+            anchors.right:  parent.right
+            anchors.bottom:    parent.bottom
+            visible:        !_root.isPhotoVideoMinimized // only when minimized
+            //visible:        _isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse)
+            height:         ScreenTools.defaultFontPixelHeight * 2.0
+            width:          ScreenTools.defaultFontPixelHeight * 2.0
+            opacity:        0.5
+            sourceSize.height:  height
+            MouseArea {
+                anchors.fill:   parent
+                onClicked:      _root.isPhotoVideoMinimized = true
+            }
+        }
+    } // PhotoVideoControl
+
+    // add a button to restore it
+    Image {
+        id:             photoVideoRestoreButton
+        source:         "/qmlimages/pipHide.svg"
+        mipmap:         true
+        fillMode:       Image.PreserveAspectFit
+        anchors.right:  photoVideoControl.right
+        anchors.bottom:    photoVideoControl.bottom
+        visible:        _root.isPhotoVideoMinimized // only when minimized
+        //visible:        _isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse)
+        height:         ScreenTools.defaultFontPixelHeight * 2.0
+        width:          ScreenTools.defaultFontPixelHeight * 2.0
+        opacity:        0.5
+        sourceSize.height:  height
+        MouseArea {
+            anchors.fill:   parent
+            onClicked:      _root.isPhotoVideoMinimized = false
+        }
     }
 
     TelemetryValuesBar {
@@ -240,8 +325,8 @@ Item {
                 // Anchor to left edge
                 return parentToolInsets.leftEdgeBottomInset + _toolsMargin
             }
-        }
-    }
+        } // end of recalcXPosition
+    } // end of telemetry values bar
 
     property bool _paramBoxShowEnable: (typeof taisyncRemoteHandler !== "undefined" && taisyncRemoteHandler.showFlyParam)
                                        && QGroundControl.settingsManager.appSettings.taisyncFlyViewShow.value
@@ -522,7 +607,7 @@ Item {
         anchors.top:            parent.top
         z:                      QGroundControl.zOrderWidgets
         maxHeight:              parent.height - y - parentToolInsets.bottomEdgeLeftInset - _toolsMargin
-        visible:                !QGroundControl.videoManager.fullScreen
+        visible:                false //!QGroundControl.videoManager.fullScreen
 
         onDisplayPreFlightChecklist: preFlightChecklistPopup.createObject(mainWindow).open()
 
diff --git a/src/FlightDisplay/TelemetryValuesBar.qml b/src/FlightDisplay/TelemetryValuesBar.qml
index 136cfe175..f72901dd5 100644
--- a/src/FlightDisplay/TelemetryValuesBar.qml
+++ b/src/FlightDisplay/TelemetryValuesBar.qml
@@ -20,8 +20,9 @@ Rectangle {
     id:                 telemetryPanel
     height:             telemetryLayout.height + (_toolsMargin * 2)
     width:              telemetryLayout.width + (_toolsMargin * 2)
-    color:              qgcPal.window
+    color:              Qt.rgba(0,0,0,0.5)
     radius:             ScreenTools.defaultFontPixelWidth / 2
+    //opacity:            0.7
 
     property bool       bottomMode: true
 
@@ -44,7 +45,7 @@ Rectangle {
                 sourceSize.width:   width
                 color:              qgcPal.text
                 fillMode:           Image.PreserveAspectFit
-                visible:            !bottomMode
+                visible:            false//!bottomMode
 
                 QGCMouseArea {
                     fillItem:   parent
@@ -60,7 +61,7 @@ Rectangle {
                 sourceSize.width:   width
                 color:              qgcPal.text
                 fillMode:           Image.PreserveAspectFit
-                visible:            bottomMode
+                visible:            false//bottomMode
 
                 QGCMouseArea {
                     fillItem:   parent
diff --git a/src/FlightMap/Widgets/QGCInstrumentWidget.qml b/src/FlightMap/Widgets/QGCInstrumentWidget.qml
index 0c69160a2..15f9e6a22 100644
--- a/src/FlightMap/Widgets/QGCInstrumentWidget.qml
+++ b/src/FlightMap/Widgets/QGCInstrumentWidget.qml
@@ -27,6 +27,8 @@ ColumnLayout {
     property real   _spacing:               ScreenTools.defaultFontPixelHeight * 0.33
     property real   _topBottomMargin:       (width * 0.05) / 2
 
+    //signal minimizeRequested() // signal to notify parent
+
     QGCPalette { id: qgcPal }
 
     Rectangle {
@@ -35,6 +37,9 @@ ColumnLayout {
         Layout.fillWidth:   true
         radius:             _outerRadius
         color:              qgcPal.window
+        opacity: 0.7
+        property real globalOpacity: 0.8
+        //property bool isMinimized: false // track minimize state
 
         DeadMouseArea { anchors.fill: parent }
 
@@ -55,6 +60,31 @@ ColumnLayout {
             vehicle:                globals.activeVehicle
             anchors.verticalCenter: parent.verticalCenter
         }
+
+//        Component.onCompleted: {
+//            for (var i = 0; i < children.length; i++) {
+//                if (children[i].hasOwnProperty("opacity")) {
+//                    children[i].opacity = globalOpacity;
+//                }
+//            }
+//        }
+//        Image {
+//            id:             minimizeButton
+//            source:         "/qmlimages/pipHide.svg"
+//            mipmap:         true
+//            rotation:       180
+//            fillMode:       Image.PreserveAspectFit
+//            anchors.right:   parent.right
+//            anchors.top:     parent.top
+//            //visible:        _isExpanded && (ScreenTools.isMobile || pipMouseArea.containsMouse)
+//            height:         ScreenTools.defaultFontPixelHeight * 2.5
+//            width:          ScreenTools.defaultFontPixelHeight * 2.5
+//            sourceSize.height:  height
+//            MouseArea {
+//                anchors.fill:   parent
+//                onClicked:      minimizeRequested() //visualInstrument.isMinimized = !visualInstrument.isMinimized
+//            }
+//        }
     }
 
     TerrainProgress {
diff --git a/src/PlanView/PlanView.qml b/src/PlanView/PlanView.qml
index cbdb67722..64fb9f118 100644
--- a/src/PlanView/PlanView.qml
+++ b/src/PlanView/PlanView.qml
@@ -524,11 +524,11 @@ Item {
             ToolStripActionList {
                 id: toolStripActionList
                 model: [
-                    ToolStripAction {
-                        text:           qsTr("Fly")
-                        iconSource:     "/qmlimages/PaperPlane.svg"
-                        onTriggered:    mainWindow.showFlyView()
-                    },
+//                    ToolStripAction {
+//                        text:           qsTr("Fly")
+//                        iconSource:     "/qmlimages/PaperPlane.svg"
+//                        onTriggered:    mainWindow.showFlyView()
+//                    },
                     ToolStripAction {
                         text:                   qsTr("File")
                         enabled:                !_planMasterController.syncInProgress
diff --git a/src/QGCPalette.cc b/src/QGCPalette.cc
index ddf26fd54..7fdc7f5f5 100644
--- a/src/QGCPalette.cc
+++ b/src/QGCPalette.cc
@@ -85,7 +85,7 @@ void QGCPalette::_buildMap()
 
     // Colors not affecting by theming
     //                                              Disabled    Enabled
-    DECLARE_QGC_NONTHEMED_COLOR(brandingPurple,     "#4A2C6D", "#4A2C6D")
+    DECLARE_QGC_NONTHEMED_COLOR(brandingPurple,     "#DE881E", "#DE881E")
     DECLARE_QGC_NONTHEMED_COLOR(brandingBlue,       "#48D6FF", "#6045c5")
     DECLARE_QGC_NONTHEMED_COLOR(toolStripFGColor,   "#707070", "#ffffff")
 
diff --git a/src/Settings/App.SettingsGroup.json b/src/Settings/App.SettingsGroup.json
index a29d45927..0c0547075 100644
--- a/src/Settings/App.SettingsGroup.json
+++ b/src/Settings/App.SettingsGroup.json
@@ -162,7 +162,7 @@
     "type":             "uint32",
     "enumStrings":      "Indoor,Outdoor",
     "enumValues":       "1,0",
-    "default":     0
+    "default":     1
 },
 {
     "name":             "showLargeCompass",
diff --git a/src/Settings/Video.SettingsGroup.json b/src/Settings/Video.SettingsGroup.json
index ed91f0155..f8da07abd 100644
--- a/src/Settings/Video.SettingsGroup.json
+++ b/src/Settings/Video.SettingsGroup.json
@@ -23,7 +23,14 @@
     "shortDesc": "Video RTSP Url",
     "longDesc":  "RTSP url address and port to bind to for video stream. Example: rtsp://192.168.42.1:554/live",
     "type":             "string",
-    "default":     ""
+    "default":     "rtsp://192.168.199.120:554/stream0"
+},
+{
+    "name":             "rtspUrl2",
+    "shortDesc": "Video RTSP Url2",
+    "longDesc":  "RTSP url second address and port to bind to for video stream. Example: rtsp://192.168.42.1:554/live",
+    "type":             "string",
+    "default":     "rtsp://192.168.199.88:8554/main.264"
 },
 {
     "name":             "tcpUrl",
diff --git a/src/Settings/VideoSettings.cc b/src/Settings/VideoSettings.cc
index 6b282e052..964a84fc0 100644
--- a/src/Settings/VideoSettings.cc
+++ b/src/Settings/VideoSettings.cc
@@ -183,6 +183,15 @@ DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, rtspUrl)
     return _rtspUrlFact;
 }
 
+DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, rtspUrl2)
+{
+    if (!_rtspUrl2Fact) {
+        _rtspUrl2Fact = _createSettingsFact(rtspUrl2Name);
+        connect(_rtspUrl2Fact, &Fact::valueChanged, this, &VideoSettings::_configChanged);
+    }
+    return _rtspUrl2Fact;
+}
+
 DECLARE_SETTINGSFACT_NO_FUNC(VideoSettings, tcpUrl)
 {
     if (!_tcpUrlFact) {
diff --git a/src/Settings/VideoSettings.h b/src/Settings/VideoSettings.h
index 0f1b15cce..795640d84 100644
--- a/src/Settings/VideoSettings.h
+++ b/src/Settings/VideoSettings.h
@@ -24,6 +24,7 @@ public:
     DEFINE_SETTINGFACT(udpPort)
     DEFINE_SETTINGFACT(tcpUrl)
     DEFINE_SETTINGFACT(rtspUrl)
+    DEFINE_SETTINGFACT(rtspUrl2)
     DEFINE_SETTINGFACT(aspectRatio)
     DEFINE_SETTINGFACT(videoFit)
     DEFINE_SETTINGFACT(gridLines)
diff --git a/src/Vehicle/Vehicle.h b/src/Vehicle/Vehicle.h
index a8d12f974..7d9d909a8 100644
--- a/src/Vehicle/Vehicle.h
+++ b/src/Vehicle/Vehicle.h
@@ -1509,6 +1509,8 @@ private:
     // We use this to limit above terrain altitude queries based on distance and altitude change
     QGeoCoordinate              _altitudeAboveTerrLastCoord;
     float                       _altitudeAboveTerrLastRelAlt = qQNaN();
+
+    QTimer                      _genStatusTimer;
 };
 
 Q_DECLARE_METATYPE(Vehicle::MavCmdResultFailureCode_t)
diff --git a/src/Vehicle/VehicleGeneratorFactGroup.cc b/src/Vehicle/VehicleGeneratorFactGroup.cc
index 9cfbd7d93..d5a4b9de5 100644
--- a/src/Vehicle/VehicleGeneratorFactGroup.cc
+++ b/src/Vehicle/VehicleGeneratorFactGroup.cc
@@ -82,6 +82,8 @@ void VehicleGeneratorFactGroup::_handleGeneratorStatus(mavlink_message_t& messag
     genTemp()->setRawValue              (generator.generator_temperature == INT16_MAX ? qQNaN() : generator.generator_temperature);
     runtime()->setRawValue              (generator.runtime == UINT32_MAX ? qQNaN() : generator.runtime);
     timeMaintenance()->setRawValue      (generator.time_until_maintenance == INT32_MAX ? qQNaN() : generator.time_until_maintenance);
+
+    _msgReceived = 1;
 }
 
 void VehicleGeneratorFactGroup::_updateGeneratorFlags() {
@@ -107,4 +109,4 @@ void VehicleGeneratorFactGroup::_updateGeneratorFlags() {
         }
     }
     emit flagsListGeneratorChanged();
-}
\ No newline at end of file
+}
diff --git a/src/Vehicle/VehicleGeneratorFactGroup.h b/src/Vehicle/VehicleGeneratorFactGroup.h
index c27db7b40..639fb4d4f 100644
--- a/src/Vehicle/VehicleGeneratorFactGroup.h
+++ b/src/Vehicle/VehicleGeneratorFactGroup.h
@@ -51,6 +51,10 @@ public:
     static const char* _runtimeFactName;
     static const char* _timeMaintenanceFactName;
 
+    uint8_t _msgReceived;
+    uint8_t _timeoutCntStarted;
+    uint8_t _timeout;
+
 signals:
     void flagsListGeneratorChanged();
 
diff --git a/src/VehicleSetup/SetupView.qml b/src/VehicleSetup/SetupView.qml
index cd190b39c..187141ad3 100644
--- a/src/VehicleSetup/SetupView.qml
+++ b/src/VehicleSetup/SetupView.qml
@@ -46,12 +46,29 @@ Rectangle {
         _showSummaryPanel()
     }
 
+//    function _showSummaryPanel() {
+//        if (_fullParameterVehicleAvailable) {
+//            if (QGroundControl.multiVehicleManager.activeVehicle.autopilot.vehicleComponents.length === 0) {
+//                panelLoader.setSourceComponent(noComponentsVehicleSummaryComponent)
+//            } else {
+//                panelLoader.setSource("VehicleSummary.qml")
+//            }
+//        } else if (QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable) {
+//            panelLoader.setSourceComponent(missingParametersVehicleSummaryComponent)
+//        } else {
+//            panelLoader.setSourceComponent(disconnectedVehicleSummaryComponent)
+//        }
+//        summaryButton.checked = true
+//    }
+
+
     function _showSummaryPanel() {
         if (_fullParameterVehicleAvailable) {
             if (QGroundControl.multiVehicleManager.activeVehicle.autopilot.vehicleComponents.length === 0) {
                 panelLoader.setSourceComponent(noComponentsVehicleSummaryComponent)
             } else {
-                panelLoader.setSource("VehicleSummary.qml")
+                panelLoader.setSourceComponent(newEntryVehicleComponent)
+                //panelLoader.setSource("VehicleSummary.qml")
             }
         } else if (QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable) {
             panelLoader.setSourceComponent(missingParametersVehicleSummaryComponent)
@@ -136,6 +153,23 @@ Rectangle {
         }
     }
 
+    Component {
+        id: newEntryVehicleComponent
+        Rectangle{
+            color: qgcPal.windowShade
+            QGCLabel {
+                anchors.margins:        _defaultTextWidth * 2
+                anchors.fill:           parent
+                verticalAlignment:      Text.AlignVCenter
+                horizontalAlignment:    Text.AlignHCenter
+                wrapMode:               Text.WordWrap
+                font.pointSize:         ScreenTools.mediumFontPointSize
+                text:                   "Select a Component to Set Up with the Menu on the Left."
+                onLinkActivated: Qt.openUrlExternally(link)
+            }
+        }
+    }
+
     Component {
         id: disconnectedVehicleSummaryComponent
         Rectangle {
@@ -230,6 +264,7 @@ Rectangle {
                 exclusiveGroup:     setupButtonGroup
                 text:               qsTr("Summary")
                 Layout.fillWidth:   true
+                visible:            false
 
                 onClicked: showSummaryPanel()
             }
@@ -262,7 +297,7 @@ Rectangle {
                 setupIndicator:     true
                 setupComplete:      _activeJoystick ? _activeJoystick.calibrated || _buttonsOnly : false
                 exclusiveGroup:     setupButtonGroup
-                visible:            _fullParameterVehicleAvailable && joystickManager.joysticks.length !== 0
+                visible:            false//_fullParameterVehicleAvailable && joystickManager.joysticks.length !== 0
                 text:               _forcedToButtonsOnly ? qsTr("Buttons") : qsTr("Joystick")
                 Layout.fillWidth:   true
                 onClicked:          showPanel(this, "JoystickConfig.qml")
@@ -282,7 +317,7 @@ Rectangle {
                     setupComplete:      modelData.setupComplete
                     exclusiveGroup:     setupButtonGroup
                     text:               modelData.name
-                    visible:            modelData.setupSource.toString() !== ""
+                    visible:            modelData.name !== "Motors" && modelData.name !== "Tuning" && modelData.name !== "Remote Support" && modelData.name !== "Frame" && modelData.name !== "Camera"
                     Layout.fillWidth:   true
                     onClicked:          showVehicleComponentPanel(modelData)
                 }
diff --git a/src/VehicleSetup/VehicleSummary.qml b/src/VehicleSetup/VehicleSummary.qml
index 9456c09cb..fa69c9021 100644
--- a/src/VehicleSetup/VehicleSummary.qml
+++ b/src/VehicleSetup/VehicleSummary.qml
@@ -103,7 +103,7 @@ Rectangle {
                         width:      _summaryBoxWidth
                         height:     ScreenTools.defaultFontPixelHeight * 13
                         color:      qgcPal.windowShade
-                        visible:    modelData.summaryQmlSource.toString() !== ""
+                        visible:    modelData.name !== "Motors" && modelData.name !== "Tuning" && modelData.name !== "Remote Support" && modelData.name !== "Frame" && modelData.name !== "Camera"
                         border.width: 1
                         border.color: qgcPal.text
                         Component.onCompleted: {
@@ -131,12 +131,12 @@ Rectangle {
                                 visible:                modelData.requiresSetup && modelData.setupSource !== ""
                             }
 
-                            onClicked : {
-                                //console.log(modelData.setupSource)
-                                if (modelData.setupSource !== "") {
-                                    setupView.showVehicleComponentPanel(modelData)
-                                }
-                            }
+//                            onClicked : {
+//                                //console.log(modelData.setupSource)
+//                                if (modelData.setupSource !== "") {
+//                                    setupView.showVehicleComponentPanel(modelData)
+//                                }
+//                            }
                         }
                         // Summary Qml
                         Rectangle {
diff --git a/src/VideoManager/VideoManager.cc b/src/VideoManager/VideoManager.cc
index b37cd1162..dca43187c 100644
--- a/src/VideoManager/VideoManager.cc
+++ b/src/VideoManager/VideoManager.cc
@@ -82,6 +82,7 @@ VideoManager::~VideoManager()
         }
 #endif
     }
+    //_currentStream = 0;
 }
 
 //-----------------------------------------------------------------------------
@@ -736,7 +737,18 @@ VideoManager::_updateSettings(unsigned id)
     else if (source == VideoSettings::videoSourceMPEGTS)
         settingsChanged |= _updateVideoUri(0, QStringLiteral("mpegts://0.0.0.0:%1").arg(_videoSettings->udpPort()->rawValue().toInt()));
     else if (source == VideoSettings::videoSourceRTSP)
-        settingsChanged |= _updateVideoUri(0, _videoSettings->rtspUrl()->rawValue().toString());
+    {
+        //qDebug() << "Settings Stream: " << _currentStream << ":" << _videoSettings->rtspUrl()->rawValue().toString();
+        // check for flag
+        if (_currentStream == 1)
+        {
+            settingsChanged |= _updateVideoUri(0, _videoSettings->rtspUrl()->rawValue().toString());
+        }
+        else if (_currentStream == 2)
+        {
+            settingsChanged |= _updateVideoUri(0, _videoSettings->rtspUrl2()->rawValue().toString());
+        }
+    }
     else if (source == VideoSettings::videoSourceTCP)
         settingsChanged |= _updateVideoUri(0, QStringLiteral("tcp://%1").arg(_videoSettings->tcpUrl()->rawValue().toString()));
     else if (source == VideoSettings::videoSource3DRSolo)
@@ -795,6 +807,7 @@ VideoManager::_updateVideoUri(unsigned id, const QString& uri)
 void
 VideoManager::_restartVideo(unsigned id)
 {
+    //qDebug() << "restarting video: " << _videoUri[id];
 #if !defined(QGC_GST_STREAMING)
     Q_UNUSED(id);
 #endif
@@ -920,3 +933,54 @@ VideoManager::_aspectRatioChanged()
 {
     emit aspectRatioChanged();
 }
+
+void
+VideoManager::switchRTSPStream()
+{
+
+    //VideoSettings* videoSettings = qgcApp()->toolbox()->settingsManager()->videoSettings();
+
+//    static bool useStream1 = true;
+//    useStream1 = !useStream1;
+
+    if(_currentStream == 1)
+    {
+        _currentStream = 2;
+    }
+    else
+    {
+        _currentStream = 1;
+    }
+
+    //_currentStream = useStream1 ? videoSettings->rtspUrl()->rawValue().toString() : videoSettings->rtspUrl2()->rawValue().toString();
+
+    // switch rtspUrl and rtspUrl2
+
+    //QString tempStream;
+    //tempStream = videoSettings->rtspUrl()->rawValue().toString();
+    //videoSettings->rtspUrl()->setRawValue(videoSettings->rtspUrl2()->rawValue().toString());
+    //videoSettings->rtspUrl2()->setRawValue(tempStream);
+
+    //qDebug() << "rtspUrl:" << videoSettings->rtspUrl()->rawValue().toString();
+    //qDebug() << "rtspUrl2:" << videoSettings->rtspUrl2()->rawValue().toString();
+
+    //qDebug() << "Updating Stream: " << _currentStream;
+    //_updateSettings(0);
+
+    emit streamChanged();
+
+    //_updateSettings
+
+    _restartAllVideos();
+
+//    if (_currentStreamIndex == 1)
+//    {
+//        _currentStreamIndex = 0;
+//    }
+//    else
+//    {
+//        _currentStreamIndex = 1;
+//    }
+
+    // call function to update settings?
+}
diff --git a/src/VideoManager/VideoManager.h b/src/VideoManager/VideoManager.h
index 91cc6486c..a59730b96 100644
--- a/src/VideoManager/VideoManager.h
+++ b/src/VideoManager/VideoManager.h
@@ -56,6 +56,7 @@ public:
     Q_PROPERTY(bool             decoding                READ    decoding                                    NOTIFY decodingChanged)
     Q_PROPERTY(bool             recording               READ    recording                                   NOTIFY recordingChanged)
     Q_PROPERTY(QSize            videoSize               READ    videoSize                                   NOTIFY videoSizeChanged)
+    Q_PROPERTY(QString          currentStream           READ    currentStream                               NOTIFY streamChanged)
 
     virtual bool        hasVideo            ();
     virtual bool        isGStreamer         ();
@@ -71,6 +72,18 @@ public:
     virtual bool        hasThermal          ();
     virtual QString     imageFile           ();
 
+    QString currentStream(){
+        //qDebug() << "Stream" << _currentStream;
+        if(_currentStream == 1)
+        {
+            return "1";
+        }
+        else
+        {
+            return "2";
+        }
+    }
+
     bool streaming(void) {
         return _streaming;
     }
@@ -113,6 +126,8 @@ public:
 
     Q_INVOKABLE void grabImage(const QString& imageFile = QString());
 
+    Q_INVOKABLE void switchRTSPStream();
+
 signals:
     void hasVideoChanged            ();
     void isGStreamerChanged         ();
@@ -129,6 +144,7 @@ signals:
     void recordingChanged           ();
     void recordingStarted           ();
     void videoSizeChanged           ();
+    void streamChanged              ();
 
 protected slots:
     void _videoSourceChanged        ();
@@ -152,6 +168,7 @@ protected:
     void _restartVideo              (unsigned id);
     void _startReceiver             (unsigned id);
     void _stopReceiver              (unsigned id);
+   // void _switchRTSPStream          ();
 
 protected:
     QString                 _videoFile;
@@ -178,6 +195,8 @@ protected:
     Vehicle*                _activeVehicle          = nullptr;
     QString                 _forwardHost;
     bool                    _forwardVideo;
+private:
+    uint8_t _currentStream = 1; // 1 or 2
 };
 
 #endif
diff --git a/src/api/QGCCorePlugin.cc b/src/api/QGCCorePlugin.cc
index 318c7a9a8..a4d652065 100644
--- a/src/api/QGCCorePlugin.cc
+++ b/src/api/QGCCorePlugin.cc
@@ -163,7 +163,7 @@ QVariantList &QGCCorePlugin::settingsPages()
         _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pConsole)));
         _p->pHelp = new QmlComponentInfo(tr("Help"),
                                          QUrl::fromUserInput("qrc:/qml/HelpSettings.qml"));
-        _p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pHelp)));
+        //_p->settingsList.append(QVariant::fromValue(reinterpret_cast<QmlComponentInfo*>(_p->pHelp)));
 #if defined(QT_DEBUG)
         //-- These are always present on Debug builds
         _p->pMockLink = new QmlComponentInfo(tr("Mock Link"),
@@ -183,15 +183,15 @@ QVariantList &QGCCorePlugin::settingsPages()
 QVariantList& QGCCorePlugin::analyzePages()
 {
     if (!_p->analyzeList.count()) {
-        _p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("Log Download"),     QUrl::fromUserInput("qrc:/qml/LogDownloadPage.qml"),        QUrl::fromUserInput("qrc:/qmlimages/LogDownloadIcon"))));
+        //_p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("Log Download"),     QUrl::fromUserInput("qrc:/qml/LogDownloadPage.qml"),        QUrl::fromUserInput("qrc:/qmlimages/LogDownloadIcon"))));
 #if !defined(__mobile__)
-        _p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("GeoTag Images"),    QUrl::fromUserInput("qrc:/qml/GeoTagPage.qml"),             QUrl::fromUserInput("qrc:/qmlimages/GeoTagIcon"))));
+        //_p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("GeoTag Images"),    QUrl::fromUserInput("qrc:/qml/GeoTagPage.qml"),             QUrl::fromUserInput("qrc:/qmlimages/GeoTagIcon"))));
 #endif
-        _p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("MAVLink Console"),  QUrl::fromUserInput("qrc:/qml/MavlinkConsolePage.qml"),     QUrl::fromUserInput("qrc:/qmlimages/MavlinkConsoleIcon"))));
+       //_p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("MAVLink Console"),  QUrl::fromUserInput("qrc:/qml/MavlinkConsolePage.qml"),     QUrl::fromUserInput("qrc:/qmlimages/MavlinkConsoleIcon"))));
 #if !defined(QGC_DISABLE_MAVLINK_INSPECTOR)
         _p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("MAVLink Inspector"),QUrl::fromUserInput("qrc:/qml/MAVLinkInspectorPage.qml"),   QUrl::fromUserInput("qrc:/qmlimages/MAVLinkInspector"))));
 #endif
-        _p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("Vibration"),        QUrl::fromUserInput("qrc:/qml/VibrationPage.qml"),          QUrl::fromUserInput("qrc:/qmlimages/VibrationPageIcon"))));
+        //_p->analyzeList.append(QVariant::fromValue(new QmlComponentInfo(tr("Vibration"),        QUrl::fromUserInput("qrc:/qml/VibrationPage.qml"),          QUrl::fromUserInput("qrc:/qmlimages/VibrationPageIcon"))));
     }
     return _p->analyzeList;
 }
@@ -235,7 +235,7 @@ bool QGCCorePlugin::adjustSettingMetaData(const QString& settingsGroup, FactMeta
 #if defined (__mobile__)
             outdoorPalette = 0;
 #else
-            outdoorPalette = 1;
+            outdoorPalette = 0;
 #endif
             metaData.setRawDefaultValue(outdoorPalette);
             return true;
@@ -310,60 +310,55 @@ void QGCCorePlugin::factValueGridCreateDefaultSettings(const QString& defaultSet
 
     InstrumentValueData* value = column->value<InstrumentValueData*>(rowIndex++);
     value->setFact("Vehicle", "AltitudeRelative");
-    value->setIcon("arrow-thick-up.svg");
-    value->setText(value->fact()->shortDescription());
+    value->setText("Alt (rel)");
     value->setShowUnits(true);
 
     value = column->value<InstrumentValueData*>(rowIndex++);
     value->setFact("Vehicle", "DistanceToHome");
-    value->setIcon("bookmark copy 3.svg");
-    value->setText(value->fact()->shortDescription());
+    value->setText("Home Dist");
     value->setShowUnits(true);
 
     rowIndex    = 0;
     column      = factValueGrid.columns()->value<QmlObjectListModel*>(1);
 
     value = column->value<InstrumentValueData*>(rowIndex++);
-    value->setFact("Vehicle", "ClimbRate");
-    value->setIcon("arrow-simple-up.svg");
-    value->setText(value->fact()->shortDescription());
+    value->setFact("Vehicle", "FlightTime");
+    value->setText("Flight Time");
     value->setShowUnits(true);
 
     value = column->value<InstrumentValueData*>(rowIndex++);
     value->setFact("Vehicle", "GroundSpeed");
-    value->setIcon("arrow-simple-right.svg");
-    value->setText(value->fact()->shortDescription());
+    value->setText("Speed");
     value->setShowUnits(true);
 
 
-    if (includeFWValues) {
-        rowIndex    = 0;
-        column      = factValueGrid.columns()->value<QmlObjectListModel*>(2);
-
-        value = column->value<InstrumentValueData*>(rowIndex++);
-        value->setFact("Vehicle", "AirSpeed");
-        value->setText("AirSpd");
-        value->setShowUnits(true);
-
-        value = column->value<InstrumentValueData*>(rowIndex++);
-        value->setFact("Vehicle", "ThrottlePct");
-        value->setText("Thr");
-        value->setShowUnits(true);
-    }
+//    if (includeFWValues) {
+//        rowIndex    = 0;
+//        column      = factValueGrid.columns()->value<QmlObjectListModel*>(2);
+
+//        value = column->value<InstrumentValueData*>(rowIndex++);
+//        value->setFact("Vehicle", "AirSpeed");
+//        value->setText("AirSpd");
+//        value->setShowUnits(true);
+
+//        value = column->value<InstrumentValueData*>(rowIndex++);
+//        value->setFact("Vehicle", "ThrottlePct");
+//        value->setText("Thr");
+//        value->setShowUnits(true);
+//    }
 
     rowIndex    = 0;
-    column      = factValueGrid.columns()->value<QmlObjectListModel*>(includeFWValues ? 3 : 2);
+    //column      = factValueGrid.columns()->value<QmlObjectListModel*>(includeFWValues ? 3 : 2);
+    column      = factValueGrid.columns()->value<QmlObjectListModel*>(2);
 
     value = column->value<InstrumentValueData*>(rowIndex++);
-    value->setFact("Vehicle", "FlightTime");
-    value->setIcon("timer.svg");
-    value->setText(value->fact()->shortDescription());
-    value->setShowUnits(false);
+    value->setFact("Battery0", "Voltage");
+    value->setText("Voltage");
+    value->setShowUnits(true);
 
     value = column->value<InstrumentValueData*>(rowIndex++);
-    value->setFact("Vehicle", "FlightDistance");
-    value->setIcon("travel-walk.svg");
-    value->setText(value->fact()->shortDescription());
+    value->setFact("Battery0", "Current");
+    value->setText("Current");
     value->setShowUnits(true);
 }
 
diff --git a/src/api/QGCOptions.h b/src/api/QGCOptions.h
index 7b5926d55..b84980ca2 100644
--- a/src/api/QGCOptions.h
+++ b/src/api/QGCOptions.h
@@ -30,7 +30,7 @@ public:
     Q_PROPERTY(bool                     guidedBarShowROI                READ guidedBarShowROI               NOTIFY guidedBarShowROIChanged)
 
 protected:
-    virtual bool    showMultiVehicleList        () const { return true; }
+    virtual bool    showMultiVehicleList        () const { return false; }
     virtual bool    showMapScale                () const { return true; }
     virtual bool    showInstrumentPanel         () const { return true; }
     virtual bool    guidedBarShowEmergencyStop  () const { return true; }
@@ -109,22 +109,22 @@ public:
     virtual QColor  toolbarBackgroundDark           () const;
     /// By returning false you can hide the following sensor calibration pages
     virtual bool    showSensorCalibrationCompass    () const { return true; }
-    virtual bool    showSensorCalibrationGyro       () const { return true; }
+    virtual bool    showSensorCalibrationGyro       () const { return false; }
     virtual bool    showSensorCalibrationAccel      () const { return true; }
     virtual bool    showSensorCalibrationLevel      () const { return true; }
-    virtual bool    showSensorCalibrationAirspeed   () const { return true; }
+    virtual bool    showSensorCalibrationAirspeed   () const { return false; }
     virtual bool    wifiReliableForCalibration      () const { return false; }
     virtual bool    sensorsHaveFixedOrientation     () const { return false; }
-    virtual bool    showFirmwareUpgrade             () const { return true; }
+    virtual bool    showFirmwareUpgrade             () const { return false; }
     virtual bool    missionWaypointsOnly            () const { return false; }  ///< true: Only allow waypoints and complex items in Plan
-    virtual bool    multiVehicleEnabled             () const { return true; }   ///< false: multi vehicle support is disabled
+    virtual bool    multiVehicleEnabled             () const { return false; }   ///< false: multi vehicle support is disabled
     virtual bool    guidedActionsRequireRCRSSI      () const { return false; }  ///< true: Guided actions will be disabled is there is no RC RSSI
     virtual bool    showOfflineMapExport            () const { return true; }
     virtual bool    showOfflineMapImport            () const { return true; }
     virtual bool    showMissionAbsoluteAltitude     () const { return true; }
     virtual bool    showSimpleMissionStart          () const { return false; }
     virtual bool    disableVehicleConnection        () const { return false; }  ///< true: vehicle connection is disabled
-    virtual bool    checkFirmwareVersion            () const { return true; }
+    virtual bool    checkFirmwareVersion            () const { return false; }
     virtual bool    showMavlinkLogOptions           () const { return true; }
     virtual bool    allowJoystickSelection          () const { return true; }   ///< false: custom build has automatically enabled a specific joystick
     /// Desktop builds save the main application size and position on close (and restore it on open)
diff --git a/src/ui/AppSettings.qml b/src/ui/AppSettings.qml
index ded41216b..d0ad19d62 100644
--- a/src/ui/AppSettings.qml
+++ b/src/ui/AppSettings.qml
@@ -69,7 +69,7 @@ Rectangle {
                     text:               modelData.title
                     autoExclusive:      true
                     Layout.fillWidth:   true
-                    visible:            modelData.url != "qrc:/qml/RemoteIDSettings.qml" ? true : QGroundControl.settingsManager.remoteIDSettings.enable.rawValue
+                    visible:            modelData.url !== "qrc:/qml/HelpSettings.qml"// modelData.url !== "qrc:/qml/RemoteIDSettings.qml" ? true : QGroundControl.settingsManager.remoteIDSettings.enable.rawValue
 
                     onClicked: {
                         if (mainWindow.preventViewSwitch()) {
@@ -92,15 +92,36 @@ Rectangle {
                         if (_commingFromRIDSettings) {
                             checked = false
                             _commingFromRIDSettings = false
-                            if (modelData.url == "qrc:/qml/RemoteIDSettings.qml") {
+                            if (modelData.url === "qrc:/qml/RemoteIDSettings.qml") {
                                 checked = true
                             }
                         }
                     }
                 }
-            }
-        }
-    }
+            } // repeater
+            Repeater {
+                id:     buttonRepeater
+                model:  QGroundControl.corePlugin ? QGroundControl.corePlugin.analyzePages : []
+
+                QGCButton {
+                    height:             _buttonHeight
+                    text:               modelData.title
+                    autoExclusive:      true
+                    Layout.fillWidth:   true
+
+                    onClicked: {
+                        if (mainWindow.preventViewSwitch()) {
+                            return
+                        }
+                        if (__rightPanel.source !== modelData.url) {
+                            __rightPanel.source = modelData.url
+                        }
+                        checked = true
+                    } // on clicked
+                } // QGCButton
+            } // second repeater
+        } //  column layout
+    } // qgc flickable
 
     Rectangle {
         id:                     divider
diff --git a/src/ui/MainRootWindow.qml b/src/ui/MainRootWindow.qml
index 65255c91a..6ad172c90 100644
--- a/src/ui/MainRootWindow.qml
+++ b/src/ui/MainRootWindow.qml
@@ -312,21 +312,21 @@ ApplicationWindow {
                         }
                     }
 
-                    SubMenuButton {
-                        id:                 analyzeButton
-                        height:             toolSelectDialog._toolButtonHeight
-                        Layout.fillWidth:   true
-                        text:               qsTr("Analyze Tools")
-                        imageResource:      "/qmlimages/Analyze.svg"
-                        imageColor:         qgcPal.text
-                        visible:            QGroundControl.corePlugin.showAdvancedUI
-                        onClicked: {
-                            if (!mainWindow.preventViewSwitch()) {
-                                toolSelectDialog.close()
-                                mainWindow.showAnalyzeTool()
-                            }
-                        }
-                    }
+//                    SubMenuButton {
+//                        id:                 analyzeButton
+//                        height:             toolSelectDialog._toolButtonHeight
+//                        Layout.fillWidth:   true
+//                        text:               qsTr("Analyze Tools")
+//                        imageResource:      "/qmlimages/Analyze.svg"
+//                        imageColor:         qgcPal.text
+//                        visible:            QGroundControl.corePlugin.showAdvancedUI
+//                        onClicked: {
+//                            if (!mainWindow.preventViewSwitch()) {
+//                                toolSelectDialog.close()
+//                                mainWindow.showAnalyzeTool()
+//                            }
+//                        }
+//                    }
 
                     SubMenuButton {
                         id:                 settingsButton
@@ -344,82 +344,82 @@ ApplicationWindow {
                         }
                     }
 
-                    ColumnLayout {
-                        width:                  innerLayout.width
-                        spacing:                0
-                        Layout.alignment:       Qt.AlignHCenter
-
-                        QGCLabel {
-                            id:                     versionLabel
-                            text:                   qsTr("%1 Version").arg(QGroundControl.appName)
-                            font.pointSize:         ScreenTools.smallFontPointSize
-                            wrapMode:               QGCLabel.WordWrap
-                            Layout.maximumWidth:    parent.width
-                            Layout.alignment:       Qt.AlignHCenter
-                        }
-
-                        QGCLabel {
-                            text:                   QGroundControl.qgcVersion
-                            font.pointSize:         ScreenTools.smallFontPointSize
-                            wrapMode:               QGCLabel.WrapAnywhere
-                            Layout.maximumWidth:    parent.width
-                            Layout.alignment:       Qt.AlignHCenter
-
-                            QGCMouseArea {
-                                id:                 easterEggMouseArea
-                                anchors.topMargin:  -versionLabel.height
-                                anchors.fill:       parent
-
-                                onClicked: {
-                                    if (mouse.modifiers & Qt.ControlModifier) {
-                                        QGroundControl.corePlugin.showTouchAreas = !QGroundControl.corePlugin.showTouchAreas
-                                        showTouchAreasNotification.open()
-                                    } else if (ScreenTools.isMobile || mouse.modifiers & Qt.ShiftModifier) {
-                                        if(!QGroundControl.corePlugin.showAdvancedUI) {
-                                            advancedModeOnConfirmation.open()
-                                        } else {
-                                            advancedModeOffConfirmation.open()
-                                        }
-                                    }
-                                }
-
-                                // This allows you to change this on mobile
-                                onPressAndHold: {
-                                    QGroundControl.corePlugin.showTouchAreas = !QGroundControl.corePlugin.showTouchAreas
-                                    showTouchAreasNotification.open()
-                                }
-
-                                MessageDialog {
-                                    id:                 showTouchAreasNotification
-                                    title:              qsTr("Debug Touch Areas")
-                                    text:               qsTr("Touch Area display toggled")
-                                    standardButtons:    StandardButton.Ok
-                                }
-
-                                MessageDialog {
-                                    id:                 advancedModeOnConfirmation
-                                    title:              qsTr("Advanced Mode")
-                                    text:               QGroundControl.corePlugin.showAdvancedUIMessage
-                                    standardButtons:    StandardButton.Yes | StandardButton.No
-                                    onYes: {
-                                        QGroundControl.corePlugin.showAdvancedUI = true
-                                        advancedModeOnConfirmation.close()
-                                    }
-                                }
-
-                                MessageDialog {
-                                    id:                 advancedModeOffConfirmation
-                                    title:              qsTr("Advanced Mode")
-                                    text:               qsTr("Turn off Advanced Mode?")
-                                    standardButtons:    StandardButton.Yes | StandardButton.No
-                                    onYes: {
-                                        QGroundControl.corePlugin.showAdvancedUI = false
-                                        advancedModeOffConfirmation.close()
-                                    }
-                                }
-                            }
-                        }
-                    }
+//                    ColumnLayout {
+//                        width:                  innerLayout.width
+//                        spacing:                0
+//                        Layout.alignment:       Qt.AlignHCenter
+
+//                        QGCLabel {
+//                            id:                     versionLabel
+//                            text:                   qsTr("%1 Version").arg(QGroundControl.appName)
+//                            font.pointSize:         ScreenTools.smallFontPointSize
+//                            wrapMode:               QGCLabel.WordWrap
+//                            Layout.maximumWidth:    parent.width
+//                            Layout.alignment:       Qt.AlignHCenter
+//                        }
+
+//                        QGCLabel {
+//                            text:                   QGroundControl.qgcVersion
+//                            font.pointSize:         ScreenTools.smallFontPointSize
+//                            wrapMode:               QGCLabel.WrapAnywhere
+//                            Layout.maximumWidth:    parent.width
+//                            Layout.alignment:       Qt.AlignHCenter
+
+//                            QGCMouseArea {
+//                                id:                 easterEggMouseArea
+//                                anchors.topMargin:  -versionLabel.height
+//                                anchors.fill:       parent
+
+//                                onClicked: {
+//                                    if (mouse.modifiers & Qt.ControlModifier) {
+//                                        QGroundControl.corePlugin.showTouchAreas = !QGroundControl.corePlugin.showTouchAreas
+//                                        showTouchAreasNotification.open()
+//                                    } else if (ScreenTools.isMobile || mouse.modifiers & Qt.ShiftModifier) {
+//                                        if(!QGroundControl.corePlugin.showAdvancedUI) {
+//                                            advancedModeOnConfirmation.open()
+//                                        } else {
+//                                            advancedModeOffConfirmation.open()
+//                                        }
+//                                    }
+//                                }
+
+//                                // This allows you to change this on mobile
+//                                onPressAndHold: {
+//                                    QGroundControl.corePlugin.showTouchAreas = !QGroundControl.corePlugin.showTouchAreas
+//                                    showTouchAreasNotification.open()
+//                                }
+
+//                                MessageDialog {
+//                                    id:                 showTouchAreasNotification
+//                                    title:              qsTr("Debug Touch Areas")
+//                                    text:               qsTr("Touch Area display toggled")
+//                                    standardButtons:    StandardButton.Ok
+//                                }
+
+//                                MessageDialog {
+//                                    id:                 advancedModeOnConfirmation
+//                                    title:              qsTr("Advanced Mode")
+//                                    text:               QGroundControl.corePlugin.showAdvancedUIMessage
+//                                    standardButtons:    StandardButton.Yes | StandardButton.No
+//                                    onYes: {
+//                                        QGroundControl.corePlugin.showAdvancedUI = true
+//                                        advancedModeOnConfirmation.close()
+//                                    }
+//                                }
+
+//                                MessageDialog {
+//                                    id:                 advancedModeOffConfirmation
+//                                    title:              qsTr("Advanced Mode")
+//                                    text:               qsTr("Turn off Advanced Mode?")
+//                                    standardButtons:    StandardButton.Yes | StandardButton.No
+//                                    onYes: {
+//                                        QGroundControl.corePlugin.showAdvancedUI = false
+//                                        advancedModeOffConfirmation.close()
+//                                    }
+//                                }
+//                            }
+//                        }
+//                    }
                 }
             }
         }
diff --git a/src/ui/preferences/GeneralSettings.qml b/src/ui/preferences/GeneralSettings.qml
index 2cfc8f69f..7f2d86281 100644
--- a/src/ui/preferences/GeneralSettings.qml
+++ b/src/ui/preferences/GeneralSettings.qml
@@ -98,202 +98,202 @@ Rectangle {
                             anchors.horizontalCenter:   parent.horizontalCenter
                             spacing:                    _margins
 
-                            FactCheckBox {
-                                id:             useCheckList
-                                text:           qsTr("Use Preflight Checklist")
-                                fact:           _useChecklist
-                                visible:        _useChecklist.visible && QGroundControl.corePlugin.options.preFlightChecklistUrl.toString().length
-
-                                property Fact _useChecklist: QGroundControl.settingsManager.appSettings.useChecklist
-                            }
-
-                            FactCheckBox {
-                                text:           qsTr("Enforce Preflight Checklist")
-                                fact:           _enforceChecklist
-                                enabled:        QGroundControl.settingsManager.appSettings.useChecklist.value
-                                visible:        useCheckList.visible && _enforceChecklist.visible && QGroundControl.corePlugin.options.preFlightChecklistUrl.toString().length
-
-                                property Fact _enforceChecklist: QGroundControl.settingsManager.appSettings.enforceChecklist
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Keep Map Centered On Vehicle")
-                                fact:       _keepMapCenteredOnVehicle
-                                visible:    _keepMapCenteredOnVehicle.visible
-
-                                property Fact _keepMapCenteredOnVehicle: QGroundControl.settingsManager.flyViewSettings.keepMapCenteredOnVehicle
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Show Telemetry Log Replay Status Bar")
-                                fact:       _showLogReplayStatusBar
-                                visible:    _showLogReplayStatusBar.visible
-
-                                property Fact _showLogReplayStatusBar: QGroundControl.settingsManager.flyViewSettings.showLogReplayStatusBar
-                            }
-
-                            RowLayout {
-                                spacing: ScreenTools.defaultFontPixelWidth
-
-                                FactCheckBox {
-                                    text:       qsTr("Virtual Joystick")
-                                    visible:    _virtualJoystick.visible
-                                    fact:       _virtualJoystick
-                                }
-
-                                FactCheckBox {
-                                    text:       qsTr("Auto-Center Throttle")
-                                    visible:    _virtualJoystickAutoCenterThrottle.visible
-                                    enabled:    _virtualJoystick.rawValue
-                                    fact:       _virtualJoystickAutoCenterThrottle
-                                }
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Use Vertical Instrument Panel")
-                                visible:    _alternateInstrumentPanel.visible
-                                fact:       _alternateInstrumentPanel
-
-                                property Fact _alternateInstrumentPanel: QGroundControl.settingsManager.flyViewSettings.alternateInstrumentPanel
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Show additional heading indicators on Compass")
-                                visible:    _showAdditionalIndicatorsCompass.visible
-                                fact:       _showAdditionalIndicatorsCompass
-
-                                property Fact _showAdditionalIndicatorsCompass: QGroundControl.settingsManager.flyViewSettings.showAdditionalIndicatorsCompass
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Lock Compass Nose-Up")
-                                visible:    _lockNoseUpCompass.visible
-                                fact:       _lockNoseUpCompass
-
-                                property Fact _lockNoseUpCompass: QGroundControl.settingsManager.flyViewSettings.lockNoseUpCompass
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Show simple camera controls (DIGICAM_CONTROL)")
-                                visible:    _showDumbCameraControl.visible
-                                fact:       _showDumbCameraControl
-
-                                property Fact _showDumbCameraControl: QGroundControl.settingsManager.flyViewSettings.showSimpleCameraControl
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Update home position based on device location. This will affect return to home")
-                                fact:       _updateHomePosition
-                                visible:    _updateHomePosition.visible
-                                property Fact _updateHomePosition: QGroundControl.settingsManager.flyViewSettings.updateHomePosition
-                            }
-
-                            FactCheckBox {
-                                text:       qsTr("Enable Custom Actions")
-                                visible:    _enableCustomActions.visible
-                                fact:       _enableCustomActions
-
-                                property Fact _enableCustomActions: QGroundControl.settingsManager.flyViewSettings.enableCustomActions
-                            }
+//                            FactCheckBox {
+//                                id:             useCheckList
+//                                text:           qsTr("Use Preflight Checklist")
+//                                fact:           _useChecklist
+//                                visible:        _useChecklist.visible && QGroundControl.corePlugin.options.preFlightChecklistUrl.toString().length
+
+//                                property Fact _useChecklist: QGroundControl.settingsManager.appSettings.useChecklist
+//                            }
+
+//                            FactCheckBox {
+//                                text:           qsTr("Enforce Preflight Checklist")
+//                                fact:           _enforceChecklist
+//                                enabled:        QGroundControl.settingsManager.appSettings.useChecklist.value
+//                                visible:        useCheckList.visible && _enforceChecklist.visible && QGroundControl.corePlugin.options.preFlightChecklistUrl.toString().length
+
+//                                property Fact _enforceChecklist: QGroundControl.settingsManager.appSettings.enforceChecklist
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Keep Map Centered On Vehicle")
+//                                fact:       _keepMapCenteredOnVehicle
+//                                visible:    _keepMapCenteredOnVehicle.visible
+
+//                                property Fact _keepMapCenteredOnVehicle: QGroundControl.settingsManager.flyViewSettings.keepMapCenteredOnVehicle
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Show Telemetry Log Replay Status Bar")
+//                                fact:       _showLogReplayStatusBar
+//                                visible:    _showLogReplayStatusBar.visible
+
+//                                property Fact _showLogReplayStatusBar: QGroundControl.settingsManager.flyViewSettings.showLogReplayStatusBar
+//                            }
+
+//                            RowLayout {
+//                                spacing: ScreenTools.defaultFontPixelWidth
+
+//                                FactCheckBox {
+//                                    text:       qsTr("Virtual Joystick")
+//                                    visible:    _virtualJoystick.visible
+//                                    fact:       _virtualJoystick
+//                                }
+
+//                                FactCheckBox {
+//                                    text:       qsTr("Auto-Center Throttle")
+//                                    visible:    _virtualJoystickAutoCenterThrottle.visible
+//                                    enabled:    _virtualJoystick.rawValue
+//                                    fact:       _virtualJoystickAutoCenterThrottle
+//                                }
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Use Vertical Instrument Panel")
+//                                visible:    _alternateInstrumentPanel.visible
+//                                fact:       _alternateInstrumentPanel
+
+//                                property Fact _alternateInstrumentPanel: QGroundControl.settingsManager.flyViewSettings.alternateInstrumentPanel
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Show additional heading indicators on Compass")
+//                                visible:    _showAdditionalIndicatorsCompass.visible
+//                                fact:       _showAdditionalIndicatorsCompass
+
+//                                property Fact _showAdditionalIndicatorsCompass: QGroundControl.settingsManager.flyViewSettings.showAdditionalIndicatorsCompass
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Lock Compass Nose-Up")
+//                                visible:    _lockNoseUpCompass.visible
+//                                fact:       _lockNoseUpCompass
+
+//                                property Fact _lockNoseUpCompass: QGroundControl.settingsManager.flyViewSettings.lockNoseUpCompass
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Show simple camera controls (DIGICAM_CONTROL)")
+//                                visible:    _showDumbCameraControl.visible
+//                                fact:       _showDumbCameraControl
+
+//                                property Fact _showDumbCameraControl: QGroundControl.settingsManager.flyViewSettings.showSimpleCameraControl
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Update home position based on device location. This will affect return to home")
+//                                fact:       _updateHomePosition
+//                                visible:    _updateHomePosition.visible
+//                                property Fact _updateHomePosition: QGroundControl.settingsManager.flyViewSettings.updateHomePosition
+//                            }
+
+//                            FactCheckBox {
+//                                text:       qsTr("Enable Custom Actions")
+//                                visible:    _enableCustomActions.visible
+//                                fact:       _enableCustomActions
+
+//                                property Fact _enableCustomActions: QGroundControl.settingsManager.flyViewSettings.enableCustomActions
+//                            }
 
                             //-----------------------------------------------------------------
                             //-- CustomAction definition path
-                            GridLayout {
-                                id: customActions
-
-                                columns:  2
-                                visible:  QGroundControl.settingsManager.flyViewSettings.enableCustomActions.rawValue
-
-                                onVisibleChanged: {
-                                    if (jsonFile.rawValue === "" && ScreenTools.isMobile) {
-                                        jsonFile.rawValue = _defaultFile
-                                    }
-                                }
-
-                                property Fact   jsonFile:     QGroundControl.settingsManager.flyViewSettings.customActionDefinitions
-                                property string _defaultDir:  QGroundControl.settingsManager.appSettings.customActionsSavePath
-                                property string _defaultFile: _defaultDir + "/CustomActions.json"
-
-                                QGCLabel {
-                                    text: qsTr("Custom Action Definitions")
-
-                                    Layout.columnSpan:  2
-                                    Layout.alignment:   Qt.AlignHCenter
-                                }
-
-                                QGCTextField {
-                                    Layout.fillWidth:   true
-                                    readOnly:           true
-                                    text:               customActions.jsonFile.rawValue === "" ? qsTr("<not set>") : customActions.jsonFile.rawValue
-                                }
-                                QGCButton {
-                                    visible:    !ScreenTools.isMobile
-                                    text:       qsTr("Browse")
-                                    onClicked:  customActionPathBrowseDialog.openForLoad()
-                                    QGCFileDialog {
-                                        id:             customActionPathBrowseDialog
-                                        title:          qsTr("Choose the Custom Action Definitions file")
-                                        folder:         customActions.jsonFile.rawValue
-                                        selectExisting: true
-                                        selectFolder:   false
-                                        onAcceptedForLoad: customActions.jsonFile.rawValue = file
-                                        nameFilters: ["JSON files (*.json)"]
-                                    }
-                                }
-                                // The file loader on Android doesn't work, so we hard code the path to the
-                                // JSON file. However, we need a button to force a refresh if the JSON file
-                                // is changed.
-                                QGCButton {
-                                    visible:    ScreenTools.isMobile
-                                    text:       qsTr("Reload")
-                                    onClicked:  {
-                                        customActions.jsonFile.valueChanged(customActions.jsonFile.rawValue)
-                                    }
-                                }
-                            }
-
-                            GridLayout {
-                                columns: 2
-
-                                QGCLabel {
-                                    text:               qsTr("Guided Command Settings")
-                                    Layout.columnSpan:  2
-                                    Layout.alignment:   Qt.AlignHCenter
-                                }
-
-                                QGCLabel {
-                                    text:       qsTr("Minimum Altitude")
-                                    visible:    guidedMinAltField.visible
-                                }
-                                FactTextField {
-                                    id:                     guidedMinAltField
-                                    Layout.preferredWidth:  _valueFieldWidth
-                                    visible:                fact.visible
-                                    fact:                   _flyViewSettings.guidedMinimumAltitude
-                                }
-
-                                QGCLabel {
-                                    text:       qsTr("Maximum Altitude")
-                                    visible:    guidedMaxAltField.visible
-                                }
-                                FactTextField {
-                                    id:                     guidedMaxAltField
-                                    Layout.preferredWidth:  _valueFieldWidth
-                                    visible:                fact.visible
-                                    fact:                   _flyViewSettings.guidedMaximumAltitude
-                                }
-
-                                QGCLabel {
-                                    text:       qsTr("Go To Location Max Distance")
-                                    visible:    maxGotoDistanceField.visible
-                                }
-                                FactTextField {
-                                    id:                     maxGotoDistanceField
-                                    Layout.preferredWidth:  _valueFieldWidth
-                                    visible:                fact.visible
-                                    fact:                  _flyViewSettings.maxGoToLocationDistance
-                                }
-                            }
+//                            GridLayout {
+//                                id: customActions
+
+//                                columns:  2
+//                                visible:  QGroundControl.settingsManager.flyViewSettings.enableCustomActions.rawValue
+
+//                                onVisibleChanged: {
+//                                    if (jsonFile.rawValue === "" && ScreenTools.isMobile) {
+//                                        jsonFile.rawValue = _defaultFile
+//                                    }
+//                                }
+
+//                                property Fact   jsonFile:     QGroundControl.settingsManager.flyViewSettings.customActionDefinitions
+//                                property string _defaultDir:  QGroundControl.settingsManager.appSettings.customActionsSavePath
+//                                property string _defaultFile: _defaultDir + "/CustomActions.json"
+
+//                                QGCLabel {
+//                                    text: qsTr("Custom Action Definitions")
+
+//                                    Layout.columnSpan:  2
+//                                    Layout.alignment:   Qt.AlignHCenter
+//                                }
+
+//                                QGCTextField {
+//                                    Layout.fillWidth:   true
+//                                    readOnly:           true
+//                                    text:               customActions.jsonFile.rawValue === "" ? qsTr("<not set>") : customActions.jsonFile.rawValue
+//                                }
+//                                QGCButton {
+//                                    visible:    !ScreenTools.isMobile
+//                                    text:       qsTr("Browse")
+//                                    onClicked:  customActionPathBrowseDialog.openForLoad()
+//                                    QGCFileDialog {
+//                                        id:             customActionPathBrowseDialog
+//                                        title:          qsTr("Choose the Custom Action Definitions file")
+//                                        folder:         customActions.jsonFile.rawValue
+//                                        selectExisting: true
+//                                        selectFolder:   false
+//                                        onAcceptedForLoad: customActions.jsonFile.rawValue = file
+//                                        nameFilters: ["JSON files (*.json)"]
+//                                    }
+//                                }
+//                                // The file loader on Android doesn't work, so we hard code the path to the
+//                                // JSON file. However, we need a button to force a refresh if the JSON file
+//                                // is changed.
+//                                QGCButton {
+//                                    visible:    ScreenTools.isMobile
+//                                    text:       qsTr("Reload")
+//                                    onClicked:  {
+//                                        customActions.jsonFile.valueChanged(customActions.jsonFile.rawValue)
+//                                    }
+//                                }
+//                            }
+
+//                            GridLayout {
+//                                columns: 2
+
+//                                QGCLabel {
+//                                    text:               qsTr("Guided Command Settings")
+//                                    Layout.columnSpan:  2
+//                                    Layout.alignment:   Qt.AlignHCenter
+//                                }
+
+//                                QGCLabel {
+//                                    text:       qsTr("Minimum Altitude")
+//                                    visible:    guidedMinAltField.visible
+//                                }
+//                                FactTextField {
+//                                    id:                     guidedMinAltField
+//                                    Layout.preferredWidth:  _valueFieldWidth
+//                                    visible:                fact.visible
+//                                    fact:                   _flyViewSettings.guidedMinimumAltitude
+//                                }
+
+//                                QGCLabel {
+//                                    text:       qsTr("Maximum Altitude")
+//                                    visible:    guidedMaxAltField.visible
+//                                }
+//                                FactTextField {
+//                                    id:                     guidedMaxAltField
+//                                    Layout.preferredWidth:  _valueFieldWidth
+//                                    visible:                fact.visible
+//                                    fact:                   _flyViewSettings.guidedMaximumAltitude
+//                                }
+
+//                                QGCLabel {
+//                                    text:       qsTr("Go To Location Max Distance")
+//                                    visible:    maxGotoDistanceField.visible
+//                                }
+//                                FactTextField {
+//                                    id:                     maxGotoDistanceField
+//                                    Layout.preferredWidth:  _valueFieldWidth
+//                                    visible:                fact.visible
+//                                    fact:                  _flyViewSettings.maxGoToLocationDistance
+//                                }
+//                            }
 
                             GridLayout {
                                 id:         videoGrid
@@ -341,6 +341,17 @@ Rectangle {
                                     visible:                rtspUrlLabel.visible
                                 }
 
+                                QGCLabel {
+                                    id:         rtspUrlLabel2
+                                    text:       qsTr("RTSP URL 2")
+                                    visible:    !_videoAutoStreamConfig && _isRTSP && _videoSettings.rtspUrl.visible
+                                }
+                                FactTextField {
+                                    Layout.preferredWidth:  _comboFieldWidth
+                                    fact:                   _videoSettings.rtspUrl2
+                                    visible:                rtspUrlLabel.visible
+                                }
+
                                 QGCLabel {
                                     id:         tcpUrlLabel
                                     text:       qsTr("TCP URL")
@@ -488,23 +499,23 @@ Rectangle {
                                     fact:                   QGroundControl.settingsManager.appSettings.defaultMissionItemAltitude
                                 }
 
-                                QGCLabel { text: qsTr("VTOL TransitionDistance") }
-                                FactTextField {
-                                    Layout.preferredWidth:  _valueFieldWidth
-                                    fact:                   QGroundControl.settingsManager.planViewSettings.vtolTransitionDistance
-                                }
+//                                QGCLabel { text: qsTr("VTOL TransitionDistance") }
+//                                FactTextField {
+//                                    Layout.preferredWidth:  _valueFieldWidth
+//                                    fact:                   QGroundControl.settingsManager.planViewSettings.vtolTransitionDistance
+//                                }
                             }
 
-                            FactCheckBox {
-                                text:   qsTr("Use MAV_CMD_CONDITION_GATE for pattern generation")
-                                fact:   QGroundControl.settingsManager.planViewSettings.useConditionGate
-                            }
+//                            FactCheckBox {
+//                                text:   qsTr("Use MAV_CMD_CONDITION_GATE for pattern generation")
+//                                fact:   QGroundControl.settingsManager.planViewSettings.useConditionGate
+//                            }
 
-                            FactCheckBox {
-                                text:       qsTr("Missions Do Not Require Takeoff Item")
-                                fact:       _planViewSettings.takeoffItemNotRequired
-                                visible:    _planViewSettings.takeoffItemNotRequired.visible
-                            }
+//                            FactCheckBox {
+//                                text:       qsTr("Missions Do Not Require Takeoff Item")
+//                                fact:       _planViewSettings.takeoffItemNotRequired
+//                                visible:    _planViewSettings.takeoffItemNotRequired.visible
+//                            }
                         }
                     }
 
@@ -845,255 +856,255 @@ Rectangle {
                         }
                     }
 
-                    Item { width: 1; height: _margins; visible: autoConnectSectionLabel.visible }
-                    QGCLabel {
-                        id:         autoConnectSectionLabel
-                        text:       qsTr("AutoConnect to the following devices")
-                        visible:    QGroundControl.settingsManager.autoConnectSettings.visible
-                    }
-                    Rectangle {
-                        Layout.preferredWidth:  autoConnectCol.width + (_margins * 2)
-                        Layout.preferredHeight: autoConnectCol.height + (_margins * 2)
-                        color:                  qgcPal.windowShade
-                        visible:                autoConnectSectionLabel.visible
-                        Layout.fillWidth:       true
-
-                        ColumnLayout {
-                            id:                 autoConnectCol
-                            anchors.margins:    _margins
-                            anchors.left:       parent.left
-                            anchors.top:        parent.top
-                            spacing:            _margins
-
-                            RowLayout {
-                                spacing: _margins
-
-                                Repeater {
-                                    id:     autoConnectRepeater
-                                    model:  [ QGroundControl.settingsManager.autoConnectSettings.autoConnectPixhawk,
-                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectSiKRadio,
-                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectPX4Flow,
-                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectLibrePilot,
-                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectUDP,
-                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectRTKGPS,
-                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectZeroConf,
-                                    ]
-
-                                    property var names: [ qsTr("Pixhawk"), qsTr("SiK Radio"), qsTr("PX4 Flow"), qsTr("LibrePilot"), qsTr("UDP"), qsTr("RTK GPS"), qsTr("Zero-Conf") ]
-
-                                    FactCheckBox {
-                                        text:       autoConnectRepeater.names[index]
-                                        fact:       modelData
-                                        visible:    modelData.visible
-                                    }
-                                }
-                            }
-
-                            GridLayout {
-                                Layout.fillWidth:   false
-                                Layout.alignment:   Qt.AlignHCenter
-                                columns:            2
-                                visible:            !ScreenTools.isMobile
-                                                    && QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaPort.visible
-                                                    && QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaBaud.visible
-
-                                QGCLabel {
-                                    text: qsTr("NMEA GPS Device")
-                                }
-                                QGCComboBox {
-                                    id:                     nmeaPortCombo
-                                    Layout.preferredWidth:  _comboFieldWidth
-
-                                    model:  ListModel {
-                                    }
-
-                                    onActivated: {
-                                        if (index != -1) {
-                                            QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaPort.value = textAt(index);
-                                        }
-                                    }
-                                    Component.onCompleted: {
-                                        model.append({text: gpsDisabled})
-                                        model.append({text: gpsUdpPort})
-
-                                        for (var i in QGroundControl.linkManager.serialPorts) {
-                                            nmeaPortCombo.model.append({text:QGroundControl.linkManager.serialPorts[i]})
-                                        }
-                                        var index = nmeaPortCombo.find(QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaPort.valueString);
-                                        nmeaPortCombo.currentIndex = index;
-                                        if (QGroundControl.linkManager.serialPorts.length === 0) {
-                                            nmeaPortCombo.model.append({text: "Serial <none available>"})
-                                        }
-                                    }
-                                }
-
-                                QGCLabel {
-                                    visible:          nmeaPortCombo.currentText !== gpsUdpPort && nmeaPortCombo.currentText !== gpsDisabled
-                                    text:             qsTr("NMEA GPS Baudrate")
-                                }
-                                QGCComboBox {
-                                    visible:                nmeaPortCombo.currentText !== gpsUdpPort && nmeaPortCombo.currentText !== gpsDisabled
-                                    id:                     nmeaBaudCombo
-                                    Layout.preferredWidth:  _comboFieldWidth
-                                    model:                  [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
-
-                                    onActivated: {
-                                        if (index != -1) {
-                                            QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaBaud.value = textAt(index);
-                                        }
-                                    }
-                                    Component.onCompleted: {
-                                        var index = nmeaBaudCombo.find(QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaBaud.valueString);
-                                        nmeaBaudCombo.currentIndex = index;
-                                    }
-                                }
-
-                                QGCLabel {
-                                    text:       qsTr("NMEA stream UDP port")
-                                    visible:    nmeaPortCombo.currentText === gpsUdpPort
-                                }
-                                FactTextField {
-                                    visible:                nmeaPortCombo.currentText === gpsUdpPort
-                                    Layout.preferredWidth:  _valueFieldWidth
-                                    fact:                   QGroundControl.settingsManager.autoConnectSettings.nmeaUdpPort
-                                }
-                            }
-                        }
-                    }
-
-                    Item { width: 1; height: _margins; visible: rtkSectionLabel.visible }
-                    QGCLabel {
-                        id:         rtkSectionLabel
-                        text:       qsTr("RTK GPS")
-                        visible:    QGroundControl.settingsManager.rtkSettings.visible
-                    }
-                    Rectangle {
-                        Layout.preferredHeight: rtkGrid.height + (_margins * 2)
-                        Layout.preferredWidth:  rtkGrid.width + (_margins * 2)
-                        color:                  qgcPal.windowShade
-                        visible:                rtkSectionLabel.visible
-                        Layout.fillWidth:       true
-
-                        GridLayout {
-                            id:                         rtkGrid
-                            anchors.topMargin:          _margins
-                            anchors.top:                parent.top
-                            Layout.fillWidth:           true
-                            anchors.horizontalCenter:   parent.horizontalCenter
-                            columns:                    3
-
-                            property var  rtkSettings:      QGroundControl.settingsManager.rtkSettings
-                            property bool useFixedPosition: rtkSettings.useFixedBasePosition.rawValue
-                            property real firstColWidth:    ScreenTools.defaultFontPixelWidth * 3
-
-                            QGCRadioButton {
-                                text:               qsTr("Perform Survey-In")
-                                visible:            rtkGrid.rtkSettings.useFixedBasePosition.visible
-                                checked:            rtkGrid.rtkSettings.useFixedBasePosition.value === false
-                                Layout.columnSpan:  3
-                                onClicked:          rtkGrid.rtkSettings.useFixedBasePosition.value = false
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCLabel {
-                                text:               rtkGrid.rtkSettings.surveyInAccuracyLimit.shortDescription
-                                visible:            rtkGrid.rtkSettings.surveyInAccuracyLimit.visible
-                                enabled:            !rtkGrid.useFixedPosition
-                            }
-                            FactTextField {
-                                fact:               rtkGrid.rtkSettings.surveyInAccuracyLimit
-                                visible:            rtkGrid.rtkSettings.surveyInAccuracyLimit.visible
-                                enabled:            !rtkGrid.useFixedPosition
-                                Layout.preferredWidth:  _valueFieldWidth
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCLabel {
-                                text:               rtkGrid.rtkSettings.surveyInMinObservationDuration.shortDescription
-                                visible:            rtkGrid.rtkSettings.surveyInMinObservationDuration.visible
-                                enabled:            !rtkGrid.useFixedPosition
-                            }
-                            FactTextField {
-                                fact:               rtkGrid.rtkSettings.surveyInMinObservationDuration
-                                visible:            rtkGrid.rtkSettings.surveyInMinObservationDuration.visible
-                                enabled:            !rtkGrid.useFixedPosition
-                                Layout.preferredWidth:  _valueFieldWidth
-                            }
-
-                            QGCRadioButton {
-                                text:               qsTr("Use Specified Base Position")
-                                visible:            rtkGrid.rtkSettings.useFixedBasePosition.visible
-                                checked:            rtkGrid.rtkSettings.useFixedBasePosition.value === true
-                                onClicked:          rtkGrid.rtkSettings.useFixedBasePosition.value = true
-                                Layout.columnSpan:  3
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCLabel {
-                                text:               rtkGrid.rtkSettings.fixedBasePositionLatitude.shortDescription
-                                visible:            rtkGrid.rtkSettings.fixedBasePositionLatitude.visible
-                                enabled:            rtkGrid.useFixedPosition
-                            }
-                            FactTextField {
-                                fact:               rtkGrid.rtkSettings.fixedBasePositionLatitude
-                                visible:            rtkGrid.rtkSettings.fixedBasePositionLatitude.visible
-                                enabled:            rtkGrid.useFixedPosition
-                                Layout.fillWidth:   true
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCLabel {
-                                text:               rtkGrid.rtkSettings.fixedBasePositionLongitude.shortDescription
-                                visible:            rtkGrid.rtkSettings.fixedBasePositionLongitude.visible
-                                enabled:            rtkGrid.useFixedPosition
-                            }
-                            FactTextField {
-                                fact:               rtkGrid.rtkSettings.fixedBasePositionLongitude
-                                visible:            rtkGrid.rtkSettings.fixedBasePositionLongitude.visible
-                                enabled:            rtkGrid.useFixedPosition
-                                Layout.fillWidth:   true
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCLabel {
-                                text:           rtkGrid.rtkSettings.fixedBasePositionAltitude.shortDescription
-                                visible:        rtkGrid.rtkSettings.fixedBasePositionAltitude.visible
-                                enabled:        rtkGrid.useFixedPosition
-                            }
-                            FactTextField {
-                                fact:               rtkGrid.rtkSettings.fixedBasePositionAltitude
-                                visible:            rtkGrid.rtkSettings.fixedBasePositionAltitude.visible
-                                enabled:            rtkGrid.useFixedPosition
-                                Layout.fillWidth:   true
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCLabel {
-                                text:           rtkGrid.rtkSettings.fixedBasePositionAccuracy.shortDescription
-                                visible:        rtkGrid.rtkSettings.fixedBasePositionAccuracy.visible
-                                enabled:        rtkGrid.useFixedPosition
-                            }
-                            FactTextField {
-                                fact:               rtkGrid.rtkSettings.fixedBasePositionAccuracy
-                                visible:            rtkGrid.rtkSettings.fixedBasePositionAccuracy.visible
-                                enabled:            rtkGrid.useFixedPosition
-                                Layout.fillWidth:   true
-                            }
-
-                            Item { width: rtkGrid.firstColWidth; height: 1 }
-                            QGCButton {
-                                text:               qsTr("Save Current Base Position")
-                                enabled:            QGroundControl.gpsRtk && QGroundControl.gpsRtk.valid.value
-                                Layout.columnSpan:  2
-                                onClicked: {
-                                    rtkGrid.rtkSettings.fixedBasePositionLatitude.rawValue =    QGroundControl.gpsRtk.currentLatitude.rawValue
-                                    rtkGrid.rtkSettings.fixedBasePositionLongitude.rawValue =   QGroundControl.gpsRtk.currentLongitude.rawValue
-                                    rtkGrid.rtkSettings.fixedBasePositionAltitude.rawValue =    QGroundControl.gpsRtk.currentAltitude.rawValue
-                                    rtkGrid.rtkSettings.fixedBasePositionAccuracy.rawValue =    QGroundControl.gpsRtk.currentAccuracy.rawValue
-                                }
-                            }
-                        }
-                    }
+//                    Item { width: 1; height: _margins; visible: autoConnectSectionLabel.visible }
+//                    QGCLabel {
+//                        id:         autoConnectSectionLabel
+//                        text:       qsTr("AutoConnect to the following devices")
+//                        visible:    QGroundControl.settingsManager.autoConnectSettings.visible
+//                    }
+//                    Rectangle {
+//                        Layout.preferredWidth:  autoConnectCol.width + (_margins * 2)
+//                        Layout.preferredHeight: autoConnectCol.height + (_margins * 2)
+//                        color:                  qgcPal.windowShade
+//                        visible:                autoConnectSectionLabel.visible
+//                        Layout.fillWidth:       true
+
+//                        ColumnLayout {
+//                            id:                 autoConnectCol
+//                            anchors.margins:    _margins
+//                            anchors.left:       parent.left
+//                            anchors.top:        parent.top
+//                            spacing:            _margins
+
+//                            RowLayout {
+//                                spacing: _margins
+
+//                                Repeater {
+//                                    id:     autoConnectRepeater
+//                                    model:  [ QGroundControl.settingsManager.autoConnectSettings.autoConnectPixhawk,
+//                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectSiKRadio,
+//                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectPX4Flow,
+//                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectLibrePilot,
+//                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectUDP,
+//                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectRTKGPS,
+//                                        QGroundControl.settingsManager.autoConnectSettings.autoConnectZeroConf,
+//                                    ]
+
+//                                    property var names: [ qsTr("Pixhawk"), qsTr("SiK Radio"), qsTr("PX4 Flow"), qsTr("LibrePilot"), qsTr("UDP"), qsTr("RTK GPS"), qsTr("Zero-Conf") ]
+
+//                                    FactCheckBox {
+//                                        text:       autoConnectRepeater.names[index]
+//                                        fact:       modelData
+//                                        visible:    modelData.visible
+//                                    }
+//                                }
+//                            }
+
+//                            GridLayout {
+//                                Layout.fillWidth:   false
+//                                Layout.alignment:   Qt.AlignHCenter
+//                                columns:            2
+//                                visible:            !ScreenTools.isMobile
+//                                                    && QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaPort.visible
+//                                                    && QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaBaud.visible
+
+//                                QGCLabel {
+//                                    text: qsTr("NMEA GPS Device")
+//                                }
+//                                QGCComboBox {
+//                                    id:                     nmeaPortCombo
+//                                    Layout.preferredWidth:  _comboFieldWidth
+
+//                                    model:  ListModel {
+//                                    }
+
+//                                    onActivated: {
+//                                        if (index != -1) {
+//                                            QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaPort.value = textAt(index);
+//                                        }
+//                                    }
+//                                    Component.onCompleted: {
+//                                        model.append({text: gpsDisabled})
+//                                        model.append({text: gpsUdpPort})
+
+//                                        for (var i in QGroundControl.linkManager.serialPorts) {
+//                                            nmeaPortCombo.model.append({text:QGroundControl.linkManager.serialPorts[i]})
+//                                        }
+//                                        var index = nmeaPortCombo.find(QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaPort.valueString);
+//                                        nmeaPortCombo.currentIndex = index;
+//                                        if (QGroundControl.linkManager.serialPorts.length === 0) {
+//                                            nmeaPortCombo.model.append({text: "Serial <none available>"})
+//                                        }
+//                                    }
+//                                }
+
+//                                QGCLabel {
+//                                    visible:          nmeaPortCombo.currentText !== gpsUdpPort && nmeaPortCombo.currentText !== gpsDisabled
+//                                    text:             qsTr("NMEA GPS Baudrate")
+//                                }
+//                                QGCComboBox {
+//                                    visible:                nmeaPortCombo.currentText !== gpsUdpPort && nmeaPortCombo.currentText !== gpsDisabled
+//                                    id:                     nmeaBaudCombo
+//                                    Layout.preferredWidth:  _comboFieldWidth
+//                                    model:                  [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
+
+//                                    onActivated: {
+//                                        if (index != -1) {
+//                                            QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaBaud.value = textAt(index);
+//                                        }
+//                                    }
+//                                    Component.onCompleted: {
+//                                        var index = nmeaBaudCombo.find(QGroundControl.settingsManager.autoConnectSettings.autoConnectNmeaBaud.valueString);
+//                                        nmeaBaudCombo.currentIndex = index;
+//                                    }
+//                                }
+
+//                                QGCLabel {
+//                                    text:       qsTr("NMEA stream UDP port")
+//                                    visible:    nmeaPortCombo.currentText === gpsUdpPort
+//                                }
+//                                FactTextField {
+//                                    visible:                nmeaPortCombo.currentText === gpsUdpPort
+//                                    Layout.preferredWidth:  _valueFieldWidth
+//                                    fact:                   QGroundControl.settingsManager.autoConnectSettings.nmeaUdpPort
+//                                }
+//                            }
+//                        }
+//                    }
+
+//                    Item { width: 1; height: _margins; visible: rtkSectionLabel.visible }
+//                    QGCLabel {
+//                        id:         rtkSectionLabel
+//                        text:       qsTr("RTK GPS")
+//                        visible:    QGroundControl.settingsManager.rtkSettings.visible
+//                    }
+//                    Rectangle {
+//                        Layout.preferredHeight: rtkGrid.height + (_margins * 2)
+//                        Layout.preferredWidth:  rtkGrid.width + (_margins * 2)
+//                        color:                  qgcPal.windowShade
+//                        visible:                rtkSectionLabel.visible
+//                        Layout.fillWidth:       true
+
+//                        GridLayout {
+//                            id:                         rtkGrid
+//                            anchors.topMargin:          _margins
+//                            anchors.top:                parent.top
+//                            Layout.fillWidth:           true
+//                            anchors.horizontalCenter:   parent.horizontalCenter
+//                            columns:                    3
+
+//                            property var  rtkSettings:      QGroundControl.settingsManager.rtkSettings
+//                            property bool useFixedPosition: rtkSettings.useFixedBasePosition.rawValue
+//                            property real firstColWidth:    ScreenTools.defaultFontPixelWidth * 3
+
+//                            QGCRadioButton {
+//                                text:               qsTr("Perform Survey-In")
+//                                visible:            rtkGrid.rtkSettings.useFixedBasePosition.visible
+//                                checked:            rtkGrid.rtkSettings.useFixedBasePosition.value === false
+//                                Layout.columnSpan:  3
+//                                onClicked:          rtkGrid.rtkSettings.useFixedBasePosition.value = false
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCLabel {
+//                                text:               rtkGrid.rtkSettings.surveyInAccuracyLimit.shortDescription
+//                                visible:            rtkGrid.rtkSettings.surveyInAccuracyLimit.visible
+//                                enabled:            !rtkGrid.useFixedPosition
+//                            }
+//                            FactTextField {
+//                                fact:               rtkGrid.rtkSettings.surveyInAccuracyLimit
+//                                visible:            rtkGrid.rtkSettings.surveyInAccuracyLimit.visible
+//                                enabled:            !rtkGrid.useFixedPosition
+//                                Layout.preferredWidth:  _valueFieldWidth
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCLabel {
+//                                text:               rtkGrid.rtkSettings.surveyInMinObservationDuration.shortDescription
+//                                visible:            rtkGrid.rtkSettings.surveyInMinObservationDuration.visible
+//                                enabled:            !rtkGrid.useFixedPosition
+//                            }
+//                            FactTextField {
+//                                fact:               rtkGrid.rtkSettings.surveyInMinObservationDuration
+//                                visible:            rtkGrid.rtkSettings.surveyInMinObservationDuration.visible
+//                                enabled:            !rtkGrid.useFixedPosition
+//                                Layout.preferredWidth:  _valueFieldWidth
+//                            }
+
+//                            QGCRadioButton {
+//                                text:               qsTr("Use Specified Base Position")
+//                                visible:            rtkGrid.rtkSettings.useFixedBasePosition.visible
+//                                checked:            rtkGrid.rtkSettings.useFixedBasePosition.value === true
+//                                onClicked:          rtkGrid.rtkSettings.useFixedBasePosition.value = true
+//                                Layout.columnSpan:  3
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCLabel {
+//                                text:               rtkGrid.rtkSettings.fixedBasePositionLatitude.shortDescription
+//                                visible:            rtkGrid.rtkSettings.fixedBasePositionLatitude.visible
+//                                enabled:            rtkGrid.useFixedPosition
+//                            }
+//                            FactTextField {
+//                                fact:               rtkGrid.rtkSettings.fixedBasePositionLatitude
+//                                visible:            rtkGrid.rtkSettings.fixedBasePositionLatitude.visible
+//                                enabled:            rtkGrid.useFixedPosition
+//                                Layout.fillWidth:   true
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCLabel {
+//                                text:               rtkGrid.rtkSettings.fixedBasePositionLongitude.shortDescription
+//                                visible:            rtkGrid.rtkSettings.fixedBasePositionLongitude.visible
+//                                enabled:            rtkGrid.useFixedPosition
+//                            }
+//                            FactTextField {
+//                                fact:               rtkGrid.rtkSettings.fixedBasePositionLongitude
+//                                visible:            rtkGrid.rtkSettings.fixedBasePositionLongitude.visible
+//                                enabled:            rtkGrid.useFixedPosition
+//                                Layout.fillWidth:   true
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCLabel {
+//                                text:           rtkGrid.rtkSettings.fixedBasePositionAltitude.shortDescription
+//                                visible:        rtkGrid.rtkSettings.fixedBasePositionAltitude.visible
+//                                enabled:        rtkGrid.useFixedPosition
+//                            }
+//                            FactTextField {
+//                                fact:               rtkGrid.rtkSettings.fixedBasePositionAltitude
+//                                visible:            rtkGrid.rtkSettings.fixedBasePositionAltitude.visible
+//                                enabled:            rtkGrid.useFixedPosition
+//                                Layout.fillWidth:   true
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCLabel {
+//                                text:           rtkGrid.rtkSettings.fixedBasePositionAccuracy.shortDescription
+//                                visible:        rtkGrid.rtkSettings.fixedBasePositionAccuracy.visible
+//                                enabled:        rtkGrid.useFixedPosition
+//                            }
+//                            FactTextField {
+//                                fact:               rtkGrid.rtkSettings.fixedBasePositionAccuracy
+//                                visible:            rtkGrid.rtkSettings.fixedBasePositionAccuracy.visible
+//                                enabled:            rtkGrid.useFixedPosition
+//                                Layout.fillWidth:   true
+//                            }
+
+//                            Item { width: rtkGrid.firstColWidth; height: 1 }
+//                            QGCButton {
+//                                text:               qsTr("Save Current Base Position")
+//                                enabled:            QGroundControl.gpsRtk && QGroundControl.gpsRtk.valid.value
+//                                Layout.columnSpan:  2
+//                                onClicked: {
+//                                    rtkGrid.rtkSettings.fixedBasePositionLatitude.rawValue =    QGroundControl.gpsRtk.currentLatitude.rawValue
+//                                    rtkGrid.rtkSettings.fixedBasePositionLongitude.rawValue =   QGroundControl.gpsRtk.currentLongitude.rawValue
+//                                    rtkGrid.rtkSettings.fixedBasePositionAltitude.rawValue =    QGroundControl.gpsRtk.currentAltitude.rawValue
+//                                    rtkGrid.rtkSettings.fixedBasePositionAccuracy.rawValue =    QGroundControl.gpsRtk.currentAccuracy.rawValue
+//                                }
+//                            }
+//                        }
+//                    }
 
                     Item { width: 1; height: _margins; visible: adsbSectionLabel.visible }
                     QGCLabel {
@@ -1236,11 +1247,11 @@ Rectangle {
 
                     Item { width: 1; height: _margins }
                     QGCLabel {
-                        text:               qsTr("%1 Version").arg(QGroundControl.appName)
+                        text:               qsTr("Arcsky Control Version")//qsTr("%1 Version").arg(QGroundControl.appName)
                         Layout.alignment:   Qt.AlignHCenter
                     }
                     QGCLabel {
-                        text:               QGroundControl.qgcVersion
+                        text:               qsTr("TS-V1.0")//QGroundControl.qgcVersion
                         Layout.alignment:   Qt.AlignHCenter
                     }
                 } // settingsColumn
diff --git a/src/ui/preferences/HelpSettings.qml b/src/ui/preferences/HelpSettings.qml
index 9fd988c3c..4e794110d 100644
--- a/src/ui/preferences/HelpSettings.qml
+++ b/src/ui/preferences/HelpSettings.qml
@@ -34,26 +34,26 @@ Rectangle {
             id:         grid
             columns:    2
 
-            QGCLabel { text: qsTr("QGroundControl User Guide") }
+            QGCLabel { text: qsTr("Arcsky Control Overview") }
             QGCLabel {
                 linkColor:          qgcPal.text
-                text:               "<a href=\"https://docs.qgroundcontrol.com\">https://docs.qgroundcontrol.com</a>"
+                text:               "<a href=\"https://youtu.be/RCtF0K-bsm4?si=SfpmDtrSIVVnfdAb\">Arcsky Control Video</a>"
                 onLinkActivated:    Qt.openUrlExternally(link)
             }
 
-            QGCLabel { text: qsTr("PX4 Users Discussion Forum") }
+            QGCLabel { text: qsTr("Arcsky Documentation") }
             QGCLabel {
                 linkColor:          qgcPal.text
-                text:               "<a href=\"http://discuss.px4.io/c/qgroundcontrol\">http://discuss.px4.io/c/qgroundcontrol</a>"
+                text:               "<a href=\"https://www.arcskytech.com/docs\">Arcsky Documentation</a>"
                 onLinkActivated:    Qt.openUrlExternally(link)
             }
 
-            QGCLabel { text: qsTr("ArduPilot Users Discussion Forum") }
-            QGCLabel {
-                linkColor:          qgcPal.text
-                text:               "<a href=\"https://discuss.ardupilot.org/c/ground-control-software/qgroundcontrol\">https://discuss.ardupilot.org/c/ground-control-software/qgroundcontrol</a>"
-                onLinkActivated:    Qt.openUrlExternally(link)
-            }
+//            QGCLabel { text: qsTr("ArduPilot Users Discussion Forum") }
+//            QGCLabel {
+//                linkColor:          qgcPal.text
+//                text:               "<a href=\"https://discuss.ardupilot.org/c/ground-control-software/qgroundcontrol\">https://discuss.ardupilot.org/c/ground-control-software/qgroundcontrol</a>"
+//                onLinkActivated:    Qt.openUrlExternally(link)
+//            }
         }
     }
 }
diff --git a/src/ui/toolbar/BatteryIndicator.qml b/src/ui/toolbar/BatteryIndicator.qml
index 9d68fc3c7..019948ac3 100644
--- a/src/ui/toolbar/BatteryIndicator.qml
+++ b/src/ui/toolbar/BatteryIndicator.qml
@@ -28,6 +28,7 @@ Item {
     property bool showIndicator: true
 
     property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
+    property var _generator: QGroundControl.multiVehicleManager.activeVehicle.generator
 
     Row {
         id:             batteryIndicatorRow
@@ -77,16 +78,40 @@ Item {
             }
 
             function getBatteryPercentageText() {
-                if (!isNaN(battery.percentRemaining.rawValue)) {
-                    if (battery.percentRemaining.rawValue > 98.9) {
-                        return qsTr("100%")
-                    } else {
-                        return battery.percentRemaining.valueString + battery.percentRemaining.units
+                if (_activeVehicle.generator.runtime.rawValue !== null)
+                {
+                    //console.log("generator fact exists")
+                     if (_activeVehicle.generator.busVoltage.rawValue > 0){
+                         //console.log("gen runtime more than 0")
+                         if (!isNaN(battery.percentRemaining.rawValue)) {
+                            if (battery.percentRemaining.rawValue > 98.9) {
+                                return qsTr("100%")
+                            } else {
+                                return battery.percentRemaining.valueString + battery.percentRemaining.units
+                            }
+                        }
+                    }
+                    if (!isNaN(battery.voltage.rawValue)) {
+                        return battery.voltage.valueString + battery.voltage.units
+                    }
+                }
+                else
+                {
+                    //console.log("generator null")
+//                    if (!isNaN(battery.percentRemaining.rawValue)) {
+//                        if (battery.percentRemaining.rawValue > 98.9) {
+//                            return qsTr("100%")
+//                        } else {
+//                            return battery.percentRemaining.valueString + battery.percentRemaining.units
+//                        }
+//                    } else if (!isNaN(battery.voltage.rawValue)) {
+//                        return battery.voltage.valueString + battery.voltage.units
+//                    } else if (battery.chargeState.rawValue !== MAVLink.MAV_BATTERY_CHARGE_STATE_UNDEFINED) {
+//                        return battery.chargeState.enumStringValue
+//                    }
+                    if (!isNaN(battery.voltage.rawValue)) {
+                        return battery.voltage.valueString + battery.voltage.units
                     }
-                } else if (!isNaN(battery.voltage.rawValue)) {
-                    return battery.voltage.valueString + battery.voltage.units
-                } else if (battery.chargeState.rawValue !== MAVLink.MAV_BATTERY_CHARGE_STATE_UNDEFINED) {
-                    return battery.chargeState.enumStringValue
                 }
                 return ""
             }
@@ -96,7 +121,7 @@ Item {
                 anchors.bottom:     parent.bottom
                 width:              height
                 sourceSize.width:   width
-                source:             "/qmlimages/Battery.svg"
+                source:             (_activeVehicle.generator.busVoltage.rawValue > 0) && (_activeVehicle.generator._timeout < 1) ? "/qmlimages/FuelTank.svg" : "/qmlimages/Battery.svg"
                 fillMode:           Image.PreserveAspectFit
                 color:              getBatteryColor()
             }
@@ -142,7 +167,7 @@ Item {
 
                 QGCLabel {
                     Layout.alignment:   Qt.AlignCenter
-                    text:               qsTr("Battery Status")
+                    text:               (_activeVehicle.generator.busVoltage.rawValue > 0) && (_activeVehicle.generator._timeout < 1) ? qsTr("Generator Status") : qsTr("Battery Status")
                     font.family:        ScreenTools.demiboldFontFamily
                 }
 
@@ -165,7 +190,7 @@ Item {
                                     property var battery: object
                                 }
 
-                                QGCLabel { text: qsTr("Battery %1").arg(object.id.rawValue) }
+                                QGCLabel { text: qsTr("Bat/Gen %1").arg(object.id.rawValue) }
                                 QGCLabel { text: qsTr("Charge State");                          visible: batteryValuesAvailable.chargeStateAvailable }
                                 QGCLabel { text: qsTr("Remaining");                             visible: batteryValuesAvailable.timeRemainingAvailable }
                                 QGCLabel { text: qsTr("Remaining") }
diff --git a/src/ui/toolbar/MainStatusIndicator.qml b/src/ui/toolbar/MainStatusIndicator.qml
index 1e2ff5f2c..9586c7ed0 100644
--- a/src/ui/toolbar/MainStatusIndicator.qml
+++ b/src/ui/toolbar/MainStatusIndicator.qml
@@ -174,31 +174,31 @@ RowLayout {
                     id:         mainLayout
                     spacing:    _spacing
 
-                    QGCButton {
-                        Layout.leftMargin:  _healthAndArmingChecksSupported ? width / 2 : 0
-                        Layout.alignment:   _healthAndArmingChecksSupported ? Qt.AlignLeft : Qt.AlignHCenter
-                        // FIXME: forceArm is not possible anymore if _healthAndArmingChecksSupported == true
-                        enabled:            _armed || !_healthAndArmingChecksSupported || _activeVehicle.healthAndArmingCheckReport.canArm
-                        text:               _armed ?  qsTr("Disarm") : (forceArm ? qsTr("Force Arm") : qsTr("Arm"))
-
-                        property bool forceArm: false
-
-                        onPressAndHold: forceArm = true
-
-                        onClicked: {
-                            if (_armed) {
-                                mainWindow.disarmVehicleRequest()
-                            } else {
-                                if (forceArm) {
-                                    mainWindow.forceArmVehicleRequest()
-                                } else {
-                                    mainWindow.armVehicleRequest()
-                                }
-                            }
-                            forceArm = false
-                            mainWindow.hideIndicatorPopup()
-                        }
-                    }
+//                    QGCButton {
+//                        Layout.leftMargin:  _healthAndArmingChecksSupported ? width / 2 : 0
+//                        Layout.alignment:   _healthAndArmingChecksSupported ? Qt.AlignLeft : Qt.AlignHCenter
+//                        // FIXME: forceArm is not possible anymore if _healthAndArmingChecksSupported == true
+//                        enabled:            _armed || !_healthAndArmingChecksSupported || _activeVehicle.healthAndArmingCheckReport.canArm
+//                        text:               _armed ?  qsTr("Disarm") : (forceArm ? qsTr("Force Arm") : qsTr("Arm"))
+
+//                        property bool forceArm: false
+
+//                        onPressAndHold: forceArm = true
+
+//                        onClicked: {
+//                            if (_armed) {
+//                                mainWindow.disarmVehicleRequest()
+//                            } else {
+//                                if (forceArm) {
+//                                    mainWindow.forceArmVehicleRequest()
+//                                } else {
+//                                    mainWindow.armVehicleRequest()
+//                                }
+//                            }
+//                            forceArm = false
+//                            mainWindow.hideIndicatorPopup()
+//                        }
+//                    }
 
                     QGCLabel {
                         Layout.alignment:   Qt.AlignHCenter
diff --git a/src/ui/toolbar/MainToolBar.qml b/src/ui/toolbar/MainToolBar.qml
index 9a183ffd2..efab631fa 100644
--- a/src/ui/toolbar/MainToolBar.qml
+++ b/src/ui/toolbar/MainToolBar.qml
@@ -75,9 +75,31 @@ Rectangle {
             Layout.preferredHeight: viewButtonRow.height
             icon.source:            "/res/QGCLogoFull"
             logo:                   true
-            onClicked:              mainWindow.showToolSelectDialog()
+            onClicked:              mainWindow.showSettingsTool()//mainWindow.showToolSelectDialog()
+        }
+        QGCToolBarButton {
+            id:                     setupButton
+            Layout.preferredHeight: viewButtonRow.height
+            icon.source:            "/qmlimages/Gears.svg"
+            logo:                   true
+            onClicked:              mainWindow.showSetupTool()
+        }
+        QGCToolBarButton {
+            id:                     flyButton
+            Layout.preferredHeight: viewButtonRow.height
+            icon.source:            "/qmlimages/PaperPlane.svg"
+            logo:                   true
+            onClicked:              mainWindow.showFlyView()
+            visible:                currentToolbar === planViewToolbar
+        }
+        QGCToolBarButton {
+            id:                     planButton
+            Layout.preferredHeight: viewButtonRow.height
+            icon.source:            "/qmlimages/Plan.svg"
+            logo:                   true
+            onClicked:              mainWindow.showPlanView()
+            visible:                currentToolbar === flyViewToolbar
         }
-
         MainStatusIndicator {
             Layout.preferredHeight: viewButtonRow.height
             visible:                currentToolbar === flyViewToolbar
@@ -119,10 +141,10 @@ Rectangle {
         anchors.right:          parent.right
         anchors.top:            parent.top
         anchors.bottom:         parent.bottom
-        anchors.margins:        ScreenTools.defaultFontPixelHeight * 0.66
-        visible:                currentToolbar !== planViewToolbar && _activeVehicle && !_communicationLost && x > (toolsFlickable.x + toolsFlickable.contentWidth + ScreenTools.defaultFontPixelWidth)
+        anchors.margins:        10 // ScreenTools.defaultFontPixelHeight * 0.1//0.66
+        visible:                currentToolbar !== planViewToolbar && x > (toolsFlickable.x + toolsFlickable.contentWidth + ScreenTools.defaultFontPixelWidth)
         fillMode:               Image.PreserveAspectFit
-        source:                 _outdoorPalette ? _brandImageOutdoor : _brandImageIndoor
+        source:                 "/qmlimages/APM/BrandImage"//_outdoorPalette ? _brandImageOutdoor : _brandImageIndoor
         mipmap:                 true
 
         property bool   _outdoorPalette:        qgcPal.globalTheme === QGCPalette.Light
diff --git a/src/ui/toolbar/TelemetryRSSIIndicator.qml b/src/ui/toolbar/TelemetryRSSIIndicator.qml
index b51a392b2..1493e28bc 100644
--- a/src/ui/toolbar/TelemetryRSSIIndicator.qml
+++ b/src/ui/toolbar/TelemetryRSSIIndicator.qml
@@ -79,7 +79,7 @@ Item {
         anchors.bottom:     parent.bottom
         width:              height
         sourceSize.height:  height
-        source:             "/qmlimages/TelemRSSI.svg"
+        source:             "/qmlimages/RC.svg"
         fillMode:           Image.PreserveAspectFit
         color:              qgcPal.buttonText
     }
