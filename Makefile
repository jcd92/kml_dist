# * 
# ****************************************************************************
# * <P> XML.c - implementation file for basic XML parser written in ANSI C++ 
# * for portability.
# * It works by using recursion and a node tree for breaking down the elements
# * of an XML document.  </P>
# *
# * @version     V2.44
# *
# * @author      Frank Vanden Berghen
# *
# * Copyright (c) 2002, Frank Vanden Berghen - All rights reserved.
# * Commercialized by <a href="http://www.Business-Insight.com">Business-Insight</a>
# * See the file "AFPL-license.txt about the licensing terms
# * 
# ****************************************************************************
#
#makefile for the xmlParser library
#
all : xmlTest

#GPP=/opt/intel/cc/9.1.039/bin/icpc
GPP=g++

xmlTest : xmlParser.cpp xmlParser.h xmlTest.cpp
	$(GPP) -g -Wall -o xmlTest \
                 xmlParser.cpp xmlTest.cpp
clean:
	-@rm xmlTest              >/dev/null 2>/dev/null
	-@rm *~                   >/dev/null 2>/dev/null
	@echo "Clean OK."
