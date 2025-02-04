/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include <algorithm>
#include <cstdlib>

#include <tools/debug.hxx>
#include <vcl/outdev.hxx>

#include <editeng/svxfont.hxx>
#include <AccessibleStringWrap.hxx>


// AccessibleStringWrap implementation


AccessibleStringWrap::AccessibleStringWrap( OutputDevice& rDev, SvxFont& rFont, const OUString& rText ) :
    mrDev( rDev ),
    mrFont( rFont ),
    maText( rText )
{
}

void AccessibleStringWrap::GetCharacterBounds( sal_Int32 nIndex, tools::Rectangle& rRect )
{
    DBG_ASSERT(nIndex >= 0,
               "SvxAccessibleStringWrap::GetCharacterBounds: index value overflow");

    mrFont.SetPhysFont(mrDev);

    // #108900# Handle virtual position one-past-the end of the string
    if( nIndex >= maText.getLength() )
    {
        // create a caret bounding rect that has the height of the
        // current font and is one pixel wide.
        rRect.SetLeft( mrDev.GetTextWidth(maText) );
        rRect.SetTop( 0 );
        rRect.SetSize( Size(mrDev.GetTextHeight(), 1) );
    }
    else
    {
        tools::Long aXArray[2];
        mrDev.GetCaretPositions( maText, aXArray, nIndex, 1 );
        rRect.SetLeft( 0 );
        rRect.SetTop( 0 );
        rRect.SetSize( Size(mrDev.GetTextHeight(), std::abs(aXArray[0] - aXArray[1])) );
        rRect.Move( std::min(aXArray[0], aXArray[1]), 0 );
    }

    if( mrFont.IsVertical() )
    {
        // #101701# Rotate to vertical
        rRect = tools::Rectangle( Point(-rRect.Top(), rRect.Left()),
                           Point(-rRect.Bottom(), rRect.Right()));
    }
}

sal_Int32 AccessibleStringWrap::GetIndexAtPoint( const Point& rPoint )
{
    // search for character bounding box containing given point
    tools::Rectangle aRect;
    sal_Int32 i, nLen = maText.getLength();
    for( i=0; i<nLen; ++i )
    {
        GetCharacterBounds(i, aRect);
        if( aRect.Contains(rPoint) )
            return i;
    }

    return -1;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
