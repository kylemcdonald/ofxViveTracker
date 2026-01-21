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
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/poser_barycentric_svd.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/driver_global_scene_solver.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/driver_simulator.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/driver_dummy.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/poser_mpfit.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/driver_udp.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/poser_dummy.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/poser_kalman_only.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/driver_vive.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/disambiguator_statebased.dylib
	ADDON_LIBS += libs/libsurvive/lib/osx/plugins/driver_playback.dylib
	# rpath for example projects inside addon folder (5 levels up from MacOS/)
	ADDON_LDFLAGS += -Wl,-rpath,@executable_path/../../../../../libs/libsurvive/lib/osx
	ADDON_LDFLAGS += -Wl,-rpath,@executable_path/../../../../../libs/libsurvive/lib/osx/plugins
	# rpath for standard apps/category/project/ structure (7 levels up to OF root)
	ADDON_LDFLAGS += -Wl,-rpath,@executable_path/../../../../../../../addons/ofxViveTracker/libs/libsurvive/lib/osx
	ADDON_LDFLAGS += -Wl,-rpath,@executable_path/../../../../../../../addons/ofxViveTracker/libs/libsurvive/lib/osx/plugins
