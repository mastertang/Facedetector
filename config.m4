PHP_ARG_WITH(face_detector, for face_detector support,
Make sure that the comment is aligned:
[  --with-face_detector    Include face_detector support])

PHP_ARG_WITH(dynamic-dir, set the dynamic dir,
[  --with-dynamic-dir=DIR  Needed dynamic so dir path],"","")

PHP_ARG_WITH(include-dir, set the files include dir,
[ --with-include-dir=DIR Include files dir path],"","")

if test "$PHP_FACE_DETECTOR" != "no"; then

    if test -z "$PHP_DYNAMIC_DIR"; then
	    AC_MSG_RESULT([dynamic dir not set])
	    AC_MSG_ERROR([Please set the dynamic dir : $PHP_DYNAMIC_DIR])
    fi

    if test !-d "$PHP_DYNAMIC_DIR"; then
        AC_MSG_RESULT([dynamic dir not exist])
        AC_MSG_ERROR([Please set the correct dynamic dir : $PHP_DYNAMIC_DIR])
    fi

    if test -z "$PHP_INCLUDE_DIR"; then
	    AC_MSG_RESULT([include dir not set])
	    AC_MSG_ERROR([Please set the include dir : $PHP_INCLUDE_DIR])
    fi

    if test !-z "$PHP_INCLUDE_DIR"; then
    	AC_MSG_RESULT([include dir not exist])
    	AC_MSG_ERROR([Please set the correct include dir : $PHP_INCLUDE_DIR])
    fi

	PHP_REQUIRE_CXX()
	PHP_ADD_INCLUDE($PHP_INCLUDE_DIR)
	PHP_ADD_LIBRARY(stdc++,"",EXTERN_NAME_LIBADD)
	if test -z "$PHP_DEBUG"; then
		AC_ARG_ENABLE(debug,[--enable-debug compile with debugging system],
		[PHP_DEBUG=no])
	fi

    PHP_ADD_LIBRARY(opencv_core,$PHP_DYNAMIC_DIR,EXTERN_NAME_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(opencv_core, $PHP_DYNAMIC_DIR, FACEDETECT_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(opencv_objdetect, $PHP_DYNAMIC_DIR, FACEDETECT_SHARED_LIBADD)
	PHP_SUBST(EXTERN_NAME_SHARED_LIBADD)
	AC_DEFINE(HAVE_FACE_DETECTOR , 1, [])
	PHP_NEW_EXTENSION(face_detector, face_detector.cpp, $ext_shared)
fi
