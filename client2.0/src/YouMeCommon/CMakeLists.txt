#
project(YouMeCommon)
INCLUDE_DIRECTORIES (${COMMON_INCLUDE} . .. ${COMMON_INCLUDE} ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_nb/common/include ./opencore/oscl ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_nb/dec/include ./opencore/opencore/codecs_v2/audio/gsm_amr/common/dec/include ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_nb/enc/src ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_nb/enc/include ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_nb/dec/src ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_wb/dec/include ./opencore/opencore/codecs_v2/audio/gsm_amr/amr_wb/dec/src)
IF(WIN32)
ADD_DEFINITIONS(-DCURL_STATICLIB -DCARES_STATICLIB -D_XKEYCHECK_H -DNOCRYPT -DOPENSSL_SYSNAME_WIN32 -DLPDIR_H -DCURL_STATICLIB -DHTTP_ONLY)
ELSE(WIN32)
ADD_DEFINITIONS(-DCURL_STATICLIB -DCARES_STATICLIB -DCURL_STATICLIB -DCURL_HAVE_CONFIG_ -DHTTP_ONLY)
ENDIF(WIN32)


FILE (GLOB header_files *.h *.hpp)
FILE (GLOB source_files *.c *.cc *.cpp)
INCLUDE (globalsetting)
LINK_DIRECTORIES(${COMMON_LIB})



FILE (GLOB CrossPlatformDefine CrossPlatformDefine/*.h CrossPlatformDefine/*.cpp CrossPlatformDefine/*.c CrossPlatformDefine/*.cc)
FILE (GLOB ares ares/*.h ares/*.cpp ares/*.c ares/*.cc)
FILE (GLOB_RECURSE crypto crypto/*.h crypto/*.cpp crypto/*.c)
FILE (GLOB_RECURSE google google/*.h google/*.cpp google/*.cc)
FILE (GLOB_RECURSE opencore opencore/*.h opencore/*.cpp opencore/*.cc)


FILE (GLOB minizip minizip/*.h minizip/*.cpp minizip/*.c)
FILE (GLOB Sqlite3 Sqlite3/*.h Sqlite3/*.cpp Sqlite3/*.c)
FILE (GLOB tinyxml tinyxml/*.h tinyxml/*.cpp tinyxml/*.c)
FILE (GLOB zlib zlib/*.h zlib/*.cpp zlib/*.c)
FILE (GLOB pb pb/*.h pb/*.cpp pb/*.cc)
FILE (GLOB json json/*.h json/*.cpp json/*.cc)
FILE (GLOB curl curl/*.h curl/*.cpp curl/*.c)
#下面是跨平台的部分

FILE (GLOB IOS CrossPlatformDefine/IOS/*.h CrossPlatformDefine/IOS/*.cpp CrossPlatformDefine/IOS/*.m CrossPlatformDefine/IOS/*.mm )
FILE (GLOB OSX CrossPlatformDefine/OSX/*.h CrossPlatformDefine/OSX/*.cpp CrossPlatformDefine/OSX/*.m CrossPlatformDefine/OSX/*.mm )
FILE (GLOB Windows CrossPlatformDefine/Windows/*.h CrossPlatformDefine/Windows/*.cpp CrossPlatformDefine/Windows/*.cc)


SOURCE_GROUP("CrossPlatformDefine" FILES ${CrossPlatformDefine})
SOURCE_GROUP("ares" FILES ${ares})
SOURCE_GROUP("crypto" FILES ${crypto})
SOURCE_GROUP("google" FILES ${google})
SOURCE_GROUP("opencore" FILES ${opencore})
SOURCE_GROUP("minizip" FILES ${minizip})
SOURCE_GROUP("zlib" FILES ${zlib})
SOURCE_GROUP("pb" FILES ${pb})
SOURCE_GROUP("json" FILES ${json})
SOURCE_GROUP("curl" FILES ${curl})
SOURCE_GROUP("tinyxml" FILES ${tinyxml})
#跨平台的子目录

SOURCE_GROUP("CrossPlatformDefine\\IOS" FILES ${IOS})
SOURCE_GROUP("CrossPlatformDefine\\OSX" FILES ${OSX})
SOURCE_GROUP("CrossPlatformDefine\\Windows" FILES ${Windows})



IF(__ANDROID__)
	SOURCE_GROUP("Sqlite3" FILES ${Sqlite3})
	ADD_LIBRARY (YouMeCommon ${header_files} ${source_files} ${opencore} ${json} ${CrossPlatformDefine} ${Android}  ${crypto} ${google} ${minizip}  ${Sqlite3} ${pb} ${curl} ${tinyxml})
ELSEIF (WIN32) 
	SOURCE_GROUP("Sqlite3" FILES ${Sqlite3})
	ADD_LIBRARY (YouMeCommon ${header_files} ${source_files} ${opencore} ${json} ${CrossPlatformDefine} ${Windows} ${zlib}    ${minizip} ${google} ${Sqlite3} ${crypto} ${pb} ${curl} ${tinyxml})
ELSEIF (__OSX__)	
	ADD_LIBRARY (YouMeCommon ${header_files} ${source_files} ${opencore} ${json} ${CrossPlatformDefine} ${OSX}  ${google} ${minizip} ${crypto} ${pb} ${curl} ${tinyxml})
ELSEIF(__IOS__)
	ADD_LIBRARY (YouMeCommon ${header_files} ${source_files} ${opencore} ${json} ${CrossPlatformDefine} ${IOS}  ${google} ${minizip} ${crypto} ${pb} ${curl} ${tinyxml})
ELSEIF(__IOSSIMULATOR__)
	ADD_LIBRARY (YouMeCommon ${header_files} ${source_files} ${opencore} ${json} ${CrossPlatformDefine} ${IOS}  ${google} ${minizip} ${crypto} ${pb} ${curl} ${tinyxml})
ENDIF(__ANDROID__)





