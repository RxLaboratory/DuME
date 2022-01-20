#ifndef APPVERSION_H
#define APPVERSION_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 4
#define VERSION_BUILD 0
#define VERSION_SUFFIX "Beta"

#define STRINGIFY_VERSION(A, B, C) CONCAT(A, B, C )
#define CONCAT(A, B, C ) STRINGIFY( A##.##B##.##C )
#define STRINGIFY(A) #A

#ifdef QT_DEBUG
#define STR_VERSION STRINGIFY_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD) VERSION_SUFFIX "_development build"
#else
#define STR_VERSION STRINGIFY_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD) VERSION_SUFFIX
#endif

#define VER_FILEVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,0
#define STR_FILEVERSION STR_VERSION

// Keep the product version as fixed
#define VER_PRODUCTVERSION VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,0
#define STR_PRODUCTVERSION STR_VERSION

#define STR_COMPANYNAME "RxLaboratory"
#define STR_FILEDESCRIPTION "DuME"
#define STR_INTERNALNAME "DuME"
#define STR_LEGALCOPYRIGHT "Copyright (c) 2020-2022 RxLaboratory, Nicolas Dufresne and contributors"
#define STR_LEGALTRADEMARKS1 "All Rights Reserved"
#define STR_ORIGINALFILENAME "DuME.exe"
#define STR_PRODUCTNAME "DuME - Duduf Media Encoder"
#define STR_PRODUCTDESCRIPTION "A fast and versatile media encoder"

#define STR_COMPANYDOMAIN "rainboxlab.org"

#define URL_CHAT "http://chat.rxlab.info"
#define URL_FORUM ""
#define URL_DOC "http://dume.rxlab.guide"
#define URL_BUGREPORT "https://github.com/Rainbox-dev/DuME/issues/new?assignees=&labels=Bug&template=bug_report.md&title="
#define URL_DONATION "http://donate.rxlab.info"
#define URL_UPDATE "http://version.rxlab.io"

#define APP_ICON ":/icons/app"
#define SPLASH_IMAGE ":/images/splash"

#endif // APPVERSION_H
