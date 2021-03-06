SET (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

FIND_PACKAGE (Gloox REQUIRED)

INCLUDE_DIRECTORIES (${AZOTH_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${GLOOX_INCLUDE_DIR})
SET (XOOX_SRCS
	xoox.cpp
	glooxprotocol.cpp
	glooxaccountconfigurationdialog.cpp
	glooxaccount.cpp
	clientconnection.cpp
	glooxclentry.cpp
	glooxmessage.cpp
	joingroupchatdialog.cpp
	roomhandler.cpp
	roomclentry.cpp
	roompublicmessage.cpp
	roomparticipantentry.cpp
	entrybase.cpp
	)
SET (XOOX_HEADERS
	xoox.h
	glooxprotocol.h
	glooxaccountconfigurationdialog.h
	glooxaccount.h
	clientconnection.h
	glooxclentry.h
	glooxmessage.h
	joingroupchatdialog.h
	roomhandler.h
	roomclentry.h
	roompublicmessage.h
	roomparticipantentry.h
	entrybase.h
	)
SET (XOOX_FORMS
	glooxaccountconfigurationdialog.ui
	glooxjoingroupchatdialog.ui
	)
SET (XOOX_TRANSLATIONS
	leechcraft_azoth_xoox_ru_RU.ts
	leechcraft_azoth_xoox_en.ts
	)
SET (XOOX_COMPILED_TRANSLATIONS
	${CMAKE_CURRENT_SOURCE_DIR}/leechcraft_azoth_xoox_ru_RU.qm
	${CMAKE_CURRENT_SOURCE_DIR}/leechcraft_azoth_xoox_en.qm
	)
QT4_WRAP_CPP (XOOX_MOC_SRCS ${XOOX_HEADERS})
QT4_WRAP_UI (XOOX_UIS_H ${XOOX_FORMS})

ADD_CUSTOM_COMMAND (OUTPUT
	${XOOX_COMPILED_TRANSLATIONS}
	COMMAND "${QT_LRELEASE_EXECUTABLE}" ${XOOX_TRANSLATIONS}
	DEPENDS ${XOOX_TRANSLATIONS}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
ADD_LIBRARY (leechcraft_azoth_xoox SHARED
	${XOOX_COMPILED_TRANSLATIONS}
	${XOOX_SRCS}
	${XOOX_MOC_SRCS}
	${XOOX_UIS_H}
	)
TARGET_LINK_LIBRARIES (leechcraft_azoth_xoox
	${QT_LIBRARIES}
	${GLOOX_LIBRARIES}
	plugininterface
	xmlsettingsdialog
	)
INSTALL (TARGETS leechcraft_azoth_xoox DESTINATION ${LC_PLUGINS_DEST})
INSTALL (FILES ${XOOX_COMPILED_TRANSLATIONS} DESTINATION ${LC_TRANSLATIONS_DEST})
