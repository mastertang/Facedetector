/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Zend/zend_API.h"
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_face_detector.h"
}
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>

#define PARAMS_NULL -1
#define INVALID_XML_FILE -2
#define XML_ANALYSIS_FAILED -3
#define PICTURE_ANALYSIS_FAILED -4

using namespace std;
using namespace cv;


/* If you declare any globals in php_face_detector.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(face_detector)
*/

/* True global resources - no need for thread safety here */
static int le_face_detector;

/* {{{ face_detector_functions[]
 *
 * Every user visible function must have an entry in face_detector_functions[].
 */
const zend_function_entry face_detector_functions[] = {
	PHP_FE(face_detector,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in face_detector_functions[] */
};
/* }}} */

/* {{{ face_detector_module_entry
 */
zend_module_entry face_detector_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"face_detector",
	face_detector_functions,
	PHP_MINIT(face_detector),
	PHP_MSHUTDOWN(face_detector),
	PHP_RINIT(face_detector),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(face_detector),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(face_detector),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_FACE_DETECTOR_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FACE_DETECTOR
ZEND_GET_MODULE(face_detector)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("face_detector.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_face_detector_globals, face_detector_globals)
    STD_PHP_INI_ENTRY("face_detector.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_face_detector_globals, face_detector_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_face_detector_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_face_detector_init_globals(zend_face_detector_globals *face_detector_globals)
{
	face_detector_globals->global_value = 0;
	face_detector_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(face_detector)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(face_detector)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(face_detector)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(face_detector)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(face_detector)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "face_detector support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_face_detector_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(face_detector)
{
	char *xmlString = NULL;
	char *picturePath = NULL;
	int xmlLen, picLen;
	char *strg=NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &xmlString, &xmlLen, &picturePath, &picLen) == FAILURE) {
		RETURN_FALSE;
	}

    if(strcmp(xmlString,"")==0 || strcmp(picturePath,"")==0){
        RETURN_LONG(PARAMS_NULL);
    }
    fstream xmlFile;
    xmlFile.open(xmlString,ios::in);
    if(!xmlFile){
        xmlFile.close();
        RETURN_LONG(INVALID_XML_FILE)
    }
    xmlFile.close();
    fstream pictureFile;
    pictureFile.open(xmlString,ios::in);
    Mat srcImage;
    if(!pictureFile){
        pictureFile.close();
        vector<char> data;
        data.resize(sizeof(picturePath));
        data.assign(picturePath,picturePath+sizeof(picturePath));
        srcImage = imdecode(Mat(data),1);
    }else{
        pictureFile.close();
        srcImage = imread(picturePath);
    }
    if(srcImage.data == NULL){
        RETURN_LONG(PICTURE_ANALYSIS_FAILED);
    }
	CascadeClassifier cascade;
	if(!cascade.load(xmlString)){
	    RETURN_LONG(XML_ANALYSIS_FAILED);
	}
	Mat grayImage;
	vector<Rect> rect;
	grayImage.create(srcImage.size(), srcImage.type());
	cvtColor(srcImage, grayImage, CV_BGR2GRAY);
	cascade.detectMultiScale(grayImage, rect, 1.1, 2, 0);

	std::string result = "[";
	int x=0,y=0,width=0,height=0;
	stringstream ssx,ssy,sswidth,ssheight;
	string tempStr;
    for (int i = 0; i < rect.size(); i++){
        ssx.clear();
        ssx.str("");
        ssy.clear();
        ssy.str("");
        sswidth.clear();
        sswidth.str("");
        ssheight.clear();
        ssheight.str("");
        tempStr = "";

    	result += "{";
    	x = rect[i].x;
    	ssx << x;
    	ssx >> tempStr;
    	result += "\"x\":" + tempStr;
        tempStr = "";
    	y = rect[i].y;
        ssy << y;
        ssy >> tempStr;
        result += ",\"y\":" + tempStr;

        tempStr = "";
        width = rect[i].width;
        sswidth << width;
        sswidth >> tempStr;
        result += ",\"width\":" + tempStr;

        tempStr = "";
        height = rect[i].height;
        ssheight << height;
        ssheight >> tempStr;
        result += ",\"height\":" + tempStr;

    	result = result + "}";
    	if (i != (rect.size() - 1))
    		result = result + ",";
    }
    result = result + "]";
    RETURN_STRING(result.c_str(), 1);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
