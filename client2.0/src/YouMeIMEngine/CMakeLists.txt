project (YouMeIMEngine)

ADD_DEFINITIONS(-DCURL_STATICLIB -DCARES_STATICLIB -DYOUMEDLL_EXPORTS)
INCLUDE_DIRECTORIES (${COMMON_INCLUDE} .. ${COMMON_INCLUDE}/YouMeCommon)

FILE (GLOB header_files *.h)
FILE (GLOB source_files *.c *.cc *.cpp)
FILE (GLOB pb pb/*.h pb/*.cpp pb/*.cc)

FILE (GLOB CInterface CInterface/*.h CInterface/*.cpp CInterface/*.cc)

FILE (GLOB Android Android/jni/*.h Android/jni/*.cpp Android/jni/*.cc)


FILE (GLOB Speech Speech/*.h Speech/*.cpp Speech/*.cc)

FILE (GLOB AndroidSpeech Speech/Android/*.h Speech/Android/*.cpp Speech/Android/*.cc)
FILE (GLOB IOSSpeech Speech/IOS/*.h Speech/IOS/*.cpp Speech/IOS/*.cc Speech/IOS/*.hpp Speech/IOS/*.mm)
FILE (GLOB WindowsSpeech Speech/Windows/*.h Speech/Windows/*.cpp Speech/Windows/*.cc Speech/Windows/*.h)

LINK_DIRECTORIES(${COMMON_LIB})

SOURCE_GROUP("pb" FILES ${pb})
SOURCE_GROUP("CInterface" FILES ${CInterface})
SOURCE_GROUP("Android" FILES ${Android})
SOURCE_GROUP("Speech" FILES ${Speech})

SOURCE_GROUP("Speech\\Android" FILES ${AndroidSpeech})
SOURCE_GROUP("Speech\\IOS" FILES ${IOSSpeech})
SOURCE_GROUP("Speech\\Windows" FILES ${WindowsSpeech})


IF(WIN32)
	ADD_LIBRARY(YouMeIMEngine SHARED ${header_files} ${source_files} ${pb} ${CInterface} ${Speech} ${WindowsSpeech})
	TARGET_LINK_LIBRARIES (YouMeIMEngine YouMeCommon wldap32.lib  Version.lib Setupapi.lib ws2_32 Winmm.lib)
ELSEIF (__ANDROID__) 
	ADD_LIBRARY(YouMeIMEngine SHARED ${header_files} ${source_files} ${pb} ${CInterface} ${Android}  ${Speech} ${AndroidSpeech})
	TARGET_LINK_LIBRARIES (YouMeIMEngine YouMeCommon z log)
ELSEIF(__IOS__)
	ADD_LIBRARY (YouMeIMEngine ${header_files} ${source_files} ${pb} ${CInterface}  ${Speech} ${IOSSpeech})
ELSEIF(__IOSSIMULATOR__)
	ADD_LIBRARY (YouMeIMEngine ${header_files} ${source_files} ${pb} ${CInterface}  ${Speech} ${IOSSpeech})
ELSEIF (__OSX__)	
	ADD_LIBRARY (YouMeIMEngine ${header_files} ${source_files} ${pb} ${CInterface}  ${Speech} )
ENDIF(WIN32)