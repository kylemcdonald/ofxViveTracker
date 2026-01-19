meta:
	ADDON_NAME = ofxViveTracker
	ADDON_DESCRIPTION = Vive Tracker wrapper (OpenVR on Windows, libsurvive on macOS)
	ADDON_AUTHOR =
	ADDON_TAGS = "vr" "tracking" "vive" "openvr" "libsurvive"
	ADDON_URL =

common:

vs:
	ADDON_INCLUDES += libs/openvr/include
	ADDON_LIBS += libs/openvr/lib/win64/openvr_api.lib

osx:
	ADDON_INCLUDES += libs/libsurvive/include
	ADDON_LIBS += libs/libsurvive/lib/osx/libsurvive.dylib
	ADDON_LDFLAGS += -Wl,-rpath,@executable_path/../../../../../libs/libsurvive/lib/osx
