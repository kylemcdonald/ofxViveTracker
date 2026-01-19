meta:
	ADDON_NAME = ofxViveTracker
	ADDON_DESCRIPTION = Vive Tracker wrapper (OpenVR or libsurvive on Windows, libsurvive on macOS)
	ADDON_AUTHOR =
	ADDON_TAGS = "vr" "tracking" "vive" "openvr" "libsurvive"
	ADDON_URL =

common:

# Windows: Default is OpenVR backend.
# To use libsurvive instead:
#   1. Define OFX_VIVE_USE_LIBSURVIVE in your project's preprocessor definitions
#   2. Replace the ADDON_LIBS line below with: libs/libsurvive/lib/win64/survive.lib
#   3. Copy libsurvive.dll and the plugins/ folder to your app's bin/ directory
vs:
	ADDON_INCLUDES += libs/openvr/include
	ADDON_INCLUDES += libs/libsurvive/include
	ADDON_LIBS += libs/openvr/lib/win64/openvr_api.lib

osx:
	ADDON_INCLUDES += libs/libsurvive/include
	ADDON_LIBS += libs/libsurvive/lib/osx/libsurvive.dylib
	ADDON_LDFLAGS += -Wl,-rpath,@executable_path/../../../../../libs/libsurvive/lib/osx
